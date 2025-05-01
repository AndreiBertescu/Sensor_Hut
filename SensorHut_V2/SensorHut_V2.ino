#include "src/dht11/dht11.h"
#include "src/ds1302/DS1302.h"
#include "src/bmp280/Adafruit_BMP280.h"
#include "src/st7789/Adafruit_ST7789.h"
#include <avr/sleep.h>
#include <avr/power.h>

#define ENABLE_SERIAL false
#define TIME_ALIVE 16000            // Rounds up to multiples of 4000
#define TIME_SHORT_SLEEP 60000 * 10 // Rounds up to multiples of 4000
#define TIME_LONG_SLEEP 60000 * 10  // Rounds up to multiples of 4000

#define DHT_PIN 4
#define LIGHT_SNSR_PIN A0  // Must be analog pin
#define BUZZER_PIN 6       // Must be 6 to use Timer #0 - HW constraint
#define BTN_1 2
#define BTN_2 3
#define LED_PIN 7          // HW constraint
#define RTC_RST_PIN 5      // Shouldn't be analog ???
#define RTC_DATA_PIN A2
#define RTC_SCKL_PIN A1
#define BMP280_SDA_PIN A4  // Must be sda pin
#define BMP280_SCL_PIN A5  // Must be scl pin
#define LCD_DC 9           // HW constraint
#define LCD_RST 10         // HW constraint
#define LCD_BRT 8          // HW constraint
#define LCD_SCK 13         // Must be sck pin - HW constraint
#define LCD_MOSI 11        // Must be mosi pin - HW constraint
#define BATT_SNSR_PIN A3   // Must be analog pin

// DHT11 temperature and humidity sensor
dht11 dht_inst;
uint8_t dht_temp;
uint8_t dht_hum;

// Battery sensor
int batt_snsr_val;

// Photoresistor
uint16_t light_snsr_val;

// Button interrupt flags
volatile bool btn_1_pressed;
volatile bool btn_2_pressed;

// DS1302 real time clock
DS1302 ds1302_inst(RTC_RST_PIN, RTC_DATA_PIN, RTC_SCKL_PIN);
Time t(2099, 1, 1, 0, 0, 0, Time::kSunday);

// BMP280 temperature, pressure and altitude sensor
Adafruit_BMP280 bmp;
int bmp_status;
int bmp_temp;
int bmp_press;
int bmp_altitude;

// ST7789 240x240 IPS display
Adafruit_ST7789 lcd = Adafruit_ST7789(-1, LCD_DC, LCD_RST);

// Other
char buf[64];
bool power_saving_mode = false;

void setup() {
  // Pin initialization
  pinMode(LIGHT_SNSR_PIN, INPUT);
  pinMode(BATT_SNSR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LCD_BRT, OUTPUT);
  pinMode(BTN_1, INPUT_PULLUP);
  pinMode(BTN_2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BTN_1), button1ISR, FALLING);
  attachInterrupt(digitalPinToInterrupt(BTN_2), button2ISR, FALLING);
  
  ACSR |= (1 << ACD);      // Disable analog comparator
  ADCSRA &= ~(1 << ADEN);  // Disable ADC
  TCCR1A = 0; // Disable Timer1
  TCCR1B = 0;
  TIMSK1 = 0;
  TCCR2A = 0; // Disable Timer2
  TCCR2B = 0;
  TIMSK2 = 0;

  // DS1302 initialization
  ds1302_inst.writeProtect(true);
  ds1302_inst.halt(false);
  t = ds1302_inst.time();
  if (t.getUnixFromMacros(__DATE__, __TIME__) > t.getUnixTime()) {
    ds1302_inst.writeProtect(false);
    ds1302_inst.time(Time(__DATE__, __TIME__));
    ds1302_inst.writeProtect(true);
  }

  // BMP280 initialization
  bmp_status = bmp.begin(BMP280_ADDRESS_ALT, BMP280_CHIPID);
  if (bmp_status) {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X2,     /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
  }

  // LCD initialization
  pinMode(LCD_BRT, OUTPUT);
  digitalWrite(LCD_BRT, HIGH);
  lcd.init(240, 240, SPI_MODE2);
  lcd.setRotation(2);
  lcd.setTextWrap(false);

  if (ENABLE_SERIAL) {
    // Serial initialization
    Serial.begin(9600);
  } else {
    // Buzzer confirm end of setup
    startupRoutine();
  }
}

void loop() {
  if (ENABLE_SERIAL) {
    Serial.println();
  }

  digitalWrite(LCD_BRT, HIGH);

  handleButtons();
  handleBATT_SNSR();
  handleLIGHT_SNSR();
  handleDHT();
  handleBMP280();
  handleDS1302();
  handleLCD();

  sleepFor(TIME_ALIVE);
  digitalWrite(LCD_BRT, LOW);
  digitalWrite(LED_PIN, LOW);

  sleepFor(power_saving_mode ? TIME_LONG_SLEEP : TIME_SHORT_SLEEP);  // ~11mA less power usage than delay function
  // delay(1000);
}

void handleButtons() {
  if (btn_1_pressed) {
    activateBuzzer(1);
    power_saving_mode = !power_saving_mode;

    btn_1_pressed = false;
  }
  if (btn_2_pressed) {
    if (!power_saving_mode) {
      activateBuzzer(2);
    } else {
      delay(500);
    }

    btn_2_pressed = false;
  }

  if (!power_saving_mode) {
    digitalWrite(LED_PIN, HIGH);
  }
}

void handleBATT_SNSR() {
  int battery_voltage = 0;
  int nr_samples = 20;

  ADCSRA |= (1 << ADEN);  // Enable ADC
  for(int i=0; i<nr_samples; i++) {
    battery_voltage += analogRead(BATT_SNSR_PIN);
    delay(10);
  }
  batt_snsr_val = map(battery_voltage/nr_samples, 0, 1023, 0, 500);
  ADCSRA &= ~(1 << ADEN);  // Disable ADC

  if (ENABLE_SERIAL) {
    sprintf(buf, "Battery sensor: %u.%.2uV", batt_snsr_val / 100 % 10, batt_snsr_val % 100);
    Serial.println(buf);
  }
}

void handleLIGHT_SNSR() {
  ADCSRA |= (1 << ADEN);  // Enable ADC
  light_snsr_val = map(analogRead(LIGHT_SNSR_PIN), 1023, 0, 0, 100);
  ADCSRA &= ~(1 << ADEN);  // Disable ADC

  if (ENABLE_SERIAL) {
    sprintf(buf, "Light sensor: %u%%", light_snsr_val);
    Serial.println(buf);
  }
}

void handleDHT() {
  int status = dht_inst.read(DHT_PIN);

  if (status == 0) {
    dht_temp = dht_inst.temperature;
    dht_hum = dht_inst.humidity;
  }

  if (ENABLE_SERIAL) {
    sprintf(buf, "DHT11:%d - T: %uC - H: %u%%", status, dht_temp, dht_hum);
    Serial.println(buf);
  }
}

void handleBMP280() {
  if (bmp_status != BMP280_CHIPID) {
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,  /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,  /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16, /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,   /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500);
  }

  bmp_status = bmp.getId();

  if (bmp_status == BMP280_CHIPID) {
    bmp_temp = bmp.readTemperature();
    bmp_press = int(bmp.readPressure() / 100);
    bmp_altitude = bmp.readAltitude(1008);
  }

  if (ENABLE_SERIAL) {
    sprintf(buf, "BMP280:%u - T: %uC - P: %uhPa - A: %u", bmp_status, bmp_temp, bmp_press, bmp_altitude);
    Serial.println(buf);
  }
}

void handleDS1302() {
  t = ds1302_inst.time();

  if (ENABLE_SERIAL) {
    sprintf(buf, "RTC: %u/%u/%u - %u:%u:%u", t.date, t.mon, t.yr, t.hr, t.min, t.sec);
    Serial.println(buf);
  }
}

void handleLCD() {
  char str[20];
  int text_color = 0;

  lcd.setTextSize(2);
  if (power_saving_mode) {
    text_color = ST77XX_RED;
    lcd.fillScreen(ST77XX_BLACK);
  } else {
    if (light_snsr_val <= 50) {
      text_color = ST77XX_WHITE;
      lcd.fillScreen(ST77XX_BLACK);
    } else {
      text_color = ST77XX_BLACK;
      lcd.fillScreen(ST77XX_WHITE);
    }
  }
  lcd.setTextColor(text_color);

  // Battery
  lcd.setCursor(172, 10);
  if(batt_snsr_val < 320) {
    lcd.setTextColor(ST77XX_RED);
  } else if(batt_snsr_val < 370) {
    lcd.setTextColor(ST77XX_ORANGE);
  }
  sprintf(str, "%d.%.2dv", batt_snsr_val / 100 % 10, batt_snsr_val % 100);
  lcd.print(str);

  // Date
  lcd.setCursor(10, 10);
  if(t.yr <= 2000 || t.yr >= 2100) {
    sprintf(str, "RTC ERROR");
    lcd.setTextColor(ST77XX_ORANGE);
  } else {
    sprintf(str, "%02u/%02u/%04u", t.date, t.mon, t.yr);
    lcd.setTextColor(text_color);
  }
  lcd.print(str);
  lcd.setTextColor(text_color);

  // Time
  lcd.setTextSize(7);
  lcd.setCursor(17, 50);
  sprintf(str, "%02u:%02u", t.hr, t.min);
  lcd.print(str);

  // Temperature and Humidity
  lcd.setTextSize(3);
  lcd.setCursor(10, 130);
  sprintf(str, "T:%dC  H:%d%%", dht_temp, dht_hum);
  lcd.print(str);

  // Atmospheric pressure
  lcd.setCursor(10, 165);
  sprintf(str, "Pres: %dhPa", bmp_press);
  lcd.print(str);

  // Brightness
  lcd.setCursor(10, 200);
  sprintf(str, "Brgt: %u%%", light_snsr_val);
  lcd.print(str);

  // Power saving mode
  lcd.setTextSize(1);
  lcd.setCursor(205, 215);
  sprintf(str, "PS:%c", power_saving_mode ? 'Y' : 'N');
  lcd.print(str);
}

// Helper functions and Interrupt routines
void startupRoutine() {
  // Splash screen
  const char* ascii_art[] = {
    "\\\\/\\\\/\\/\\/",
    "\\/\\\\/\\/\\\\\\",
    "\\\\//\\/\\\\/\\",
    "/\\SENSOR\\\\",
    "/\\\\HUT/\\//",
    "/\\\\V2.0\\/\\",
    "\\\\/\\\\/\\/\\/",
    "/\\/\\\\//\\\\\\",
    "\\/\\\\/\\/\\\\\\"
  };

  digitalWrite(LED_PIN, HIGH);
  lcd.fillScreen(ST77XX_RED);
  lcd.setTextColor(ST77XX_BLACK);
  lcd.setTextSize(4);
  lcd.setCursor(0,-5);
  for (int i = 0; i < sizeof(ascii_art) / sizeof(ascii_art[0]); i++) {
    lcd.println(ascii_art[i]);  // Print each line of the ASCII art
  }

  // Mii melody
  tone(6, 740); delay(108); // NOTE_FS5 (90% of 960/8 = 120ms -> 108ms), plus 12ms pause
  noTone(6); delay(120);    // REST (960 / 8)

  tone(6, 880); delay(108); // NOTE_A5
  noTone(6); delay(12);

  tone(6, 554); delay(108); // NOTE_CS5
  noTone(6); delay(120);    // REST

  tone(6, 880); delay(108); // NOTE_A5
  noTone(6); delay(120);    // REST

  tone(6, 740); delay(108); // NOTE_FS5
  noTone(6);

  tone(6, 587); delay(108); // NOTE_D5
  noTone(6);

  tone(6, 587); delay(108); // NOTE_D5
  noTone(6);

  tone(6, 587); delay(108); // NOTE_D5
  noTone(6);

  digitalWrite(LED_PIN, LOW);
  delay(500);
}

void button1ISR() {
  btn_1_pressed = true;
}

void button2ISR() {
  btn_2_pressed = true;
}

void activateBuzzer(uint8_t type) {
  switch (type) {
    case 1:
      tone(BUZZER_PIN, 500, 100);
      delay(110);
      tone(BUZZER_PIN, 800, 100);
      delay(110);
      tone(BUZZER_PIN, 1000, 100);
      delay(110);
      noTone(BUZZER_PIN);
      break;
    default:
      tone(BUZZER_PIN, 700, 100);
      delay(110);
      tone(BUZZER_PIN, 1000, 100);
      delay(110);
      noTone(BUZZER_PIN);
      break;
  }
}

// Watchdog ISR
ISR(WDT_vect) {}

// Setup Watchdog Timer for 8s sleep
void setupWatchdog() {
  MCUSR &= ~(1 << WDRF);  // Clear watchdog reset flag

  WDTCSR |= (1 << WDCE) | (1 << WDE);
  // WDTCSR = (1 << WDP3) | (1 << WDP0); // 8.0 seconds
  WDTCSR = (1 << WDP3);  // 4.0 seconds

  WDTCSR |= (1 << WDIE);  // Enable the WD interrupt (note no reset).
}

void sleepFor(unsigned long duration) {
  unsigned long slept = 0;

  setupWatchdog();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  power_all_disable();  // Disables SPI, USART, TWI, Timers

  while (slept < duration && !btn_1_pressed && !btn_2_pressed) {

    sleep_cpu();
    slept += 4000;
  }

  power_all_enable();  // Enables SPI, USART, TWI, Timers
  sleep_disable();
}

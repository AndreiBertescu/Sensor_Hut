#include <LowPower.h>
//DHT11
#include <DFRobot_DHT11.h>
//DS1302 RTC
#include <ThreeWire.h>
#include <RtcDS1302.h>
//BMP280
#include <Adafruit_BMP280.h>
//LCD
#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

//CONFIG
#define LIGHT_SENSOR A0
#define CLOCK_CLOCK A1
#define CLOCK_DATA A2
#define BATTERY_SENSOR A3
#define BUTTON1 3
#define BUTTON2 2
#define SWITCH 12

#define CLOCK_RESET 4
#define BUZZER 5
#define DHT_SENSOR 7
#define SCK_LIGHT 8
#define TFT_DC 9
#define TFT_RST 10
#define TFT_BRT 6

#define OFF_TIME 64  //Amount of seconds: multiple of 8, at least 8;
#define countof(a) (sizeof(a) / sizeof(a[0]))

DFRobot_DHT11 DHT;
Adafruit_BMP280 bmp;
ThreeWire myWire(CLOCK_DATA, CLOCK_CLOCK, CLOCK_RESET);
RtcDS1302<ThreeWire> rtc(myWire);
Adafruit_ST7789 tft = Adafruit_ST7789(-1, TFT_DC, TFT_RST);

uint8_t battery;
uint16_t lightData;
bool gasDanger;
uint8_t temperature, temperature_bmp;
uint8_t humidity;
uint16_t pressure;
uint8_t day;
uint8_t month;
uint16_t year;
uint8_t second;
uint8_t minute;
uint8_t hour;
unsigned long ellapsedTime;
bool update, updateBrt, powerSaver;

void setup() {
  // Serial.begin(9600);
  update = false;
  updateBrt = false;
  powerSaver = false;
  ellapsedTime = 0;

  //light sensor & battery & TFT_BRT
  pinMode(LIGHT_SENSOR, INPUT);
  pinMode(BATTERY_SENSOR, INPUT);
  pinMode(TFT_BRT, OUTPUT);
  pinMode(SCK_LIGHT, OUTPUT);
  digitalWrite(SCK_LIGHT, LOW);

  //buttons init
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  //power enable
  pinMode(SWITCH, OUTPUT);
  digitalWrite(SWITCH, HIGH);
  delay(2);

  //bmp280
  bmp.begin(0x76, 0x58);
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,  /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X16, /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16, /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X8,    /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_1);

  //clock
  rtc.Begin();
  // setClock();

  //lcd
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
}

void loop() {
  digitalWrite(TFT_BRT, HIGH);

  if (update) {
    tone(BUZZER, 1000);
    delay(60);
    noTone(BUZZER);
    update = false;
  }
  if (updateBrt) {
    buzzer();
    powerSaver = !powerSaver;
    digitalWrite(SCK_LIGHT, powerSaver ? HIGH : LOW);
    updateBrt = false;
  }

  digitalWrite(SCK_LIGHT, powerSaver ? LOW : HIGH);
  updateSensors();
  // printValues();
  updateLcd();
  digitalWrite(SCK_LIGHT, powerSaver ? HIGH : LOW);

  delay(20);
  attachInterrupt(0, button1, LOW);
  attachInterrupt(1, button2, LOW);

  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);
  if (powerSaver)
    digitalWrite(TFT_BRT, LOW);
  for (int i = 0; i < OFF_TIME / 8 - 1; i++)
    LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);

  detachInterrupt(1);
  detachInterrupt(0);
}

void updateSensors() {
  digitalWrite(SWITCH, HIGH);
  delay(2);

  batterySensor(battery);
  lightSensor(lightData);
  dhtSensor(temperature, humidity);
  clockModule(day, month, year, second, minute, hour);
  bmp280(temperature_bmp, pressure);

  temperature = (temperature + temperature_bmp) / 2;

  digitalWrite(SWITCH, LOW);
}

void batterySensor(uint8_t& battery) {
  if (analogRead(BATTERY_SENSOR) > 1024) {
    battery = 0;
    return;
  }

  battery = int((float)map(analogRead(BATTERY_SENSOR), 0, 1024, 0, 5) * 10);
}

void lightSensor(uint16_t& lightData) {
  lightData = 1024 - analogRead(LIGHT_SENSOR);
}

void dhtSensor(uint8_t& temperature, uint8_t& humidity) {
  DHT.read(DHT_SENSOR);

  temperature = DHT.temperature;
  humidity = DHT.humidity;
}

void clockModule(uint8_t& day, uint8_t& month, uint16_t& year, uint8_t& second, uint8_t& minute, uint8_t& hour) {
  RtcDateTime dt = rtc.GetDateTime();

  day = dt.Day();
  month = dt.Month();
  year = dt.Year();
  second = dt.Second();
  minute = dt.Minute();
  hour = dt.Hour();
}

void setClock() {
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  rtc.SetDateTime(compiled);
}

void bmp280(uint8_t& temperature_bmp, uint16_t& pressure) {
  temperature_bmp = static_cast<uint8_t>(bmp.readTemperature());
  pressure = static_cast<uint16_t>(bmp.readPressure() / 100);
}

void updateLcd() {
  char str[20];

  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE);

  //battery
  tft.setCursor(182, 10);
  sprintf(str, "%d.%dv", battery / 10 % 10, battery % 10);
  tft.print(str);

  //date
  tft.setCursor(10, 10);
  sprintf(str, "%02u/%02u/%04u", day, month, year);
  tft.print(str);

  //time
  tft.setTextSize(7);
  tft.setCursor(17, 50);
  sprintf(str, "%02u:%02u", hour, minute);
  tft.print(str);

  //dht11
  tft.setTextSize(3);
  tft.setCursor(10, 130);
  sprintf(str, "T:%dC  H:%d%%", temperature, humidity);
  tft.print(str);

  //bmp280
  tft.setCursor(10, 165);
  sprintf(str, "Pres: %dhPa", pressure);
  tft.print(str);

  //light sensor
  tft.setCursor(10, 200);
  if (lightData < 200)
    tft.print("Brgt:Dark");
  else if (lightData < 500)
    tft.print("Brgt:Dim");
  else
    tft.print("Brt:Bright");

  //power saving mode
  tft.setTextSize(1);
  tft.setCursor(205, 215);
  if (powerSaver)
    tft.setTextColor(ST77XX_GREEN);
  else
    tft.setTextColor(ST77XX_RED);
  sprintf(str, "PS:%c", powerSaver ? 'E' : 'D');
  tft.print(str);
}

void printValues() {
  char str[60];
  Serial.println();

  sprintf(str, "Battery voltage: %d.%dv  Battery reading: %d", battery / 10 % 10, battery % 10, analogRead(BATTERY_SENSOR));
  Serial.println(str);

  sprintf(str, "Brightness: %d", lightData);
  Serial.println(str);

  sprintf(str, "T:%dC  H:%d%%", temperature, humidity);
  Serial.println(str);

  sprintf(str, "T_bmp:%dC  Pressure:%dhPa", temperature_bmp, pressure);
  Serial.println(str);

  sprintf(str, "Time: %02u/%02u/%04u  %02u:%02u:%02u", day, month, year, hour, minute, second);
  Serial.println(str);

  Serial.print("\n");
}

void buzzer() {
  tone(BUZZER, 500);
  delay(100);

  tone(BUZZER, 800);
  delay(100);

  tone(BUZZER, 1000);
  delay(100);

  noTone(BUZZER);
}

void button1() {
  update = true;
}

void button2() {
  updateBrt = true;
}

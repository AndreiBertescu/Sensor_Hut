//DHT11
#include <DFRobot_DHT11.h>
//DS1302 RTC
#include <ThreeWire.h>
#include <RtcDS1302.h>
//BMP280
#include <Wire.h>
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

#define CLOCK_RESET 4
#define BUZZER 5
#define DHT_SENSOR 7
#define MQ9_SENSOR 8
#define TFT_DC 9
#define TFT_RST 10
#define SWITCH 12

#define ON_TIME 1 * 1000  //not used
#define OFF_TIME 30 * 1000
#define countof(a) (sizeof(a) / sizeof(a[0]))

DFRobot_DHT11 DHT;
Adafruit_BMP280 bmp;
ThreeWire myWire(CLOCK_DATA, CLOCK_CLOCK, CLOCK_RESET);
RtcDS1302<ThreeWire> rtc(myWire);
Adafruit_ST7789 tft = Adafruit_ST7789(-1, TFT_DC, TFT_RST);

uint8_t battery;
uint16_t lightData;
bool gasDanger;
uint8_t temperature;
uint8_t humidity;
uint8_t day;
uint8_t month;
uint16_t year;
uint8_t second;
uint8_t minute;
uint8_t hour;
unsigned long ellapsedTime;
bool update, buzzerTone;

void setup() {
  // Serial.begin(9600);
  update = true;
  buzzerTone = false;
  ellapsedTime = 0;

  //light sensor & battery
  pinMode(LIGHT_SENSOR, INPUT);
  pinMode(BATTERY_SENSOR, INPUT);

  //buttons init
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);

  //power enable
  pinMode(SWITCH, OUTPUT);
  digitalWrite(SWITCH, HIGH);
  delay(2);

  //clock
  rtc.Begin();
  // setClock();

  //lcd
  tft.init(240, 240, SPI_MODE2);
  tft.setRotation(2);
}

void loop() {
  if (millis() - ellapsedTime >= OFF_TIME) {
    ellapsedTime = millis();
    update = true;
  }

  if (!update && !digitalRead(BUTTON2)) {
    update = true;
    tone(BUZZER, 1000);
    delay(60);
    noTone(BUZZER);
  }
  if (!buzzerTone && !digitalRead(BUTTON1))
    buzzerTone = true;

  if (buzzerTone)
    buzzer();

  if (update) {
    updateSensors();
    updateLcd();
    // printValues();

    update = false;
  }

  delay(100);
}

void updateSensors() {
  batterySensor(battery);
  lightSensor(lightData);
  dhtSensor(temperature, humidity);
  clockModule(day, month, year, second, minute, hour);
  // mq9(gasDanger);
  // bmp280();
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

void mq9(bool gasDanger) {
  gasDanger = !digitalRead(MQ9_SENSOR);
}

void bmp280() {
  Serial.print("--- Temperature  = ");
  Serial.print(bmp.readTemperature());
  Serial.print(" *C ");

  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure() / 100);  //displaying the Pressure in hPa, you can change the unit
  Serial.print("  hPa  ");

  Serial.print("Approx altitude = ");
  Serial.print(bmp.readAltitude(1019.66));  //The "1019.66" is the pressure(hPa) at sea level in day in your region
  Serial.println("  m ");                   //If you don't know it, modify it until you get your current  altitude
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
  sprintf(str, "Temp:%dC", temperature);
  tft.print(str);

  tft.setCursor(10, 165);
  sprintf(str, "Hum:%d%%", humidity);
  tft.print(str);

  tft.setCursor(10, 200);
  if (lightData < 200)
    tft.print("Brgt:Dark");
  else if (lightData < 500)
    tft.print("Brgt:Dim");
  else
    tft.print("Brgt:Bright");
}

void printValues() {
  char str[60];
  Serial.println();

  sprintf(str, "Battery voltage: %u.%uv  Battery reading: %u", battery / 10 % 10, battery % 10, analogRead(BATTERY_SENSOR));
  Serial.println(str);

  sprintf(str, "Brightness: %u", lightData);
  Serial.println(str);

  sprintf(str, "Temp: %u%% °C Hum: %d", temperature, humidity);
  Serial.println(str);

  sprintf(str, "Time: %02u/%02u/%04u  %02u:%02u:%02u", day, month, year, hour, minute, second);
  Serial.println(str);

  Serial.print("\n");
}

void buzzer() {
  tone(BUZZER, 500);
  delay(300);

  tone(BUZZER, 800);
  delay(300);

  noTone(BUZZER);
  buzzerTone = false;
}

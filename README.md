# SensorHut
## Description & Features
- The SensorHut is an Arduino-powered sensor array that can measure temperature, humidity, barometric pressure, light level, along with a time-keeping feature. It has a 2.4" LCD display that shows all this data (along with the battery voltage and the power-saving mode), with a refresh rate of approximately 1 Hz.
- <strong>All the sensors are removable</strong>, along with the battery and the power module (including the solar panel, the DC-DC converter, solar controller, and charging module), thus allowing for maintenance and the replacement of defective components.
- It is powered by a 2000mAh LiPo battery that can be recharged through USB-C or passively through the incorporated solar panel.
- It also features two push buttons (left and right).
  - The <strong>right button</strong> is used to refresh the sensor data (thus ignoring the time delay).
  - The <strong>left button</strong> can be used to enable or disable the <strong>power-saving mode</strong>. This mode disables the attached LED and display backlight for most of the time (it only remains lit for the first 8 seconds after a sensor data refresh).
    
<p align="center"><br>
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/dd75fdbe-2ac5-4cdf-b565-bf0fa5709cc7" width="400" alt="Image 1">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/520b7183-37d1-44b8-b051-6cda2a163015" width="400" alt="Image 2">
</p>

## Components
- Arduino Uno Rev3
- DHT11 temperature and humidity sensor
- BMP280 barometric pressure and temperature sensor
- HY-018 photoresistor for detecting light level
- DS1302 real-time clock module
- GMT130 240x240 lcd using ST7789 driver
- Passive buzzer
- 2000mAh 3.7V LiPo rechargable battery
- 5.5V 120mA 84.5mm x 55.5mm solar panel
- MPPT Solar Charge Controller
- Voltage regulator 2.5-15V to 5V
- Charging circuit for Li-Ion battery
- Green LED
- One switch
- Single strand wires
- Two push buttons
- 1mm steel wires (for the frame)


## Schematics
![Schematic1](https://github.com/AndreiBertescu/SensorHut/assets/126001291/dc3ed218-7b95-4bda-ba9b-36e1568e4085)
![Schematic2](https://github.com/AndreiBertescu/SensorHut/assets/126001291/4d6c4f88-61c4-4152-8422-c382b5eaca3c)


## Other Images
Bare frame with all detachable components removed 
<p align="center">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/0c8be7b0-a188-4d4c-afc6-682cc39ee0b4" width="400" alt="Image 1">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/7171b2f4-20a1-4e05-8352-f7f4256d8491" width="400" alt="Image 2">
</p>

Components
<p align="center">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/6f095a60-3a2a-462e-828e-9cf62da795ed" width="400" alt="Image 1">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/20773fd0-5b10-4ed9-9a0b-dbfabeb3b997" width="400" alt="Image 2">
</p>

# SensorHut V2

<p align="center"><br>
  <img src="https://github.com/user-attachments/assets/40d12303-b0a0-46a4-97ea-b4b78551137f" width="400" alt="render">
  <img src="https://github.com/user-attachments/assets/40d12303-b0a0-46a4-97ea-b4b78551137f" width="400" alt="Image 2">
</p>

## Description
- The SensorHut is an Arduino-powered sensor array that can measure temperature, humidity, barometric pressure, light level, along with a time-keeping feature and battery voltege measurement. It has a 2.4" LCD display that shows all this data, two push buttons and a 6-cell rechargable battery pack that can keep it powered for up to 1 month.
### Features
- <strong>All the sensors are removable</strong>, along with the battery pack, arduino and the power circuit, thus allowing for maintenance and replacement or reusal of components.
- It is powered by 6 22000mAh LiPo batteries that can be recharged through USB-C or through an atachable solar panel.
- It also features two push buttons.
  - The first button is used to refresh the sensor data and turn on the display for the programmed 16 seconds.
  - The second button can be used to enable or disable the <strong>power-saving mode</strong>. This mode disables the attached LED, buzzer and display backlight for a much longer time than the standard mode (10 minutes for power-saving, 1 minute for standard).
  - The main Arduino board has been heavily modified to decrease power consumption and to facilitate assembly:
    - All the femate pin sockets were removed and repatached on the underside of the board.
    - Since the 5V required voltage is generated with an external step-up converter, the 12V power connector was replaced with the a buzzer.
    - An LED and the 1.3inch lcd display where wired and atached straight on the arduino board, to reduce the ammount of components that need to be placed and to be visible.
    - The always-on and auxiliary LEDs were removed.
    - The power pins on the ATMEGA16U2 auxiliary IC, along with the RX and TX LEDs, were rewired to be active only when the board is connected through the USB-B port ([tutorial](https://www.defproc.co.uk/tutorial/how-to-reduce-arduino-uno-power-usage-by-95/)).
 
<p align="center"><br>
  <img src="https://github.com/user-attachments/assets/40d12303-b0a0-46a4-97ea-b4b78551137f" width="350" alt="Image 2">
  <img src="https://github.com/user-attachments/assets/40d12303-b0a0-46a4-97ea-b4b78551137f" width="350" alt="Image 2">
  <br>
  <img src="V2/Photos/Sensor_Hut_3D.gif" width="700" alt="gif">
</p>

## Improvements added in version 2
- Version 2 two's frame was built from popsicle sticks, atached with Loctite superglue, which offers a much stronger structure compared to the first's version 1mm steel wireframe.
- 5 more batteries were added to greately increase battery capacity.
- Greater attention was placed on the designing of the frame. The whole SensorHut was designed in tandem on Fusion 360 to ensure propper sizing and simplify assembly.
- The Arduino board is now much more visible and plays a bigger role in the general asthetich of the SensorHut.
- The power delivery circuits were removed from the solar panle and placed on a dedicated frame on the top of the hut.
- The SensorHut cand now be functional  entirelly without the solar panel, since in the first version it didn't actually charge very much.
- The wire harness was greatelly simplified from the "spiderweb". Wires were placed on the left and right sides of the hut, and can be easely be removed by dethaching the right-angle pin headers from the Arduino's sockets.


## Bill of materials
- Arduino Uno Rev3 - [www.arduino.cc](https://store.arduino.cc/en-ro/products/arduino-uno-rev3?srsltid=AfmBOoqwYtjsYzje5ZUn_HA9sHTWeZQG49rOnZmOxhYvX3LoTWP9tqha)
- DHT11 module - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/temperature-sensors/4762-modul-senzor-de-temperatura-dht11-cu-led.html?search_query=dht+11&results=25)
- BMP280 module - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/pressure-sensors/12417-modul-senzor-de-presiune-barometric-bmp280.html?search_query=bmp280&results=17)
- KY-018 module - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/optical-sensors/13060-photosensitive-resistor-sensor-module-for-arduino.html?search_query=photoresistor&results=19)
- DS1302 module - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/all-products/12988-ds1302-real-time-clock-module-no-battery.html)
- ST7789 tft lcd - [www.aliexpress.com](https://www.aliexpress.com/item/1005008175467166.html)
- 2x push buttons - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/buttons-and-switches/1119-6x6x6-push-button.html?search_query=tactile+switch&results=25)
- Passive buzzer - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/buzzers/634-5v-passive-buzzer.html?search_query=buzzer&results=87)
- Single strand wires - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/kits/11951-plusivo-pvc-insulated-wire-kit-20awg-6-colors-7m-each.html)
- 5.5V 120mA polycrystalline solar cell - [www.aliexpress.com](https://www.aliexpress.com/item/1005007402518746.html)
- 1A 5V for 3.7V solar controller  - [www.aliexpress.com](https://www.aliexpress.com/item/1005003063466363.html)
- 2.5-15V to 5V step-up voltage regulator - [www.aliexpress.com](https://www.aliexpress.com/item/1005005911490910.html)
- 5V 1A USB-C lithium battery charger - [www.aliexpress.com](https://www.aliexpress.com/item/1005003598600322.html)
- 6x 3.7V 2200mAh rechargable battery - [www.aliexpress.com](https://www.aliexpress.com/item/1005001265547747.html)
- 2.54 pitch pin headers and sockets of various lengths - [www.optimusdigital.ro](https://www.optimusdigital.ro/en/kits/4737-connector-assortment-kit-1004-pcs.html?search_query=pin+header+kit&results=105)
- 1Mohm resistor
- 330ohm resistor
- Green SMD LED
- SPDT switch
- 30x1mm steel wires (for 2x clamps)
- Popsicle sticks for the frame


## Version 1 schematic
(version 2 has a different pinout)
![Schematic1](https://github.com/AndreiBertescu/SensorHut/assets/126001291/dc3ed218-7b95-4bda-ba9b-36e1568e4085)


## Other Images
Bare frame with all detachable components removed 
<p align="center">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/0c8be7b0-a188-4d4c-afc6-682cc39ee0b4" width="400" alt="Image 1">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/7171b2f4-20a1-4e05-8352-f7f4256d8491" width="400" alt="Image 2">
</p>

Components
<p align="center">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/6f095a60-3a2a-462e-828e-9cf62da795ed" width="400" alt="Image 1">
  <img src="https://github.com/AndreiBertescu/SensorHut/assets/126001291/9341f76c-d073-4718-a8da-f3d14810d1c2" width="400" alt="Image 2">
</p>

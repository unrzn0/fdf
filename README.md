# fdf
Alternative console for First Degree Fitness water rowers using an ESP32 and an small OLED display.

## Hardware Requirements

* ESP32 development board (I've used an MHET ET LIVE ESP32MiniKit)
* SSD1306 OLED display (I've used the 4-pin variant - e.g. https://www.aliexpress.com/item/32639731302.html)
* extra reed sensor e.g. from an old bike computer
* some wires, XH2.54 connectors, etc
* power supply (am using USB to the ESP32 board so far)
* 100kOhm potentiometer to simulate the 5-level resistance switch (not yet implemented)

## Software Requirements

* ESP32 Arduino environment
* ThingPulse SSD1306 library for the SSD1306 OLED display. (https://github.com/ThingPulse/esp8266-oled-ssd1306)

## Hardware connections

* Reed sensor: connect to VCC and pin 23
* OLED display: connect to SDA,SCL,VCC,GND to the OLED pins
* Potentiometer: not yet implemented

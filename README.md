# DK-Kombuchatron
The _Kombuchatron_ is an experimental Arduino-based monitoring system to monitor Kombucha progress during the home brewing process.

![The Kombuchatron](/images/kombuchatron.jpg)
## Features
- Real-time clock (RTC) tracks date & time with battery backup
- Batch start date / days elapsed display
- Temperature probe
- Liquid level float sensor
## How it works
Press and hold the reset button to start a new brew batch. A LED inside the project enclosure will notify you of the following:
- Blinking green: 14 days have elapsed, your Kombucha should be done
- Blinking red: Temperature problem, make sure the brew process remains between 76° - 88° F for best results
- Blinking blue: Liquid level is low, check for evaporation / Kombucha level

The clock module and the 20x4 LCD module both communicate over an I2C bus. To set the clock, upload the code and the time will be set to your computer's clock.
## Ingredients
This project uses all inexpensive parts: a Pro Micro board, a DS1307 RTC, a PCF8574T 20x4 LCD, and an [upcycled project enclosure](https://www.ebay.com/itm/292067232173).

#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Thermistor.h>
#include <EEPROM.h>
#include <LightDK.h>

#define I2C_ADDR    0x3f  // Define I2C Address for the PCF8574T
#define BACKLIGHT_PIN  3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

#define LED_OFF  1
#define LED_ON  0

#define buttonPin 14
#define floatPin 18 // A0

LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);
LightDK ledAlert(7,6,5);

RTC_DS1307 rtc;
DateTime start;
static const char daysOfTheWeek[7][4] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
#define SECONDS_PER_DAY 86400

Thermistor temp(21); // A3

bool redAlert, greenAlert, blueAlert;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running! Setting clock...");
    // Set the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(__DATE__, __TIME__));
  }
  
  // Initialize 20x4 LCD
  lcd.begin (20,4); 
  lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
  lcd.setBacklight(LED_ON);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(floatPin, INPUT_PULLUP);

  // Read start time from memory
  EEPROM.get(0, start);

  lcd.backlight();  //Backlight ON if under program control
}

void loop() 
{ 
  lcd.clear();
  delayWithStatusLED(1);

  // RTC time debug
  DateTime now = rtc.now(); 
  Serial.println("Current DateTime:");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println(); 

  Serial.println("Start DateTime:");
  Serial.print(start.year(), DEC);
  Serial.print('/');
  Serial.print(start.month(), DEC);
  Serial.print('/');
  Serial.print(start.day(), DEC);
  Serial.print(' ');
  Serial.print(start.hour(), DEC);
  Serial.print(':');
  Serial.print(start.minute(), DEC);
  Serial.print(':');
  Serial.print(start.second(), DEC);
  Serial.println();   

  // Check days elapsed - green steady alert if batch is done
  DateTime diff = DateTime(now.unixtime() - start.unixtime());
  int daysElapsed = diff.unixtime() / SECONDS_PER_DAY;
  greenAlert = daysElapsed >= 14;

  // Read temperature - red alert if temp is out of range
  int temperature = temp.getTemp();
  redAlert = temperature > 88 || temperature < 76;

  // Read float level - blue alert if level is low
  blueAlert = digitalRead(floatPin) == 0;

  // Update LCD display
  lcd.setCursor(1,0);
  lcd.print("Kombuchatron " + String(now.hour() % 12 == 0 ? 12 : now.hour() % 12) + ":" + (now.minute() < 10 ? "0" + String(now.minute()) : String(now.minute()))); // + (now.hour() < 12 ? "am" : "pm"));
  delayWithStatusLED(1);
  lcd.setCursor(1,1); 
  lcd.print("Started " + String(daysOfTheWeek[start.dayOfWeek()]) + " " + (start.month() < 10 ? "0" + String(start.month()) : String(start.month())) + "/" + (start.day() < 10 ? "0" + String(start.day()) : String(start.day()))); // + "/" + String(start.year()).substring(2));
  delayWithStatusLED(1);
  lcd.setCursor(1,2);  
  lcd.print("Days Elapsed: " + String(daysElapsed));
  lcd.setCursor(1,3);
  delayWithStatusLED(1);  
  lcd.print("Temp: " + String(temperature) + "\xDF F");
  delayWithStatusLED(8);

  // Reset start time and write to memory if reset button is pushed
  if(digitalRead(buttonPin) == 0) {
      start = rtc.now(); 
      EEPROM.put(0, start);

      lcd.clear();
      lcd.home();
      lcd.print(" Reset!");
      delay(3000);
  }
}

void delayWithStatusLED(int seconds) {
  // Alternate blue and red alerts, green is steady on if no other alerts active
  for(int i = 0; i <= seconds * 2; i++) {
    ledAlert.setColor(redAlert, greenAlert && !redAlert && !blueAlert, false);
    delay(250);
  
    ledAlert.setColor(false, greenAlert && !redAlert && !blueAlert, blueAlert);
    delay(250);
  }
}


/*
  LiquidCrystal Library - Autoscroll

  Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
  library works with all LCD displays that are compatible with the
  Hitachi HD44780 driver. There are many of them out there, and you
  can usually tell them by the 16-pin interface.

  This sketch demonstrates the use of the autoscroll()
  and noAutoscroll() functions to make new text scroll or not.

  The circuit:
   LCD RS pin to digital pin 12
   LCD Enable pin to digital pin 11
   LCD D4 pin to digital pin 5
   LCD D5 pin to digital pin 4
   LCD D6 pin to digital pin 3
   LCD D7 pin to digital pin 2
   LCD R/W pin to ground
   10K resistor:
   ends to +5V and ground
   wiper to LCD VO pin (pin 3)

  Library originally added 18 Apr 2008
  by David A. Mellis
  library modified 5 Jul 2009
  by Limor Fried (http://www.ladyada.net)
  example added 9 Jul 2009
  by Tom Igoe
  modified 22 Nov 2010
  by Tom Igoe

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/LiquidCrystalAutoscroll

*/

// include the library code:
#include <stdio.h>
#include <string.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <DS3231.h>

#define BUFF_MAX 128

void timeStamp();

// Init the DS3231 using the hardware interface
DS3231 rtc;

// Init a Time-data structure
RTCDateTime  t;


// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
const int button1Pin = 6;  // pushbutton 1 pin
const int ledPin =  13;    // LED pin
const int motorPin = 10;  // Motor Pin
unsigned long buttonPushedMillis; // when button was released
unsigned long autoMillis; //when the auto feed last ran
bool ledOn = false;

void setup()
{
  // Setup Serial connection
  Serial.begin(9600);

  // Initialize the rtc object
  rtc.begin();

  // Manual (YYYY, MM, DD, HH, II, SS uncomment to set date
  //rtc.setDateTime(2016, 12, 16, 00, 01, 00);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // Set up the pushbutton pins to be an input:
  pinMode(button1Pin, INPUT);

  // Set up the LED pin to be an output:
  pinMode(ledPin, OUTPUT);

  // set up the motor pin to be an output:
  pinMode(motorPin, OUTPUT);
  
  // clear alarm
  //rtc.armAlarm1(false);
  //rtc.clearAlarm1();
  //rtc.armAlarm2(false);
  //rtc.clearAlarm2();
  
  // setAlarm1(Date or Day, Hour, Minute, Second, Mode, Armed = true)
  rtc.setAlarm1(0, 00, 00, 59, DS3231_MATCH_H_M_S);
  rtc.setAlarm2(0, 23, 33,    DS3231_MATCH_H_M);
  
  // Check alarm settings
  checkAlarms();
  
}

void loop()
{
  // Get data from the DS3231
  t = rtc.getDateTime();

  char buff[BUFF_MAX];

  //snprintf(buff, BUFF_MAX, "%d.%02d.%02d %02d:%02d:%02d", t.year, t.mon, t.mday, t.hour, t.min, t.sec);

  //lcd.print(buff);

  // get current millis to track initial time
  unsigned long currentMillis = millis();

  // set the cursor to (0,0):
  lcd.setCursor(0, 0);

  // string representation of time
  lcd.print(rtc.dateFormat("M d  h:i:s", t));

  // start on second row
  lcd.setCursor(0, 1);

  // display time fed and how
  lcd.print("BLF: ");

	// variables to hold the pushbutton states
  int button1State = digitalRead(button1Pin);  
  if (button1State == LOW)
  {
    buttonPushedMillis = currentMillis;
    ledOn = true;
    digitalWrite(ledPin, HIGH);
    timeStamp();
    lcd.print(" (B)");
    feed();
  }
  
  if (ledOn)
  {
    //this is typical millis code here:
    if ((unsigned long)(currentMillis - buttonPushedMillis) >= 3000)
    {
      // okay, enough time has passed since the button was let go.
      digitalWrite(ledPin, LOW);
      ledOn=false;
    }
    if ((unsigned long)(currentMillis - buttonPushedMillis) >= 8000)
    {
      // okay, enough time has passed since the button was let go.
      digitalWrite(motorPin, LOW);
    }
  }
  if((rtc.isAlarm1() || rtc.isAlarm2()) && checkLastFed(buttonPushedMillis, currentMillis))
  {
	  feed();
    Serial.println("ALARM 1 TRIGGERED!");
    digitalWrite(ledPin, HIGH);
  }
}


void timeStamp()
{
  lcd.print(rtc.dateFormat("h:ia", t));
}

void feed()
{
  int speed = 255;
  analogWrite(motorPin, speed);
}

void checkAlarms()
{
  RTCAlarmTime a1;  
  RTCAlarmTime a2;

  if (rtc.isArmed1())
  {
    a1 = rtc.getAlarm1();

    Serial.print("Alarm1 is triggered ");
    switch (rtc.getAlarmType1())
    {
      case DS3231_EVERY_SECOND:
        Serial.println("every second");
        break;
      case DS3231_MATCH_S:
        Serial.print("when seconds match: ");
        Serial.println(rtc.dateFormat("__ __:__:s", a1));
        break;
      case DS3231_MATCH_M_S:
        Serial.print("when minutes and sencods match: ");
        Serial.println(rtc.dateFormat("__ __:i:s", a1));
        break;
      case DS3231_MATCH_H_M_S:
        Serial.print("when hours, minutes and seconds match: ");
        Serial.println(rtc.dateFormat("__ H:i:s", a1));
        break;
      case DS3231_MATCH_DT_H_M_S:
        Serial.print("when date, hours, minutes and seconds match: ");
        Serial.println(rtc.dateFormat("d H:i:s", a1));
        break;
      case DS3231_MATCH_DY_H_M_S:
        Serial.print("when day of week, hours, minutes and seconds match: ");
        Serial.println(rtc.dateFormat("l H:i:s", a1));
        break;
      default: 
        Serial.println("UNKNOWN RULE");
        break;
    }
  } else
  {
    Serial.println("Alarm1 is disarmed.");
  }

  if (rtc.isArmed2())
  {
    a2 = rtc.getAlarm2();

    Serial.print("Alarm2 is triggered ");
    switch (rtc.getAlarmType2())
    {
      case DS3231_EVERY_MINUTE:
        Serial.println("every minute");
        break;
      case DS3231_MATCH_M:
        Serial.print("when minutes match: ");
        Serial.println(rtc.dateFormat("__ __:i:s", a2));
        break;
      case DS3231_MATCH_H_M:
        Serial.print("when hours and minutes match:");
        Serial.println(rtc.dateFormat("__ H:i:s", a2));
        break;
      case DS3231_MATCH_DT_H_M:
        Serial.print("when date, hours and minutes match: ");
        Serial.println(rtc.dateFormat("d H:i:s", a2));
        break;
      case DS3231_MATCH_DY_H_M:
        Serial.println("when day of week, hours and minutes match: ");
        Serial.print(rtc.dateFormat("l H:i:s", a2));
        break;
      default: 
        Serial.println("UNKNOWN RULE"); 
        break;
    }
  } else
  {
    Serial.println("Alarm2 is disarmed.");
  }
}

bool checkLastFed(unsigned long actionMillis, unsigned long current)
{
	if(current - actionMillis <= 36000000)
		return 0;
	else
		return 1;
}

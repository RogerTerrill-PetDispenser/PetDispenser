/*
  LiquidCrystal Library - Autoscroll

 Demonstrates the use a 16x2 LCD display.  The LiquidCrystal
 library works with all LCD displays that are compatible with the
 Hitachi HD44780 driver. There are many of them out there, and you
 can usually tell them by the 16-pin interface.

 This sketch demonstrates the use of the autoscroll()
 and noAutoscroll() functions to make new text scroll or not.

 The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 11
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 * LCD R/W pin to ground
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)

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
unsigned long buttonPushedMillis; // when button was released
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
  /*
	if(now() == 1481583010) //if time is 10:50 pm
	{
		timeStamp();
		lcd.print(" (A)"); //automatic feed
	}
	if(now() == 1481583015) //if time is 10:50 pm
	{
		timeStamp();
		lcd.print(" (W)"); //web feed
	}
*/

  int button1State = digitalRead(button1Pin);  // variables to hold the pushbutton states
  int ledState = digitalRead(ledPin);
  if (button1State == LOW)
  {
    buttonPushedMillis = currentMillis;
    ledOn = true;
    digitalWrite(ledPin, HIGH);
    timeStamp();
    lcd.print(" (B)");
  }
  if (ledOn) 
  {
   //this is typical millis code here:
   if ((unsigned long)(currentMillis - buttonPushedMillis) >= 10000) 
   {
     // okay, enough time has passed since the button was let go.
     digitalWrite(ledPin, LOW);
     // setup our next "state"
     ledState = false;
   }
  }
}


void timeStamp()
{
  lcd.print(rtc.dateFormat("h:ia", t));
}

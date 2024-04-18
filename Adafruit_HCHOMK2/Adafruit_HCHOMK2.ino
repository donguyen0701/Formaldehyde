// Adafruit IO Publish & Subscribe, Digital Input and Output Example
//
// Adafruit invests time and resources providing this open source code.
// Please support Adafruit and open source hardware by purchasing
// products from Adafruit!
//
// Written by Todd Treece for Adafruit Industries
// Modified by Brent Rubell for Adafruit Industries
// Copyright (c) 2020 Adafruit Industries
// Licensed under the MIT license.
//
// All text above must be included in any redistribution.

/************************** Configuration ***********************************/

// edit the config.h tab and enter your Adafruit IO credentials
// and any additional configuration needed for WiFi, cellular,
// or ethernet clients.
#include "config.h"

/************************ Example Starts Here *******************************/
#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <RTClib.h>

#define TFT_DC   12  // A0
#define TFT_CS   13  // CS
#define TFT_MOSI 14  // SDA
#define TFT_CLK  27  // SCK
#define TFT_RST   0
#define TFT_MISO  0

RTC_DS3231 rtc; 
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// set up the 'analog_data' feed
AdafruitIO_Feed *analog_data = io.feed("data");

void printTwoDigits(int number);
void sendScreen(int analogValue);

void setup() {
  Serial.begin(115200);
  Serial.println(F("Hello, Welcome to the HCHO sensor demo"));
  //Screen preping
  tft.initR(INITR_BLACKTAB); 
  tft.fillScreen(ST7735_BLACK);
  
  pinMode(33, INPUT); // Set pin 33 as an input for analog reading
  Serial.println(F("Initialized test sensor"));
  
  // wait for serial monitor to open
  while(! Serial);
  Wire.begin();
  
  //Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));

  //Connect To AIO
  Serial.print("Connecting to Adafruit IO");

  // connect to io.adafruit.com
  io.connect();

  // wait for a connection
  while(io.status() < AIO_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  // we are connected
  Serial.println();
  Serial.println(io.statusText());
}

void loop() {
  int i = 0;
  int analogValue;
  while (i < 10){
    io.run();
    analogValue = analogRead(33); // Read analog value from pin 33
    sendScreen(analogValue);
    i += 1;
  }
  analog_data->save(analogValue);
}

void printTwoDigits(int number) {
  if (number < 10) {
    tft.print("0");
  }
  tft.print(number);
}
void sendScreen(int analogValue){
tft.fillScreen(ST7735_BLACK);
  tft.setCursor(10, 10);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Analog Value: ");
  tft.println(analogValue);

  DateTime now = rtc.now();
  tft.setCursor(10, 40);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(1);
  tft.print("Date: ");
  tft.print(now.year(), DEC);
  tft.print("/");
  printTwoDigits(now.month());
  tft.print("/");
  printTwoDigits(now.day());

  tft.setCursor(10, 70);
  tft.setTextColor(ST7735_BLUE);
  tft.setTextSize(1);
  tft.print("Time: ");
  printTwoDigits(now.hour());
  tft.print(":");
  printTwoDigits(now.minute());
  tft.print(":");
  printTwoDigits(now.second());
  delay(200);  
}

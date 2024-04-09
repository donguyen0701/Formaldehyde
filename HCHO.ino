#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <RTClib.h>

RTC_DS3231 rtc; 

#define TFT_DC   12  // A0
#define TFT_CS   13  // CS
#define TFT_MOSI 14  // SDA
#define TFT_CLK  27  // SCK
#define TFT_RST   0
#define TFT_MISO  0

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

void setup() {
  Serial.begin(115200);
  Serial.println(F("Hello, Welcome to the HCHO sensor demo"));
  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  pinMode(33, INPUT); // Set pin 33 as an input for analog reading
  Serial.println(F("Initialized test sensor"));
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  time = millis() - time;
  Serial.println(time / 1000, DEC);
  delay(500);
  Wire.begin();
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  tft.fillScreen(ST7735_BLACK);
  int analogValue = analogRead(33); // Read analog value from pin 33
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

  delay(1000); // Update every second
}

void printTwoDigits(int number) {
  if (number < 10) {
    tft.print("0");
  }
  tft.print(number);
}

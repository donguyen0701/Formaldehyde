#include <WiFi.h>
#include <WebServer.h>
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


const char* ssid = "Hoa";  // Enter SSID here
const char* password = "hoa12345";  //Enter Password here
WebServer server(80);

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);
void setup() {
  Serial.begin(115200);
  Serial.println(F("Hello, Welcome to the HCHO sensor demo"));
 
  tft.initR(INITR_BLACKTAB); //Screen Prep
  tft.fillScreen(ST7735_BLACK);
//////////////////////////////////////////// Connect Wifi and Initialize Server
  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP server started");

////////////////////////////////////////////////////////////////////////////////////
   
  pinMode(33, INPUT); // Set pin 33 as an input for analog reading
  Serial.println(F("Initialized test sensor"));
  uint16_t time = millis();
  tft.fillScreen(ST7735_BLACK);
  delay(500);
  Wire.begin();

/////////////////////////////// Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));
}

void loop() {
  tft.fillScreen(ST7735_BLACK);
  int analogValue = analogRead(33); // Read analog value from pin 33
  server.handleClient();
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
void handle_OnConnect() {
  int analogValue = analogRead(33);
  server.send(200, "text/html", SendHTML(analogValue));
}
void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(float analog_val){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP32 Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px;color: #444444;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>HCHO RMMS</h1>\n";
  
  ptr +="<p>Analog Value: ";
  ptr +=(int)analog_val;
  ptr +=" Unit</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

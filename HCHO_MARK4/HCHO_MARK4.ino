#include <SD.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <ThingSpeak.h>
#include <WiFi.h>
#include <Wire.h>
#include "RTClib.h"
#include <Arduino.h>
#include <SensirionI2CSfa3x.h>

SensirionI2CSfa3x sfa3x;
/*--------------------------------------*/
RTC_DS3231 rtc;
WiFiClient  client;
File myFile;
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
#define PIN_SPI_CS 5 
const unsigned long WIFI_TIMEOUT_MS = 20000;
unsigned int WIFI_flag = 0;
unsigned int BATT_flag = 0;
const char* ssid = "Hoa";   // your Wifi's SSID (name) 
const char* password = "hoa12345";   // your Wifi's password
unsigned long myChannelNumber = 1; // Thingspeak's
const char * myWriteAPIKey = "LYFK762J5FKILTPU"  ; // Thinkspeak's

float senSignal = 0.0;
const int senPin = 33;
int first_init = 1;
int16_t hcho;
int16_t humidity;
int16_t temperature;
/*------------X bit map for display_START_--------------------------------*/
// ' Battery Capa', 3x6px
const unsigned char epd_bitmap__Battery_Capa [] U8X8_PROGMEM = {
  0x07, 0x07, 0x07, 0x07, 0x07, 0x07
};
// ' Battery_Emp', 21x10px

const unsigned char epd_bitmap__Battery_Emp [] U8X8_PROGMEM = {
  0xfc, 0xff, 0x1f, 0x04, 0x00, 0x10, 0x07, 0x00, 0x10, 0x07, 0x00, 0x10, 0x07, 0x00, 0x10, 0x07, 
  0x00, 0x10, 0x07, 0x00, 0x10, 0x07, 0x00, 0x10, 0x04, 0x00, 0x10, 0xfc, 0xff, 0x1f
};
// ' charge', 7x11px
const unsigned char epd_bitmap__charge [] U8X8_PROGMEM = {
  0x60, 0x70, 0x38, 0x1c, 0x1e, 0x3f, 0x38, 0x1c, 0x0e, 0x07, 0x03
};
// ' Excalm', 2x14px
const unsigned char epd_bitmap__exclam [] U8X8_PROGMEM = {
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x0
};
// ' wifi', 14x11px
const unsigned char epd_bitmap__wifi [] U8X8_PROGMEM = {
  0xff, 0x3f, 0xff, 0x3f, 0x00, 0x00, 0xfc, 0x0f, 0xfc, 0x0f, 0x00, 0x00, 0xf0, 0x03, 0xf0, 0x03, 
  0x00, 0x00, 0xc0, 0x00, 0xc0, 0x0
};

/*------------X bit map for display_END_--------------------------------*/

void wifiRenew(void * parameters) {
    for ( ; ; ){
        Serial.println(xPortGetCoreID());
        if (WiFi.status() == WL_CONNECTED){
            Serial.print("Wifi is still connected"); 
            WIFI_flag = 1;
            int x = ThingSpeak.writeField(myChannelNumber, 1, senSignal, myWriteAPIKey);
            if(x == 200){
                Serial.println("Channel update successful.");
            }
            else{
                Serial.println("Problem updating channel. HTTP error code " + String(x));
            }
            vTaskDelay(15000/ portTICK_PERIOD_MS);
            continue;   
        }
        else{
            Serial.println("Trying to connect to Wifi");\
            WiFi.begin(ssid, password);
            WIFI_flag = 0;
            unsigned long WIFI_ATTEMPT_TIME = millis();

            while (WiFi.status() != WL_CONNECTED && millis() - WIFI_ATTEMPT_TIME < WIFI_TIMEOUT_MS){
                Serial.print(" . ");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            if (WiFi.status() != WL_CONNECTED){
                Serial.println("!WIFI! FAILED TO CONNECT");
                Serial.println("!WIFI! RETRYING IN 20s");
                vTaskDelay(20000/ portTICK_PERIOD_MS);
                continue;
            }
            else{
                Serial.print("!WIFI! CONNECTED: ");
                Serial.println(WiFi.localIP());
            }
        }
    } 
}

void runner(void * parameters){
    for ( ; ; ){  
        if (first_init == 1){
            while (!rtc.begin()) {
                Serial.println(F("RTC module is NOT found"));
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            Wire.begin(21, 22);   
            u8g2.begin();
            Serial.print("Screen Innitialized");
            uint16_t error;
            char errorMessage[256];
            sfa3x.begin(Wire);
            error = sfa3x.startContinuousMeasurement();
            if (error) {
                Serial.print("Error trying to execute startContinuousMeasurement(): ");
                errorToString(error, errorMessage, 256);
                Serial.println(errorMessage);
            }
            
            while (!SD.begin(PIN_SPI_CS)) {
                Serial.println(F("SD CARD FAILED, OR NOT PRESENT!"));
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            Serial.println(F("SD CARD INITIALIZED."));
            Serial.println(F("--------------------"));
            first_init --;
        }
        u8g2.clearBuffer();
        u8g2_prepare();
        bitmap_battery_emp();
        if (BATT_flag == 1){
            bitmap_battery_charge();    
        }
        if (WIFI_flag == 0){
            bitmap_exclam();
        }
        bitmap_wifi();
        batt_capacity(100);  
        
        Serial.print("                                         ");
        Serial.println(xPortGetCoreID());
        uint16_t error;
        char errorMessage[256];

        error = sfa3x.readMeasuredValues(hcho, humidity, temperature);
        if (error) {
            Serial.print("Error trying to execute readMeasuredValues(): ");
            errorToString(error, errorMessage, 256);
            Serial.println(errorMessage);
            senSignal = 6969.0;
        } else {
            senSignal = hcho / 5.0;
            Serial.print("Hcho:");
            Serial.print(senSignal);
            Serial.print("\t");
            Serial.print("Humidity:");
            Serial.print(humidity / 100.0);
            Serial.print("\t");
            Serial.print("Temperature:");
            Serial.println(temperature / 200.0);
        }
        
        show_value(senSignal);
        writeSD(senSignal);
        show_date();
        printTime(); 
        u8g2.sendBuffer();
        Serial.print("                               ANALOG VALUE:");
        Serial.println(senSignal);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}



void setup(){
    Serial.begin(115200);
    xTaskCreatePinnedToCore(
    wifiRenew,
    "Renew Wifi",
    10000,
    NULL,
    1,
    NULL,
    CONFIG_ARDUINO_RUNNING_CORE
    );
    delay(5000);
    
    xTaskCreatePinnedToCore(
    runner,
    "Runner",
    20000,
    NULL,
    1,
    NULL,
    0
    );
    
    WiFi.mode(WIFI_STA);   
    ThingSpeak.begin(client);
    
} 

void loop(){
}

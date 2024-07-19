void printTwoDigits(int number) {
    if (number < 10) {
        Serial.print("0");
    }
    Serial.print(number);
}
void printTime(){
    DateTime now = rtc.now();
    Serial.print("ESP32 RTC Date Time: ");
    printTwoDigits(now.year());
    Serial.print('/');
    printTwoDigits(now.month());
    Serial.print('/');
    printTwoDigits(now.day());
    printTwoDigits(now.hour());
    Serial.print(':');
    printTwoDigits(now.minute());
    Serial.print(':');
    printTwoDigits(now.second());
    Serial.println("");
    
    
    
}
    

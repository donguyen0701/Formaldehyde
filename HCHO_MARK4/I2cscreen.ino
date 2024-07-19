void TwoDigits(int number) {
    if (number < 10) {
        u8g2.print("0");
    }
    u8g2.print(number);
}
void bitmap_battery_emp(void) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawXBMP(104, 4, 21, 10, epd_bitmap__Battery_Emp );
  }
void bitmap_battery_charge(void) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawXBMP(95, 4, 7, 11, epd_bitmap__charge);
}
void bitmap_wifi(void) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawXBMP(110, 47, 14, 11, epd_bitmap__wifi);
}
void bitmap_exclam(void) {
    u8g2.setBitmapMode(false /* solid */);
    u8g2.drawXBMP(107, 45, 2, 14, epd_bitmap__exclam);
}
void batt_capacity(float volt){
    u8g2.setBitmapMode(true/* transparent */);
    if (volt > 25){
       u8g2.drawXBMP(120, 6, 3, 6, epd_bitmap__Battery_Capa);
    }
    if (volt > 40){
       u8g2.drawXBMP(116, 6, 3, 6, epd_bitmap__Battery_Capa);
    }
    if (volt > 65){
       u8g2.drawXBMP(112, 6, 3, 6, epd_bitmap__Battery_Capa);
    }
    if (volt > 85){
       u8g2.drawXBMP(108, 6, 3, 6, epd_bitmap__Battery_Capa);
    }
}
void u8g2_prepare(void) {
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setFontRefHeightExtendedText();
    u8g2.setDrawColor(1);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
}
void show_date(void){
    DateTime now = rtc.now();
    u8g2.setFont(u8g2_font_6x10_tf);
    u8g2.setCursor(1, 2);
    TwoDigits(now.day());
    u8g2.print("/");
    TwoDigits(now.month());
    u8g2.print("/");
    TwoDigits(now.year());
    u8g2.setCursor(1, 10);
    TwoDigits(now.hour());
    u8g2.print(":");
    TwoDigits(now.minute());
    u8g2.print(":");
    TwoDigits(now.second());
}
void show_value(float value){
    u8g2.setCursor(5, 25);
    u8g2.setFont(u8g2_font_crox5h_tf );
    u8g2.print(value);
    u8g2.print("ppb");
    }

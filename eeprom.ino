#include <EEPROM.h>

void setupEeprom(){
  EEPROM.begin(512);
  
  byte r = EEPROM.read(0);
  byte g = EEPROM.read(1);
  byte b = EEPROM.read(2);
  color = strip.Color(r, g, b);
  
  Serial.printf("Load LED color from EEPROM: rgb[%d,%d,%d]\n", r, g, b);
}

void storeColor(int r, int g, int b){
  if (r < 0 || r > 255 || g < 0 || g > 255 || b <0 || b > 255)
    return;
  EEPROM.write(0, r);
  EEPROM.write(1, g);
  EEPROM.write(2, b);
  EEPROM.commit();
}


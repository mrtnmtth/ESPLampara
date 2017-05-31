#include <EEPROM.h>

void setupEeprom(){
  EEPROM.begin(512);

  Serial.println("Read configuration from EEPROM:");
  
  byte r = EEPROM.read(0);
  byte g = EEPROM.read(1);
  byte b = EEPROM.read(2);
  color = strip.Color(r, g, b);
  Serial.printf("- LED color: rgb[%d,%d,%d]\n", r, g, b);

  String hostname = "";
  for (int i = 32; i < 64; i++){
    hostname += char(EEPROM.read(i));
  }
  sprintf(hostString, "%s", hostname.c_str());
  Serial.printf("- Hostname: %s\n", hostname.c_str());

  String essid = "";
  for (int i = 64; i < 96; i++){
    essid += char(EEPROM.read(i));
  }
  sprintf(ssid, "%s", essid.c_str());
  Serial.printf("- Wifi-SSID: %s\n", essid.c_str());
  
  String epassword = "";
  for (int i = 96; i < 160; i++){
    epassword += char(EEPROM.read(i));
  }
  sprintf(password, "%s", epassword.c_str());
  Serial.printf("- Wifi-Password: ****\n");
}

void storeColor(int r, int g, int b){
  if (r < 0 || r > 255 || g < 0 || g > 255 || b <0 || b > 255)
    return;
  EEPROM.write(0, r);
  EEPROM.write(1, g);
  EEPROM.write(2, b);
  EEPROM.commit();
}

void storeHostname(String hostname){
  if (hostname.length() < 0)
    return;
  for (int i = 32; i < 64; i++){
    EEPROM.write(i, 0);
  }
  for (int i = 0; i < hostname.length(); i++){
    EEPROM.write(32 + i, hostname[i]);
  }
  EEPROM.commit();
}

void storeWifi(String ssid, String password){
  // SSID: 32bit
  // Password: 64bit
  if (ssid.length() < 0 || password.length() < 0)
    return;
  for (int i = 64; i < 160; i++){
    EEPROM.write(i, 0);
  }
  for (int i = 0; i < ssid.length(); i++){
    EEPROM.write(64 + i, ssid[i]);
  }
  for (int i = 0; i < password.length(); i++){
    EEPROM.write(96 + i, password[i]);
  }
  EEPROM.commit();
}


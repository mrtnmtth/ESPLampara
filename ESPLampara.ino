#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>
#include <FS.h>

ESP8266WebServer server(80);

const int LED_PIN = D4;
const int LED_COUNT = 24;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t color = strip.Color(127, 127, 127);

char ssid[32] = {0};
char password[64] = {0};
char hostString[32] = {0};

void handleRoot() {
  if(SPIFFS.exists("/index.html")){
    File file = SPIFFS.open("/index.html", "r");
    String message = file.readString();
    message.replace("{{hostname}}", hostString);
    server.send(200, "text/html", message);
    file.close();
  }
  else handleNotFound();
}

void setColor() {
  String message = "";
  int r, g, b;
  for (uint8_t i=0; i<server.args(); i++) {
    String arg = server.argName(i);
    int val = server.arg(i).toInt();
    if ((val >= 0) && (val <= 255)) {
      if (arg == "r") {
        r = val;
      }
      if (arg == "g") {
        g = val;
      }
      if (arg == "b") {
        b = val;
      }
    }
  }
  color = strip.Color(r, g, b);
  Serial.printf("Color changed to rgb[%d,%d,%d]\n", r, g, b);
  server.send(200, "text/plain", message);
  storeColor(r, g, b);
  uniColor();
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void uniColor() {
  // Simulate fading by changing pixel color one after another
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(10);
  }
}

void setup(void){
  //pinMode(LED_BUILTIN, OUTPUT);
  //digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  Serial.println("");

  setupEeprom();

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  uniColor();
  Serial.println("LEDs initialized");

  // set standard hostname if not loaded from EEPROM
  if (!hostString[0]){
    sprintf(hostString, "LAMPARA_%06X", ESP.getChipId());
  }
  WiFi.hostname(hostString);
  WiFi.begin(ssid, password);
  Serial.println("Connecting Wifi");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  setupFS();
  
  server.on("/", handleRoot);
  server.on("/color", setColor);
  server.serveStatic("/iro.js", SPIFFS, "/iro.js");
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  // Interferes with first RGB-LED
  //digitalWrite(LED_BUILTIN, HIGH);
  server.handleClient();
}

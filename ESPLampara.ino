#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Adafruit_NeoPixel.h>

const char* ssid = "xxx";
const char* password = "xxx";

ESP8266WebServer server(80);

const int LED_PIN = D4;
const int LED_COUNT = 24;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
uint32_t color = strip.Color(127, 127, 127);

char hostString[16] = {0};

void handleRoot() {
  String message = "<html><title>" + String(hostString) + "</title><body><h2>hello from esp8266!</h2>";
  message += "<div id=\"picker\"></div>";
  message += "<script src=\"https://cdn.rawgit.com/jaames/iro.js/master/dist/iro.js\"></script>";
  message += "<script type=\"text/javascript\">var size = Math.min(window.innerWidth, window.innerHeight) *0.9; var colorPicker = new iro.ColorPicker(\"#picker\",{width: size,height: size,markerRadius: 8,css: {\"body\":{\"background-color\": \"rgb\"}}});</script>";
  message += "<script type=\"text/javascript\">document.getElementById(\"picker\").addEventListener(\"click\", setColor);document.getElementById(\"picker\").addEventListener(\"touchend\", setColor);";
  message += "function setColor() {var rgb = colorPicker.color.rgb;var req = new XMLHttpRequest(); req.open(\"GET\", \"/color?r=\"+rgb.r+\"&g=\"+rgb.g+\"&b=\"+rgb.b, true); req.send();}</script>";
  message += "</body></html>";
  server.send(200, "text/html", message);
}

void setColor() {
  char message[16] = {0};
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
  Serial.println("LEDs initialized");

  sprintf(hostString, "LAMPARA_%06X", ESP.getChipId());
  WiFi.hostname(hostString);
  WiFi.begin(ssid, password);
  Serial.println("");

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

  server.on("/", handleRoot);
  server.on("/color", setColor);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  uniColor();
}

void loop(void){
  // Interferes with first RGB-LED
  //digitalWrite(LED_BUILTIN, HIGH);
  server.handleClient();
}

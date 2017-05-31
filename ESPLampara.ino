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
int color_r = 127;
int color_g = 127;
int color_b = 127;
uint32_t color = strip.Color(color_r, color_g, color_b);

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
  String message = "";
  for (uint8_t i=0; i<server.args(); i++) {
    String arg = server.argName(i);
    int val = server.arg(i).toInt();
    if ((val >= 0) && (val <= 255)) {
      if (arg == "r") {
        color_r = val;
      }
      if (arg == "g") {
        color_g = val;
      }
      if (arg == "b") {
        color_b = val;
      }
    }
  }
  color = strip.Color(color_r, color_g, color_b);
  message += "R:" + String(color_r) + " G:" + String(color_g) + " B:" + String(color_b);
  server.send(200, "text/plain", message);
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
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);

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

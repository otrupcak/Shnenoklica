#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN     14
#define LED_COUNT  8

String r,g,b,s;
int r_pos,g_pos,b_pos,s_pos;
String buttonValue;
const char* PARAM_INPUT = "value";

int BRIGHTNESS;

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char* ssid     = "Naziv_wifi";
const char* password = "Sifra";

AsyncWebServer server(80);

const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };


void colorWipe(int c_r, int c_g, int c_b, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { 
    strip.setPixelColor(
      i,
      pgm_read_byte_inlined(&gamma8[c_r]),
      pgm_read_byte_inlined(&gamma8[c_g]),
      pgm_read_byte_inlined(&gamma8[c_b]));

    strip.show();
    delay(wait);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Dobrodošla šnenoklice! \r\n Srećno igranje!");

  if(!LittleFS.begin()){
    Serial.println("Something went wrong with LittleFS");
    return;
  }

  File html = LittleFS.open("/index.html","r");
  File css = LittleFS.open("/styles.css","r");

  if (html){
    Serial.println("HTML uploaded");
  }

  if (css){
    Serial.println("CSS uploaded");
  }

  //Starting soft AP
  WiFi.softAP(ssid,password);

  Serial.println(WiFi.softAPIP());

    // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", String(), false);
  });
  
  // Route to load style.css file
  server.on("/styles.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/styles.css", "text/css");
  });

  server.on("/button", HTTP_GET, [](AsyncWebServerRequest *request){
    String inputMessage;

    if (request->hasParam(PARAM_INPUT)) {
      inputMessage = request->getParam(PARAM_INPUT)->value();
      buttonValue = inputMessage;
      
      r_pos = buttonValue.indexOf("R")+1;
      g_pos = buttonValue.indexOf("G")+1;
      b_pos = buttonValue.indexOf("B")+1;
      s_pos = buttonValue.indexOf("S")+1;

      r = buttonValue.substring(r_pos,g_pos-1);
      g = buttonValue.substring(g_pos,b_pos-1);
      b = buttonValue.substring(b_pos,s_pos-1);
      s = buttonValue.substring(s_pos);

      BRIGHTNESS = s.toInt();

      //provera
      Serial.println("RED: " + r + "\n" + "GREEN: " + g + "\n" + "BLUE: " + b + "\n" + "BRIGHTNESS: " + s + "\n");
      Serial.println(r_pos);
      Serial.println(g_pos);
      Serial.println(b_pos);
      Serial.println(s_pos);

      colorWipe(r.toInt(),g.toInt(),b.toInt(), 0);
      strip.setBrightness(BRIGHTNESS);
    }
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  strip.begin();
  strip.show();
}

void loop() {
}


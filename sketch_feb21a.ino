#ifdef ESP32
  #include <WiFi.h>
  #include <ESPAsyncWebServer.h>
  #include <SPIFFS.h>
#else
  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
#endif
#include <DHT.h> // Include the DHT library

#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE); // Initialize DHT sensor

// Replace with your network credentials
const char* ssid = "Tele2_fe196d";
const char* password = "u2y4mtze";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

float temp = 0; // Variable to store temperature
float hum = 0;  // Variable to store humidity

void readSensorValues() {
  // Read temperature and humidity from DHT sensor
  temp = dht.readTemperature();
  hum = dht.readHumidity();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();

  // Initialize SPIFFS
  if(!SPIFFS.begin()){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(temp).c_str());
  });
  server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(hum).c_str());
  });

  // Start server
  server.begin();
}

void loop(){
  // Read sensor values every 2 seconds (adjust as needed)
  readSensorValues();
  delay(2000);
}

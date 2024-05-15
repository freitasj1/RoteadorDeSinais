#include <WiFi.h>
#include <WiFiAP.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>

const char *ssid = "TELECO_2k24"; 
const char *password = "analelealan";

AsyncWebServer server(8081);

String latitude;
String longitude;
String tipoLocal;

void setup() {
  Serial.begin(115200);

  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", "<form method='post' action='/submit'>"
                  "Latitude: <input type='text' name='latitude'><br>"
                  "Longitude: <input type='text' name='longitude'><br>"
                  "Tipo de Local: <input type='text' name='tipo-local'><br>"
                  "<input type='submit' value='Enviar'>"
                  "</form>");
  });

  
  server.on("/submit", HTTP_POST, [](AsyncWebServerRequest * request) {
   
    latitude = request->arg("latitude");
    longitude = request->arg("longitude");
    tipoLocal = request->arg("tipo-local");

    
    request->send(200, "text/plain", "Dados recebidos com sucesso!");

    Serial.println("Latitude: " + latitude);
    Serial.println("Longitude: " + longitude);
    Serial.println("Tipo de Local: " + tipoLocal);
  });

  
  server.begin();
}

void loop() {
  
}

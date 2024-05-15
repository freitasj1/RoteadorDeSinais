#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebServer.h>

#define TESTE

const char *ssid = "TELECO_2k24";
const char *password = "analelealan";
const String hostClima = "api.open-meteo.com";
const String hostRouter = "192.168.100.122";
AsyncWebServer server(8081);
WiFiClient client;


String latitude;
String longitude;
String tipoLocal;
float rain;
int local;
float custos[10];
float calcRouter[10];
int estado = 3;
float linkOptico1;
float linkOptico2;
float linkRadio1;
float linkRadio2;


// barrabarra

//endereçamento pinESP/pinMT8816
#define AX0 32//5
#define AX1 33//22
#define AX2 25//23
#define AY0 26//24
#define AY1 27//25
//os demais pinos de endereço deven ser ligado ao GND

//controles pinESP/pinMT8816
#define STROBE 22//18
#define DATA 19//38
#define RESET 18//3

//WANs, LANs e BROADCAST
//pinaddress/codebin/pinname/pinMT8816
int BROADCAST[3] = {0, 0, 0}; //X0 33

int X1[3] = {1, 0, 0}; //X1 32
int X2[3] = {0, 1, 0}; //X2 31
int X3[3] = {1, 1, 0}; //X3 30
int X4[3] = {0, 0, 1}; //X4 29

int L1[2] = {0, 0}; //Y0 35
int L2[2] = {1, 0}; //Y1 37
int L3[2] = {0, 1}; //Y2 39
int L4[2] = {1, 1}; //Y3 1

int deleio = 12;

//prototipagem das funçãoes
void setaddress(int x[], int y[]);
void resetaddress();
void conect(int wan[], int lan[]);
void desconect(int wan[], int lan[]);
void resetmatriz();
void Broadcast();


void setup() {

  Serial.begin(115200);
  delay(10);

  Serial.println();
  Serial.println();
  //Serial.print("Connecting to: ");
  //erial.println(ssid);

  WiFi.disconnect();
  delay(1000);

  WiFi.begin(ssid, password);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("Ip address: ");
  Serial.println(WiFi.localIP());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(200, "text/html", "<form method='post' action='/submit'>"
                  "Latitude: <input type='text' name='latitude'><br>"
                  "Longitude: <input type='text' name='longitude'><br>"
                  "Tipo de Local: externo ou interno: <input type='text' name='tipo-local'><br>"
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


  pinMode(AX0, OUTPUT);
  pinMode(AX1, OUTPUT);
  pinMode(AX2, OUTPUT);
  pinMode(AY0, OUTPUT);
  pinMode(AY1, OUTPUT);

  digitalWrite(AX0, LOW);
  digitalWrite(AX1, LOW);
  digitalWrite(AX2, LOW);
  digitalWrite(AY0, LOW);
  digitalWrite(AY1, LOW);

  pinMode(STROBE, OUTPUT);
  pinMode(DATA, OUTPUT);
  pinMode(RESET, OUTPUT);

  digitalWrite(STROBE, LOW);
  digitalWrite(DATA, LOW);
  digitalWrite(RESET, LOW);
  resetmatriz();
}


        const size_t bufferSize = JSON_OBJECT_SIZE(8) + 200;
        DynamicJsonDocument doc(bufferSize);

void loop() {


  if (tipoLocal == "externo" || tipoLocal == "Externo") {

    const int httpPort = 80;
    if (!client.connect("api.open-meteo.com", 80)) {
      Serial.println("connection at open meteo failed");
      return;
    }

    String msg = "GET /v1/forecast?";
    msg = msg + "latitude=" + String(latitude) + "&";
    msg = msg + "longitude=" + String(longitude) + "&";
    msg = msg + "current=rain&";
    msg = msg + "timezone=auto";
    msg = msg + " HTTP/1.1\r\n";
    Serial.print(msg);

    client.print(msg);
    client.print("Host: api.open-meteo.com\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n\r\n");


    unsigned long timeout = millis();

    while (client.available() == 0) {
      if (millis() - timeout > 10000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }

    while (client.available()) {
      String json = client.readStringUntil('\r');
      if (json.indexOf("{") >= 0 && json.indexOf("}") >= 0)
      {


        // JSON input string.
        Serial.print("json="); Serial.println(json);
        deserializeJson(doc, json);


        // Obtém o valor do parâmetro "chuva"
        rain = doc["current"]["rain"];

        // Exibe o valor obtido
        Serial.print("Chuva atual (mm): ");
        Serial.println(rain);
      }
    }

    client.stop();
    delay(200);


    if (!client.connect("192.168.100.177", 8080)) {
      Serial.println("connection at router api failed");
      return;
    }


    String message = "GET /";
    message = message + " HTTP/1.1\r\n";

    client.print(message);
    client.print("Host: 192.168.100.177\r\n");
    client.print("Connection: close\r\n");
    client.print("\r\n\r\n");



    while (client.available() == 0) {
      if (millis() - timeout > 10000) {
        Serial.println(">>> Client Timeout !");
        client.stop();
        return;
      }
    }
    while (client.available()) {
      String line = client.readStringUntil('\r');
      if (line.indexOf('{') >= 0 && line.indexOf("}") >= 0) {

        //const size_t bufferSize = JSON_OBJECT_SIZE(10) + 10;
        //DynamicJsonDocument doc(bufferSize);
 #ifdef TESTE
        Serial.print("line=");Serial.println(line);
 #endif
        deserializeJson(doc, line);
        Serial.print("3:");
        Serial.println(rain);
        custos[0] = doc["r1"];
        custos[1] = doc["r2"];
        custos[2] = doc["r3"];
        custos[3] = doc["r4"];
        custos[4] = doc["r5"];
        custos[5] = doc["r6"];
        custos[6] = doc["r7"];
        custos[7] = doc["r8"];
        custos[8] = doc["r9"];
        custos[9] = doc["r10"];
        Serial.print("4:");
Serial.println(rain); 
        calcRouter[0] = 1 / (custos[0] + 0.01);
        calcRouter[1] = 0.75 / (custos[1] + 0.01);
        calcRouter[2] = 0.1 / (custos[2] + 0.01);
        calcRouter[3] = 0.12 / (custos[3] + 0.01);
        calcRouter[4] = 0.8 / (custos[4] + 0.01);
        calcRouter[5] = 0.15 / (custos[5] + 0.01);
        calcRouter[6] = 0.15 / (custos[6] + 0.01);
        calcRouter[7] = 1.5 / (custos[7] + 0.01);
        calcRouter[8] = 1.2 / (custos[8] + 0.01);
        calcRouter[9] = 0.08 / (custos[9] + 0.01);

        
        linkOptico1 = calcRouter[0] + calcRouter[4] + calcRouter[7];
        linkOptico2 = calcRouter[1] + calcRouter[8];
        linkRadio1 = calcRouter[2] + calcRouter[5] + calcRouter[10];
        linkRadio2 = calcRouter[3] + calcRouter[6];

       // Serial.println("local: " + tipoLocal);
    Serial.print("6:");
        Serial.println(rain);
        
        if (rain > 0) {
          if (linkOptico1 > linkOptico2) {
            resetmatriz();
            conect(X1, L1);
            Serial.println("Rota óptica 1 escolhida");
          }
          else if (linkOptico2 > linkOptico1) {
            resetmatriz();
            conect(X2, L1);
            Serial.println("Rota óptica 2 escolhida");
          }
        }
        else if (rain == 0) {
          if (linkRadio1 > linkRadio2) {
            resetmatriz();
            conect(X4, L1);
            Serial.println("Rota de rádio 2 escolhida");

          }
          else if (linkRadio2 > linkRadio1) {
            resetmatriz();
            conect(X3, L1);
            Serial.print("Rota de rádio 1 escolhida");
          }
          
        }
        delay(10000);
        Serial.print(rain);
        client.stop();
      }
    }


    if (estado == 0 || estado == 2) {
      resetmatriz();
      estado = 1;
    }
  }
  else if (tipoLocal == "interno" || tipoLocal == "Interno") {
    Broadcast();
    if (estado == 0 || estado == 1) {
      resetmatriz();
      estado = 2;
      Serial.println("local: " + tipoLocal);
      Serial.println("Broadcast");
    }
  }


  else if (tipoLocal == "")
  {
    if (estado == 3) {
      resetmatriz();
      estado = 0;

      Serial.println("Rota óptica 1 definida como padrão");
      conect(X1, L1);
      Serial.println("Insira seu local");
    }

  }

}


void setaddress(int x[], int y[])
{
  digitalWrite(AX0, x[0]);
  digitalWrite(AX1, x[1]);
  digitalWrite(AX2, x[2]);
  digitalWrite(AY0, y[0]);
  digitalWrite(AY1, y[1]);
}

void resetaddress()
{
  digitalWrite(AX0, LOW);
  digitalWrite(AX1, LOW);
  digitalWrite(AX2, LOW);
  digitalWrite(AY0, LOW);
  digitalWrite(AY1, LOW);
}

void conect(int wan[], int lan[])
{
  setaddress(wan, lan);
  digitalWrite(DATA, HIGH);
  //delay(deleio);
  digitalWrite(STROBE, HIGH);
  delay(deleio);
  digitalWrite(STROBE, LOW);
  //delay(deleio);
  resetaddress();
  digitalWrite(DATA, LOW);
}

void desconect(int wan[], int lan[])
{
  setaddress(wan, lan);
  digitalWrite(DATA, LOW);
  //delay(deleio);
  digitalWrite(STROBE, HIGH);
  delay(deleio);
  digitalWrite(STROBE, LOW);
  //delay(deleio);
  resetaddress();
}

void resetmatriz()
{
  digitalWrite(RESET, HIGH);
  //delay(deleio);
  digitalWrite(RESET, LOW);
}

void Broadcast()
{
  conect(BROADCAST, L1);
  conect(BROADCAST, L2);
  conect(BROADCAST, L3);
  conect(BROADCAST, L4);
}

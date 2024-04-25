#include <WiFi.h>
#include <ESPAsyncWebServer.h>

const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {

    case WS_EVT_CONNECT:
        Serial.println("Client connecté");
        server->cleanupClients(1);  // Gardez 1 client, le nouveau
        break;
    case WS_EVT_DISCONNECT:
        Serial.println("Client deconnecté");
        break;
    case WS_EVT_DATA:
      // Imprime directement les données reçues
      Serial.write(data, len);
      // Assurez-vous d'ajouter une nouvelle ligne si nécessaire
      Serial.println();
      break;
  }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("Hotspot WiFi démarré");
    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();
}

void loop() {


}
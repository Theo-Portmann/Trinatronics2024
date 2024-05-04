#include <WiFi.h>
#include <ESPAsyncWebServer.h>

//code qui cree un serveur websocket sur l'ESP32 
//modifie letat de la LED en fonction de la variable globale ledValue
//LedValue est actualisee a chaque WS_EVT_DATA avc la valeur de uint8_t *data convertie en int

//04/05/24


const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";

// Définir la variable globale pour stocker la valeur pour la LED
int ledValue = 0;
const int ledPin = 1;  // Assurez-vous que c'est le bon pin pour votre ESP32

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
            // Conversion des données en int et mise à jour de la variable globale
            data[len] = '\0'; // Assurez-vous que la chaîne est terminée proprement pour conversion
            ledValue = atoi((char*)data);  // Convertit les données en integer
            // Renvoyer les données au client pour confirmation
            break;
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.softAP(ssid, password);
    Serial.println("Hotspot WiFi démarré");

    Serial.print("IP Address: ");
    Serial.println(WiFi.softAPIP());

    pinMode(ledPin, OUTPUT);  // Configure le pin de la LED comme sortie

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
    server.begin();
}

void loop() {
    // Contrôlez la LED basée sur la valeur de ledValue
    if (ledValue > 150) {
        digitalWrite(ledPin, HIGH);  // Allume la LED
    } else if (ledValue < 50) {
        digitalWrite(ledPin, LOW);  // Éteint la LED
    }
}

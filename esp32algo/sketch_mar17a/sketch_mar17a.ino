#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";

int in1Pin = 12;      // Define L293D channel 1 pin
int in2Pin = 14;      // Define L293D channel 2 pin
int enable1Pin = 13;  // Define L293D enable 1 pin
int channel = 0;
boolean rotationDir;  // Define a variable to save the motor's rotation direction
int rotationSpeed;    // Define a variable to save the motor rotation speed
int DataWifi;

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
    case WS_EVT_CONNECT:
      Serial.println("Client connecté");
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("Client déconnecté");
      break;
    case WS_EVT_DATA:
if (len > 0 && len < sizeof(DataWifi)) 
  {
  // Assurez-vous que la chaîne est terminée correctement pour la conversion
    char tempStr[sizeof(DataWifi) + 1]; // +1 pour le caractère de fin '\0'
    memcpy(tempStr, data, len);
    tempStr[len] = '\0'; // Ajoute le caractère de fin de chaîne
  
  // Conversion de la chaîne de caractères en entier
    DataWifi = atoi(tempStr);
    } 
    else {
      Serial.println("Erreur : la taille des données reçues est trop grande.");
    }
    Serial.println(DataWifi);
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
  pinMode(in1Pin, OUTPUT);
  pinMode(in2Pin, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  
  ledcSetup(channel,1000,11);         //Set PWM to 11 bits, range is 0-2047
  ledcAttachPin(enable1Pin,channel);
}

void loop() {
  //Calculate the rotor speed, range of 0 to 2048
  if (DataWifi > 0)
  {
    rotationSpeed = DataWifi*10;
  }
  else
  {
    rotationSpeed = 0;
  }

  rotationSpeed = abs(rotationSpeed);
  rotationDir = false;
  
  //Control the steering and speed of the motor
  driveMotor(rotationDir, constrain(rotationSpeed,0,2048));
}

void driveMotor(boolean dir, int spd) {
  // Control motor rotation direction
  if (dir) {
    digitalWrite(in1Pin, HIGH);
    digitalWrite(in2Pin, LOW);
  }
  else {
    digitalWrite(in1Pin, LOW);
    digitalWrite(in2Pin, HIGH);
  }
  // Control motor rotation speed
  ledcWrite(channel, spd);
}
/**********************************************************************
  Filename    : Thomas_MotorControllWithPoti.ino
  Description : Use PWM to set the speed of motor, using Mosfet
  Auther      : Thomas Eyer, Johan Maring
  Création    : 04/02/2024
  Modification: V1 06/05/2024 
            
  Description : Crée un serveur websocket sur l'ESP32, et commande 
                un moteur par l'intermédiare d'un mosfet. Implémentation 
                pour commande du deuxième moteur déjà commencer, mais 
                reste encore à décommenter certaine partie pour faire 
                fonctionner avec 2 moteur.
**********************************************************************/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// set wifi communication access 
const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Define of globale varibale
int32_t u32JoystickValue = 0;    // take the websocket give-value

// Pin definition 
uint32_t u32Motor1 = 2;         // Define the pin to commande motor 1
uint32_t u32Motor2 = 4;         // Define the pin to commande motor 2
int32_t s32Channel1 = 0;         // PWM-channel for motor 1
int32_t s32Channel2 = 0;         // PWM channel for motor 2

void setup() 
{
  Serial.begin(115200);
  // Initialize wifi server and the websockets protocole
  WiFi.softAP(ssid, password);
  // debug output to serial monitor
  Serial.println("Hotspot WiFi démarré");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  // call onWsEvent function (see at the end of file = "EOF")
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();  
  // Pin initialisation for motor
  pinMode(u32Motor1, OUTPUT);
  pinMode(u32Motor2, OUTPUT);
  // Set PWM to 12 bits for motor 1, range is 0-4095
  ledcSetup(s32Channel1,1000,12);
  ledcAttachPin(u32Motor1,s32Channel1);
  // Set PWM for motor2
  ledcSetup(s32Channel2,1000,12);
  ledcAttachPin(u32Motor2,s32Channel2);
}

void loop() 
{
  // Take the Joystick value from onWsEvent and change the range from 0-200 to 0-4095
  //int32_t s32MotorSpeed = 20*u32JoystickValue;
  int32_t s32MotorSpeed = 1000;
  //Control the speed of the motor1 and motor2
  //vDriveThe2Motor(constrain(s32MotorSpeed,0,4096),constrain(s32MotorSpeed,0,4096));  
  // test function
  vTestSequenceToMeasure();
}

/**
* @param u32spdm1
* @param u32spdM2
* @brief set PWM signal to control motor1 and motor2 speed
*        0 is no speed and 4095 is max speed
**/
void vDriveThe2Motor(uint32_t u32spdM1, uint32_t u32spdM2) 
{
  // set Pull-down
  digitalWrite(u32Motor1, LOW);   
  digitalWrite(u32Motor2, LOW);       
  // Control motor rotation speed, PWM signal
  ledcWrite(s32Channel1, u32spdM1);
  ledcWrite(s32Channel2, u32spdM2);
}

// excel data to see result of speed, current and voltage measure
void vTestSequenceToMeasure (void)
{
  uint32_t u32spd = 0;
  digitalWrite(u32Motor1, LOW);        //Low = pole - of motor
  //digitalWrite(u32Motor2, LOW);    
  // Motor 2 at speed 0
  //ledcWrite(s32Channel2, 0);
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = u32spd + 500;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
  u32spd = 4096;
  ledcWrite(s32Channel1, u32spd);
  delay(3000);
}

/**
* @param server
* @param client
* @param type
* @param arg
* @param data
* @param len
* @brief communicate per Wifi with Websockets protocole. 
*         take the transmitted char data to int value 
*         and save it in globale variable u32JoystickValue
**/
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) 
{
    switch (type) 
    {
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
            u32JoystickValue = atoi((char*)data);  // Convertit les données en integer
            // Renvoyer les données au client pour confirmation
            break;
    }
}
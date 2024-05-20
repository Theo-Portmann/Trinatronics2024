/************************
  Filename    : Thomas_MotorControllWithPoti.ino
  Description : Use PWM to set the speed of motor, using Mosfet
  Auther      : Thomas Eyer
  Modification: 05/02/2024
************************/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
//code qui cree un serveur websocket sur l'ESP32, et commande un moteur par l'intermédiare d'un mosfet.
//modifie letat de la LED en fonction de la variable globale ledValue
//LedValue est actualisee a chaque WS_EVT_DATA avc la valeur de uint8_t *data convertie en int

//06/05/24
// For wifi communication
const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Définir la variable globale pour stocker la valeur pour la LED
int JoystickValue = 0;

// Initialize pin
uint32_t u32PWMmotor1 = 2;      // Define the pin for PWM, commande motor 1
uint32_t u32PWMmotor2 = 4;      // Define the pin for PWM, commande motor 2
int32_t s32Channel1 = 0;  
int32_t s32Channel2 = 0;    

// Define of variable, to use later
uint32_t u32RotationSpeedMotor1 = 0;   
uint32_t u32RotationSpeedMotor2 = 0;


void setup() 
{
  // For wifi communication
  Serial.begin(115200);
  WiFi.softAP(ssid, password);
  Serial.println("Hotspot WiFi démarré");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();

  // jOYSTICK CONTROLL
  //z axis is a button, with Pulldown confiuration
  //pinMode(u32xyzPins[2], INPUT_PULLDOWN);  

  // Initialize the pin into an output mode for motor1
  pinMode(u32PWMmotor1, OUTPUT);
  pinMode(u32PWMmotor2, OUTPUT);
  
  // Set PWM to 12 bits, range is 0-4095 for motor1
  ledcSetup(s32Channel1,1000,12);         
  ledcAttachPin(u32PWMmotor1,s32Channel1);
  // Set PWM for motor2
  ledcSetup(s32Channel2,1000,12);         
  ledcAttachPin(u32PWMmotor2,s32Channel2);
}

void loop() 
{
  // Convert the voltage of Joystick into digital
  //int32_t s32XVal = analogRead(u32xyzPins[0]);
  //int32_t s32YVal = analogRead(u32xyzPins[1]);
  //bool bZVal = digitalRead(u32xyzPins[2]);

  // Take the Joystick value and change the range from 0-200 to 0-4095
  //int s32MotorSpeed = 20*JoystickValue;

  //Control the steering and speed of the motor
  //vDriveThe2Motor(constrain(s32MotorSpeed,0,4096),0);  
  vTestSequenceToMeasure();
}

void vDriveThe2Motor(uint32_t u32spdM1, uint32_t u32spdM2) 
{
  // set Pull-down
  digitalWrite(u32PWMmotor1, LOW);   
  //digitalWrite(u32PWMmotor2, LOW);       
  // Control motor rotation speed, PWM signal
  Serial.println("PWM value");
  Serial.println(u32spdM1);
  ledcWrite(s32Channel1, u32spdM1);
  //ledcWrite(s32Channel2, u32spdM2);
}

// excel data to see result of speed, current and voltage measure
void vTestSequenceToMeasure (void)
{
  uint32_t u32spd = 0;
  digitalWrite(u32PWMmotor1, LOW);        //Low = pole - of motor
  //digitalWrite(u32PWMmotor2, LOW);       //High = pole + of motor
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

// function to communicate per Wifi with Websockets protocole
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
            JoystickValue = atoi((char*)data);  // Convertit les données en integer
            // Renvoyer les données au client pour confirmation
            break;
    }
}
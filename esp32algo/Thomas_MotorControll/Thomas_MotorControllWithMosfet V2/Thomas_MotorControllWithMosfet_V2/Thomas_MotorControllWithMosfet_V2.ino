/**********************************************************************
  Filename    : Thomas_MotorControllWithPoti.ino
  Description : Use PWM to set the speed of motor, using Mosfet
  Auther      : Thomas Eyer, Johan Maring
  Création    : 04/02/2024
  Modification: V1 06/05/2024 
                V2 20/05/2024
            
  Description : Crée un serveur websocket sur l'ESP32, et commande 
                2 moteurs par l'intermédiare de mosfets. 
**********************************************************************/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// set wifi communication access 
const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Pin definition 
uint16_t u16PinMotor1 = 2;         // Define the pin to commande motor 1, at left
uint16_t u16PinMotor2 = 4;         // Define the pin to commande motor 2, at right
int16_t u16Channel1 = 0;         // PWM-channel for motor 1
int16_t u16Channel2 = 0;         // PWM channel for motor 2

// Define of globale varibale
uint16_t u16JoystickValue = 0;    // take the websocket give-value
uint16_t u16MotorSpeed1 = 0;          // speed variable of motor 1
uint16_t u16MotorSpeed2 = 0;          // speed variable of motor 2
uint16_t u16DifferenceOfSpeed = 0;

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
  pinMode(u16PinMotor1, OUTPUT);
  pinMode(u16PinMotor2, OUTPUT);
  // Set PWM to 12 bits for motor 1, range is 0-4095
  ledcSetup(u16Channel1,1000,12);     // PWM frequenz of 10000 Hz, see 
  ledcAttachPin(u16PinMotor1,u16Channel1);
  // Set PWM for motor2
  ledcSetup(u16Channel2,1000,12);
  ledcAttachPin(u16PinMotor2,u16Channel2);
}

void loop() 
{
  // *****When 1 cursor value is giving******
  // Take the Joystick value from onWsEvent and change the range from 0-200 to 0-4200.
  uint16_t u16MotorSpeed = 21*u16JoystickValue; 

  // // *****When 2 cursor value are giving*****
  // // see Trinatronics_Bericht.pdf Abbildung 5 to understand command fonction
  // // giving value between 0-255 for power and between 256-511 for direction
  // if (u16JoystickValue < 256)
  // {
  //   u16MotorSpeed1 = u16JoystickValue;
  //   u16MotorSpeed2 = u16JoystickValue;
  // }
  // // when the direction change
  // if (u16JoystickValue >= 256)
  // {
  //   if (u16JoystickValue <= 383)      // range of 256-383 decrease speed of motor 1 
  //   {
  //     // calcul the difference of speed, see fonction setting in Trinatronics_Bericht.docx
  //     u16MotorSpeed1 = (uint16_t)u16JoystickValue*2047/127 + 2048 - (uint16_t)256*2047/127;
  //   }
  //   else if(u16JoystickValue > 383)   // range of 384-512 decrease speed of motor 2
  //   {
  //     u16MotorSpeed2 = (uint16_t)u16JoystickValue*(-2047/129) + 4095 - (uint16_t)383*(-2047/129);
  //   }
  // }
  // //Control the speed of the motor1 and motor2. Use constrain because 21*200 > 4066
  vDriveThe2Motor(constrain(u16MotorSpeed,0,4096),constrain(u16MotorSpeed,0,4096));  

  //uncomment if you want to use test function
  //vTestSequenceToMeasure();
}

/**
* @param u16spdM1
* @param u16spdM2
* @brief set PWM signal to control motor1 and motor2 speed
*        0 is no speed and 4095 is max speed
**/
void vDriveThe2Motor(uint32_t u16spdM1, uint32_t u16spdM2) 
{
  // set Pull-down
  digitalWrite(u16PinMotor1, LOW);   
  digitalWrite(u16PinMotor2, LOW);       
  // Control motor rotation speed, PWM signal
  ledcWrite(u16Channel1, u16spdM1);
  ledcWrite(u16Channel2, u16spdM2);
}

// test fonction to make measure of 1 Motor, add every 3s the value 500 to PWM
// excel data to see result of speed, current and voltage measure
void vTestSequenceToMeasure (void)
{
  uint32_t u32spd = 3500;
  //digitalWrite(u16PinMotor1, LOW);        // Configure logic level of GPIO
  //digitalWrite(u16PinMotor2, LOW);
  ledcWrite(u16Channel1, u32spd);         // Set the PWM signal 
  ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = u32spd + 500;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
  // u32spd = 4096;
  // ledcWrite(u16Channel1, u32spd);
  // ledcWrite(u16Channel2, u32spd);
  // delay(3000);
}

/**
* @param server
* @param client
* @param type
* @param arg
* @param data, transmitting array of char
* @param len, length of the array
* @brief communicate per Wifi with Websockets protocole. 
*         take the transmitted char data to int value 
*         and save it in globale variable u16JoystickValue
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
            u16JoystickValue = atoi((char*)data);  // Convertit les données en integer
            // Renvoyer les données au client pour confirmation
            break;
    }
}
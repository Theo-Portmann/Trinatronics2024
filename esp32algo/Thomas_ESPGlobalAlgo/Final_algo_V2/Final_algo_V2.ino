/**********************************************************************************
  Filename    : Final_Algo_V2.ino
  Description : The code is controlled via a websocket connection and receives data 
                via I2C from a BNO085 sensor. Depending on the mode, the code 
                controls the speed of 2 motors.

  Good to know: To see serial monitor print, set DEBUG to 1.
  Author      : Thomas Eyer, Johan Maring
  Creation    :  24/05/2024:  assembly of functional code prepared before
  Modification:  27/05/2024: Websockets enhancement, ping pong added
                 27/05/2024: code implementation, motor control with sensor data
                 31/05/2024: added #define DEBUG, structuration of code
                 05/06/2024: correction and addition of the mode selection command
                 16/07/2024: correction of ledcAttachChannel to ledcAttach

*********************************************************************************/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>

#define DEBUG 1                      // set 1 to see debug print at serial monitor
#define IS_BNO085_ON_CHIP 0

// RST not use for I2C, so take -1 : notavalue
#define BNO08X_RESET -1
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

// Set wifi communication access 
const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Pin definition 
const uint16_t cu16Motor1Pin = 2;       // Define the pin to commande motor 1, at left
const uint16_t cu16Motor2Pin = 4;       // Define the pin to commande motor 2, at right
const uint16_t cu16Frequence = 1000;    // Define the frequence of PWM
const uint16_t cu16Resolution = 12;     // Define the resolution in bit

// Define of global variable
uint16_t volatile u16JoystickValue = 512;    // Take the websocket give-value
uint16_t u16SpeedGlobal = 0;      // Speed of right-joystick for both motor
int16_t s16SpeedDifference = 0;   // based on left-joystick, change individual motor speed
uint16_t u16MotorSpeed1 = 0;      // Speed variable of motor 1
uint16_t u16MotorSpeed2 = 0;      // Speed variable of motor 2
uint16_t u16MotorSpeed = 0;
bool bAutoMode = false;           // Auto mode activation
uint32_t u32TimeIncr = 0;
double dAlpha = 0.5;              // Alpha coefficient for motor control

// ********************************* SETUP **************************************
void setup() 
{
  //------------------ Initializatin of Serial Monitor --------------------------
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10); // Will pause ESP until serial console opens
  }
  #if DEBUG   // Debug output to serial monitor
  Serial.println("Trinatronics 2024 test!");
  #endif     //DEBUG
  //------------------ Initialize wifi server and the WebSocket protocol---------
  WiFi.softAP(ssid, password);
  // Call onWsEvent function (see at the end of file = "EOF")
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
  #if DEBUG   // Debug output to serial monitor
  Serial.println("Hotspot WiFi démarré");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  #endif    // DEBUG

  //------------------ Initialize I2C communication ------------------------
 #if IS_BNO085_ON_CHIP
  if (!bno08x.begin_I2C()) 
  {
    #if DEBUG // Debug output to serial monitor
    Serial.println("Failed to find BNO085 chip");
    #endif  // DEBUG
    while (1) 
    {

      delay(10);
    }
  }
  #if DEBUG // Debug output to serial monitor
  Serial.println("BNO085 Found!");
  for (int n = 0; n < bno08x.prodIds.numEntries; n++) 
  {
    Serial.print("Part ");
    Serial.print(bno08x.prodIds.entry[n].swPartNumber);
    Serial.print(": Version :");
    Serial.print(bno08x.prodIds.entry[n].swVersionMajor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionMinor);
    Serial.print(".");
    Serial.print(bno08x.prodIds.entry[n].swVersionPatch);
    Serial.print(" Build ");
    Serial.println(bno08x.prodIds.entry[n].swBuildNumber);
  }
  setReports();
  #endif    // DEBUG
  #endif    // IS_BNO085_ON_CHIP

  //------------------ Initialize motor controll --------------------------
  // Pin initialization for motor
  pinMode(cu16Motor1Pin, OUTPUT);
  pinMode(cu16Motor2Pin, OUTPUT);
  // Set parameter of PWM signal
  ledcAttach(cu16Motor1Pin, cu16Frequence, cu16Resolution);
  ledcAttach(cu16Motor2Pin, cu16Frequence, cu16Resolution);
  #if DEBUG // Debug output to serial monitor
  Serial.println("Setup complete, reading events and controlling motors");
  #endif // DEBUG
}

// ************************************* LOOP ***************************************
void loop() 
{
  //------------------ take BNO085 data ----------------------------------
  if (bAutoMode)
  {
    #if DEBUG
    if ( u32TimeIncr % 100000 == 0)
        {
        Serial.println("Pilotage auto actif");
        }
    #endif 
    #if IS_BNO085_ON_CHIP   
    delay(10);
    if (bno08x.wasReset()) 
    {
      Serial.print("Sensor was reset ");
      setReports();
    }
    if (!bno08x.getSensorEvent(&sensorValue)) 
    {
      return;
    }
    if (sensorValue.sensorId == SH2_ROTATION_VECTOR) 
    {
      float qw = sensorValue.un.rotationVector.real;
      float qx = sensorValue.un.rotationVector.i;
      float qy = sensorValue.un.rotationVector.j;
      float qz = sensorValue.un.rotationVector.k;
      float roll  = atan2(2.0 * (qw * qx + qy * qz), 
                  1.0 - 2.0 * (qx * qx + qy * qy)) * 180.0 / PI;
      float pitch = asin(2.0 * (qw * qy - qz * qx)) * 180.0 / PI;
      float yaw   = atan2(2.0 * (qw * qz + qx * qy), 
                  1.0 - 2.0 * (qy * qy + qz * qz)) * 180.0 / PI;
      #if DEBUG
      Serial.print("Roll: ");
      Serial.print(roll);
      Serial.print(" Pitch: ");
      Serial.print(pitch);
      Serial.print(" Yaw: ");
      Serial.println(yaw);
      #endif // DEBUG
      //------------------ controll motor with BNO085 -------------------------------
      u16MotorSpeed = 3500;
      if (pitch > 20.0)
      {
        u16MotorSpeed = 0;
      }
      if (pitch < 5.0)
      {
        u16MotorSpeed = 4095;
      }
      else 
      {
        u16MotorSpeed = (uint16_t) (818*pitch);
      }
      //Control the speed of the motors. Use constrain to security
      vDriveThe2Motor(constrain(u16MotorSpeed,0,4096),
                        constrain(u16MotorSpeed,0,4096)); 
    }
    #endif    // IS_BNO085_ON_CHIP
  }
  else// if auto mode is not activated, controll motor with app
  {
    // giving value between 0-255 for direction and between 256-511 for power
    #if DEBUG  // Debug output to serial monitor
    if ( u32TimeIncr % 100000 == 0)
    {
      Serial.print(" u16Joystickvalue: ");
      Serial.println(u16JoystickValue);
    }
    #endif    // DEBUG
    if (u16JoystickValue >= 256)
    {
      u16SpeedGlobal = 4095 - static_cast<uint16_t>(u16JoystickValue - 256)*15.9961;
    }
    // when the direction change // ERROR JOYSTICK GAUCHE 
    else
    {
      // Calculate the coefficent alpha for motor control (0.5 when middle of joystick)
      if (u16JoystickValue == 128)
      {
        dAlpha = 0.5 ;
      }
      else
      {
        dAlpha = static_cast<double>(u16JoystickValue)/255.0;
      }
    }
    // Calculate the speed of the motors based on the alpha coefficient and the global speed
    if (dAlpha == 0.5)
    {
      u16MotorSpeed1 = u16SpeedGlobal;
      u16MotorSpeed2 = u16SpeedGlobal;
    }
    else if (dAlpha < 0.5) // we want to turn right
    {
      u16MotorSpeed2 = u16SpeedGlobal; //u16MotorSpeed2 = motor on the right
      u16MotorSpeed1 = static_cast<uint16_t>(2*u16SpeedGlobal * dAlpha); //u16MotorSpeed1 = motor on the left
    }
    else // we want to turn left
    {
      u16MotorSpeed2 = static_cast<uint16_t>(2*u16SpeedGlobal - 2*u16SpeedGlobal * dAlpha); //u16MotorSpeed2 = motor on the right
      u16MotorSpeed1 = u16SpeedGlobal;  //u16MotorSpeed1 = motor on the left
    }
    vDriveThe2Motor(4095, 2048);
    //vDriveThe2Motor(constrain(u16MotorSpeed1,0,4096), constrain(u16MotorSpeed2,0,4096));
  }
   u32TimeIncr++;
}

// ******************************* function definition ******************************
/**
* @brief Set PWM signal to control motor1 and motor2 speed
*        0 is no speed and 4095 is max speed
* @param u16spdM1 Speed of motor 1
* @param u16spdM2 Speed of motor 2
**/
void vDriveThe2Motor(uint32_t u16spdM1, uint32_t u16spdM2) 
{
  // Sanity Check, pull down
  digitalWrite(cu16Motor1Pin, LOW);   
  digitalWrite(cu16Motor2Pin, LOW);       
  // Control motor rotation speed, PWM signal
  ledcWrite(cu16Motor1Pin, u16spdM1);
  ledcWrite(cu16Motor2Pin, u16spdM2);

   // Print motor speeds to Serial Monitor
  #if DEBUG
  if ( u32TimeIncr % 100000 == 0)
  {
    Serial.print("Motor 1 speed: ");
    Serial.println(u16spdM1);
    Serial.print(" Motor 2 speed: ");
    Serial.println(u16spdM2);
  }
  #endif // DEBUG
}

/**
* @brief Here you define the sensor outputs you want to receive
**/
void setReports() 
{
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ROTATION_VECTOR)) 
  {
    Serial.println("Could not enable rotation vector");
  }
}

/**
* @brief Communicate per Wifi with Websockets protocol. 
*        Take the transmitted char data to int value 
*        and save it in global variable u16JoystickValue
* @param server AsyncWebSocket server
* @param client AsyncWebSocket client
* @param type Event type
* @param arg Additional arguments
* @param data Transmitting array of char
* @param len Length of the array
**/
void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) 
{
  switch (type) 
  {
    case WS_EVT_CONNECT:
      Serial.println("Client connecté");
      server->cleanupClients(1); // Keep 1 client, the new one
      break;
    case WS_EVT_DISCONNECT:
      Serial.println("Client déconnecté");
      break;
    case WS_EVT_DATA:
      if (!len) // when len = 0, no data transmit, break process
      {         // else it transmit 0 and the motor take speed 0
        break;
      }

      Serial.println();
      Serial.write(data,len);
      Serial.println("\n");
      if (strcmp((char*)data, "ping") == 0) 
      {
        client->text("pong"); //Ajout d'une logique ping pong qui permet au 
                              //client de se reconnecter si le serveur ne repond pas
        
      } 
      else if (strcmp((char*)data, "end")== 0)
       {
         bAutoMode = false;
         u16JoystickValue = 512;
       }
      else if (strcmp((char*)data, "auto")== 0)//activation du mode automatique
       {
         client->text("auto");  
          bAutoMode = true;
       }
      else if (strcmp((char*)data, "main")== 0)//activation du mode manuel
       {
         client->text("main");  
          bAutoMode = false;
       }
      else
      {
        data[len] = '\0'; // S'assure que la chaîne est terminée proprement pour conversion
        u16JoystickValue = atoi((char*)data);  // Convertit les données en integer
      }
      break;
  }
}

/**********************************************************************
  Filename    : MotorControll_PrintBNO085Data_V2.ino
  Description : Use I2C to get orientation data from BNO085 and calculate 
                aircraft inclination in degrees, and separated control motor speed 
                using WebSocket on ESP32 C3 SuperMini.
  Good to know: If BNO085 not found, programm will not run    
  Author      : Thomas Eyer
  Creation    : 24/05/2024
  Modification:  27/05/2024: amélioration du Websockets, ajout ping pong
                 27/05/2024: implémentation code, controle moteur avec donée capteur
***********************************************************************/
#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>
#define CONTROLL_PER_APP 1          // change to enabled/disabled controll motor with sensor
#if CONTROLL_PER_APP
   #define CONTROLL_WITH_BNO085 0
#else 
  #define CONTROLL_WITH_BNO085 1
#endif

// Set wifi communication access 
const char* ssid = "ESP32-Access-Point1";
const char* password = "123456789";
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// Pin definition 
uint16_t u16PinMotor1 = 2; // Define the pin to commande motor 1, at left
uint16_t u16PinMotor2 = 4; // Define the pin to commande motor 2, at right
int16_t u16Channel1 = 0; // PWM-channel for motor 1
int16_t u16Channel2 = 0; // PWM channel for motor 2

// Define of global variable
uint16_t u16JoystickValue = 0;    // Take the websocket give-value
uint16_t u16SpeedGlobal = 0;      // Speed of right-joystick for both motor
int16_t s16SpeedDifference = 0;  // based on left-joystick, change individual motor speed
uint16_t u16MotorSpeed1 = 0;      // Speed variable of motor 1
uint16_t u16MotorSpeed2 = 0;      // Speed variable of motor 2

#if CONTROLL_WITH_BNO085
// RST not use for I2C, so take -1 : notavalue
#define BNO08X_RESET -1
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;
#endif // BNO08X_RESET

void setup() 
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10); // Will pause ESP until serial console opens
  }
  Serial.println("Trinatronics 2024 test!");


  //-------------------- Initialize I2C communication------------------------
  #if CONTROLL_WITH_BNO085
  if (!bno08x.begin_I2C()) 
  {
    Serial.println("Failed to find BNO085 chip");
    while (1) 
    {
      delay(10);
    }
  }
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
  #endif // CONTROLL_WITH_BNO085

  //------------------ Initialize wifi server and the WebSocket protocol---------
  WiFi.softAP(ssid, password);
  // Debug output to serial monitor
  Serial.println("Hotspot WiFi démarré");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());
  // Call onWsEvent function (see at the end of file = "EOF")
  ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
  // ----------------- Initialize motor controll --------------------------------
  // Pin initialization for motor
  pinMode(u16PinMotor1, OUTPUT);
  pinMode(u16PinMotor2, OUTPUT);
  // Set PWM to 12 bits for motor 1, range is 0-4095
  ledcSetup(u16Channel1, 1000, 12); // PWM frequency of 1000 Hz
  ledcAttachPin(u16PinMotor1, u16Channel1);
  // Set PWM for motor 2
  ledcSetup(u16Channel2, 1000, 12);
  ledcAttachPin(u16PinMotor2, u16Channel2);

  Serial.println("Setup complete, reading events and controlling motors");
}

void loop() 
{
  // ----------------------- take BNO085 data --------------------------
  #if CONTROLL_WITH_BNO085
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
    float roll  = atan2(2.0 * (qw * qx + qy * qz), 1.0 - 2.0 * (qx * qx + qy * qy)) * 180.0 / PI;
    float pitch = asin(2.0 * (qw * qy - qz * qx)) * 180.0 / PI;
    float yaw   = atan2(2.0 * (qw * qz + qx * qy), 1.0 - 2.0 * (qy * qy + qz * qz)) * 180.0 / PI;

    // uncomment if you want to print BNO085 data
    // Serial.print("Roll: ");
    // Serial.print(roll);
    // Serial.print(" Pitch: ");
    // Serial.print(pitch);
    // Serial.print(" Yaw: ");
    // Serial.println(yaw);
    // ------------------- controll motor with BNO085 -------------------------------
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
    //Control the speed of the motor1 and motor2. Use constrain to security
    vDriveThe2Motor(constrain(u16MotorSpeed,0,4096),constrain(u16MotorSpeed,0,4096)); 
  }
  #endif // CONTROLL_WITH_BNO085

  #if CONTROLL_PER_APP
  // -------------------------- controll motor with Websockets -------------------------------
  // *****When 1 cursor value is giving******
  // Take the Joystick value from onWsEvent and change the range from 0-200 to 0-4200.
  // uint16_t u16MotorSpeed = 21*u16JoystickValue; 

  // *****When 2 cursor value are giving*****
  // giving value between 0-255 for direction and between 256-511 for power
  if (u16JoystickValue >= 256)
  {
    u16SpeedGlobal = (u16JoystickValue - 256)*16;
    u16MotorSpeed1 = u16SpeedGlobal;
    u16MotorSpeed2 = u16SpeedGlobal;
  }
  // when the direction change
  else if (u16JoystickValue < 256)
  {
    s16SpeedDifference = -(uint16_t)(2*4096/256) + 4096;
    if (s16SpeedDifference > 0)
    {
      u16MotorSpeed1 = u16SpeedGlobal - abs(s16SpeedDifference);
      u16MotorSpeed2 = u16SpeedGlobal + abs(s16SpeedDifference);
    }
    else
    {
      u16MotorSpeed1 = u16SpeedGlobal + abs(s16SpeedDifference);
      u16MotorSpeed2 = u16SpeedGlobal - abs(s16SpeedDifference);
    }
  }
  vDriveThe2Motor(constrain(u16MotorSpeed1,0,4096),constrain(u16MotorSpeed2,0,4096));   

  //uncomment if you want to use test function
  //vTestSequenceToMeasure(); 
}

// ------------------------------------ function definition -----------------------------
/**
* @brief Set PWM signal to control motor1 and motor2 speed
*        0 is no speed and 4095 is max speed
* @param u16spdM1 Speed of motor 1
* @param u16spdM2 Speed of motor 2
**/
void vDriveThe2Motor(uint32_t u16spdM1, uint32_t u16spdM2) 
{
  // Set Pull-down
  digitalWrite(u16PinMotor1, LOW);   
  digitalWrite(u16PinMotor2, LOW);       
  // Control motor rotation speed, PWM signal
  ledcWrite(u16Channel1, u16spdM1);
  ledcWrite(u16Channel2, u16spdM2);
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
      Serial.write(data,len);
      Serial.println("");
      if (strcmp((char*)data, "ping") == 0) 
      {
        client->text("pong"); //Ajout d'une logique ping pong qui permet au client de se reconnecter si le serveur ne repond pas
      } 
      else if (strcmp((char*)data, "end")== 0)
      {
        u16JoystickValue = 0;
      }
      else
      {
        data[len] = '\0'; // Assurez-vous que la chaîne est terminée proprement pour conversion
        u16JoystickValue = atoi((char*)data);  // Convertit les données en integer
      }
      break;
  }
}

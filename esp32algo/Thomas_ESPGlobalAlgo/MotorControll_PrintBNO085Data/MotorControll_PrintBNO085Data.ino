/**********************************************************************
  Filename    : MotorControll_PrintBNO085Data.ino
  Description : Use I2C to get orientation data from BNO085 and calculate 
                aircraft inclination in degrees, and separated control motor speed 
                using WebSocket on ESP32 C3 SuperMini.
  Author      : Thomas Eyer
  Creation    : 24/05/2024
  Modification:  
***********************************************************************/

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>
#include <Adafruit_BNO08x.h>

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
uint16_t u16JoystickValue = 0; // Take the websocket give-value
uint16_t u16MotorSpeed1 = 0; // Speed variable of motor 1
uint16_t u16MotorSpeed2 = 0; // Speed variable of motor 2

// RST not use for I2C, so take -1 : notavalue
#define BNO08X_RESET -1
Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setup() 
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10); // Will pause ESP until serial console opens
  }
  Serial.println("Trinatronics BNO085 I2C test!");

  //Wire.begin(21, 22); // Re-initialize I2C with SDA and SCL pins

  //-------------------- Initialize I2C communication------------------------
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
  // ----------------------- take and print BNO085 data --------------------------
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

    Serial.print("Roll: ");
    Serial.print(roll);
    Serial.print(" Pitch: ");
    Serial.print(pitch);
    Serial.print(" Yaw: ");
    Serial.println(yaw);
  }
  
  // -------------------------- take transmitted Websockets value --------------------------
  // -------------------------- and controll the motor speed -------------------------------
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

// Here must be written doxygen comment 
// Here is where you define the sensor outputs you want to receive
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
      // Conversion des données en int et mise à jour de la variable globale
      data[len] = '\0'; // Assurez-vous que la chaîne est terminée proprement pour conversion
      u16JoystickValue = atoi((char*)data); // Convertit les données en integer
      // Renvoyer les données au client pour confirmation
      break;
  }
}

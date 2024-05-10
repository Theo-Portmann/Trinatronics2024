
/**********************************************************************
  Filename    : Thomas_MotorControllWithPoti.ino
  Description : Use joystick to set the speed of motor, using L293D driver
  Auther      : Thomas Eyer
  Modification: 05/02/2024
**********************************************************************/
// Initialize pin for motor1
uint32_t u32PinIn1 = 12;      // Define L293D channel 1 pin, control motor1 direction
uint32_t u32PinIn2 = 14;      // Define L293D channel 2 pin, control motor1 direction
uint32_t u32EnablePin1 = 13;  // Define L293D enable 1 pin, PWM signal control motor1 speed
int32_t s32Channel1 = 0;      // IDK ?

// Initialize pin for motor2
uint32_t u32PinIn3 = 32;
uint32_t u32PinIn4 = 35;
uint32_t u32EnablePin2 = 33;
int32_t s32Channel2 = 0;

// Initialize pin for Joystick
uint32_t u32xyzPins[] = {15, 2, 0};

// Define of variable, to use later
uint32_t u32RotationSpeedMotor1 = 0;   
uint32_t u32RotationSpeedMotor2 = 0;


void setup() 
{
  Serial.begin(115200);
  //z axis is a button, with Pulldown confiuration
  pinMode(u32xyzPins[2], INPUT_PULLDOWN);  

  // Initialize the pin into an output mode for motor1
  pinMode(u32PinIn1, OUTPUT);
  pinMode(u32PinIn2, OUTPUT);
  pinMode(u32EnablePin1, OUTPUT);
  // Initialiaze for motor2
  pinMode(u32PinIn3, OUTPUT);
  pinMode(u32PinIn4, OUTPUT);
  pinMode(u32EnablePin2, OUTPUT);
  
  // Set PWM to 12 bits, range is 0-4095 for motor1
  ledcSetup(s32Channel1,1000,12);         
  ledcAttachPin(u32EnablePin1,s32Channel1);
  // Set PWM for motor2
  ledcSetup(s32Channel2,1000,12);         
  ledcAttachPin(u32EnablePin2,s32Channel2);
}

void loop() 
{
  // Convert the voltage of Joystick into digital
  //int32_t s32XVal = analogRead(u32xyzPins[0]);
  //int32_t s32YVal = analogRead(u32xyzPins[1]);
  bool bZVal = digitalRead(u32xyzPins[2]);     

  // Calculate the motor speed, Joystick has a range of 4096 also
  //u32RotationSpeedMotor1 = (uint32_t)(4095 - s32YVal);
  //u32RotationSpeedMotor2 = (uint32_t)(s32XVal);
  //Control the steering and speed of the motor
  //vDriveThe2Motor(constrain(u32RotationSpeedMotor1,0,4096), constrain(u32RotationSpeedMotor2,0,4096));  
  vTestSequenceToMeasure();
}

void vDriveThe2Motor(uint32_t u32spdM1, uint32_t u32spdM2) 
{
  // *********Drive Motor1**********
  // define if CW or CCW 
  digitalWrite(u32PinIn1, LOW);        //Low = pole - of motor
  digitalWrite(u32PinIn2, HIGH);       //High = pole + of motor
  // Control motor rotation speed, PWM signal
  ledcWrite(s32Channel1, u32spdM1);

  // *********Drive Motor2**********
  digitalWrite(u32PinIn3, LOW); 
  digitalWrite(u32PinIn4, HIGH);
  ledcWrite(s32Channel2, u32spdM2);
}

// excel data to see result of speed, current and voltage measure
void vTestSequenceToMeasure (void)
{
  uint32_t u32spd = 0;
  digitalWrite(u32PinIn1, LOW);        //Low = pole - of motor
  digitalWrite(u32PinIn2, HIGH);       //High = pole + of motor
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


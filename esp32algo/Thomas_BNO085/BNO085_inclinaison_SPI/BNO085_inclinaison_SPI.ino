/**********************************************************************
  Filename    : Thomas_BNO085_Orientation.ino
  Description : Get orientation data from BNO085 and calculate  
                inclination in degrees.
  Author      : Thomas Eyer
  Creation    : 23/05/2024
  Modification: 23/05/2024 
***********************************************************************/

#include <Adafruit_BNO08x.h>

// For SPI mode, we need a CS pin
#define BNO08X_CS 2
#define BNO08X_INT 10
// For SPI mode, we also need a RESET
#define BNO08X_RESET -1

Adafruit_BNO08x bno08x(BNO08X_RESET);
sh2_SensorValue_t sensorValue;

void setup() 
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }
  Serial.println("Adafruit BNO08x test!");

  if (!bno08x.begin_SPI(BNO08X_CS, BNO08X_INT)) 
  {
    Serial.println("Failed to find BNO08x chip");
    while (1) 
    {
      delay(10);
    }
  }
  Serial.println("BNO08x Found!");

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
  delay(3000); // Wait 3 seconds for calibration

  Serial.println("Reading events");
  delay(100);
}

void loop() 
{
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
}

void setReports() 
{
  Serial.println("Setting desired reports");
  if (!bno08x.enableReport(SH2_ROTATION_VECTOR)) 
  {
    Serial.println("Could not enable rotation vector");
  }
}

/**********************************************************************
  Filename    : Thomas_BNO085_Orientation_I2C.ino
  Description : Get orientation data from BNO085 and calculate aircraft 
                inclination in degrees using I2C.
  Author      : Thomas Eyer, Johan Maring
  Creation    : 23/05/2024
  Modification: V1 23/05/2024 
***********************************************************************/

#include <Wire.h>
#include <Adafruit_BNO08x.h>

// for I2C, don't need the RST pin, take notavalue
#define BNO08x_RESET -1

Adafruit_BNO08x bno08x(BNO08x_RESET);
sh2_SensorValue_t sensorValue;

void setup() 
{
  Serial.begin(115200);
  while (!Serial)
  {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }
  Serial.println("Adafruit BNO08x I2C test!");

  //Wire.begin(21, 22); // Initialize I2C with SDA and SCL pins

  // Initialize I2C communication
  if (!bno08x.begin_I2C()) 
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

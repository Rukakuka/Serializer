#include "LSM9DS1.h"
#include <I2C.h>
#include <StandardCplusplus.h>
#include <string>
#include <sstream>

#define GET_VAR_NAME(Variable) (#Variable)

I2C Bus;
LSM9DS1 Sensor(Bus);

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(1286400);

    Bus.begin();
    Bus.setSpeed(true);
    Bus.timeOut(10);

    Sensor.Set_CTRL_REG1_G(LSM9DS1::GyroODR::ODR_952, LSM9DS1::GyroFullscale::FS_2000);
    Sensor.Set_CTRL_REG2_G(LSM9DS1::GyroInterruptGeneration::None, LSM9DS1::GyroOutDataConfiguration::OutAfterLPF1);
    Sensor.Set_CTRL_REG3_G(LSM9DS1::GyroLowPowerMode::Disabled);
    Sensor.Set_ORIENT_CFG_G(LSM9DS1::GyroAxisSign::Negative, LSM9DS1::GyroAxisSign::Positive, LSM9DS1::GyroAxisSign::Positive, LSM9DS1::GyroAxisOrder::XYZ);

    Sensor.Set_REG5_XL(LSM9DS1::AccelDecimation::NoDecimation);
    Sensor.Set_REG6_XL(LSM9DS1::AccelODR::ODR_952, LSM9DS1::AccelFullscale::G_2, LSM9DS1::AccelAntiAliasingBandwidth::BW_408);
    Sensor.Set_REG7_XL(LSM9DS1::AccelHighResolutionMode::Enabled, LSM9DS1::AccelFilterCutoff::ODR_TO_9, LSM9DS1::AccelDataSelection::Bypassed);

    Sensor.Set_REG1_M(LSM9DS1::MagnetTempCompensation::Enabled, LSM9DS1::MagnetAxisOperativeMode::UltraHighPerfomance, LSM9DS1::MagnetODR::ODR_80, LSM9DS1::MagnetFastODRMode::Enabled);
    Sensor.Set_REG2_M(LSM9DS1::MagnetFullscale::GAUSS_16);
    Sensor.Set_REG3_M(LSM9DS1::MagnetLowPowerMode::Disabled, LSM9DS1::MagnetOperatingMode::ContinuousConversion);
    Sensor.Set_REG4_M(LSM9DS1::MagnetAxisOperativeMode::UltraHighPerfomance);
    Sensor.Set_REG5_M(LSM9DS1::MagnetDataUpdate::Continuous);
}

unsigned long oldtime = 0;
unsigned long newtime = 0;
unsigned long t = 0;

void loop()
{
    t++;
    //delay(1000);

    if (t % 1000 == 0)
    {
        pulse();
        /*
        newtime = micros();
        Serial.print("Average measure time, milliseconds : ");
        Serial.println(((float)(newtime - oldtime)) / (1000 * t));
        t = 0;
        oldtime = micros();
        */
    }

    /*
    Sensor.Update();
    Sensor.Accelerometer.X = 0x5A;
    Sensor.Accelerometer.Y = 0x5A;
    Sensor.Accelerometer.Z = 0x5A;
    Sensor.Gyroscope.X = 0x5A;
    Sensor.Gyroscope.Y = 0x5A;
    Sensor.Gyroscope.Z = 0x5A;
    Sensor.Magnetometer.X = 0x5A;
    Sensor.Magnetometer.Y = 0x5A;
    Sensor.Magnetometer.Z = 0x5A;
    */
   
    Sensor.UpdateBuffer();
    for (int i = 0; i <18; i++)
    {
        //Sensor.Buf[i] = i;
        Serial.write(Sensor.Buf[i]);
        //Serial.write("\t");
    }
    Serial.write("\r\n"); // ASCII ">" symbol
    
    /*
    Serial.write(0x3C);
    Serial.print(Sensor.Accelerometer.X);
    Serial.print("\t");
    Serial.print(Sensor.Accelerometer.Y);
    Serial.print("\t");
    Serial.print(Sensor.Accelerometer.Z);
    Serial.print("\t");
    Serial.print(Sensor.Gyroscope.X);
    Serial.print("\t");
    Serial.print(Sensor.Gyroscope.Y);
    Serial.print("\t");
    Serial.print(Sensor.Gyroscope.Z);
    Serial.print("\t");
    Serial.print(Sensor.Magnetometer.X);
    Serial.print("\t");
    Serial.print(Sensor.Magnetometer.Y);
    Serial.print("\t");
    Serial.print(Sensor.Magnetometer.Z);
    Serial.write(0x3E);
    Serial.write("\n");
    */
}

void pulse()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}
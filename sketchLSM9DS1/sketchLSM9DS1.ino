#include "LSM9DS1.h"
#include "xmlmessage.h"

#include <I2C.h>
#include <StandardCplusplus.h>
#include <string>
#include <sstream>

#define GET_VAR_NAME(Variable) (#Variable)


// <TAGS TO TRANSMIT TO PC>
#define ax Sensor.Accelerometer.X
#define ay Sensor.Accelerometer.Y
#define az Sensor.Accelerometer.Z
#define gx Sensor.Gyroscope.X
#define gy Sensor.Gyroscope.Y
#define gz Sensor.Gyroscope.Z
#define mx Sensor.Magnetometer.X
#define my Sensor.Magnetometer.Y
#define mz Sensor.Magnetometer.Z

const string deviceid = "LSM9DS1";
const string errorcode = "noerror"; // TODO : manage error codes
 // </TAGS>

I2C Bus;
LSM9DS1 Sensor(Bus);
XmlMessage Message;

void setup() {

    Serial.begin(115200);
    Bus.begin();
    Bus.setSpeed(true);
    Bus.timeOut(1000);

    Sensor.Set_CTRL_REG1_G(LSM9DS1::GyroODR::ODR_238, LSM9DS1::GyroFullscale::FS_2000);
    Sensor.Set_CTRL_REG2_G(LSM9DS1::GyroInterruptGeneration::None, LSM9DS1::GyroOutDataConfiguration::OutAfterLPF1);
    Sensor.Set_CTRL_REG3_G(LSM9DS1::GyroLowPowerMode::Disabled);
    Sensor.Set_ORIENT_CFG_G(LSM9DS1::GyroAxisSign::Negative, LSM9DS1::GyroAxisSign::Positive, LSM9DS1::GyroAxisSign::Positive, LSM9DS1::GyroAxisOrder::XYZ);
    
    Sensor.Set_REG5_XL(LSM9DS1::AccelDecimation::NoDecimation);
    Sensor.Set_REG6_XL(LSM9DS1::AccelODR::ODR_238, LSM9DS1::AccelFullscale::G_16, LSM9DS1::AccelAntiAliasingBandwidth::BW_50);
    Sensor.Set_REG7_XL(LSM9DS1::AccelHighResolutionMode::Enabled, LSM9DS1::AccelFilterCutoff::ODR_TO_400, LSM9DS1::AccelDataSelection::Filtered);

    Sensor.Set_REG1_M(LSM9DS1::MagnetTempCompensation::Enabled, LSM9DS1::MagnetAxisOperativeMode::UltraHighPerfomance, LSM9DS1::MagnetODR::ODR_80, LSM9DS1::MagnetFastODRMode::Enabled);
    Sensor.Set_REG2_M(LSM9DS1::MagnetFullscale::GAUSS_16);
    Sensor.Set_REG3_M(LSM9DS1::MagnetLowPowerMode::Disabled, LSM9DS1::MagnetOperatingMode::ContinuousConversion);
    Sensor.Set_REG4_M(LSM9DS1::MagnetAxisOperativeMode::UltraHighPerfomance);
    Sensor.Set_REG5_M(LSM9DS1::MagnetDataUpdate::Continuous);

    Message.SetRoot("SerialDeviceData");

    Message.AddTag(Message.toString(GET_VAR_NAME(deviceid)), deviceid);
    Message.AddTag(Message.toString(GET_VAR_NAME(errorcode)), Message.toString(errorcode));

    Message.AddTag(Message.toString(GET_VAR_NAME(ax)), Message.toString(ax));
    Message.AddTag(Message.toString(GET_VAR_NAME(ay)), Message.toString(ay));
    Message.AddTag(Message.toString(GET_VAR_NAME(az)), Message.toString(az));

    Message.AddTag(Message.toString(GET_VAR_NAME(gx)), Message.toString(gx));
    Message.AddTag(Message.toString(GET_VAR_NAME(gy)), Message.toString(gy));
    Message.AddTag(Message.toString(GET_VAR_NAME(gz)), Message.toString(gz));

    Message.AddTag(Message.toString(GET_VAR_NAME(mx)), Message.toString(mx));
    Message.AddTag(Message.toString(GET_VAR_NAME(my)), Message.toString(my));
    Message.AddTag(Message.toString(GET_VAR_NAME(mz)), Message.toString(mz));  
}

int i = 0 ;
void loop()
{
    i++;
    if (Sensor.Update() == LSM9DS1::Error::None)
    {
       
        Message.SetValueByTag(GET_VAR_NAME(ax), Message.toString(ax));
        Message.SetValueByTag(GET_VAR_NAME(ay), Message.toString(ay));
        Message.SetValueByTag(GET_VAR_NAME(az), Message.toString(az));

        
        Message.SetValueByTag(GET_VAR_NAME(gx), Message.toString(gx));
        Message.SetValueByTag(GET_VAR_NAME(gy), Message.toString(gy));
        Message.SetValueByTag(GET_VAR_NAME(gz), Message.toString(gz));

        Message.SetValueByTag(GET_VAR_NAME(mx), Message.toString(mx));
        Message.SetValueByTag(GET_VAR_NAME(my), Message.toString(my));
        Message.SetValueByTag(GET_VAR_NAME(mz), Message.toString(mz));

        Serial.println(Message.GetString().c_str());
        Serial.println("---------------------");

        //Serial.print("X=");
        //Serial.print(Sensor.Gyroscope.X);
        //Serial.print("      Y=");
        //Serial.print(Sensor.Gyroscope.Y);
        //Serial.print("      Z=");
        //Serial.print(Sensor.Gyroscope.Z);
        //Serial.println();
        //Serial.print("X=");

        Serial.print(Sensor.Accelerometer.X);
        Serial.print("      Y=");
        Serial.print(ay);
        Serial.print("      Z=");
        Serial.print(az);
        Serial.println();
        /*
        Serial.print("X=");
        Serial.print(Sensor.Magnetometer.X);
        Serial.print("      Y=");
        Serial.print(Sensor.Magnetometer.Y);
        Serial.print("      Z=");
        Serial.print(Sensor.Magnetometer.Z);
        Serial.println();
        */
    }
    delay(1000);
}

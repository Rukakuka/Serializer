
#include <I2C.h>
#include <StandardCplusplus.h>
#include <string>
#include <sstream>

#define GET_VAR_NAME(Variable) (#Variable)

I2C Bus;

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(2500000); // 1286400

    Bus.begin();
    Bus.setSpeed(true);
    Bus.timeOut(10);

}

unsigned long oldtime = 0;
unsigned long newtime = 0;
unsigned long t = 0;
byte tbuf[4];
int ct = 0;

void loop()
{
    t++;
    if (t == 1000)
    {
      t = 0;
      pulse();
    }    
    ct++;
    if (ct == 255)
    {
      ct = 0;
    }    
    delay(1);
    for (int i = 0; i <18; i++)
    {
        //Sensor.Buf[i] = i;
        Serial.write(ct);
        //Serial.write("\t");
    }
    newtime = micros();
    tbuf[0] = newtime & 255;
    tbuf[1] = (newtime >> 8)  & 255;
    tbuf[2] = (newtime >> 16) & 255;
    tbuf[3] = (newtime >> 24) & 255;

    Serial.write(tbuf, sizeof(tbuf));
    Serial.write("\r\n");
}

void pulse()
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

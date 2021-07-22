/*
 * Project MQ5
 * Description:  Seeed Studio TCS3472 I2C Color Sensor V2
 * Author:  Patrick Bolton 
 * Date:  05/27/21
 */

#include "Particle.h"
#include "math.h"
#include "Adafruit_TCS34725.h"

SYSTEM_THREAD(ENABLED);

boolean commonAnode = false;
char szInfo[128];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

#define dbSerial Serial

SerialLogHandler logHandler;

#define UPDATE_INTERVAL 5000  //1 sec = 1000 millis

unsigned long UpdateInterval;
int min_last, min_time;
int tcs_lux, tcs_cct;

void setup() 
{
  dbSerial.begin(9600);

  if (tcs.begin()) {Log.info("TCS34725 Began");} 
  else{Log.info("TCS34725 Error");}

  UpdateInterval = millis();
  min_last=-1;
}

void loop()
{
  Time.zone(-7);
  if(Particle.disconnected()){return;}

  uint16_t clear, red, green, blue;

  if ((millis() - UpdateInterval) > UPDATE_INTERVAL)
  {
    tcs.setInterrupt(false);      // turn on LED
    delay(60);  // takes 50ms to read 
      
    tcs.getRawData(&red, &green, &blue, &clear);
    tcs.setInterrupt(true);  // turn off LED
    
    tcs_lux=tcs.calculateLux(red, green, blue);
    tcs_cct=tcs.calculateColorTemperature(red, green, blue);


    // Figure out some basic hex code for visualization
    uint32_t sum = clear;
    float r, g, b;
    
    r = red; r /= sum;
    g = green; g /= sum;
    b = blue; b /= sum;
    r *= 256; g *= 256; b *= 256;
    
    //sprintf(szInfo, "%d,%d,%d", (int)r, (int)g, (int)b);

    Log.info("Raw Red: %d", red);
    Log.info("Raw Green: %d", green);
    Log.info("Raw Blue: %d", blue);
    Log.info("Raw Clear: %d", clear);
    Log.info("Color Coordinates");
    Log.info("Red: %d", int(r));
    Log.info("Green: %d", int(g));
    Log.info("Blue: %d", int(b));
    Log.info("Illum: %d", tcs_lux);
    Log.info("ColorTempK: %d", tcs_cct);
    
    
    Serial.println(szInfo);

       
    min_time=Time.minute();
    if((min_time!=min_last)&&(min_time==0||min_time==15||min_time==30||min_time==45))
    {
      //createEventPayload1();
      min_last = min_time;    
      Log.info("Last Update: %d", min_last);
      Log.info(Time.timeStr());
    }
    Log.info("loop");
    Log.info(Time.timeStr());
    UpdateInterval = millis();
  }
}
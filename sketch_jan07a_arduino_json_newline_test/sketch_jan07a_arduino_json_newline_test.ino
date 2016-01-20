#include <Wire.h>
#include <ArduinoJson.h>
#include <SPI.h>

#include "winbondflash.h"
#include "init.h"
#include "led.h"
#include "config.h"
#include "mserial.h"
#include "words.h"
#include "timer.h"

//develop log:
//2016 01 19 四种特效搞定,剩下stalled效果,但是可以用了,32pix没有做

void setup() 
{
  // put your setup code here, to run once:

  init_config(&cfg);
  
  init_serial();

  init_words(&cfg);
  
  led_setup(&cfg);
  
  init_timer();

  //SPI.begin();
  gspi_setup();
}

void loop() 
{
   while(!Serial.available()) 
   {
      led_loop(&cfg);
   }
   
   while (Serial.available()) 
   {
      //stop_timer();
      Serial_process(&cfg);
      //restart_timer();
   }
   
   Serial.flush();
}


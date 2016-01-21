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
//2016 01 20 stalled通过 一个函数led_check_stalled_time来实现,不能用delay,否则一个特效的delay会影响另一个特效,用计数的方式实现
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


#include "mserial.h"

/*
 * 这个串口以"行"为单位进行处理 即 \n 
 * 用在stm32上适合,Arduino 未必
 */
char serial_cache[SERIAL_CACHE];//常驻内存,提高性能

byte CRC8( char *data, int len) 
{
  byte crc = 0x00;
  while (len--) 
  {
    byte extract = *data++;
    for (byte tempI = 8; tempI; tempI--) 
    {
      byte sum = (crc ^ extract) & 0x01;
      crc >>= 1;
      if (sum) {
        crc ^= 0x8C;
      }
      extract >>= 1;
    }
  }
  return crc;
}


void init_serial()
{
  Serial.begin(BAUDRATE);
  Serial.setTimeout(30000); //30 秒
}

#if 0 //实验串口是否是 Serial.read稳定还是 Serial.readByteUtil 稳定,事实是差不多,主要是要做校验
void Serial_process()
{
  unsigned char inByte;

   if(Serial.available()>0)
   {
      inByte = Serial.read();
      if(inByte != 0x0a)
      {
        mserial_add_byte(&ms1,inByte);
      }
      else if(inByte == 0x0a)
      { 
        lisp_return(&ms1);// 先返回CRC较验,此地返回一新行的newlisp代码,可被newlisp的exec截获,然后eval-string,不影响下面json的任何输出
        
        serial_parse_json(ms1.buffer);
        reset_mserial(&ms1);
      }
   }
}
#else
//这是旧的方式,效率不怎么样,出错率太高了
void Serial_process(CONFIG*cfg)
{
  int total_read;
  String topnt;
  
  if(Serial.available() > 0)
  {
    total_read = Serial.readBytesUntil('\n',serial_cache,SERIAL_CACHE-1);
    total_read = total_read > (SERIAL_CACHE-1)?(SERIAL_CACHE-1):total_read;
    serial_cache[total_read] = '\0';

    //crc较验在*_parse_*之前,返回结果
    topnt = "(setq mcu_ret ";
    topnt+=CRC8(serial_cache,total_read);
    topnt+= ")";
    Serial.println(topnt);
    
    if( serial_parse_json(serial_cache) == -1)
    {
      serial_parse_eqled(serial_cache,cfg);
    }
  }  
}
#endif


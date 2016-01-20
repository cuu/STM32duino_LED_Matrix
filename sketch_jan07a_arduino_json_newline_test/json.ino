#include "json.h"
#include "words.h"
#include "led.h"

#include "config.h"


uint8_t get_cur_screen()
{
  return screen;
}

void set_cur_screen(uint8_t num)
{
  screen = num;
}

void json_test(char*ptr,unsigned char len)
{
  Serial.print("json_test: ");
  Serial.println(ptr);
}

void json_hex(JsonObject& root)//这个函数处理了串口传进来的点阵信息,从simsun.ttc中得到,有些都不是完整的16行,需要补全等复杂操作,不如eqled从hzk*中得到工整的点阵信息来的简单
{
  uint8_t cols = root["cols"];
  uint8_t rows = root["rows"];
  uint8_t zhn  = root["zhn" ];
  
  WORDS*wds_ptr;
  wds_ptr = &wp[screen]; // select current "WORDS"
  uint8_t *ptr2;

  
  char buff[3];
  const char * ptr = root["hex"];
  buff[2] = '\0';
  int a;
  int diff = wds_ptr->font_size - rows;
  
  diff = (diff>=0)?diff:0; 

  wds_ptr->last_time = millis();
  
  wds_ptr->wd[zhn].cols = cols;
  wds_ptr->last_zhn = zhn;
  
  wds_ptr->cur_pos = 0;
  for(int i=0;i<zhn;i++)//compute the start position
  {
    wds_ptr->cur_pos += wds_ptr->wd[i].cols* wds_ptr->font_size;
  }

  ptr2 = wds_ptr->hex;
  
  //整个hex 字符串的长度是 cols*rows*2 
  for(int i=0;i<cols*rows*2;i+=rows*2)// search in root["hex"]
  {

    for(int q=0;q<diff;q++) //放这儿是要把 高度不到16的 rows的前几行空白rows填上0,让字儿是底部对齐
    {
      //wds_ptr->hex[wds_ptr->cur_pos+q]=0;
      *(ptr2+wds_ptr->cur_pos+q)=0;
    }
    wds_ptr->cur_pos+= diff;
    for(int j=0;j<rows*2;j+=2)// 这里循环的次数是 rows*2/2 次 
    {
      buff[0] = ptr[i+j];
      buff[1] = ptr[i+j+1];
      
      a = strtol(buff,NULL,16);
      //wds_ptr->hex[wds_ptr->cur_pos] = (uint8_t)a;
      *(ptr2+wds_ptr->cur_pos) = (uint8_t)a;
      wds_ptr->cur_pos++;
    }
   
    // 如果rows不等于 font_size的高度,要补全 rows的长度,做到每个"字符"的宽和高是统一的8*font_size;
  }

  reset_effect(wds_ptr);
}

void json_zhlen(JsonObject& root)
{
  set_cur_screen( root["screen"]);
  
  wp[screen].zhlen = root["zhlen"];// 字儿在GBK下的计数
  wp[screen].asclen = root["asclen"];//字儿在ascii下的计数
  wp[screen].eff.idx = root["effect"];

}

void json_debug(JsonObject& root)
{
  String toprint = "";
  toprint += "font_size: ";
  toprint += wp[screen].font_size;
  int j,k,u,v;
  uint8_t*ptr;
  Serial.println(toprint);
  toprint = "cur_pos: ";
  toprint+= wp[screen].cur_pos;

  Serial.println(toprint);


  // 按字打印出来所有的bitmap 信息
  /*
  j = get_string_count(&wp[screen]);
 
  for(int i=0;i<j;i++)
  {
    v = get_hex_offset((uint8_t)i,&wp[screen]);
    for(k=v;k<(v+wp[screen].wd[i].cols*wp[screen].font_size);k++)
    {
      Serial.print(wp[screen].hex[k],HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  */  
  for(int i=0;i<get_string_length(&wp[screen]);i++)
  {
    for(k=0;k<wp[screen].font_size;k++)
    {
      ptr = wp[screen].hex;
      ptr+= wp[screen].font_size*i+k;
      
      Serial.print(ptr[0],HEX);
      Serial.print(" ");
    }
    Serial.println();
  }
  
  Serial.print("total screen:");
  Serial.println(cfg.total_scr);
  Serial.print("Width: ");
  Serial.println(cfg.width);
  Serial.print("Height: ");
  Serial.println(cfg.height);

  Serial.print("string length: ");
  Serial.println( get_string_length(&wp[screen]));


  Serial.print("Effect: ");
  for(int i=0;i<cfg.total_scr;i++)
  {
    Serial.print(wp[i].eff.idx);
    Serial.print(" ");
  }
  Serial.println();
  
}

void json_cfg(JsonObject& root)
{
   int w,h;
   int all_scr;
   w = root["w"];
   h = root["h"];
   all_scr = root["scrs"];
   
   config_set(&cfg,SCRS,all_scr);
   config_set(&cfg,WIDTH,w);
   config_set(&cfg,HEIGHT,h);
   
   init_words(&cfg);//reset all words
   for(int i=0;i<all_scr;i++)
   {
      wp[i].font_size = root["fs"][i];
   }
   
   led_setup(&cfg);//update led buffer
}

void json_func_callbacks( JsonObject& root )
{
  int i;
  
  if( root.containsKey("zhlen"))
  {
    json_zhlen(root);
  }else if( root.containsKey("hex"))   
  {
    
    json_hex(root);
  }else if( root.containsKey("debug"))
  {
    json_debug(root);
  }else if( root.containsKey("cfg"))
  {
    json_cfg(root);
  }
  
  
}

int serial_parse_json(char*buffer)
{
  StaticJsonBuffer<1024> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(buffer);

  if(!root.success()) 
  {
    Serial.println("ArduinoJson parseObject() failed ");
    return -1;
  }else
  {
    //parse serial input data 
    //Serial.println("ArduinoJson parse json success");
    json_func_callbacks( root );
    return 0;
  }  
}



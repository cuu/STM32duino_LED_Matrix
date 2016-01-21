#include "eqled.h"
#include "gspi.h"


///针对上海EQLED 的协议的处理
//只是显示部分,配置 还是用config(json格式)
/*
 * 
 * !#: 起始标志(必选项)
 * 001:控制卡地址(必选项)
 * $$: 结束标志(必选项)
 * %ZI 01 //指定 screen
 * %ZA 01 设置区域的特技
 * eg:
 * !#001%ZI02%ZA04FUCK$$
 * !#001%ZI02%ZA04啊$$
 */

void eqled_setup()
{

}

int serial_parse_eqled(char*buffer,CONFIG*cfg)
{
  String buff(buffer);
  int len = buff.length();
  int pos;
  char tmp[5];
  tmp[2] = '\0';
  int i,k;
  i=0;
  k=0;
  uint8_t scr;
  WORDS*w;
  uint32_t addr;
  uint8_t bytes;
  uint8_t cols;
  uint8_t zhn_number;
  uint8_t *ptr;
  uint8_t offset_y;
  
  w = NULL;
  scr = 0;
  zhn_number = 0;
  offset_y = 0;
  
  if(buff.startsWith("!#") && buff.endsWith("$$"))//legal
  {
    Serial.println("serial_parse_eqled ok");
    pos = buff.indexOf("!#");
    for(i=pos;i<buff.length();i++)
    {
      if(buff.charAt(i) == '%' && (i+4)< buff.length() )
      {
        if(buff.charAt(i+1) == 'Z' && buff.charAt(i+2) == 'I')
        {
          memset(tmp,0,sizeof(tmp));
          tmp[0] = buff.charAt(i+3);
          tmp[1] = buff.charAt(i+4);
          //Serial.print("%ZI ");Serial.println(strtol(tmp,NULL,0));
          scr =  strtol(tmp,NULL,10);
          
          offset_y = 0;
          if(scr< cfg->total_scr)
          { 
            for(uint8_t j=0;j<scr;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
            {
              w = get_words_pointer(j);
              offset_y += w->font_size;
            }
            w = get_words_pointer(scr);
            led_clear_screen(cfg->width,cfg->height,offset_y,offset_y+w->font_size);

            set_cur_screen( scr );
          }
          
          i+=4;pos = i;
        }
        
        if(buff.charAt(i+1) == 'Z' && buff.charAt(i+2) == 'A')
        {
          memset(tmp,0,sizeof(tmp));
          tmp[0] = buff.charAt(i+3);
          tmp[1] = buff.charAt(i+4);
          //Serial.print("%ZA ");Serial.println(strtol(tmp,NULL,0)); 
          scr = get_cur_screen();
          w = get_words_pointer(scr);
          set_effect(w,strtol(tmp,NULL,10));
          i+=4;pos = i;   
        }
        
        if(buff.charAt(i+1) == 'Z' && buff.charAt(i+2) == 'H')//选择stalled的时间
        {
          //%ZH0001 代表最小 ,参数x0.1秒 =  参数x 100ms ,最大是 9999x100ms = 999.9s=1000秒,最小是100ms(毫秒)
          
          memset(tmp,0,sizeof(tmp));
          tmp[0] = buff.charAt(i+3);
          tmp[1] = buff.charAt(i+4);
          tmp[2] = buff.charAt(i+5);
          tmp[3] = buff.charAt(i+6);
          
          scr = get_cur_screen();
          w = get_words_pointer(scr);
         
          set_stalled_time(w,(uint16_t)strtol(tmp,NULL,10));
          
          i+=6;pos = i;  
        }
        
      }      
    }
    
    String content = buff.substring(pos+1,buff.lastIndexOf("$$"));
    //Serial.print("content: "); Serial.println(content);
    
    scr = get_cur_screen();
    w = get_words_pointer(scr);
    
    if(w == NULL) return -1;
    ptr = w->hex;
    
    for(int j=0;j<content.length();j++)//是字儿
    {
      cols = w->font_size >>3; // 除以8 
      
      switch(w->font_size)
      {
        case FONT_PIX16:
        {
          if(ifasc(content.charAt(j)))
          {
            bytes = w->font_size*(cols>>1);
            addr = ASC16_BEGIN+content.charAt(j)*bytes;
            //Serial.println(addr);
            gspi_read_zk(addr,gspi_buf,bytes);

            /*
            for(int u=0;u<cols/2;u++)
            for(int q=0;q<w->font_size;q++)
            {
              Serial.print(gspi_buf[q*(cols/2)+u],HEX);
            }
            
            Serial.println();
            */
            w->wd[zhn_number].cols = cols>>1;//除以2
            w->last_zhn = zhn_number;
            w->cur_pos = 0;            
            for(int q=0;q<zhn_number;q++)//compute the start position
            {
              w->cur_pos += w->wd[q].cols* w->font_size;
            }
            
            for(int u=0;u<cols>>1;u++)
            {
              for(int q=0;q<w->font_size;q++)
              {
                *(ptr+w->cur_pos) = gspi_buf[q*(cols>>1)+u];
                w->cur_pos++;
              }
            }            

          }else // not ASCII
          {
            
            bytes = w->font_size*cols;
            addr = HZK16_BEGIN + ( 94* (content.charAt(j) - 0xa0 -1) + (content.charAt(j+1) - 0xa0 -1))*bytes;
            gspi_read_zk(addr,gspi_buf,bytes);
            
            //because bytes = cols*font_size ,so ..
            //一条一条的
            j+=JUMP;//跳一个字节
            
            w->wd[zhn_number].cols = cols;
            w->last_zhn = zhn_number;
            w->cur_pos = 0;
            for(int q=0;q<zhn_number;q++)//compute the start position
            {
              w->cur_pos += w->wd[q].cols* w->font_size;
            }
            
            for(int u=0;u<cols;u++)
            {
              for(int q=0;q<w->font_size;q++)
              {
                *(ptr+w->cur_pos) = gspi_buf[q*cols+u];
                w->cur_pos++;
              }
            }
          }
        }break;
        
      }// end switch w->font_size

      zhn_number++;
      Serial.print(".");
    }
    
    Serial.println();
    reset_effect(w);

#ifdef LEFT_RIGHT_ADJUST  
    if(w->eff.idx ==LEFTMOVE)
    {
      led_effect_left(scr,cfg);
    }
    
    if(w->eff.idx ==RIGHTMOVE)
    {
      led_effect_right(scr,cfg);
    } 
#endif       
  }
  return 0;
}


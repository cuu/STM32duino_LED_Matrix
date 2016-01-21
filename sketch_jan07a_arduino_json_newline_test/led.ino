#include "led.h"
#include "words.h"

// led 是主要的执行者,WORDS,CONFIG等主要是配合其工作的周边 
// json主要是处理从串口输入的指令

//这边的Ascii字是 8x16,16行,8个pixels
//这边要一六一六的画这个字,才能让moveLeft不停的>> 操作
//y == 0 or 16

byte led_get_fontrows(byte ch)
{
  byte fontrows;
  
  fontrows = 16;//字体的rows,与WORDS里的要一样
  
  return fontrows;
}

int led_get_idx(byte ch)// index in font8x16[]
{
    byte font,fontrows;
    int index;

    font = 0;

    fontrows = led_get_fontrows(ch);
    
    if ((ch>=0) && (ch<=9)) index=(ch+16)*fontrows; 
    else index=(ch-32)*fontrows; // go to the right code for this character

    return index;
}

void led_clear_screen( int w,int h,byte rowstart, byte rowstop)
{
  //清除从 rowstart -> rowstop的buffer内容
  byte row;
  int column; //必须是int类型
  short unsigned int address;
  int cols;
  cols = w>>3;
  column = cols;
       
  for (column=0;column<cols;column++) // X
  { 
    for (row=rowstart;row<rowstop;row++) // Y
    {
      //address = (row<<3) + column; /// right here! row <<3 == row * 8
      address = (row*cols) + column;
      buffer[address] = 0x00;
    }
  }
     
}
void led_clear_one_line(int w,int h,uint16_t x,uint16_t y,byte font,byte row,byte*buffer,int message_length) //上下移时,要清行
{
  byte charbyte, i;// = x>>3; 
  int index;
  
  byte *pDst;
  int cols;
  cols = w>>3;
  x = x>>3;// in which column  

  for(i=0;i<message_length;i++) //边历message
  {
    if((i+x)<cols)
    {
      index = font*i;
      //row = row % font;//check row 
      //if(font ==0)
      charbyte = 0x00;
      pDst = buffer + y*cols + i+x;
      *pDst = charbyte;
    }else break;
  }  
}

//move down/up 就是一行一行的撸
void led_set_one_line(int w,int h,uint16_t x,uint16_t y,byte font,byte row,byte*buffer,int message_length,uint8_t*font_bitmap)
{
  byte charbyte, i;// = x>>3; 
  int index;
  
  byte *pDst;
  int cols;
  cols = w>>3;
  x = x>>3;// in which column
  
  while(y>= h) y--;
  
  for(i=0;i<message_length;i++) //边历message
  {
    if((i+x)<cols)
    {
      index = font*i;
      //row = row % font;//check row 
      //if(font ==0)
      charbyte = font_bitmap[index+row];
      pDst = buffer + y*cols + i+x;
      *pDst = charbyte;
    }else break;
  }

}

void led_set_one_col(int w,int h, uint16_t x, uint16_t y,uint8_t pixel, byte ch, byte font,byte apix,byte *buffer,uint8_t*font_bitmap) 
{ 
    uint8_t xover8 = x>>3; 
    //x>>3 就等于 x/8
    byte pix;
    
    int cols = w >> 3;
    int index;

    if(xover8 >= cols)
      xover8 = cols- 1;

    if(y >= h) return;  
    //index = led_get_idx(ch);
    index = font*ch;
    byte *pDst = buffer + y*cols + xover8;
    uint8_t *pSrc = font_bitmap +index; // point at the first set of 8 pixels  
    
    
    for(byte i=0;i<font;i++)//一竖条一竖条的对 这"块" 进行改变
    {
      pix = (*pSrc & ( 0x80 >> pixel));
      if(pix)
      {
        *pDst |= 0x80 >> apix;
      }else
      {
        *pDst &= ~(0x80 >> apix);
      }
      pDst += cols;
      pSrc++;
    }

}

void led_move_up( int w,int h,byte rowstart, byte rowstop,byte *buffer)// start 比 stop 要大,如 31->0
{
  byte x,y;
  short unsigned int address;
  short unsigned int address1;
  int cols;
  cols = w>>3;
  
  if(rowstart < rowstop) return; //rowstart必须大于rowstop
  
  y = rowstop;
  
  while(y <rowstart)
  {
    for(x=0;x<cols;x++)
    {
      address = y*cols + x;
      address1 = (y+1)*cols+x;
      buffer[address] = buffer[address1];
    }
    y++;
  }
  return;  
}

void led_move_down( int w,int h,byte rowstart, byte rowstop,byte *buffer)//eg:0-31
{
  byte x,y;
  short unsigned int address;
  short unsigned int address1;
  int cols;
  cols = w>>3;//除以8
  
  y = rowstop;
  
  while(y>=h) y--;
  
  while(y >rowstart )
  {
    for(x=0;x<cols;x++)
    {
      address = y*cols + x;
      address1 = (y-1)*cols+x;
      buffer[address] = buffer[address1];
    }
    y--;
  }
}

void led_move_right( int w,int h,byte rowstart, byte rowstop,byte *buffer) //1,1,32
{ 
  byte row;
  int column; //必须是int类型
  byte pixels = 1;
  short unsigned int address;
  int cols;
  cols = w>>3;
  column = cols-1;
  
  while(column >= 0)//用while,并且一定和moveLeft一样,倒着来
  { 
    for (row=rowstart;row<rowstop;row++) // Y
    {
      //address = (row<<3) + column; /// right here! row <<3 == row * 8
      address = (row*cols) + column;
      if (column==0) 
         buffer[address] = buffer[address] >> pixels; // shuffle pixels left on last column and fill with a blank
        else 
        {                // shuffle pixels left and add leftmost pixels from next column
         byte prevchar   = buffer[address-1];//同row上的取地址
         buffer[address] = buffer[address] >> pixels;
         for (byte x=0;x<pixels;x++) 
         {           
          buffer[address]+=((prevchar & (1 << x)) >> x) << (7-pixels+1); 
          //上句是取出rightmost的pixels位的2进制值,0或值1,最后<< 进位成为相应的数据,如 10,11 或是单个 0 或是 1
         }
        }
     }
     column--;
  }
}

void led_move_left( int w,int h,byte rowstart, byte rowstop,byte *buffer) //2,1,32
{ 
  //moveLeft就是对整个显示屏的byte进行点化的移动
  //目前的问题是如何在这个屏上开始画出第一条字儿,然后不停的跟据 pixels进行画点....
  
  // routine to move certain rows on the screen "pixels" pixels to the left
  byte row, column; 
  short unsigned int address;
  byte pixels = 1;
  int cols;
  cols = w>>3;

  if( rowstart >= h) return;
  
  for (column=0;column<cols;column++) // X
  { 
    for (row=rowstart;row<rowstop;row++) // Y
    {
      //address = (row<<3) + column; /// right here! row <<3 == row * 8
      address = (row*cols) + column;
      if (column==(cols-1)) 
         buffer[address] = buffer[address]<<pixels; // shuffle pixels left on last column and fill with a blank
        else 
        {                // shuffle pixels left and add leftmost pixels from next column
         byte incomingchar = buffer[address+1];//同row上的取地址
         
         buffer[address] = buffer[address]<<pixels;//2进制上,是在往左填pixels 个 0 移动 ,因为这儿往左移了pixels个0,然后才有下面的补上下一个column的leftmost的pixels个2进制进来
         for (byte x=0;x<pixels;x++) 
         { 
          buffer[address] += ((incomingchar & (0x80 >> x)) >> (7 - x)) <<  ( pixels - x - 1); //与上buffer[address] << pixels 配合
          //上句是取出leftmost的pixels位的2进制值,0或值1,最后<< 进位成为相应的数据,如 10,11 或是单个 0 或是 1
         }
       }
     }
  }
}


void led_shift_out(byte row,CONFIG*cfg)//R1管半屏,R2,管半屏,再有屏,就要更多的R1,R2,G1,G2来管 
{ // fast routine to shove out 8 columns into *two* rows via board's shift registers

  uint16_t half_scr;
  half_scr = (cfg->width>>3)*(cfg->height>>1);//
  
  for(byte column=0;column<(cfg->width>>3);column++) 
  {
    //byte index = (row<<3)+column; //row << 3 代表 * 8 ,每次 
    byte index = row*(cfg->width>>3) + column;
    
    for(byte i=0;i<BITS_WIDTH;i++) 
    { 
      //digitalWrite(data_R1,LOW); digitalWrite(data_R2,LOW);//有或是没有,都一样
      digitalWrite(clockPin,LOW);

      digitalWrite(data_R1,  !((buffer[index]>>(7-i)) & 0x01)); // r1 与 r2是同时显示的
      //上句是和bits功效一样,把buffer[index]的8位一位一位中的 1 的通过&0x01 给摘出来,并且是按 位顺序的
       
      digitalWrite(data_R2,  !((buffer[index+half_scr]>>(7-i)) & 0x01)); //64*16/8 半屏
      // 上句就是 把 buffer中r2的内容用类似上上句的功效一样,摘出1来显示,然后是reverse掉,依然是低电平有效,高电平无效
      digitalWrite(clockPin,HIGH);
    }
  }
}

void led_set_row(int row)
{
  uint8_t val;

  val = (row & 0x01) > 0?1:0;
  //gpio_write_bit(GPIOB,8, val);
  digitalWrite(la_74138,val);
  val = (row & 0x02) > 0?1:0;
//  gpio_write_bit(GPIOB,10,val);
  digitalWrite(lb_74138,val);

  val = (row & 0x04) > 0?1:0;
//  gpio_write_bit(GPIOB,11,val);
  digitalWrite(lc_74138,val);

  val = (row & 0x08) > 0?1:0;
//  gpio_write_bit(GPIOB,12, val);
  digitalWrite(ld_74138,val);

}

void led_scan(CONFIG*cfg)
{
     //cli(); // clear interrupts
     noInterrupts();
     digitalWrite(en_74138, HIGH);     // Turn off display
     
     led_shift_out(ScanRow,cfg);                // Shift out 8 columns
     
     digitalWrite(latchPin, LOW);
     digitalWrite(latchPin, HIGH);

     
     led_set_row(ScanRow);
     digitalWrite(en_74138, LOW);     // Turn on display
     
     ScanRow++;                       // Do the next pair of rows next time this routine is called
     if (ScanRow==16) ScanRow=0;  

     //sei();// enable/allow interrupts
     interrupts();
}
  
void led_init_gpio()
{
  pinMode(latchPin,OUTPUT);  pinMode(clockPin,OUTPUT);
  pinMode(data_R1,OUTPUT);   pinMode(data_R2,OUTPUT);
    
  pinMode(en_74138,OUTPUT);
  pinMode(la_74138,OUTPUT);  pinMode(lb_74138,OUTPUT);
  pinMode(lc_74138,OUTPUT);  pinMode(ld_74138,OUTPUT);
  
  digitalWrite(en_74138, LOW);
  digitalWrite(data_R1, HIGH); 
  digitalWrite(data_R2, HIGH);  
}

void led_setup(CONFIG*) 
{
  pinMode(BOARD_LED_PIN, OUTPUT); 
  led_init_gpio(); // gpio 会导致timer 不能工作,可能是占用了Timer的端口,影响了Timer
  
  f = 0;
  
  if(buffer != NULL)
  {
    free(buffer);
    buffer = NULL;
  }
  
  buffer = (unsigned char*)malloc(cfg.width*(cfg.height>>3));
  memset(buffer,0,cfg.width*(cfg.height>>3));
}

void led_effect_right(uint8_t i,CONFIG*cfg)// align right display
{
    uint8_t len;
    WORDS*w;
    int offset_y;
    int total_pix;
    w = &wp[i];
    len = get_string_length(w);
    len = (len >0)?len:(cfg->width>>3);
    offset_y = 0; 
    for(uint8_t j=0;j<i;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
    {
      offset_y += wp[j].font_size;
    }
    w->eff.count = len-1; //always reset
    w->eff.pixel_count =7;
    
    total_pix = cfg->width-1;
    w->eff.moved_pix=0;
    
    while(w->last_zhn >= 0)
    {
      
      led_set_one_col(cfg->width,cfg->height,total_pix,offset_y,w->eff.pixel_count,w->eff.count%len,w->font_size,w->eff.pixel_count,buffer,w->hex);    
      w->eff.pixel_count--;
      total_pix--;
      w->eff.moved_pix++;
      if(w->eff.pixel_count < 0)
      {
        w->eff.count--; // next word
        w->eff.pixel_count = 7;
      }
      
      if(total_pix < 0) break;
      if( w->eff.count < 0) break; //如果到最后一个字,就break
    }
        
}


void led_effect_left(uint8_t i,CONFIG*cfg)// align left display
{
    uint8_t len;
    WORDS*w;
    int offset_y;
    int total_pix;
    w = &wp[i];
    len = get_string_length(w);
    len = (len >0)?len:(cfg->width>>3);
    offset_y = 0; 
    for(uint8_t j=0;j<i;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
    {
      offset_y += wp[j].font_size;
    }
    w->eff.count = 0; //always reset
    total_pix = 0;
    w->eff.moved_pix=0;
    while(w->last_zhn >= 0)
    {
      
      led_set_one_col(cfg->width,cfg->height,total_pix,offset_y,w->eff.pixel_count,w->eff.count%len,w->font_size,w->eff.pixel_count,buffer,w->hex);    
      w->eff.pixel_count++;
      total_pix++;
      w->eff.moved_pix++;
      if(w->eff.pixel_count >7)
      {
        w->eff.count++; // next word
        w->eff.pixel_count = 0;
      }
      
      if(total_pix>= cfg->width) break;
      if( w->eff.count >= len && (w->eff.count % len) == 0) break; //如果到最后一个字,就break
    }
        
}

void led_effect_movedown(uint8_t i,CONFIG*cfg)// down
{
    uint8_t len;
    WORDS*w;
    int offset_y;
    
    w = &wp[i];//根据编号挑对的WORDS  
    len = get_string_length(w);
    len = (len >0)?len:(cfg->width>>3);
    offset_y = 0;
    for(uint8_t j=0;j<i;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
    {
      offset_y += wp[j].font_size;
    }

    led_move_down(cfg->width,cfg->height,offset_y, offset_y+w->font_size-1,buffer);//eg:0-31   
    
    if(w->eff.last_pix_count < (w->font_size<<1))//font_size即屏的高度
    {
      if(w->eff.count >= 0)
      {
        led_set_one_line(cfg->width,cfg->height,0,offset_y,w->font_size,w->eff.count,buffer,len,w->hex); 
        w->eff.count--;
      }else
      {
        led_clear_one_line(cfg->width,cfg->height,0,offset_y,w->font_size,0,buffer,len); 
      }
      w->eff.last_pix_count++;
      w->eff.moved_pix++;
    }
    else
    {
      w->eff.count = w->font_size-1;
      w->eff.last_pix_count = 0;
      w->eff.moved_pix=0;
    }  
    
}

void led_effect_moveup(uint8_t i,CONFIG*cfg)// up up up
{
    uint8_t len;
    WORDS*w;
    int offset_y;
    
    w = &wp[i];//根据编号挑对的WORDS  
    len = get_string_length(w);
    len = (len >0)?len:(cfg->width>>3);
    offset_y = 0;
    for(uint8_t j=0;j<i;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
    {
      offset_y += wp[j].font_size;
    }

    led_move_up(cfg->width,cfg->height,offset_y+w->font_size-1,offset_y,buffer);//eg:31-0
    
    if(w->eff.last_pix_count < (w->font_size<<1))
    {
      if(w->eff.count < w->font_size)
      {
        led_set_one_line(cfg->width,cfg->height,0,offset_y+w->font_size-1,w->font_size,w->eff.count,buffer,len,w->hex); 
        w->eff.count++;
      }else
      {
        led_clear_one_line(cfg->width,cfg->height,0,offset_y+w->font_size-1,w->font_size,0,buffer,len); //最后一行
      }
      
      w->eff.moved_pix++;
      w->eff.last_pix_count++;
    }
    else
    {
      w->eff.count = 0;
      w->eff.last_pix_count = 0;
      w->eff.moved_pix = 0;
    }
    
}

void led_effect_moveright(uint8_t i,CONFIG*cfg)
{
    uint8_t len;
    WORDS*w;
    int offset_y;
    
    w = &wp[i];//根据编号挑对的WORDS  
    len = get_string_length(w);
    len = (len >0)?len:(cfg->width>>3);
    offset_y = 0;
    for(uint8_t j=0;j<i;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
    {
      offset_y += wp[j].font_size;
    }
    
    led_move_right( cfg->width, cfg->height,offset_y,offset_y+w->font_size,buffer);

    /*
    if(w->eff.moved_pix == (cfg->width-1))
    {
      delay(w->eff.stalled*100);
    }
    */    
    //if( w->eff.count >= len && (w->eff.count % len) == 0 && w->last_zhn >=0)
    if(w->eff.count < 0 && w->last_zhn >= 0)
    {
      w->eff.moved_pix++;
      w->eff.last_pix_count++;
    }
    
    if(w->eff.last_pix_count > cfg->width)
    {
       reset_effect(w);
       w->eff.moved_pix=0;
    }
    
    if(w->eff.last_pix_count == 0 && w->last_zhn >=0)
    {
      //set_one_col(64,32,64-1,0,pixel_count,message[count%(sizeof(message)-1)],0,7,buffer);
      //set_one_col(64,32,0,0,pixel_count,message[count%(sizeof(message)-1)],0,7,buffer);
      
      led_set_one_col(cfg->width,cfg->height,0,offset_y,w->eff.pixel_count,w->eff.count%len,w->font_size,0,buffer,w->hex);
      
      //led_set_one_col(64,32,63,0,w->eff.pixel_count,w->eff.count%strlen(w->string),w->font_size,0,buffer,font8x16);
      
      w->eff.pixel_count--;
      if(w->eff.pixel_count <0)
      {
        w->eff.count--;
        w->eff.pixel_count = 7;
      }
      w->eff.moved_pix++;
    }       
}

void led_effect_moveleft(uint8_t i,CONFIG*cfg)
{
    uint8_t len;
    WORDS*w;
    int offset_y;
    w = &wp[i];//根据编号挑对的WORDS
    len = get_string_length(w);
    len = (len >0)?len:(cfg->width>>3);
      
    offset_y = 0;
    for(uint8_t j=0;j<i;j++) //要有一个Y的偏移,这个偏移是 之前所有屏的Y的总和
    {
      offset_y += wp[j].font_size;
    }
    
    led_move_left(cfg->width,cfg->height,offset_y,offset_y+w->font_size,buffer);//moveLeft只管屏的缓存,不管字儿的,
    
    if( w->eff.count >= len && (w->eff.count % len) == 0 && w->last_zhn >=0)
    {
      w->eff.moved_pix++;
      w->eff.last_pix_count++;
    }
    
    if(w->eff.last_pix_count > cfg->width)
    {
       reset_effect(w);
       w->eff.moved_pix=0;
    }
    
    if(w->eff.last_pix_count == 0 && w->last_zhn >=0)
    {
      //set_one_col(64,32,64-1,0,pixel_count,message[count%(sizeof(message)-1)],0,7,buffer);
      //set_one_col(64,32,0,0,pixel_count,message[count%(sizeof(message)-1)],0,7,buffer);
      
      led_set_one_col(cfg->width,cfg->height,cfg->width,offset_y,w->eff.pixel_count,w->eff.count%len,w->font_size,7,buffer,w->hex);
      
      //led_set_one_col(64,32,63,0,w->eff.pixel_count,w->eff.count%strlen(w->string),w->font_size,0,buffer,font8x16);
      
      w->eff.pixel_count++;
      if(w->eff.pixel_count >7)
      {
        w->eff.count++;
        w->eff.pixel_count = 0;
      }
      
      w->eff.moved_pix++;
    }

}

int led_check_stalled_time(WORDS*w,CONFIG*cfg)
{

  
  switch(w->eff.idx)
  {
    case LEFTMOVE:
    case RIGHTMOVE:
    {
      if(w->eff.moved_pix == cfg->width)
      {
        if(millis() - w->eff.last_time >= w->eff.stalled*DELAY_UNIT)
        {
          w->eff.last_time = 0;
          
          return 0;
        }
        else return -1;
      }else return 0;
    }break;
    case UPMOVE:
    case DOWNMOVE:
    {
      if(w->eff.moved_pix == w->font_size)
      {
        if(millis() - w->eff.last_time >= w->eff.stalled*DELAY_UNIT)
        {
          w->eff.last_time = 0;
          
          return 0;
        }
        else return -1; 
      }else return 0;
    }break;
    default:
      return 0;
  }
}

void led_loop(CONFIG*cfg)
{

  for(int i=0;i<cfg->total_scr;i++)
  {
    switch(wp[i].eff.idx)
    {
      case LEFTMOVE:
      case RIGHTMOVE:
      {
        if(wp[i].eff.moved_pix == (cfg->width))
        {
          if(wp[i].eff.last_time == 0)
          {
            wp[i].eff.last_time = millis();
          }
          
        }
      }break;
      case UPMOVE:
      case DOWNMOVE:
      {
        if(wp[i].eff.moved_pix == wp[i].font_size )
        {
          if(wp[i].eff.last_time == 0)
          {
            wp[i].eff.last_time = millis();
          }
        }
      }break;
    }
  }

  
  for(int i=0;i<cfg->total_scr;i++)
  {
    if(millis() - wp[i].last_time  >= wp[i].rate  )
    {
      switch(wp[i].eff.idx)
      {
        case LEFT:
        {
         led_effect_left(i,cfg);
        }break;
        case LEFTMOVE:
        {
          if(led_check_stalled_time(&wp[i],cfg) == 0)
          {
            led_effect_moveleft(i,cfg);
          }
        }break;
        case RIGHT:
        {
          led_effect_right(i,cfg);
        }break;
        case RIGHTMOVE:
        {
          if(led_check_stalled_time(&wp[i],cfg) == 0)
          {
            led_effect_moveright(i,cfg);
          }
        }break;
        case UPMOVE:
        {
          if(led_check_stalled_time(&wp[i],cfg) == 0) //时间没到,或者是时间过了
          {
            led_effect_moveup(i,cfg); 
          }
        }break;
        case DOWNMOVE:
        {
          if(led_check_stalled_time(&wp[i],cfg) == 0) //时间没到,或者是时间过了
          {
            led_effect_movedown(i,cfg);
          }
        }break;
      }

      wp[i].last_time = millis();
    }
  }
}


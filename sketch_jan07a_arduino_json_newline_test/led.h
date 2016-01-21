#ifndef LED_H
#define LED_H

#include "config.h"
/*
 * 74595 shift register LED 控制屏
 * 一个用Timer控制的 LED屏
 * DMA是更好的处理办法,刷新率在3.xMhz左右
 * LED屏是刷的越多,单次显示效果越稳定
 */
#define BITS_WIDTH 8
#define DELAY_UNIT 100 //100毫秒

  #define BOARD_LED_PIN PC13
  int f;
  
 const byte latchPin=PB15;
 const byte clockPin=PA8;
 const byte data_R1=PB0;
 const byte data_R2=PA1;
 const byte data_G1=PB13;
 const byte data_G2=PB14;
 const byte en_74138=PB1;
 const byte la_74138=PB8;
 const byte lb_74138=PB10;
 const byte lc_74138=PB11;
 const byte ld_74138=PB12;
 
  
 /* guu version
 const byte latchPin=PA14;
 const byte clockPin=PA15;
 const byte data_R1=PA9;
 const byte data_R2=PA10;
 const byte en_74138=PA8;
 const byte la_74138=PA0;
 const byte lb_74138=PA1;
 const byte lc_74138=PA2;
 const byte ld_74138=PA3;
  */
  
  byte ScanRow = 0;
  unsigned long counter;
  
  unsigned char* buffer=NULL;

void led_effect_left(uint8_t i,CONFIG*cfg);// align left display
void led_effect_right(uint8_t i,CONFIG*cfg);// align right display;

  void led_move_down( int w,int h,byte rowstart, byte rowstop,byte *buffer);//eg:0-31
  void led_move_up( int w,int h,byte rowstart, byte rowstop,byte *buffer);// start 比 stop 要大,如 31->0  
  void led_move_right( int w,int h,byte rowstart, byte rowstop,byte *buffer);// 1,1,32
  void led_move_left( int w,int h,byte rowstart, byte rowstop,byte *buffer); //2,1,32
  byte led_get_fontrows(byte ch);
  int  led_get_idx(byte ch);// index in font8x16[]

  void led_set_one_line(int w,int h,uint16_t x,uint16_t y,byte font,byte row,byte*buffer,char*message,char*font_bitmap);  
  
  void led_set_one_col(int w,int h, uint16_t x, uint16_t y,uint8_t pixel, byte ch, byte font,byte apix,byte *buffer,char*font_bitmap); 
  
  
  void led_shiftOut(byte row,CONFIG*cfg);
  void led_set_row(int row);
  void led_scan(CONFIG*cfg);
  
  void led_init_gpio();
  void led_setup(CONFIG*); //sth like arduino's setup
  void led_loop(CONFIG*);


#endif

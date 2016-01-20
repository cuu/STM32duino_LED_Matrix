#ifndef WORDS_H
#define WORDS_H

#include "config.h"

/*
 * 本LED屏所含的字的内容以及bitmap信息
 */
#define WORDS_LENGTH 2048 // 2k
#define ZH_MAX_WORDS 64 // 64bytes == 32 zh words  

enum framerate_list
{
  RATE10=100, // 100ms
  RATE20=50,
  RATE30=30,
  RATE60=16,
  RATE100=1
};

enum effect_lst
{
  NORMAL=0,
  LEFT, // 1
  LEFTMOVE, // 2
  RIGHT, // 3
  RIGHTMOVE, //4
  UPMOVE, //5 
  DOWNMOVE// 6
  
};

typedef struct _word
{
  //uint8_t rows; 就是 font_size
  uint8_t cols;
  
}WORD;

typedef struct _effect
{
  uint8_t  idx;//用哪个特效
  int  count;
  int pixel_count;
  int last_pix_count;
  uint16_t moved_pix;//计录移动过的次数,然后是 到"边上"了,就delay一下stalled*100的时间
  
  uint16_t stalled; //以ms为单位,是字在LED屏上停顿时间,配合LEFTMOVE,RIGHTMOVE等动态特效在屏上的小停顿,默认是0
  unsigned long last_time;
}EFFECT;

typedef struct _words
{
  
  uint8_t hex[WORDS_LENGTH]; //存所有的字的点阵信息
  WORD wd[ZH_MAX_WORDS/2]; // 记录 所有字的bitmap的宽度,用了几个0xff
  
  int      cur_pos; // hex 的seek postition
  uint8_t  font_size;// 这些字儿的统一font_size,因为拆成了8xfont_size,这儿的font_size实际上是一个字儿要的高度,多少个row
  uint8_t  zhlen;
  uint16_t asclen;
  int      last_zhn;
  
  EFFECT eff;//特效存值
  uint8_t rate;// frame rate
  unsigned long last_time=0;
  
  
}WORDS;

//并且,每条WORDS的开始Y位置,都是继承上一次WORDS的位置,也就是上一条WORDS的font_size的高度+Y+X,动态的哟
//这样上一个WORDS可以任意高度,甚至撑满全屏


WORDS *wp=NULL;

int ifasc(unsigned char a); //检查是否是 ascii 码
void reset_words(WORDS*w);

void set_stalled_time(WORDS*w,uint16_t tm);

void init_words(CONFIG*cfg);
uint8_t get_string_count(WORDS*w);
int get_hex_offset(uint8_t words_number,WORDS*w);
WORDS*get_words_pointer(uint8_t i);

#endif

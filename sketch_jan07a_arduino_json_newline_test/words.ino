#include "words.h"
#include "led.h"


int ifasc(unsigned char a) //检查是否是 ascii 码
{
  if(a > 31 && a < 128)
  return 1;
  else return 0;
  
}

int get_hex_offset(uint8_t words_number,WORDS*w)// 把words_number前的所有bitmap的长度加起来,成为最新的cur_pos,做为取bitmap的起点
{
    int v;
    uint8_t u;
    v = 0;
    for(u = 0;u<words_number;u++)
    {
      v+=w->wd[u].cols*w->font_size;
    }

    return v;
}

uint8_t get_string_length(WORDS*w) //所有字节数...
{
  uint8_t ret;
  ret = 0;

  if(w->last_zhn >=0)
  {
    for(int i=0;i<=w->last_zhn;i++)
    {
      ret+=w->wd[i].cols;
    }
    return ret;
  }else
    return 0;
  
}

uint8_t get_words_count(WORDS*w)//正确的获得string中汉字数
{
  return w->zhlen;
}


void reset_words(WORDS*w)
{
  w->cur_pos = 0;
  w->font_size = 16;
  
  memset(w->hex,   0,WORDS_LENGTH);
  memset(w->wd,    0,ZH_MAX_WORDS/2);
  w->eff.idx = LEFTMOVE;
  w->last_zhn = -1;
  w->rate = RATE20; 
  w->zhlen = 0;
  w->asclen = 0;
  reset_effect(w);
}

void set_stalled_time(WORDS*w,uint16_t tm)
{
  w->eff.stalled = tm;
}

void set_effect(WORDS*w, uint8_t eff)
{
  if(eff >  DOWNMOVE) return;

  w->eff.idx = eff;
  reset_effect(w);
}

void reset_effect(WORDS*w)
{
  switch(w->eff.idx)
  {
    case NORMAL:
    {
      w->eff.count = 0;
      w->eff.pixel_count = 0;
      w->eff.last_pix_count = 0;
      
    }break;
    case LEFT:
    case LEFTMOVE:
    {
      w->eff.count = 0;
      w->eff.pixel_count = 0;
      w->eff.last_pix_count = 0;     
      
    }break;
    case RIGHT:
    case RIGHTMOVE:
    {
      w->eff.count = get_string_length(w)-1;
      w->eff.pixel_count = BITS_WIDTH-1;
      w->eff.last_pix_count = 0;
    }break;
    case DOWNMOVE:
    {
      w->eff.count = w->font_size-1;
      w->eff.pixel_count =0;
      w->eff.last_pix_count = 0;
    }break;
    default:
    {
      w->eff.count = 0;
      w->eff.pixel_count = 0;
      w->eff.last_pix_count = 0;         
    }break;
  }

  w->eff.moved_pix=0;
}

WORDS*get_words_pointer(uint8_t i)
{
  return &wp[i];
}

void init_words(CONFIG*cfg) //从CONFIG*cfg的条件中,初始化+重置清0化 WORDS数组
{
  if(wp!=NULL)
  {
    free(wp);
  }
  wp = (WORDS*)malloc(sizeof(WORDS)*cfg->total_scr);
  for(int i=0;i<cfg->total_scr;i++)
  {
    reset_words(&wp[i]);
    wp[i].eff.stalled=0;//默认400ms
    wp[i].eff.last_time = 0;
  }

}


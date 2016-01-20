#ifndef CONFIG_H
#define CONFIG_H

#define DEFAULT_WIDTH  128
#define DEFAULT_HEIGHT 32


#define SCRS 0
#define WIDTH 1
#define HEIGHT 2

typedef struct _config
{
  int total_scr;//总屏数
  int width;//LED屏的宽,以pixel 为基础
  int height; // LED屏的高,以pixel 为基础
  
}CONFIG;

CONFIG cfg;

void init_config(CONFIG*);

void config_set(CONFIG*cfg,int key,int value);


#endif


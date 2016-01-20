#include "config.h"

void init_config(CONFIG*cfg)
{
  cfg->total_scr =2;
  cfg->width = DEFAULT_WIDTH;
  cfg->height= DEFAULT_HEIGHT;
}

void config_set(CONFIG*cfg,int key,int value)
{
  switch(key)
  {
    case SCRS:
    {
      cfg->total_scr = value;
    }break;
    case WIDTH:
    {
      cfg->width = value;
    }break;
    case HEIGHT:
    {
      cfg->height = value;
    }break;
  }
}


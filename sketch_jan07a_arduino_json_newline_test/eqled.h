#ifndef EQLED_H
#define EQLED_H

#include "config.h"

#include "json.h"
#include "words.h"

#define GBK 1

#define GBK_JUMP 1
#define UTF8_JUMP 2

#define JUMP 1 //EQ处理时,跳过的默认字节数

#ifdef GBK
#define JUMP GBK_JUMP
#endif

#ifdef UTF8
#define JUMP UTF8_JUMP
#endif


#define FONT_PIX16 16
#define FONT_PIX32  32

//#define LEFT_RIGHT_ADJUST 1   //代表LEFT 或是 RIGHT Move 首先会显示全部的字,然后开始慢慢的滚动

void eqled_setup();
int serial_parse_eqled(char*buffer,CONFIG*cfg);



#endif

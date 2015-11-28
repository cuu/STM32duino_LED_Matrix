#include "LEDMatrix.h"
#include "Arduino.h"

#if 1
#define ASSERT(e)   if (!(e)) { Serial.println(#e); while (1); }
#else
#define ASSERT(e)
#endif

LEDMatrix::LEDMatrix(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1, uint8_t r2,uint8_t g1,uint8_t g2, uint8_t stb, uint8_t clk)
{
    this->clk = clk;
    this->r1 = r1;
    this->r2 = r2;
    this->g1 = g1;
    this->g2 = g2;
    this->stb = stb;
    this->oe = oe;
    this->a = a;
    this->b = b;
    this->c = c;
    this->d = d;

    mask = 0xff;
    state = 0;
}

void LEDMatrix::begin(uint8_t *displaybuf, uint16_t width, uint16_t height)
{
    ASSERT(0 == (width % 32));
    ASSERT(0 == (height % 16));

    this->displaybuf = displaybuf;
    this->width = width;
    this->height = height;

    pinMode(a, OUTPUT);
    pinMode(b, OUTPUT);
    pinMode(c, OUTPUT);
    pinMode(d, OUTPUT);
    pinMode(oe, OUTPUT);
    pinMode(r1, OUTPUT);
    pinMode(r2, OUTPUT);
    pinMode(g1, OUTPUT);
    pinMode(g2, OUTPUT);
    pinMode(clk, OUTPUT);
    pinMode(stb, OUTPUT);

		// 初始化,禁用状态
		digitalWrite(g1,HIGH);
		digitalWrite(g2,HIGH);
		digitalWrite(r2,HIGH);
		digitalWrite(r1,HIGH);
    state = 1;
		row = 0;
}

void LEDMatrix::drawPoint(uint16_t x, uint16_t y, uint8_t pixel)
{
    if( x >= width)
    {
    	x = x - width;
    }
    if( y >= height )
    {
    	y = y - height;
    }

    uint8_t *byte = displaybuf + x / 8 + y * width / 8;
    uint8_t  bit = x % 8;

    if (pixel) {
        *byte |= 0x80 >> bit; //不改变原有bits的情况下,改变相应的bits的状态
    } else {
        *byte &= ~(0x80 >> bit);
    }
}

void LEDMatrix::drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t pixel)
{
    for (uint16_t x = x1; x < x2; x++) {
        for (uint16_t y = y1; y < y2; y++) {
            drawPoint(x, y, pixel);
        }
    }
}

void LEDMatrix::drawImage(uint16_t xoffset, uint16_t yoffset, uint16_t width, uint16_t height, const uint8_t *image)
{
    for (uint16_t y = 0; y < height; y++) {
        for (uint16_t x = 0; x < width; x++) {
            const uint8_t *byte = image + (x + y * width) / 8;
            uint8_t  bit = 7 - x % 8;
            uint8_t  pixel = (*byte >> bit) & 1;

            drawPoint(x + xoffset, y + yoffset, pixel);
        }
    }
}

void LEDMatrix::clear()
{
    uint8_t *ptr = displaybuf; // clear the displaybuf to be 0x00
    for (uint16_t i = 0; i < (width * height / 8); i++) {
        *ptr = 0x00;
        ptr++;
    }
}

void LEDMatrix::reverse()
{
    mask = ~mask;
}

uint8_t LEDMatrix::isReversed()
{
    return mask;
}




void LEDMatrix::scan()
{
		
    if (!state) 
    {
        return;
    }
		uint8_t flag;
		uint8_t row_tmp;

		if( row < 16 ) flag = 0;
		else if( row >=16 && row < 32 ) flag=1;

    	uint8_t *head = displaybuf + row * (width / 8); //每行每行的扫 byte为单位
      uint8_t *ptr = head;
//      head += width * 2;              // width * 16 / 8

      for (uint8_t byte = 0; byte < (width / 8); byte++) // 一行有多少个byte 
			{
				// byte < 64/8 ; byte++, byte={0-7}
        uint8_t pixels = *ptr;
        ptr++;
        pixels = pixels ^ mask;     // reverse: mask = 0xff, normal: mask =0x00
        for (uint8_t bit = 0; bit < 8; bit++) // 每个byte是8 个bit 
				{
          digitalWrite(clk, LOW);
					// 0x80>> bit{0-7} == 128 64 32 16 ||  8 4 2 1
					// 128 10000000
					// 64  01000000
					// 32  00100000
          // 16  00010000
					// 8   00001000
          // 4   00000100
          // 2   00000010
          // 1   00000001
					// so a number will be like "10010011" ,8 times to check the 1 or 0
          //digitalWrite(g1, pixels & (0x80 >> bit));
          //
      		if(flag == 0)
      		{
          	//digitalWrite(g1, HIGH); // disable green
          	digitalWrite(r1, pixels & (0x80 >> bit));
					}else if(flag == 1)
					{
	          //digitalWrite(g2, HIGH); // disable green
          	digitalWrite(r2, pixels & (0x80 >> bit));
					}
          //digitalWrite(r2, pixels & (0x80 >> bit));
          digitalWrite(clk, HIGH);
         }
      }

    digitalWrite(oe, HIGH);              // disable display

    // select row
  	if(flag == 0)
  	{
    	digitalWrite(a, (row & 0x01)); // 1
    	digitalWrite(b, (row & 0x02)); // 2
    	digitalWrite(c, (row & 0x04)); // 4
    	digitalWrite(d, (row & 0x08)); // 8
		}
		else if( flag == 1)
		{
			row_tmp = row - ( flag*16 );
    	digitalWrite(a, (row_tmp & 0x01)); // 1
    	digitalWrite(b, (row_tmp & 0x02)); // 2
    	digitalWrite(c, (row_tmp & 0x04)); // 4
    	digitalWrite(d, (row_tmp & 0x08)); // 8
		}
    // latch data
    digitalWrite(stb, LOW);
    digitalWrite(stb, HIGH);
    digitalWrite(stb, LOW);

    digitalWrite(oe, LOW);              // enable display

		row = (row + 1) & (height -1);
		//row++; if( row > (height -1)) { row = 0; }
}

void LEDMatrix::on()
{
    state = 1;
}

void LEDMatrix::off()
{
    state = 0;
    digitalWrite(oe, HIGH);
}

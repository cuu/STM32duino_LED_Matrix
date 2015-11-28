#include <Wire.h>
#include "LEDMatrix.h"

#define WIDTH   64
#define HEIGHT  32


#if 1
#define A  PB0
#define B  PB1
#define C  PB2
#define D  PB3
#define G1  PB4
#define G2  PB5
#define LAT PB6
#define CLK PB7
#define EN  PC13
#define R1  PC14
#define R2  PC15
#else
#define A  4
#define B  5
#define C  6
#define D  7
#define G1  12
#define G2  13
#define LAT 10
#define CLK 11
#define EN  8
#define R1  9
#define R2  3
#endif

//const int ledpins[11] = {PB0,PB1,PB2,PB3,PB4,PB5,PB6,PB7,PC13,PC14,PC15};
                       // 0  1   2   3   4   5   6   7   8    9    10
                       //LA  LB  LC  LD  G1  G2  LAT CLK EN   R1   R2


LEDMatrix matrix(A,B,C,D,EN,R1,R2,G1,G2,LAT,CLK);     // LEDMatrix(a, b, c, d, en, r1,r2,g1,g2, lat, clk);
// LEDMatrix(a, b, c, d, en, r1, lat, clk);
//LEDMatrix matrix(A,B,C,D,EN,R1,LAT,CLK);     // LEDMatrix(a, b, c, d, en, r1, lat, clk);
// Display Buffer 64 * 32 / 8 = 16x16=256
uint8_t displaybuf[WIDTH *HEIGHT / 8];

// 16 * 8 digital font
const uint8_t digitals[] = {
    0x00, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00, // 0,16
    0x00, 0x18, 0x78, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00, 0x00, 0x00, 0x00, // 1
    0x00, 0x3E, 0x63, 0x63, 0x63, 0x06, 0x06, 0x0C, 0x18, 0x30, 0x63, 0x7F, 0x00, 0x00, 0x00, 0x00, // 2
    0x00, 0x3E, 0x63, 0x63, 0x06, 0x1C, 0x06, 0x03, 0x03, 0x63, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, // 3
    0x00, 0x06, 0x0E, 0x1E, 0x36, 0x36, 0x66, 0x66, 0x7F, 0x06, 0x06, 0x1F, 0x00, 0x00, 0x00, 0x00, // 4
    0x00, 0x7F, 0x60, 0x60, 0x60, 0x7C, 0x76, 0x03, 0x03, 0x63, 0x66, 0x3C, 0x00, 0x00, 0x00, 0x00, // 5
    0x00, 0x1E, 0x36, 0x60, 0x60, 0x7C, 0x76, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x00, 0x00, 0x00, 0x00, // 6
    0x00, 0x7F, 0x66, 0x66, 0x0C, 0x0C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, // 7
    0x00, 0x3E, 0x63, 0x63, 0x63, 0x36, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x3E, 0x00, 0x00, 0x00, 0x00, // 8
    0x00, 0x1C, 0x36, 0x63, 0x63, 0x63, 0x37, 0x1F, 0x03, 0x03, 0x36, 0x3C, 0x00, 0x00, 0x00, 0x00, // 9
};

// (x, y) top-left position, x should be multiple of 8
void drawDigital(uint16_t x, uint16_t y, uint8_t n)
{
    if ((n >= 10) || (0 != (x % 8))) 
    {
        return;
    }

    uint8_t *pDst = displaybuf + y * (WIDTH / 8) + x / 8;
    const uint8_t *pSrc = digitals + n * 16;//换字儿
    for (uint8_t i = 0; i < 16; i++) 
    {
        *pDst = *pSrc;
        pDst += WIDTH / 8;// 8
        pSrc++;
    }
}


void setup()
{
    Serial.begin(9600);

    matrix.begin(displaybuf, WIDTH, HEIGHT);

    Serial.println("led matrix");

}

void loop()
{
    static uint32_t lastCountTime = 0;
    static uint8_t count = 0;

    matrix.scan();
    
    if ((millis() - lastCountTime) > 1000) 
    {
        lastCountTime = millis();
        matrix.clear();
        
        matrix.drawPoint(0,0,1);
        matrix.drawPoint(0,1,1);
        matrix.drawPoint(10,1,1);
        
        matrix.drawPoint(20,13,1);
        
        matrix.drawPoint(32,15,1);
        matrix.drawPoint(33,14,1);
        matrix.drawPoint(34,13,1);
        
        matrix.drawPoint(32,18,1);
        matrix.drawPoint(33,19,1);
        matrix.drawPoint(34,19,1);
        matrix.drawPoint(35,19,1);
        matrix.drawPoint(36,20,1);
        matrix.drawPoint(37,21,1);
        matrix.drawPoint(38,22,1);
        matrix.drawPoint(39,23,1);
        matrix.drawPoint(40,24,1);
        matrix.drawPoint(41,25,1); 
        
        matrix.drawPoint(28,25,1);
        
        matrix.drawPoint(42,26,1);                                         
        matrix.drawPoint(33,25,1);
        
        drawDigital( 40,0,count);
             
        count = (count + 1) & 0x7;
    }
}

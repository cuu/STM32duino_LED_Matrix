
#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__

 #include <stdint.h>

class LEDMatrix {
public:
    LEDMatrix(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t oe, uint8_t r1,uint8_t r2,uint8_t g1,uint8_t g2, uint8_t stb, uint8_t clk);

    /**
     * set the display's display buffer and number, the buffer's size must be not less than 512 * number / 8 bytes
     * @param displaybuf    display buffer
     * @param number        panels' number
     */
    void begin(uint8_t *displaybuf, uint16_t width, uint16_t height);

    /**
     * draw a point
     * @param x     x
     * @param y     y
     * @param pixel 0: led off, >0: led on
     */
    void drawPoint(uint16_t x, uint16_t y, uint8_t pixel);

    /**
     * draw a rect
     * @param (x1, y1)   top-left position
     * @param (x2, y2)   bottom-right position, not included in the rect
     * @param pixel      0: rect off, >0: rect on
     */
    void drawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint8_t pixel);

    /**
     * draw a image
     * @param (xoffset, yoffset)   top-left offset of image
     * @param (width, height)      image's width and height
     * @param pixels     contents, 1 bit to 1 led
     */
    void drawImage(uint16_t xoffset, uint16_t yoffset, uint16_t width, uint16_t height, const uint8_t *image);

    /**
     * Set screen buffer to zero
     */
    void clear();

    /**
     * turn off 1/16 leds and turn on another 1/16 leds
     */
    void scan();

    void reverse();

    uint8_t isReversed();

    void on();

    void off();

private:
    uint8_t clk, r1,r2,g1,g2, stb, oe, a, b, c, d;
    uint8_t *displaybuf;
    uint16_t width;
    uint16_t height;
    uint8_t  mask;
    uint8_t  state;
		uint8_t row;

};

#endif

STM32duino LED Matrix Implimention
----------------------------
demo 64x32 or 128x32 
74hc595 and 74hc245d

shift register led matrix stm32duino library

HUB08

#### Pins  
+ A, B, C, D - line select for two 74HC138
+ EN - Ouput Enable(active low) for 74HC595
+ LAT - data latch for 74HC595
+ R1,R2,G1,G2 data pin

### Screenshots

![Alt text](/screenshots/IMG_20151201_212956.jpg?raw=true "screenshot")
![Alt text](/screenshots/IMG_20151201_214632.jpg?raw=true "screenshot")
![Alt text](/screenshots/animation.gif?raw=true "screenshot")

### update
--- 2016 01 20,add four basic led effects 

* LEFTMOVE
* RIGHTMOVE
* DOWNMOVE
* UPMOVE


Assume to use socat to proxy the serial port to internet

socat -d -d -v -x tcp4-listen:9999,reuseaddr,fork file:/dev/ttyACM0,nonblock,echo=0,raw,waitlock=/var/run/ttyACM0.lock

then 

echo '!#001%ZI00%ZA05%ZH0040我赵日天不服AbcF$$' | UTF8 | socat STDIO tcp:localhost:9999

*	!# is the command start,end with $$
* %ZI to set which part of screen,usually I split 64x32 to be the double of 64x16 
* %ZA set effect
* %ZH set the hold time when the words on led matrix reach the side 

UTF8 is a bash alias to use iconv to convert UTF8 string to GBK, this edition is based od GBK 

**
alias UTF8="iconv -f UTF8 -t GBK"




#include <Wire.h>
#include "LEDMatrix.h"
#include <json_arduino.h> // based on jsmn

#define MAX_WIDTH 512    // 64 bytes max == 32个中文
#define MAX_HEIGHT 32   // 
#define MAX_WIDTH_BYTES MAX_WIDTH/8
#define MAX_HEIGHT_BYTES MAX_HEIGHT/8

#define MAX_WIDTH_SKIP MAX_WIDTH*16/8

#define WIDTH   64 //显示屏的物理宽度
#define HEIGHT  32 // 物理高度

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

unsigned long prev_ms = 0;        // will store last time LED was updated

token_list_t *token_list = NULL;

LEDMatrix matrix(A,B,C,D,EN,R1,R2,G1,G2,LAT,CLK);     // LEDMatrix(a, b, c, d, oe, r1,r2,g1,g2, stb, clk);
// LEDMatrix(a, b, c, d, oe, r1, stb, clk);
//LEDMatrix matrix(A,B,C,D,EN,R1,LAT,CLK);     // LEDMatrix(a, b, c, d, oe, r1, stb, clk);
// Display Buffer 64 * 32 / 8 = 16x16=256
uint8_t displaybuf[MAX_WIDTH_BYTES *MAX_HEIGHT_BYTES]; //最大 8192(8KB)内存消耗 64x4

uint8_t screen;// 最多三位数，三个屏
uint8_t font_size;
uint8_t real_width;
uint8_t real_height;
/*
 * effect list:
 * 0 "随机"
 * 1 "立即显示"
 * 2 "左移"
 * 3 "连续左移"
 * 4 "右移"
 */

 
typedef struct _screen
{
  uint8_t width;
  uint8_t height;
  uint8_t number;//编号
  int x;//start x 
  int y; // start y
  uint8_t effect;
  uint8_t swapbuf[MAX_WIDTH_SKIP]; //半块屏的高度,长度最大化
  //[MAX_WIDTH*MAX_HEIGHT/8]; /// KEEP the orignal data from serial port  
}SCREEN;

SCREEN screens[2];//目前只支持最大2块16高的屏

#define JSON_CALLBACK 7

typedef struct _json_func //json数据的callback函数结构体
{
  char func_name[12]; // 3 bytes 
  void (*func)(char*arg,unsigned int arg_len); // 
}json_func;
json_func  js_cb[JSON_CALLBACK]; //json数据的回调函数 数组

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

void json_func_callbacks( token_list_t *tlst)
{
  int i,j;
  for(i=0;i<JSON_CALLBACK;i++)
  { 
    char*data =  json_get_value(tlst,js_cb[i].func_name);
    if(data !=NULL)
    {
      js_cb[i].func( (char*)data,strlen(data));        
    }
  }
}

void swap_to_display(uint8_t u)
{
  int x,y,i;
  int j;
  int q;
  
  for(i=0;i<=u;i++)
  {
    for(y=0;y<font_size/(u+1);y++)
    for(x=0;x<real_width/8;x++)
    {
      q = x+(y+i*font_size/(u+1))*real_width/8;   //不同大小的buff不同的定位,如果u大于0,表示致少第2个区也有显示，必须要类加q,让displaybuf下面的部分也要显示出来
      
      j = x+y*MAX_WIDTH_BYTES;      /// swapbuf比displaybuf 要宽的多
      displaybuf[q] = screens[i].swapbuf[j];
    }
  }  
}

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

void setup_ledmatrix(char*ptr, unsigned int len)
{
  //real_width 和real_height要小于 MAX_WIDTH,MAX_HEIGHT
  matrix.begin(displaybuf,real_width, real_height);//可以重复运行,不产生内存消耗
}

void set_height(char*ptr,unsigned int len)
{
  sscanf(ptr,"%d",&real_height);
}

void set_width(char*ptr,unsigned int len)
{
  sscanf(ptr,"%d",&real_width);
}

void set_font_size(char*ptr,unsigned int len)
{

  sscanf(ptr,"%d",&font_size);
  
}

void set_screen(char*ptr,unsigned int len)
{
  //设定 屏目编号,从上往下,1,2,3....以16为一个单位
  sscanf(ptr,"%d",&screen);
  
}

void set_effect(char*ptr,unsigned int len)
{
  //设定 屏目编号,从上往下,1,2,3....以16为一个单位
  uint8_t e;
  sscanf(ptr,"%d",&e);

  screens[ screen ].effect = e;
  
}

void store_words(char*ptr,unsigned int len)
{
  uint8_t a,b;
  int i,j;
  long x,y;
  int cell;
  char buff[3];
  uint8_t c,d;
  uint8_t u;
  c = 0;
  d = 0;

  if(screen == 0)
  {
    memset(screens[0].swapbuf,0,screens[0].width*screens[0].height/8);
  }
  else if(screen > 0 && screen < 10) // screen的规则是 用数字组合,比如要用1和2号屏,screen的编号是12,用2和3,screen是23,screen的数字第一个和第2个是相近的
  {
    //合法的screen是 1,2,3,4...个位数
    
    memset(screens[screen-1].swapbuf,0,screens[screen-1].width*screens[screen-1].height/8);
    
  }else if(screen > 10 && screen < 100) //最大支持到此,910这样的不支持
  {
     //合法的screen是 12,23,34,45
     c = screen/10;
     d = screen - c*10;
     c--;
     d--;
     screen=0;
     //memset two screens
  }
  
  matrix.clear(); // clear displaybuf
  clear_screens();// clear all of the swapbuf
  
  buff[2] = '\0';
  a = (font_size*font_size) / 8;// 一个字几个byte组成
  b = len / a/2;//  这么多byte数组包含 多少个字
  j=0;
  
  for(i=0;i<b;i++)
  {
    for(y=0;y<font_size;y++)
    {
      for(x=0;x<font_size/8;x++)
      {
        cell = x + i*font_size/8 + y*MAX_WIDTH_BYTES + screen*MAX_WIDTH_SKIP;//用MAX_WIDTH_BYTES是因为arduino运算y*MAX_WIDTH/8只等于0了
       
        buff[0] = ptr[j];
        buff[1] = ptr[j+1];

        u =  cell/(MAX_WIDTH_SKIP);//  本来是 MAX_WIDTH*16(HEIGHT)/8,u代表screens的跳转，连续线性
        cell -= u*MAX_WIDTH_SKIP;
        
        screens[u].swapbuf[cell] = strtol(buff,NULL,16);//PTR[j]PTR[j+1] ===>hex number
        
        j+=2;
      }
    }
  }
  
  Serial.print("{\"stat\":\"ok\",\"zh_words\":"); //记录中文字数
  Serial.print(b);
  Serial.println("}");

  screen = 0;// 重置screen


  swap_to_display(u);
}

void Serial_process()
{
  int total_read; 
  int ret;
  int size_num;
  size_num = MAX_WIDTH*MAX_HEIGHT/8;
  //size_num = 200;
  char serial_cache[size_num];
  
  if(Serial.available())
  {
    total_read = Serial.readBytesUntil('\n',serial_cache,size_num);
    serial_cache[total_read] = '\0';
    ret = json_to_token_list(serial_cache, token_list); 
    if(ret >= 0) // parse success
    {
     json_func_callbacks( token_list );
     
    }else
    {
      Serial.print("parse json failed ");
      Serial.println(ret);
    }
  }
}

void setup_screens()
{
  int i;
  
  for(i=0;i<2;i++)
  {
    screens[i].x = 0;
    screens[i].y = i*16;
    screens[i].number = i+1;
    screens[i].width = MAX_WIDTH;//screen 是虚拟的,是抽象的,长度就是最大长度
    screens[i].height = 16;
    screens[i].effect = 1; //立即显示
  }
  
}
void clear_screens()
{
  int i,j;
  for(i=0;i<2;i++)
  {
    for(j=0;j<MAX_WIDTH_SKIP;j++)
    {
      screens[i].swapbuf[j]=0x00;
    }
  }
}

void setup()
{
    int i=0;
    Serial.begin(115200);
    Serial.setTimeout(30000); //30 秒
    matrix.begin(displaybuf, WIDTH, HEIGHT);//可以重复运行,不产生内存消耗

    token_list = create_token_list(25); 
      
    ///----- 如下的顺序很重要,包含处理顺序,平铺化json数据,不使用 object包含object
    strcpy(js_cb[0].func_name,"width"); // LED displaybuf 的宽度
    strcpy(js_cb[1].func_name,"height");// LED xxxxxxxxf 的高度
    strcpy(js_cb[2].func_name, "screen"); //字的内容[具体]
    strcpy(js_cb[3].func_name, "effect");
    strcpy(js_cb[4].func_name,"setup"); // 设置led屏的长宽[功能]

    
    strcpy(js_cb[5].func_name,"font-size");//设定当前的字体大小[ 具体]
    strcpy(js_cb[6].func_name, "words"); //字的内容[具体]

    
    js_cb[6].func=store_words;
    js_cb[5].func=set_font_size;
    js_cb[4].func=setup_ledmatrix;
    js_cb[3].func=set_effect;
    js_cb[2].func=set_screen;
    js_cb[1].func=set_height;
    js_cb[0].func=set_width;

    font_size = 16; //默认字体高度 16
    screen =0;
    real_width=WIDTH;//屏的实际大小
    real_height=HEIGHT;
    setup_screens();
}

void loop()
{  
    

    matrix.scan();

    //特效如果是1,立即显示,还必须要显示一下 超出屏目的那些字
    //别的移动特效会自动显示所有非0的
    
    /*
    if ((millis() - lastCountTime) > 1000) 
    {
        lastCountTime = millis();
        matrix.clear();
        drawDigital(16,0,count);
        count = (count + 1) & 0x7;
   
    }
    */
    
  Serial_process();
}

#include "gspi.h"

void W25Q64_begin()
{
    spi_is_ok = false;
    pinMode (SPI_SLAVE_SEL_PIN, OUTPUT);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    digitalWrite(SPI_SLAVE_SEL_PIN, HIGH);
}

void W25Q64_end() 
{
   powerDown();
   SPI.end();
}

void select() {
   digitalWrite(SPI_SLAVE_SEL_PIN, LOW);
}

void deselect() {
   digitalWrite(SPI_SLAVE_SEL_PIN, HIGH);
}

void powerDown() {
  select();
  SPI.transfer(CMD_POWER_DOWN);
  deselect();
}


void gspi_setup()
{
  W25Q64_begin();
  if(mem.begin(_W25Q64,SPI,SS))
  {
    spi_is_ok = true;
  } 
  else
  {
    spi_is_ok = false;
    while(1)
    {
      Serial.println("spi winbond failed");
      delay(1000);
    }
  }

  memset(gspi_buf,0,GSPI_BUF_LEN);
}

int gspi_read_zk(int addr,uint8_t * buf, int len)
{
  if(spi_is_ok == false) return -1;
  //uint8_t *buf = new uint8_t[len];
  if(addr > SPI_MAX_ADDR ) return -1;
  
  memset(gspi_buf,0,GSPI_BUF_LEN);

  while(mem.busy());
  mem.read(addr,buf,len);
  return 0;
}


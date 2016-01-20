#ifndef GSPI_H
#define GSPI_H

/*
 * 操作 winbondQ64 spi FLASH 上的字库
 */
#include "config.h"
#include "words.h"


#define CMD_POWER_DOWN        0xB9


#define ASC16_BEGIN 0
#define HZK16_BEGIN 4096
#define ASC32_BEGIN 271712
#define HZK32_BEGIN 288096

#define SPI_MAX_ADDR 1334880

#define GSPI_BUF_LEN 512

#define SPI_SLAVE_SEL_PIN PA4 //STM32 SPI

winbondFlashSPI mem;

uint8_t gspi_buf[GSPI_BUF_LEN];

void W25Q64_begin();
void gspi_setup();
int gspi_read_zk(int addr,uint8_t * buf, int len);


boolean spi_is_ok;
#endif

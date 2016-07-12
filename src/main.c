/*
 * main.c
 *
 *
 *  Created on: 14.08.2013
 *      Author: alexs
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/wait.h>
#include <signal.h>
#include "rfid.h"
#include "bcm2835.h"

uint8_t spi_init(uint32_t spi_speed);

int main(int argc, char *argv[]) {

	uint8_t SN[10];
	uint16_t CType=0;
	uint8_t SN_len=0;
	char status;
	int tmp,i;

	char str[255];
	char *p;
	char sn_str[23];
	pid_t child;
	int max_page=0;
	uint8_t page_step=0;

	FILE * fmem_str;
	char save_mem=0;
	char fmem_path[255];

	uint32_t spi_speed = 10000000L;

	if (argc != 2) {
        fprintf(stderr, "Usage: rc522 <spi_speed>\n");
        return 1;
    }

    spi_speed = (uint32_t)strtoul(argv[1],NULL,10);
    if (spi_speed>125000L) spi_speed=125000L;
    if (spi_speed<4) spi_speed=4;

	if (spi_init(spi_speed)) {
        fprintf(stderr, "SPI initialization failed.\n");
        return 1;
    }

	InitRc522();

	for (;;) {
		status= find_tag(&CType);
		if (status==TAG_NOTAG) {
			usleep(50000);
			continue;
		}else if ((status!=TAG_OK)&&(status!=TAG_COLLISION)) {continue;}

		if (select_tag_sn(SN,&SN_len)!=TAG_OK) {continue;}

		p=sn_str;
		*(p++)='[';
		for (tmp=0;tmp<SN_len;tmp++) {
			sprintf(p,"%02x",SN[tmp]);
			p+=2;
		}
		//for debugging
        *p=0;
        fprintf(stderr,"Type: %04x, Serial: %s\n",CType,&sn_str[1]);
		*(p++)=']';
		*(p++)=0;

		PcdHalt();
	}

	bcm2835_spi_end();
	bcm2835_close();
	return 0;

}


uint8_t spi_init(uint32_t spi_speed) {
	uint16_t sp;

	sp=(uint16_t)(250000L/spi_speed);
	if (!bcm2835_init()) {
		syslog(LOG_DAEMON|LOG_ERR,"Can't init bcm2835!\n");
		return 1;
	}

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
	bcm2835_spi_setClockDivider(sp); // The default
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default
	return 0;
}

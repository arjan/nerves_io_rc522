/*
 * main.c
 *
 *  Created on: 14.08.2013
 *      Author: alexs
 *
 * Modified by Arjan Scherpenisse, july 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include "rfid.h"
#include "bcm2835.h"

#include <ei.h>

#define err(code, msg) (fprintf(stderr, msg "\n"),exit(code));
#define dbg(msg) (fprintf(stderr, msg "\n"));

void erlcmd_send(char *response, size_t len);
uint8_t spi_init(uint32_t spi_speed);
void send_tag(const char *uid, size_t len);


int main(int argc, char *argv[]) {

	uint8_t SN[10];
	uint16_t CType = 0;
	uint8_t SN_len = 0;
	char status;
	int tmp;

	char *p;
	char sn_str[23];

	uint32_t spi_speed = 10000000L;


	if (argc != 2) {
        err(1, "Usage: rc522 <spi_speed|test>");
    }

    // test mode; send tag to host every second
    if (!strcmp(argv[1], "test")) {
        dbg("RC522 port test mode.");
        for (;;) {
            send_tag("foo", 3);
            usleep(1000000);
        }
    }

    spi_speed = (uint32_t)strtoul(argv[1],NULL,10);
    if (spi_speed>125000L) spi_speed=125000L;
    if (spi_speed<4) spi_speed=4;

	if (spi_init(spi_speed)) {
        err(1, "SPI initialization failed.");
    }

	InitRc522();

	for (;;) {
		status= find_tag(&CType);
		if (status == TAG_NOTAG) {
			usleep(50000);
			continue;
		} else if ((status!=TAG_OK) && (status!=TAG_COLLISION)) {
            continue;
        }

		if (select_tag_sn(SN,&SN_len) != TAG_OK) {
            continue;
        }

		p = sn_str;
		for (tmp=0;tmp<SN_len;tmp++) {
			sprintf(p,"%02X",SN[tmp]);
			p+=2;
		}
        *p = 0;

        fprintf(stderr,"Type: %04X, Serial: %s\n",CType,&sn_str[1]);
        send_tag(sn_str, 2 * SN_len);

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
		dbg("Can't init BCM2835!");
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

void send_tag(const char *uid, size_t len) {
    char resp[1024];
    int resp_index = sizeof(uint16_t); // Space for payload size
    ei_encode_version(resp, &resp_index);

    ei_encode_tuple_header(resp, &resp_index, 2);
    ei_encode_atom(resp, &resp_index, "tag");
    ei_encode_binary(resp, &resp_index, uid, len);

    erlcmd_send(resp, resp_index);
}

/**
 * @brief Synchronously send a response back to Erlang
 *
 * @param response what to send back
 */
void erlcmd_send(char *response, size_t len)
{
    uint16_t be_len = htons(len - sizeof(uint16_t));
    memcpy(response, &be_len, sizeof(be_len));

    size_t wrote = 0;
    do {
        ssize_t amount_written = write(STDOUT_FILENO, response + wrote, len - wrote);
        if (amount_written < 0) {
            if (errno == EINTR)
                continue;

            //err(EXIT_FAILURE, "write");
            exit(0);
        }

        wrote += amount_written;
    } while (wrote < len);
}

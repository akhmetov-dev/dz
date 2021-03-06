#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>

#include <sys/mman.h>
#include <sched.h>

#include "oled.h"
#include "font.h"

int oled_close(struct display_info *disp) {
	if (close(disp->file) < 0)
		return -1;

	return 0;
}

void cleanup(int status, void *disp) {
	oled_close((struct display_info *)disp);
}

int oled_open(struct display_info *disp, char *filename) {
	disp->file = open(filename, O_RDWR);
	if (disp->file < 0)
		return -1;

	if (ioctl(disp->file, I2C_SLAVE, disp->address) < 0)
		return -2;
	return 0;
}

int oled_send(struct display_info *disp, struct sized_array *payload) {
	if (write(disp->file, payload->array, payload->size) != payload->size)
		return -1;

	return 0;
}

int oled_init(struct display_info *disp) {
	struct sched_param sch;
	int status = 0;
	struct sized_array payload;

	payload.size = sizeof(display_config);
	payload.array = display_config;

	status = oled_send(disp, &payload);
	if (status < 0)
		return 666;

	memset(disp->buffer, 0, sizeof(disp->buffer));

	return 0;
}

int oled_send_buffer(struct display_info *disp) {
	struct sized_array payload;
	uint8_t packet[129];

	for (int index = 0; index < 8; index++) {	
		packet[0] = OLED_CTRL_BYTE_DATA_STREAM;
		memcpy(packet + 1, disp->buffer[index], 128);
		payload.size = 129;
		payload.array = packet;
		oled_send(disp, &payload);
	}
	return 0;
}

void oled_clear(struct display_info *disp) {
	memset(disp->buffer, 0, sizeof(disp->buffer));
	oled_send_buffer(disp);
}

void oled_putstr(struct display_info *disp, uint8_t line, uint8_t *str) {
	uint8_t a;
	int slen = strlen(str);
	uint8_t fwidth = disp->font.width;
	uint8_t foffset = disp->font.offset;
	uint8_t fspacing = disp->font.spacing;

	for (int i=0; i<slen; i++) {
		a=(uint8_t)str[i];
		if (i >= 128/fwidth)
			break; // no text wrap
		memcpy(disp->buffer[line] + i*fwidth + fspacing, &disp->font.data[(a-foffset) * fwidth], fwidth);
	}
}

void oled_putpixel(struct display_info *disp, uint8_t x, uint8_t y, uint8_t on) {
	uint8_t pageId = y / 8;
	uint8_t bitOffset = y % 8;
	if (x < 128-2) {
		if (on == 1)
			disp->buffer[pageId][x] |= (1<<bitOffset);
		else
			disp->buffer[pageId][x] &= ~(1<<bitOffset);
	}
}

void oled_putstrto(struct display_info *disp, uint8_t x, uint8_t y, uint8_t *str) {
	uint8_t a;
	int slen = strlen(str);
	uint8_t fwidth = disp->font.width;
	uint8_t fheight = disp->font.height;
	uint8_t foffset = disp->font.offset;
	uint8_t fspacing = disp->font.spacing;

	for (int i=0; i<slen; i++) {
		a=(uint8_t)str[i];
		for (int i=0; i<fwidth; i++) {
			for (int j=0; j<fheight; j++) {
				if (((disp->font.data[(a-foffset)*fwidth + i] >> j) & 0x01))
					oled_putpixel(disp, x+i, y+j, 1);
				else
					oled_putpixel(disp, x+i, y+j, 0);
			}
		}
		x+=fwidth+fspacing;
	}
}

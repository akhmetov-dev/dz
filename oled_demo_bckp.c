#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "oled.h"
#include "font.h"


int oled_demo(struct display_info *disp) {
	
	oled_putstrto(disp, 0, 9+1, "Akhmetov");
	disp->font = font1;
	oled_putstrto(disp, 0, 18+2, "Dimitrov");
	disp->font = font2;
	oled_putstrto(disp, 0, 27+3, "Marilova");
	//oled_putstrto(disp, 0, 36+4, "");
	//oled_putstrto(disp, 0, 45+5, "");
	//disp->font = font1;
	//oled_putstrto(disp, 0, 54, "Total cur  2.36 A");

	oled_send_buffer(disp);
	
	uint8_t buf[100];

	disp->font = font3;
	for (int i=0; i<100; i++) {
		sprintf(buf, "Spnd spd  %d rpm", i);
		oled_putstrto(disp, 0, 0, buf);
		oled_putstrto(disp, 135-i, 36+4, "===");
		oled_putstrto(disp, 100, 0+i/2, ".");
  	oled_send_buffer(disp);
	}
	//oled_putpixel(disp, 60, 45);
	//oled_putstr(disp, 1, "hello");
		
	return 0;
}



void show_error(int err, int add) {
	//const gchar* errmsg;
	//errmsg = g_strerror(errno);
	printf("\nERROR: %i, %i\n\n", err, add);
	//printf("\nERROR\n");
}

void show_usage(char *progname) {
	printf("\nUsage:\n%s <I2C bus device node >\n", progname);
}

int main(int argc, char **argv) {
	char filename[32];
	struct display_info disp;

	if (argc < 2) {
		show_usage(argv[0]);
		return -1;
	}

	memset(&disp, 0, sizeof(disp));

	sprintf(filename, "%s", argv[1]);
	disp.address = OLED_I2C_ADDR;
	disp.font = font2;

	//	if (oled_open(&disp, filename) < 0 ||
	//	    oled_init(&disp)           < 0 ||
	//	    oled_demo(&disp)           < 0)
	//	{

	int e;

	e = oled_open(&disp, filename);

	if (e < 0) {
		show_error(1, e);
	} else {
		e = oled_init(&disp);
		if (e < 0) {
			show_error(2, e);
		} else {

			printf("-----------------\n");

			if (oled_demo(&disp) < 0) {
				show_error(3, 777);
			}
		}
	}

	return 0;
}

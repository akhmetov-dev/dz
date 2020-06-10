#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>

#include "oled.h"
#include "font.h"


int oled_demo(struct display_info *disp, int mode) {
	printf("%d\n", mode);
	if (mode == 1)
	{
		disp->font = font3;
		oled_putstrto(disp, 0, 9+1, "Akhmetov Nikita");
		disp->font = font1;
		oled_putstrto(disp, 0, 18+1, "Dimitrov Dima");
		disp->font = font2;
		oled_putstrto(disp, 0, 27+1, "Marikova Katya");
		oled_send_buffer(disp);
	}
	if (mode == 2)							
	{
		int h = 10;
		int m = 0;
		int s = 0;
		char time [10];
		while(1)
		{
			sprintf(time, "%d:%d:%d\n", h, m, s);
			printf(time);
			disp->font = font2;
			oled_putstrto(disp, 0, 27+1, time);
			oled_send_buffer(disp);
			s++;
			if (s == 60)
			{
				s = 0;
				m++;
			}
			if (m == 60)
			{
				m = 0;
				h++;
			}
			usleep(1);
		}
	}
	if (mode == 3)
	{
		char str[100];
		while (1)
		{
			scanf("%s", str);
			oled_clear(disp);
			oled_putstrto(disp, 0, 27+1, str);
			oled_send_buffer(disp);
		}
	}
	return 0;
}



void show_error(int err, int add) {
	printf("\nERROR: %i, %i\n\n", err, add);
}

void show_usage(char *progname) {
	printf("\nUsage:\n%s <I2C bus device node >\n", progname);
}

int main(int argc, char **argv) {
	char filename[32];
	struct display_info disp;
	if (argc < 3) {
		show_usage(argv[0]);
		return -1;
	}

	memset(&disp, 0, sizeof(disp));

	sprintf(filename, "%s", argv[1]);
	disp.address = OLED_I2C_ADDR;
	disp.font = font2;
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

			if (oled_demo(&disp, atoi(argv[2])) < 0) {
				show_error(3, 777);
			}
		}
	}

	return 0;
}

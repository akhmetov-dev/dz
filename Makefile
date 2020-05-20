CC = gcc
LD = gcc

CFLAGS=-std=gnu99


all: oled_demo

oled_demo: oled_demo.o oled.o
	$(LD) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf oled_demo oled*.o

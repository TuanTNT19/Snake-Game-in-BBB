
BBB_KERNEL := /home/tuantnh/Desktop/BBB/kernelbuildscripts/KERNEL
TOOLCHAIN := /home/tuantnh/Desktop/BBB/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

EXTRA_CFLAGS=-Wall
obj-m := ssd1306_driver_lcd.o
ssd1306_driver_lcd-objs = ssd1306_lib.o ssd1306_driver.o

driver:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(BBB_KERNEL) M=$(shell pwd) modules
app:
	arm-linux-gnueabihf-gcc -o snake snake.c
clean:
	make -C $(BBB_KERNEL) M=$(shell pwd) clean
	rm -rf snake

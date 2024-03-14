
BBB_KERNEL := /home/tuantnt19/Desktop/BBB/kernelbuildscripts/KERNEL
TOOLCHAIN := /home/tuantnt19/Desktop/BBB/kernelbuildscripts/dl/gcc-8.5.0-nolibc/arm-linux-gnueabi/bin/arm-linux-gnueabi-

EXTRA_CFLAGS=-Wall
obj-m := ssd1306_lcd_driver.o
ssd1306_lcd_driver-objs = ssd1306_lib.o ssd1306_driver.o

all:
	make ARCH=arm CROSS_COMPILE=$(TOOLCHAIN) -C $(BBB_KERNEL) M=$(shell pwd) modules
	
clean:
	make -C $(BBB_KERNEL) M=$(shell pwd) clean

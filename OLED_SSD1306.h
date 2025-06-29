#ifndef OLED_SSD1306_H
#define OLED_SSD1306_H
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define OLED_ADDR 0x3C
#define I2C_DEV   "/dev/i2c-1"
#define WIDTH     128
#define HEIGHT     32
#define PAGES     (HEIGHT / 8)
#define BUFSIZE   (WIDTH * PAGES)

//extern int fd;
#endif



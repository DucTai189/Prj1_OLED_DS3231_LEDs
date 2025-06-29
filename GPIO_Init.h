
#ifndef GPIO_INIT_H
#define GPIO_INIT_H

#include <stdio.h>
#include <unistd.h>
#include <gpiod.h>

#define CONSUMER "BlinkApp"
#define GPIO_CHIP "/dev/gpiochip0"
#define GPIO_LINE_16 16
#define GPIO_LINE_12 12

extern struct gpiod_line *line_GPIO16;
extern struct gpiod_line *line_GPIO12;

void Init_GPIO();

#endif // GPIO_INIT_H    
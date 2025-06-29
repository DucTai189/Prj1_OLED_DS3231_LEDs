#include "GPIO_Init.h"
#include "DS3231.h"
#include "OLED_SSD1306.h"
#include "pthread.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>   // For timer functions
#include <sched.h>  // For setting thread priority
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>

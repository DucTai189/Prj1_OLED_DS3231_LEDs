CC = gcc
CFLAGS = -Wall -g
LIBS = -lpthread -lgpiod

SRCS = main.c OLED_SSD1306.c GPIO_Init.c DS3231.c
OBJS = $(SRCS:.c=.o)
TARGET = Prj_DS3231_LEDs_OLED

ifdef DEBUG
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O2
endif

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS)	-o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS)	-c	$<

clean:
	rm	-f $(OBJS) $(TARGET)
.PHONY: all clean
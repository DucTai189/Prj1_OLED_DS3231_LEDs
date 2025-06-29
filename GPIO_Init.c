#include "GPIO_Init.h"


struct gpiod_line *line_GPIO16;
struct gpiod_line *line_GPIO12;

void Init_GPIO() {
    struct gpiod_chip *chip;
    __uint8_t ret;

    // Open the GPIO chip
    chip = gpiod_chip_open(GPIO_CHIP);

    if (!chip) {
        perror("Open chip failed");
    }

    // Get the GPIO line
    line_GPIO12 = gpiod_chip_get_line(chip, GPIO_LINE_12);
    if (!line_GPIO12) {
        perror("Get line failed");
        gpiod_chip_close(chip);
    }

    // Request the line as output
    ret = gpiod_line_request_output(line_GPIO12, CONSUMER, 0);
    if (ret < 0) {
        perror("Request line as output failed");
        gpiod_chip_close(chip);
    }

    // Get the second GPIO line
    line_GPIO16 = gpiod_chip_get_line(chip, GPIO_LINE_16);
    if (!line_GPIO16) {
        perror("Get line failed");
        gpiod_chip_close(chip);
    }

    // Request the second line as output
    ret = gpiod_line_request_output(line_GPIO16, CONSUMER, 0);
    if (ret < 0) {
        perror("Request line as output failed");
        gpiod_chip_close(chip);
    }

}





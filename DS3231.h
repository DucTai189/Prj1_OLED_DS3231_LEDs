#ifndef DS3231_H
#define DS3231_H    

#define DS3231_ADDR 0x68
#define I2C_DEV   "/dev/i2c-1"

// Setting time to 2024-06-28 22:56:00
// BCD format: sec, min, hour, day of week, date, month, year
// 00 seconds, 56 minutes, 22 hours, Friday (6),
// 28th day of the month, June (6), year 2025 (0x25 in BCD)
// Note: The year is stored as the last two digits (0-99) in BCD format.
// For example, 2025 is stored as 0x25.
// Day of week: 1 = Sunday, 2 = Monday, ..., 7 = Saturday
// In this case, 7 = Saturday.

#define SETTING_TIME            0x00 // Register address to set time        
#define SETTING_SECOND          0x00 // Register address for seconds
#define SETTING_MINUTE          14 // Register address for minutes
#define SETTING_HOUR            23 // Register address for hours
#define SETTING_DAY_OF_WEEK     0x02 // Register address for day of week
#define SETTING_DATE            24 // Register address for date
#define SETTING_MONTH           11 // Register address for month    
#define SETTING_YEAR            25 // Register address for yeaR

#include <linux/types.h>
#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

typedef struct 
{
    __uint8_t seconds;  // 0-59
    __uint8_t minutes;  // 0-59
    __uint8_t hours;    // 0-23
    __uint8_t dayOfWeek; // 1-7 (1 = Sunday)
    __uint8_t date;     // 1-31
    __uint8_t month;    // 1-12 
    __uint8_t year;     // 0-99 (2000-2099)    
}DS3231_st;
int bcd_to_dec(unsigned char bcd) ;
unsigned char dec_to_bcd(int dec)  ;
__uint8_t DS3231_SetTime() ;
__uint8_t DS3231_GetTime(DS3231_st *GettingTime_st) ;

#endif
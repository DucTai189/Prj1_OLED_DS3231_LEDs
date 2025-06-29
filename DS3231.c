#include "DS3231.h"

static int file;

int bcd_to_dec(unsigned char bcd) 
{
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

unsigned char dec_to_bcd(int dec) 
{
    return ((dec / 10) << 4) | (dec % 10);
}

__uint8_t DS3231_SetTime(int file) 
{
    __uint8_t ret = 0;
    unsigned char SettingTime_au8[8] = {0x00};

    // Set pointer to register 0x00
    SettingTime_au8[0] = SETTING_TIME;

    // Fill in time and date (BCD format)
    SettingTime_au8[1] = dec_to_bcd(SETTING_SECOND);  // Seconds
    SettingTime_au8[2] = dec_to_bcd(SETTING_MINUTE);  // Minutes
    SettingTime_au8[3] = dec_to_bcd(SETTING_HOUR);  // Hours (24H mode)
    SettingTime_au8[4] = dec_to_bcd(SETTING_DAY_OF_WEEK);   // Day of week (1–7)
    SettingTime_au8[5] = dec_to_bcd(SETTING_DATE);   // Date (day of month)
    SettingTime_au8[6] = dec_to_bcd(SETTING_MONTH);   // Month
    SettingTime_au8[7] = dec_to_bcd(SETTING_YEAR);  // Year (2024)

    // Write 7 bytes to registers 0x00–0x06
    if (write(file, SettingTime_au8, 8) != 8) {
        perror("Failed to write time to DS3231");
         ret = 1;
    }

    return ret;
}

__uint8_t DS3231_GetTime(DS3231_st *GettingTime_st) 
{
    // Set register pointer to 0x00
    unsigned char reg = 0x00;
    unsigned char buf[7];
    DS3231_st DS3231;
    __uint8_t ret = 0;


    if (write(file, &reg, 1) != 1) 
    {
        perror("Failed to write register address");
        ret = 1;
    }

    // Read 3 bytes: sec, min, hour
    if (read(file, buf, 7) != 7) 
    {
        perror("Failed to read time");
        ret = 2;
    }

    GettingTime_st->seconds      = bcd_to_dec(buf[0] & 0x7F);
    GettingTime_st->minutes      = bcd_to_dec(buf[1]);
    GettingTime_st->hours        = bcd_to_dec(buf[2] & 0x3F);
    GettingTime_st->dayOfWeek    = bcd_to_dec(buf[3]);  // Day of week
    GettingTime_st->date         = bcd_to_dec(buf[4]);  // Day of month
    GettingTime_st->month        = bcd_to_dec(buf[5] & 0x1F); // Bit 7 is century flag
    GettingTime_st->year         = bcd_to_dec(buf[6]);

   // printf("Date: %02d/%02d/20%02d\n", date, month, year);
   // printf("Time: %02d:%02d:%02d\n", hours, minutes, seconds);
   return ret;

}
void INIT_DS3231() 
{
    // Open the I2C device
    file = open("/dev/i2c-1", O_RDWR);
    if (file < 0) 
    {
        perror("Failed to open I2C device");
        return;
    }

    // Set the I2C slave address
    if (ioctl(file, I2C_SLAVE, DS3231_ADDR) < 0) 
    {
        perror("Failed to set I2C slave address");
        close(file);
        return;
    }


}
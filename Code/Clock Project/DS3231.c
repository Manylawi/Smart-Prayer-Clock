#include "I2C.h"
#include <stdint.h>

#define DS3231_ADDRESS 0x68

// ---------------------- Helper Functions ----------------------

static uint8_t DecimalToBCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

static uint8_t BCDToDecimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

// ---------------------- Time Functions ------------------------

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    I2C_start();
    I2C_write_address(DS3231_ADDRESS << 1);   // Write mode
    I2C_write_data(0x00);                     // Set register pointer to seconds
    I2C_write_data(DecimalToBCD(seconds));
    I2C_write_data(DecimalToBCD(minutes));
    I2C_write_data(DecimalToBCD(hours));
    I2C_stop();
}

void RTC_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds) {
    I2C_start();
    I2C_write_address(DS3231_ADDRESS << 1);   // Write mode
    I2C_write_data(0x00);                     // Set register pointer to seconds
    I2C_repeated_start();
    I2C_write_address((DS3231_ADDRESS << 1) | 1);  // Read mode

    *seconds = BCDToDecimal(I2C_read_with_ACK());
    *minutes = BCDToDecimal(I2C_read_with_ACK());
    *hours   = BCDToDecimal(I2C_read_with_NACK());

    I2C_stop();
}

// ---------------------- Date Functions ------------------------

void RTC_SetDate(uint8_t dayOfWeek, uint8_t date, uint8_t month, uint8_t year) {
    I2C_start();
    I2C_write_address(DS3231_ADDRESS << 1);   // Write mode
    I2C_write_data(0x03);                     // Start at Day of Week register
    I2C_write_data(DecimalToBCD(dayOfWeek));  // 1 = Sunday
    I2C_write_data(DecimalToBCD(date));
    I2C_write_data(DecimalToBCD(month));
    I2C_write_data(DecimalToBCD(year));
    I2C_stop();
}

void RTC_GetDate(uint8_t* dayOfWeek, uint8_t* date, uint8_t* month, uint8_t* year) {
    I2C_start();
    I2C_write_address(DS3231_ADDRESS << 1);   // Write mode
    I2C_write_data(0x03);                     // Start at Day of Week register
    I2C_repeated_start();
    I2C_write_address((DS3231_ADDRESS << 1) | 1);  // Read mode

    *dayOfWeek = BCDToDecimal(I2C_read_with_ACK());
    *date      = BCDToDecimal(I2C_read_with_ACK());
    *month     = BCDToDecimal(I2C_read_with_ACK());
    *year      = BCDToDecimal(I2C_read_with_NACK());

    I2C_stop();
}

// ---------------------- Extra Utility -------------------------

uint8_t RTC_GetDayOfWeek(void) {
    uint8_t day;
    I2C_start();
    I2C_write_address(DS3231_ADDRESS << 1);
    I2C_write_data(0x03);                     // Day of Week register
    I2C_repeated_start();
    I2C_write_address((DS3231_ADDRESS << 1) | 1);
    day = BCDToDecimal(I2C_read_with_NACK());
    I2C_stop();
    return day;
}
// ---------------------- Alarm Functions -------------------------

void RTC_SetAlarm1(uint8_t hours, uint8_t minutes) {
	RTC_ClearAlarm1Flag();  // ??? ???? ??? ???????

	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1); // Write mode
	I2C_write_data(0x07); // Alarm1 register start

	I2C_write_data(0x00);                         // Seconds = 0, A1M1 = 0 (match seconds = 0)
	I2C_write_data(DecimalToBCD(minutes) & 0x7F); // A1M2 = 0 (match minutes)
	I2C_write_data(DecimalToBCD(hours) & 0x7F);   // A1M3 = 0 (match hours)
	I2C_write_data(0x80);                         // A1M4 = 1 (ignore day/date)
	I2C_stop();
}


void RTC_EnableAlarm1(void) {
	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1);
	I2C_write_data(0x0E); // Control Register

	I2C_repeated_start();
	I2C_write_address((DS3231_ADDRESS << 1) | 1);
	uint8_t control = I2C_read_with_NACK();
	I2C_stop();

	// Set INTCN = 1, A1IE = 1, clear A2IE to avoid ?????
	control |= (1 << 2);   // INTCN
	control |= (1 << 0);   // A1IE
	control &= ~(1 << 1);  // A2IE = 0

	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1);
	I2C_write_data(0x0E);
	I2C_write_data(control);
	I2C_stop();
}


void RTC_ClearAlarm1Flag(void) {
	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1);
	I2C_write_data(0x0F); // Status Register

	I2C_repeated_start();
	I2C_write_address((DS3231_ADDRESS << 1) | 1);
	uint8_t status = I2C_read_with_NACK();
	I2C_stop();

	status &= ~(1 << 0); // Clear A1F

	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1);
	I2C_write_data(0x0F);
	I2C_write_data(status);
	I2C_stop();
}
int8_t DS3231_GetTemperature(void) {
	uint8_t temp_msb, temp_lsb;
	I2C_start();
	I2C_write_address(0xD0); // Address with write bit
	I2C_write_data(0x11);    // Temp MSB register address
	I2C_start();             // Repeated start
	I2C_write_address(0xD1); // Address with read bit
	temp_msb = I2C_read_with_ACK();
	temp_lsb = I2C_read_with_NACK(); // Not using fraction
	I2C_stop();
	return (int8_t)temp_msb;
}

void RTC_DisableAlarm1(void) {
	uint8_t control;

	// Start I2C and point to the Control Register (0x0E)
	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1); // Write mode
	I2C_write_data(0x0E);                   // Address of Control Register

	// Read the current value of the Control Register
	I2C_repeated_start();
	I2C_write_address((DS3231_ADDRESS << 1) | 1); // Read mode
	control = I2C_read_with_NACK();
	I2C_stop();

	// Clear the A1IE bit (bit 0) to disable the interrupt
	control &= ~(1 << 0); // A1IE = 0

	// Write the new value back to the Control Register
	I2C_start();
	I2C_write_address(DS3231_ADDRESS << 1); // Write mode
	I2C_write_data(0x0E);                   // Address of Control Register
	I2C_write_data(control);                // Write the modified control byte
	I2C_stop();
}
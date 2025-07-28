#include <avr/io.h>
#include "I2C.h"
#define F_CPU 8000000UL
#include <util/delay.h>

void I2C_Master_init(unsigned long SCL_Clock) {
    TWBR = (unsigned char) (((F_CPU / SCL_Clock) - 16) / (2 * 1));  // Assuming prescaler = 1
    TWSR = 0;  // Set prescaler bits to 0 for no prescaling
}

void I2C_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete
}

void I2C_repeated_start(void) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete
}

void I2C_write_address(unsigned char address) {
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete
}

void I2C_write_data(unsigned char data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT))); // Wait for transmission to complete
}

void I2C_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

unsigned char I2C_read_with_ACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA); // Enable ACK
    while (!(TWCR & (1 << TWINT))); // Wait for reception to complete
    return TWDR;
}

unsigned char I2C_read_with_NACK(void) {
    TWCR = (1 << TWINT) | (1 << TWEN); // No ACK
    while (!(TWCR & (1 << TWINT))); // Wait for reception to complete
    return TWDR;
}

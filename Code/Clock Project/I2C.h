#ifndef I2C_H_
#define I2C_H_

void I2C_Master_init(unsigned long SCL_Clock);
void I2C_start(void);
void I2C_repeated_start(void);
void I2C_write_address(unsigned char address);
void I2C_write_data(unsigned char data);
void I2C_stop(void);
unsigned char I2C_read_with_NACK(void);
unsigned char I2C_read_with_ACK(void);

#endif /* I2C_H_ */

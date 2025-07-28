#ifndef DIO_H_
#define DIO_H_

void DIO_vsetPINDir(unsigned char portname,unsigned char pinnumber,unsigned char direction);
void DIO_write(unsigned char portname,unsigned char pinnumber,unsigned char outputvalue);
void DIO_set_port_direction(unsigned char portname,unsigned char direction);
void DIO_write_port(unsigned char portname,unsigned char portvalue);
void write_low_nibble(unsigned char portname,unsigned char value);
void write_high_nibble(unsigned char portname,unsigned char value);
unsigned char DIO_u8read(unsigned char portname,unsigned char pinnumber);

#endif


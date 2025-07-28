/*
 * seven_segment.c
 *
 * Created: 8/6/2018 10:31:18 PM
 *  Author: Mohamed Zaghlol
 */ 
#include <avr/io.h>
#include "DIO.h"
void seven_seg_vinit(unsigned char port_name)
{
	DIO_set_port_direction(port_name,0xFF);
}

void seven_seg_write(unsigned char portname,unsigned char number)
{
unsigned char arr[]={ 0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f};

DIO_write_port(portname,arr[number]);
}
void seven_seg_vinitT(void)
{
	DIO_vsetPINDir('A', 0, 1);
	DIO_vsetPINDir('A', 1, 1);
	DIO_vsetPINDir('A', 2, 1);
	DIO_vsetPINDir('A', 3, 1);
	DIO_vsetPINDir('A', 4, 1);
	DIO_vsetPINDir('A', 5, 1);
	DIO_vsetPINDir('A', 6, 1);
}

void seven_seg_writeT(unsigned char number)
{
	unsigned char arr[] = {
		0x3F, // 0
		0x06, // 1
		0x5B, // 2
		0x4F, // 3
		0x66, // 4
		0x6D, // 5
		0x7D, // 6
		0x07, // 7
		0x7F, // 8
		0x6F  // 9
	};

	if (number < 10)
	{
		PORTA = (PORTA & 0x80) | arr[number]; // ????? ??? ?? A7 ?? ???????? ????? ?????
	}
}
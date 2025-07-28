#define F_CPU 800000ul
#include <util/delay.h>
#include "DIO.h"
#define DS_PORT 'C'
#define DS_PIN  5
#define SH_CP_PORT 'C'
#define SH_CP_PIN  6
#define ST_CP_PORT 'C'
#define ST_CP_PIN  7


#define DS_PORT7 'C'
#define DS_PIN7  3
#define SH_CP_PORT7 'C'
#define SH_CP_PIN7  2
#define ST_CP_PORT7 'C'
#define ST_CP_PIN7  4

void shift_register_init()
{
	DIO_vsetPINDir(DS_PORT, DS_PIN, 1);     // ???? Output
	DIO_vsetPINDir(SH_CP_PORT, SH_CP_PIN, 1); // ???? Output
	DIO_vsetPINDir(ST_CP_PORT, ST_CP_PIN, 1); // ???? Output
}
void shift_register_send_16bit(uint16_t data) {
	for (int i = 15; i >= 0; i--) {
		DIO_write(DS_PORT, DS_PIN, (data >> i) & 0x01);

		// Pulse the shift clock
		DIO_write(SH_CP_PORT, SH_CP_PIN, 1);
		_delay_us(1);
		DIO_write(SH_CP_PORT, SH_CP_PIN, 0);
		_delay_us(1);
	}

	// Latch to output
	DIO_write(ST_CP_PORT, ST_CP_PIN, 1);
	_delay_us(1);
	DIO_write(ST_CP_PORT, ST_CP_PIN, 0);
}

void shift_registers_init16(void) {
	DIO_vsetPINDir(DS_PORT7, DS_PIN7, 1);       // DS (Data)
	DIO_vsetPINDir(SH_CP_PORT7, SH_CP_PIN7, 1); // SH_CP (Shift Clock)
	DIO_vsetPINDir(ST_CP_PORT7, ST_CP_PIN7, 1); // ST_CP (Latch)
}

void shift_register_send_digit_select16(uint16_t data) {
	for (int i = 15; i >= 0; i--) {
		DIO_write(DS_PORT7, DS_PIN7, (data >> i) & 0x01); // Send bit
		DIO_write(SH_CP_PORT7, SH_CP_PIN7, 1); _delay_us(1);
		DIO_write(SH_CP_PORT7, SH_CP_PIN7, 0); _delay_us(1);
	}
	// Latch
	DIO_write(ST_CP_PORT7, ST_CP_PIN7, 1); _delay_us(1);
	DIO_write(ST_CP_PORT7, ST_CP_PIN7, 0);
}
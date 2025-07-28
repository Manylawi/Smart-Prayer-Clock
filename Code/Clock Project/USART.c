#define F_CPU 8000000UL
#include <avr/io.h>
#include "USART.h"
#include "std_macros.h"

void UART_vInit(unsigned long baud)
{
	unsigned short UBRR = (F_CPU / (16UL * baud)) - 1;
	UBRRH = (unsigned char)(UBRR >> 8);
	UBRRL = (unsigned char)UBRR;

	SET_BIT(UCSRB, TXEN);   // Enable Transmitter
	SET_BIT(UCSRB, RXEN);   // Enable Receiver

	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1); // 8-bit data, no parity, 1 stop
}

void UART_vSendData(char data)
{
	while (!READ_BIT(UCSRA, UDRE)); // Wait until buffer is empty
	UDR = data;
}

void UART_vSendString(const char *str)
{
	while (*str)
	{
		UART_vSendData(*str++);
	}
}

char UART_u8ReceiveData(void)
{
	while (!READ_BIT(UCSRA, RXC)); // Wait until data is received
	return UDR;
}

#ifndef UART_H_
#define UART_H_

void UART_vInit(unsigned long baud);
void UART_vSendData(char data);
void UART_vSendString(const char *str);
char UART_u8ReceiveData(void);

#endif /* UART_H_ */

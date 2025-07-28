#ifndef SHIFTREGISTER_H_
#define SHIFTREGISTER_H_
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


void shift_register_init();
void shift_register_send_16bit(uint16_t data);
void shift_registers_init16(void);
void shift_register_send_digit_select16(int data);





#endif /* SHIFTREGISTER_H_ */
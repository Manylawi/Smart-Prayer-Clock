#define F_CPU 8000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include "I2C.h"
#include "DS3231.h"
#include "DIO.h"
#include "seven_segment.h"
#include "std_macros.h"
#include "SHIFTREGISTER.h"
#include "prayerTimes.h"
#include "USART.h"
#include "HijriConverter.h"
#include "EEPROM_driver.h"


#define ISD1820_PIN 6


volatile int8_t current_adhan = -1;

#define LATITUDE 30.0076
#define LONGITUDE 31.2461

#define ADDR_HIJRI_OFFSET  0x02
#define ADDR_DST_ACTIVE 0x03
#define ADDR_ALARM_HOUR 0x04
#define ADDR_ALARM_MINUTE 0x05

typedef struct {
	uint8_t hour;
	uint8_t minute;
} PrayerTime;

PrayerTime prayer_times[5];

const uint8_t prayer_leds_data[5] = {
	0b00000001, 0b00000010, 0b00000100, 0b00001000, 0b00010000
};

const uint8_t day_leds_data[7] = {
	0b00000001, 0b00000010, 0b00000100, 0b00001000,
	0b00010000, 0b00100000, 0b01000000
};

const uint8_t digit_map[16] = {
	15, 14, 13, 12, 11, 10, 9, 8,
	7, 6, 5, 4, 3, 2, 1, 0
};

volatile uint8_t current_digit = 0;
volatile uint8_t digit_values[16];
volatile uint16_t ms_counter = 0;
volatile uint16_t five_sec_counter = 0;

volatile uint8_t buzzer_on = 0;
volatile uint16_t buzzer_on_time = 0;
volatile uint16_t buzzer_off_time = 0;
volatile uint8_t is_alarm_sounding = 0;
volatile uint8_t alarm_beep_count = 0;


volatile uint32_t motion_idle_counter = 0;
volatile uint8_t system_awake = 1;
volatile uint8_t current_prayer = 0;
volatile uint8_t power_save_enabled = 0;
int time_offset = -1;

uint8_t hours = 0, minutes = 0, seconds = 0;
uint8_t dayofweek = 0, day = 0, month = 0, year = 0;
uint8_t last_displayed_day = 0;

char uart_buffer[20];
uint8_t uart_index = 0;
int temp_digit=0;
uint8_t hijri_toggle = 0;
int8_t hijri_day_offset_new = 0;
int Temp;
volatile uint8_t is_dst_active = 1;        // 0 for standard time, 1 for DST


void timer0_init_ctc_mode(void) {
	TCCR0 = (1 << WGM01);
	TCCR0 |= (1 << CS01) | (1 << CS00);
	OCR0 = 124;
	TIMSK |= (1 << OCIE0);
}


void stop_alarm_and_feedback(void) {
	is_alarm_sounding = 0;
	buzzer_on = 0;
	buzzer_on_time = 0;
	buzzer_off_time = 0;
	alarm_beep_count = 0;
	DIO_write('D', 5, 0);       // Ensure buzzer is off
	play_feedback_tone();       // Now play tone (even if alarm was on)
}

void play_feedback_tone(void) {
	cli();
	buzzer_on = 1;
	buzzer_on_time = 60; // 60 ms beep
	buzzer_off_time = 0;
	DIO_write('D', 5, 1);
	sei();
}


void play_alarm_tone(void) {
	if (!is_alarm_sounding) return;

	cli();
	if (alarm_beep_count < 5) {
		buzzer_on = 1;
		buzzer_on_time = 60;
		buzzer_off_time = 40;
		alarm_beep_count++;
		} else {
		buzzer_on = 0;
		buzzer_on_time = 0;
		buzzer_off_time = 1000;
		alarm_beep_count = 0;
	}
	DIO_write('D', 5, buzzer_on);
	sei();
}


void update_date_display(void) {
	uint8_t d, m;
	uint16_t full_year;

	if (hijri_toggle == 0) {
		d = day;
		m = month;
		full_year = 2000 + year;
		} else {
		uint8_t d_hijri, m_hijri;
		uint16_t y_hijri;
		
		convertToHijriWithOffset(2000 + year, month, day, hijri_day_offset_new,
		&d_hijri, &m_hijri, &y_hijri);

		d = d_hijri;
		m = m_hijri;
		full_year = y_hijri;
	}

	digit_values[8]  = d % 10;
	digit_values[9]  = d / 10;
	digit_values[10] = m % 10;
	digit_values[11] = m / 10;
	digit_values[12] = full_year % 10;
	digit_values[13] = (full_year / 10) % 10;
	digit_values[14] = (full_year / 100) % 10;
	digit_values[15] = (full_year / 1000) % 10;
}

void update_display_array(void) {
	// --- Time Display Conversion (24h to 12h) ---
	uint8_t display_hours = hours;

	// Convert current time to 12-hour format for display
	if (display_hours >= 12) {
		if (display_hours > 12) {
			display_hours -= 12;
		}
		} else if (display_hours == 0) { // Midnight case
		display_hours = 12;
	}

	// Update time digits with 12-hour format
	digit_values[0] = minutes % 10;
	digit_values[1] = minutes / 10;
	digit_values[2] = display_hours % 10;
	digit_values[3] = display_hours / 10;

	// --- Prayer Time Display Conversion (24h to 12h) ---
	PrayerTime p = prayer_times[current_prayer];
	uint8_t display_prayer_hour = p.hour;

	// Convert prayer time to 12-hour format for display
	if (display_prayer_hour >= 12) {
		if (display_prayer_hour > 12) {
			display_prayer_hour -= 12;
		}
		} else if (display_prayer_hour == 0) { // Fajr might be midnight
		display_prayer_hour = 12;
	}

	// Update current prayer time digits with 12-hour format
	digit_values[4] = p.minute % 10;
	digit_values[5] = p.minute / 10;
	digit_values[6] = display_prayer_hour % 10;
	digit_values[7] = display_prayer_hour / 10;

	// Update LEDs and date display
	update_prayer_and_day_leds();
	update_date_display();
}

void update_prayer_and_day_leds(void) {
	uint8_t dow = (RTC_GetDayOfWeek() + 6) % 7;
	uint16_t combined = ((uint16_t)day_leds_data[dow] << 8) | prayer_leds_data[current_prayer];
	if (hijri_toggle == 0) combined |= (1 << 6);
	else combined |= (1 << 5);
	shift_register_send_16bit(combined);
}

void timeToHM(double time, uint8_t* hour, uint8_t* minute) {
	time = fmod(time, 24.0);
	if (time < 0) time += 24.0;

	time += (0.5 / 60.0);  // Add 0.5 minute to minimize float rounding issues

	int h = (int)floor(time);
	int m = (int)((time - h) * 60.0);

	if (m >= 60) { m -= 60; h = (h + 1) % 24; }

	*hour = (uint8_t)h;
	*minute = (uint8_t)m;
}

void update_prayer_times_today(void) {
	uint8_t fajr_h, fajr_m;
	uint8_t dhuhr_h, dhuhr_m, asr_h, asr_m, maghrib_h, maghrib_m, isha_h, isha_m;

	double times[5];
	calcPrayers(2000 + year, month, day, LATITUDE, LONGITUDE, 2, is_dst_active, times);

	timeToHM(times[0], &fajr_h, &fajr_m);
	timeToHM(times[1], &dhuhr_h, &dhuhr_m);
	timeToHM(times[2], &asr_h, &asr_m);
	timeToHM(times[3], &maghrib_h, &maghrib_m);
	timeToHM(times[4], &isha_h, &isha_m);

	prayer_times[0].hour = fajr_h;    prayer_times[0].minute = fajr_m;
	prayer_times[1].hour = dhuhr_h;   prayer_times[1].minute = dhuhr_m;
	prayer_times[2].hour = asr_h;     prayer_times[2].minute = asr_m;
	prayer_times[3].hour = maghrib_h; prayer_times[3].minute = maghrib_m;
	prayer_times[4].hour = isha_h;    prayer_times[4].minute = isha_m;
}

void clear_shiftregister() {
	shift_register_send_16bit(0);
	shift_register_send_digit_select16(0xFFFF);
}

void disable_display(void) {
	DDRC &= ~0b11111100;
	PORTC &= ~0b11111100;

	DDRA &= ~(1 << PA7);
	DDRD &= ~(1 << PD7);
}

void enable_display(void) {
	DDRC |= 0b11111100;

	DDRA |= (1 << PA7);
	DDRD |= (1 << PD7);
	PORTA |= (1 << PA7);
	PORTD |= (1 << PD7);
}


void process_uart_command(void) {
	play_feedback_tone();
	uint8_t len = strlen(uart_buffer);
	if (uart_buffer[len - 1] == '\n' || uart_buffer[len - 1] == '\r') uart_buffer[len - 1] = '\0';

	if (strncmp(uart_buffer, "T:", 2) == 0) {
		uint8_t h, m;
		if (sscanf(uart_buffer + 2, "%2hhu:%2hhu", &h, &m) == 2) RTC_SetTime(h, m, 0);
		} else if (strncmp(uart_buffer, "D:", 2) == 0) {
		uint8_t dow, d, mo; uint16_t y;
		if (sscanf(uart_buffer + 2, "%hhu:%hhu/%hhu/%4hu", &dow, &d, &mo, &y) == 4)
		RTC_SetDate(dow, d, mo, (uint8_t)(y - 2000));
	} else if (strncmp(uart_buffer, "A:", 2) == 0) {
		uint8_t h, m;
		if (sscanf(uart_buffer + 2, "%2hhu:%2hhu", &h, &m) == 2) {
			RTC_SetAlarm1(h, m);
			RTC_EnableAlarm1();
			EEPROM_write(ADDR_ALARM_HOUR, h);
			EEPROM_write(ADDR_ALARM_MINUTE, m);
		}
	} else if (strcmp(uart_buffer, "d") == 0) { // Using lowercase 'd' for safety
		RTC_DisableAlarm1();
		RTC_ClearAlarm1Flag();
		EEPROM_write(ADDR_ALARM_HOUR, 0xFF);
		EEPROM_write(ADDR_ALARM_MINUTE, 0xFF);
	} else if (strcmp(uart_buffer, "S") == 0) {
			stop_alarm_and_feedback();
		} else if (strcmp(uart_buffer, "H") == 0) {
		power_save_enabled = 1;
		motion_idle_counter = 0;
		} else if (strcmp(uart_buffer, "P") == 0) {
		power_save_enabled = 0;
		system_awake = 1;
		enable_display();
		} else if (strcmp(uart_buffer, "+1") == 0) {
		hijri_day_offset_new = 1;
		EEPROM_write(ADDR_HIJRI_OFFSET, (char)hijri_day_offset_new);
		} else if (strcmp(uart_buffer, "+2") == 0) {
		hijri_day_offset_new = 2;
		EEPROM_write(ADDR_HIJRI_OFFSET, (char)hijri_day_offset_new);
		} else if (strcmp(uart_buffer, "0") == 0) {
		hijri_day_offset_new = 0;
		EEPROM_write(ADDR_HIJRI_OFFSET, (char)hijri_day_offset_new);
		} else if (strcmp(uart_buffer, "-1") == 0) {
		hijri_day_offset_new = -1;
		EEPROM_write(ADDR_HIJRI_OFFSET, (char)hijri_day_offset_new);
		} else if (strcmp(uart_buffer, "-2") == 0) {
		hijri_day_offset_new = -2;
		EEPROM_write(ADDR_HIJRI_OFFSET, (char)hijri_day_offset_new);
		} else if (strcmp(uart_buffer, "C") == 0) {
		is_dst_active = 1;
		EEPROM_write(ADDR_DST_ACTIVE, (char)is_dst_active);
		update_prayer_times_today();
		} else if (strcmp(uart_buffer, "W") == 0) {
		is_dst_active = 0;
		EEPROM_write(ADDR_DST_ACTIVE, (char)is_dst_active);
		update_prayer_times_today();
	}

	memset(uart_buffer, 0, sizeof(uart_buffer));
	uart_index = 0;
}

ISR(TIMER0_COMP_vect) {
	if (system_awake) {
		uint8_t real_digit = digit_map[current_digit];
		uint16_t digit_mask = ~(1 << real_digit);
		shift_register_send_digit_select16(digit_mask);
		seven_seg_write('B', digit_values[current_digit]);
		current_digit = (current_digit + 1) % 16;
	}

	ms_counter++;
	five_sec_counter++;

	if (power_save_enabled) {
		if (PIND & (1 << PD3)) {
			motion_idle_counter = 0;
			if (!system_awake) {
				system_awake = 1;
				enable_display();
			}
			} else {
			if (system_awake && ++motion_idle_counter >= 60000) {
				system_awake = 0;
				clear_shiftregister();
				disable_display();
			}
		}
	}
	
	DIO_write('A', 7, 1);
	DIO_write('D', 7, 1);

	if (temp_digit == 0) {
		seven_seg_writeT(Temp % 10);
		DIO_write('D', 7, 0);
		} else {
		seven_seg_writeT(Temp / 10);
		DIO_write('A', 7, 0);
	}
	temp_digit ^= 1;


	

	if (UCSRA & (1 << RXC)) {
		char c = UART_u8ReceiveData();
		if (c == '\n') {
			uart_buffer[uart_index] = '\0';
			process_uart_command();
			} else if (uart_index < sizeof(uart_buffer) - 1) {
			uart_buffer[uart_index++] = c;
		}
	}
	
	// Handle buzzer timing
	static uint16_t buzzer_counter = 0;
	if (buzzer_on_time > 0) {
		buzzer_counter++;
		if (buzzer_counter >= buzzer_on_time) {
			buzzer_counter = 0;
			buzzer_on_time = 0;
			buzzer_off_time = 1;
			buzzer_on = 0;
			DIO_write('D', 5, 0);
		}
		} else if (buzzer_off_time > 0) {
		buzzer_counter++;
		if (buzzer_counter >= buzzer_off_time) {
			buzzer_counter = 0;
			buzzer_off_time = 0;
			play_alarm_tone(); // Continue alarm sequence if it's sounding
		}
	}

}

ISR(INT0_vect) {
	is_alarm_sounding = 1;
	alarm_beep_count = 0;
	play_alarm_tone();
	if (power_save_enabled && !system_awake) {
		system_awake = 1;
		enable_display();
	}
}

int main(void) {
	I2C_Master_init(100000);
	UART_vInit(9600);
	seven_seg_vinit('B');
	seven_seg_vinitT();
	shift_register_init();
	shift_registers_init16();
	timer0_init_ctc_mode();

	DIO_vsetPINDir('D', 3, 0); // PIR
	DIO_vsetPINDir('D', 5, 1); // Buzzer
	DIO_vsetPINDir('D', 2, 0); // INT0
	DIO_vsetPINDir('D',7,1);
	DIO_vsetPINDir('A',7,1);
	DIO_vsetPINDir('D', ISD1820_PIN, 1); // ISD1820 OUTPUT PIN

	MCUCR |= (1 << ISC01);
	MCUCR &= ~(1 << ISC00);
	GICR |= (1 << INT0);

	enable_display();

	sei();

	hijri_day_offset_new = (int8_t)EEPROM_read(ADDR_HIJRI_OFFSET);
	is_dst_active = (uint8_t)EEPROM_read(ADDR_DST_ACTIVE);

	// New: Load and set alarm from EEPROM on startup
	uint8_t alarm_h = EEPROM_read(ADDR_ALARM_HOUR);
	uint8_t alarm_m = EEPROM_read(ADDR_ALARM_MINUTE);

	// Check if the stored alarm time is valid (not 0xFF)
	if (alarm_h != 0xFF && alarm_m != 0xFF) {
		RTC_SetAlarm1(alarm_h, alarm_m);
		RTC_EnableAlarm1();
		} else {
		RTC_DisableAlarm1();
	}
	

	RTC_ClearAlarm1Flag();

	RTC_GetTime(&hours, &minutes, &seconds);
	RTC_GetDate(&dayofweek, &day, &month, &year);
	last_displayed_day = day;

	update_prayer_times_today();
	update_display_array();
	update_prayer_and_day_leds();
	
	while (1) {
		if (ms_counter >= 1000) {
			ms_counter = 0;
			RTC_GetTime(&hours, &minutes, &seconds);
			RTC_GetDate(&dayofweek, &day, &month, &year);
			update_display_array();

			if (day != last_displayed_day) {
				last_displayed_day = day;
				update_prayer_times_today();
			}

			// ? ????? ISD1820 ??? ??? ??????
			for (int i = 0; i < 5; i++) {
				if (hours == prayer_times[i].hour && minutes == prayer_times[i].minute) {
					if (current_adhan != i) {
						current_adhan = i;
						DIO_write('D', ISD1820_PIN, 1);
						_delay_ms(200);  // ????? ??? ????
						DIO_write('D', ISD1820_PIN, 0);
					}
					break;
				}
			}

			// ? ????? ????? ??? ???? ??? ??????
			if (current_adhan != -1 &&
			(hours != prayer_times[current_adhan].hour || minutes != prayer_times[current_adhan].minute)) {
				current_adhan = -1;
			}
			Temp = DS3231_GetTemperature();  // ???? ??? ms_counter >= 1000

		}

		if (five_sec_counter >= 5000) {
			five_sec_counter = 0;
			current_prayer = (current_prayer + 1) % 5;
			update_display_array();
			update_prayer_and_day_leds();
			hijri_toggle ^= 1;
			
		}

		if (DIO_u8read('D', 4) == 1) {
				is_alarm_sounding = 0;
				buzzer_on = 0;
				buzzer_on_time = 0;
				buzzer_off_time = 0;
				alarm_beep_count = 0;
				DIO_write('D', 5, 0);       // Ensure buzzer is off
		}

	}
}
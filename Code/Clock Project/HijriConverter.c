#include "HijriConverter.h"

// Epoch of Islamic calendar: 16 July 622 CE (Julian)
#define JD_ISLAMIC_EPOCH 1948439

long gregorianToJD(uint16_t year, uint8_t month, uint8_t day) {
	if (month <= 2) {
		year -= 1;
		month += 12;
	}
	long A = year / 100;
	long B = 2 - A + (A / 4);
	return (long)(365.25 * (year + 4716)) +
	(long)(30.6001 * (month + 1)) +
	day + B - 1524;
}

void jdToHijri(long JD, uint8_t* h_day, uint8_t* h_month, uint16_t* h_year) {
	long daysSinceHijri = JD - JD_ISLAMIC_EPOCH;

	long hY = (30 * daysSinceHijri + 10646) / 10631;
	long priorDays = daysSinceHijri - ((hY - 1) * 354 + (int)((3 + (11 * hY)) / 30));

	long hM = (priorDays + 28) / 29.5;
	if (hM < 1) hM = 1;
	if (hM > 12) hM = 12;

	long hD = JD - JD_ISLAMIC_EPOCH -
	((hY - 1) * 354 + (int)((3 + (11 * hY)) / 30) + (int)(29.5 * (hM - 1))) + 1;

	if (hD < 1) hD = 1;
	if (hD > 30) hD = 30;

	*h_day = (uint8_t)hD;
	*h_month = (uint8_t)hM;
	*h_year = (uint16_t)hY;
}


void convertToHijriWithOffset(uint16_t g_year, uint8_t g_month, uint8_t g_day,
int8_t day_offset,
uint8_t* h_day, uint8_t* h_month, uint16_t* h_year) {
	long jd = gregorianToJD(g_year, g_month, g_day);
	jd += day_offset;
	jdToHijri(jd, h_day, h_month, h_year);
}

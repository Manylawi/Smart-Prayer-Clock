#ifndef HIJRI_CONVERTER_H
#define HIJRI_CONVERTER_H

#include <stdint.h>

// Convert Gregorian date to Julian Day Number
long gregorianToJD(uint16_t year, uint8_t month, uint8_t day);

// Convert Julian Day Number to Hijri date
void jdToHijri(long JD, uint8_t* h_day, uint8_t* h_month, uint16_t* h_year);

// Combined helper: Gregorian ? Hijri with optional offset
void convertToHijriWithOffset(uint16_t g_year, uint8_t g_month, uint8_t g_day,
int8_t day_offset,
uint8_t* h_day, uint8_t* h_month, uint16_t* h_year);

#endif

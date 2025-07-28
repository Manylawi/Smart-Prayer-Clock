#ifndef PRAYERTIMES_H
#define PRAYERTIMES_H

#include <stdint.h>

// Calculates Julian Date for given Gregorian date
double julianDate(int year, int month, int day);

// Calculates Sun declination and Equation of Time
void sunPosition(double jd, double* declination, double* equation_of_time);

// Returns the hour angle for a specific solar depression angle
double hourAngle(double latitude, double declination, double angle);

// Returns Asr offset (Standard = 1.0, Hanafi = 2.0)
double asrOffset(double latitude, double declination, double factor);

// Main EGAS-compliant calculator: times[] = {Fajr, Dhuhr, Asr, Maghrib, Isha}
void calcPrayers(
int year, int month, int day,
double latitude, double longitude,
int timezone, int dst,
double times[5]
);

#endif // PRAYERTIMES_H

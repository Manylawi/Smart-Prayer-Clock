#include <math.h>
#include <stdint.h>
#include <stdio.h>

#define PI 3.14159265358979323846
#define DEG_TO_RAD (PI/180.0)
#define RAD_TO_DEG (180.0/PI)

// EGAS method angles & Asr factor
#define ANGLE_FAJR   19.5
#define ANGLE_ISHA   17.5
#define ASR_FACTOR   1.0   // Standard
#define SUNSET       0.833 // Refraction + radius

// Utility functions
static double fixAngle(double a) {
	a = fmod(a, 360.0);
	return (a < 0) ? a + 360.0 : a;
}
static double fixHour(double h) {
	h = fmod(h, 24.0);
	return (h < 0) ? h + 24.0 : h;
}

// Julian date calculation
double julianDate(int y, int m, int d) {
	if (m <= 2) { y--; m += 12; }
	int A = y / 100;
	int B = 2 - A + A/4;
	return floor(365.25*(y+4716)) + floor(30.6001*(m+1)) + d + B - 1524.5;
}

// Sun position: declination (°) and equation of time (hours)
void sunPosition(double jd, double* D, double* EoT) {
	double Dn = jd - 2451545.0;
	double g = fixAngle(357.529 + 0.98560028 * Dn);
	double q = fixAngle(280.459 + 0.98564736 * Dn);
	double L = fixAngle(q + 1.915*sin(g*DEG_TO_RAD) + 0.020*sin(2*g*DEG_TO_RAD));
	double e = 23.439 - 0.00000036 * Dn;

	double RA = atan2(cos(e*DEG_TO_RAD)*sin(L*DEG_TO_RAD),
	cos(L*DEG_TO_RAD)) * RAD_TO_DEG / 15.0;
	RA = fixHour(RA);

	*D = asin(sin(e*DEG_TO_RAD)*sin(L*DEG_TO_RAD)) * RAD_TO_DEG;
	*EoT = (q/15.0) - RA;
}

// Hour angle for a given solar angle
double hourAngle(double lat, double dec, double angle) {
	return acos((sin(-angle*DEG_TO_RAD) - sin(lat*DEG_TO_RAD)*sin(dec*DEG_TO_RAD)) /
	(cos(lat*DEG_TO_RAD)*cos(dec*DEG_TO_RAD))) * RAD_TO_DEG / 15.0;
}

// Asr time offset
double asrOffset(double lat, double dec, double factor) {
	double angle = RAD_TO_DEG * atan(1.0 / (factor + tan(fabs(lat-dec)*DEG_TO_RAD)));
	return hourAngle(lat, dec, angle);
}

// Main calculation for prayer times (in hours)
void calcPrayers(
int year, int month, int day,
double lat, double lon,
int tzOffset, int dstOffset,
double times[5]
) {
	double jd = julianDate(year, month, day);
	double D, EoT;
	sunPosition(jd, &D, &EoT);

	double noon = fixHour(12 + tzOffset + dstOffset - (lon/15.0) - EoT);
	
	// Accurate Asr calculation based on sun altitude
	double phi = lat - D;
	double temp1 = 1.0 / (1.0 + tan(phi * DEG_TO_RAD));
	double alt = atan(temp1) * RAD_TO_DEG;

	double angle_alt = alt;
	double term = (sin(angle_alt * DEG_TO_RAD) - sin(D * DEG_TO_RAD) * sin(lat * DEG_TO_RAD)) /
	(cos(D * DEG_TO_RAD) * cos(lat * DEG_TO_RAD));
	term = fmin(fmax(term, -1.0), 1.0);  // clamp to avoid acos domain error
	double asr_ha = acos(term) * RAD_TO_DEG / 15.0;
	
	times[0] = fixHour(noon - hourAngle(lat, D, ANGLE_FAJR));
	times[1] = noon;  // Dhuhr
	times[2] = fixHour(noon + asr_ha);  // Asr
	times[3] = fixHour(noon + hourAngle(lat, D, SUNSET)); // Maghrib
	times[4] = fixHour(noon + hourAngle(lat, D, ANGLE_ISHA));
}
#ifndef DS3231_H
#define DS3231_H
#include <stdint.h>
#ifndef RTC_DS3231_H_
#define RTC_DS3231_H_

void RTC_SetTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_GetTime(uint8_t* hours, uint8_t* minutes, uint8_t* seconds);
void RTC_SetDate(uint8_t dayOfWeek, uint8_t date, uint8_t month, uint8_t year);
void RTC_GetDate(uint8_t* dayOfWeek, uint8_t* date, uint8_t* month, uint8_t* year);
uint8_t RTC_GetDayOfWeek(void);
void RTC_EnableAlarm1(void);
void RTC_DisableAlarm1(void);
void RTC_ClearAlarm1Flag(void);
void RTC_SetAlarm1(uint8_t hours, uint8_t minutes);
int8_t DS3231_GetTemperature(void);

#endif /* RTC_DS3231_H_ */


#endif /* DS3231.H_H_ */
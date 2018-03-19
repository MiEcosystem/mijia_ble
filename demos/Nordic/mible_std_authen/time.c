#include <time.h>
#include <string.h>
#include "nrf_rtc.h"

#pragma import(__use_no_semihosting)

extern volatile uint32_t rtc1_overflow_cnt;

static uint32_t ticks_per_cnt = 1;
static time_t offset_time_in_sec;             /* Time passed since Unix epoch */
static const char * _month[] =  {  
	"Jan", "Feb", "Mar", "Apr", "May", "Jun",
	"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

clock_t clock(void)
{
	/* Be carefull the overflow. It should only be used to profile. */
	return nrf_rtc_counter_get(NRF_RTC1);
}

time_t time(time_t *p_time)
{
	time_t seconds;

    if(nrf_rtc_event_pending(NRF_RTC1, NRF_RTC_EVENT_OVERFLOW))
    {  
        nrf_rtc_event_clear(NRF_RTC1, NRF_RTC_EVENT_OVERFLOW);
        rtc1_overflow_cnt++;
    }

	/* seconds = (rtc1_overflow_cnt * 2^24 + RTC_COUNT) * TICKS_PER_CNT / 32768 */
	seconds = ((rtc1_overflow_cnt<<24) + nrf_rtc_counter_get(NRF_RTC1)) * ticks_per_cnt >> 15;
	seconds += offset_time_in_sec;

	if ( p_time != NULL )
		*p_time = seconds;

	return seconds;
}

static int month2int( char * str )
{
	int i;
	for (i = 0; i < 12; i++)
		if (strcmp(_month[i], str) == 0)
			break;
	return i;
}

void set_time_rtc_prescaler(uint32_t pre)
{
	ticks_per_cnt = pre + 1;
}

void time_init(struct tm * time_ptr) 
{
	if ( time_ptr == NULL ) {
		/* Use Compiled time as system init time. */
		char month_name[5];
		struct tm compiled_tm = {0};
		sscanf(__DATE__, "%s %d %d", month_name, &compiled_tm.tm_mday, &compiled_tm.tm_year);
		sscanf(__TIME__, "%d:%d:%d", &compiled_tm.tm_hour, &compiled_tm.tm_min, &compiled_tm.tm_sec);
		
		compiled_tm.tm_mon = month2int(month_name);
		compiled_tm.tm_year -= 1900;
		
		offset_time_in_sec = mktime(&compiled_tm) - 8 * 3600;  // compiled time is UTC+8
	} else {
		time_t current_time = time(NULL);
		time_t new_time     = mktime(time_ptr);
		if ( new_time > current_time )
			offset_time_in_sec += new_time - current_time;
		else
			offset_time_in_sec -= current_time - new_time;
	}
}

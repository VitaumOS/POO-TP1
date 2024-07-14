/*	<src/headers/commons.h>

	Where common definitions used throughtout the program are given. */


#ifndef _COMMONS_HEADER_
#define _COMMONS_HEADER_


#include <inttypes.h>


/*  Defines the standard string size for names. */
#define NAME_SIZE    		64u


// A type representing an ID.
typedef uint64_t id_t;


// Ref: https://www.ibm.com/docs/en/zos/3.1.0?topic=functions-timetime64-determine-current-utc-time

typedef struct _Date { 
	uint8_t day, month, year, hour, minutes, seconds; 


	inline bool operator==(const struct _Date & y) {
		return (year == y.year) && (month == y.month) && (day == y.day)
			&& (hour == y.hour) && (minutes == y.minutes) && (seconds == y.seconds);
	}

	inline bool operator!=(const struct _Date & y) {
		return (year != y.year) || (month != y.month) || (day != y.day)
			|| (hour != y.hour) || (minutes != y.minutes) || (seconds != y.seconds);
	}

	inline bool operator>(const struct _Date & y) { 
		return (year > y.year) && (month > y.month) && (day > y.day)
			&& (hour > y.hour) && (minutes > y.minutes) && (seconds > y.seconds);
	}

	inline bool operator<(const struct _Date & y) {
		return (year < y.year) && (month < y.month) && (day < y.day)
			&& (hour < y.hour) && (minutes < y.minutes) && (seconds < y.seconds);
	}

	inline bool operator>=(const struct _Date & y) {
		return (year >= y.year) && (month >= y.month) && (day >= y.day)
			&& (hour >= y.hour) && (minutes >= y.minutes) && (seconds >= y.seconds);
	}

	inline bool operator<=(const struct _Date & y) {
		return (year <= y.year) && (month <= y.month) && (day <= y.day)
			&& (hour <= y.hour) && (minutes <= y.minutes) && (seconds <= y.seconds);
	}

} Date;


/*	Currency
	======== */

// A type representing currency values - with two decimal places for cents.
typedef uint64_t currency_t;

#define fprint_currency(_Stream, _CurrencyValue)	fprintf(_Stream, "%05.2lf", ((double) _CurrencyValue) / 100.0L)
#define print_currency(_CurrencyValue)				fprint_currency(stdout, _CurrencyValue)



#endif //_COMMONS_HEADER_

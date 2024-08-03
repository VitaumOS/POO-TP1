/*	<src/headers/date.h>
	
	(...) */


#ifndef _DATE_HEADER_
#define _DATE_HEADER_


#include <inttypes.h>
#include <ostream>


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

std::ostream & operator<<(std::ostream & stream, const struct _Date & the_date);

void get_date_e(struct _Date & the_date);
bool get_date(struct _Date & the_date);
void fprint_date(FILE * const _PrintStream, const Date & the_date);



#endif // _DATE_HEADER
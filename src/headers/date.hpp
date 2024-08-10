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

	inline struct _Date operator-(const struct _Date & x)
	{
		int s = seconds - x.seconds;
		int m = minutes - x.minutes;
		int h = hour - x.hour;
		int d = day - x.day;
		int M = month - x.month;
		int y = year - x.year;

		if (s < 0)
		{
			s += 60;
			m -= 1;
		}

		if (m < 0)
		{
			m += 60;
			h -= 1;
		}

		if (h < 0)
		{
			h += 24;
			d -= 1;
		}

		if (d < 0)
		{
			d += 30; // **
			M -= 1;
		}

		if (M < 0)
		{
			M += 12;
			y -= 1;
		}

		const struct _Date date = { 
			.day = static_cast<uint8_t> (d), 
			.month = static_cast<uint8_t> (M), 
			.year = static_cast<uint8_t> (y),
			.hour = static_cast<uint8_t> (h),
			.minutes = static_cast<uint8_t> (m),
			.seconds = static_cast<uint8_t> (s) };
		return date;
	}

} Date;

std::ostream & operator<<(std::ostream & stream, const struct _Date & the_date);

void get_date_e(struct _Date & the_date);
bool get_date(struct _Date & the_date);
void fprint_date(FILE * const _PrintStream, const Date & the_date);



#endif // _DATE_HEADER
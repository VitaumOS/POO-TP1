/*	<src/date.cpp>

	(...) */


#include "headers/date.hpp"
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <stdexcept>


class date_error : virtual public std::runtime_error {
public:
	date_error(const char * string) : std::runtime_error(string) {

	}
};

void get_date_e(struct _Date & the_date) {
	if (! get_date(the_date))
		throw date_error("Failed to get the current date.");
}

bool get_date(struct _Date & the_date) {

	time_t current_time = time(nullptr);
	struct tm * time_components = localtime(&current_time);
	if (time_components == nullptr) {
		return false;
	}

	the_date.year = time_components->tm_year;
	the_date.month = time_components->tm_mon;
	the_date.day = time_components->tm_mday;
	the_date.hour = time_components->tm_hour;
	the_date.minutes = time_components->tm_min;
	the_date.seconds = time_components->tm_sec;

	return true;
}

void fprint_date(FILE * const _PrintStream, const Date & the_date)
{
	fprintf(_PrintStream, "(%02hhu/%02hhu/%02d %02hhu:%02hhu:%02hhu)",
		the_date.day, the_date.month + 1, ((int) the_date.year) + 1900,
		the_date.hour, the_date.minutes, the_date.seconds);
}

std::ostream & operator<<(std::ostream & stream, const struct _Date & the_date)
{
	char date_buffer[64];
	sprintf(date_buffer, "(%02hhu/%02hhu/%02d %02hhu:%02hhu:%02hhu)",
		the_date.day, the_date.month + 1, ((int) the_date.year) + 1900,
		the_date.hour, the_date.minutes, the_date.seconds);
	stream << std::string(date_buffer);
	return stream;
}

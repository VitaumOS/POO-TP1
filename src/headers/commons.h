#ifndef _COMMONS_HEADER_
#define _COMMONS_HEADER_


/*  Defines the standard string size*/
#define NAMESIZE    64
#define MAX_VEHICLE 8
#define MAX_ORDERS 16

typedef uint64_t id_t;

typedef struct Date{
	uint8_t day, month;
	uint16_t year;

	uint8_t hour, minutes, seconds;
} Date;


#endif //_COMMONS_HEADER_

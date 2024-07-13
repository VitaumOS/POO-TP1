#ifndef _COMMONS_HEADER_
#define _COMMONS_HEADER_


#include <inttypes.h>


/*  Defines the standard string size*/
#define NAME_SIZE    		64u

/*	*/
#define DESCRIPTION_SIZE 	64u

#define MAX_PIECES			8
#define MAX_VEHICLE 		8
#define MAX_ORDERS 			16


// A type representing an ID.
typedef uint64_t id_t;


// Ref: https://www.ibm.com/docs/en/zos/3.1.0?topic=functions-timetime64-determine-current-utc-time

typedef struct _Date { uint8_t day, month, year, hour, minutes, seconds; } Date;


// A type representing currency values - with two decimal places for cents.
// "Money".
typedef uint64_t currency_t;





#endif //_COMMONS_HEADER_

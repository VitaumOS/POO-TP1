/*	<src/headers/commons.h>

	Where common definitions used throughtout the program are given. */


#ifndef _COMMONS_HEADER_
#define _COMMONS_HEADER_


#include <inttypes.h>

#if defined(__cplusplus)
#	include "date.hpp"
#endif

/*	Sizes & limits *
 *	============== */

/*  Defines the standard string size for names. */
constexpr size_t NAME_SIZE		= 64ULL;
constexpr int64_t INT64_T_MAX	= 9223372036854775807LL;


/*	Types *
 * ====== */

// Represents a constant filepath.
typedef const char * c_filepath;

// A type representing generically an ID.
typedef uint64_t id_t;


/*	The Currency *
 *	============ */

// A type representing currency values - with two decimal places for cents.
typedef uint64_t currency_t;
#define CURRENCY_REPR_SIZE		"6"

#define fprint_currency(_Stream, _CurrencyValue)	fprintf(_Stream, "%0" CURRENCY_REPR_SIZE ".2lf", ((double) _CurrencyValue) / ((double) 100.0))
#define sprint_currency(_Buffer, _CurrencyValue)	sprintf(_Buffer, "%0" CURRENCY_REPR_SIZE ".2lf", ((double) _CurrencyValue) / ((double) 100.0))
#define print_currency(_CurrencyValue)				fprint_currency(stdout, _CurrencyValue)



#endif //_COMMONS_HEADER_

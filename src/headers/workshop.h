/*	<src/headers/databases.h>
	
	Inclusion header file for <src/workshop.cpp>. */


#ifndef __DATABASES_INCLUDE_HEADER__
#define __DATABASES_INCLUDE_HEADER__


// IO & STL
#include <iostream>
#include <list>
#include <stdio.h>


// Database systems
#if defined(__cplusplus)
#	include "../databases/so-db.hpp"	// Includes "src/databases/clients-db.hpp" and "src/databases/databases.hpp" as a chain.
#endif // defined(__cplusplus)


// Login systems
#if defined(__cplusplus)
#	include "login.h"
#endif // defined(__cplusplus)


#endif // __DATABASES_INCLUDE_HEADER__
/*	<src/headers/databases.h>
	
	Inclusion header file for <src/workshop.cpp>. */


#ifndef __DATABASES_INCLUDE_HEADER__
#define __DATABASES_INCLUDE_HEADER__


// IO & STL
#include <iostream>
#include <list>
#include <stdio.h>

// Database
#if defined(__cplusplus)
#	include "../databases/so-db.hpp"	// Includes "src/databases/clients-db.hpp" and "src/databases/databases.hpp" as a chain.
#	include "../databases/users-db.hpp"
#endif // defined(__cplusplus)

// Login
#include "login.h"

// Users
#if defined(__cplusplus)
#	include "../users/seller.hpp"
#	include "../users/admin.hpp"
#endif // defined(__cplusplus)


#endif // __DATABASES_INCLUDE_HEADER__
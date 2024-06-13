#ifndef _VEHICLE_HEADER_
#define _VEHICLE_HEADER_


#include "commons.h"


class Vehicle {
public: 
    char type[NAMESIZE];
    char model[NAMESIZE];
    int mileage;
};

#endif // _VEHICLE_HEADER_

#ifndef _VEHICLE_HEADER_
#define _VEHICLE_HEADER_


#include "commons.h"


typedef enum {
    TIRE,
    SEAT,
    WHEEL,
    STEERING_WHEEL,
    CAR_HEADLIGHT,
} PIECE_ID;

static __attribute__((always_inline)) inline const float hash_piece_price(const PIECE_ID piece_id) {
    switch (piece_id) {
    TIRE:                   return 0.76;
    SEAT:                   return 100;
    WHEEL:                  return 719.99;
    STEERING_WHEEL:         return 213.98;
    CAR_HEADLIGHT:          return 121.30;
    }
    return -1;
}

class Vehicle {
public: 
    char type[NAMESIZE];
    char model[NAMESIZE];
    int mileage;
};  

#endif // _VEHICLE_HEADER_

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

static __attribute__((always_inline)) inline float hash_piece_price(const PIECE_ID piece_id) {
    switch (piece_id) {
    case TIRE:                  return 0.76;
    case SEAT:                  return 100;
    case WHEEL:                 return 719.99;
    case STEERING_WHEEL:        return 213.98;
    case CAR_HEADLIGHT:         return 121.30;
    }
    return -1;
}


struct Vehicle {
    char type[NAME_SIZE];
    char model[NAME_SIZE];
    int mileage;
};

#endif // _VEHICLE_HEADER_

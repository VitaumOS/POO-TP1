/*  <src/headers/vehicle.h>
    
    Holds on the information about the vehicle the workshop system will deal with. */


#ifndef _VEHICLE_HEADER_
#define _VEHICLE_HEADER_


#include "commons.h"


typedef enum {
    PIECE_TIRE,
    PIECE_SEAT,
    PIECE_WHEEL,
    PIECE_STEERING_WHEEL,
    PIECE_CAR_HEADLIGHT,
} PIECE_ID;

#define LAST_PIECE_ID   PIECE_CAR_HEADLIGHT


__attribute__((always_inline)) inline currency_t hash_piece_price(const PIECE_ID piece_id) {
    switch (piece_id) {
    case PIECE_TIRE:                  return 76;
    case PIECE_SEAT:                  return 10000;
    case PIECE_WHEEL:                 return 71999;
    case PIECE_STEERING_WHEEL:        return 21398;
    case PIECE_CAR_HEADLIGHT:         return 12130;
    }
    return 0;
}


/*  (...) */
struct Vehicle {
    char type[NAME_SIZE];
    char model[NAME_SIZE];
    int mileage;
};


/*  The maximum number of pieces that can be associated with a single
    parts-budget item. (By extension, the upper bound on pieces by SOs.) */
constexpr uint64_t MAX_PIECES = 8ULL;

struct PartsBudget {
    uint8_t n_pieces; // Tracks the number of pieces on the budget.
    PIECE_ID pieces[MAX_PIECES]; // Tracks each piece ID.
};

#endif // _VEHICLE_HEADER_

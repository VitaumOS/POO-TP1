#ifndef _DATABASES_HEADER_
#define _DATABASES_HEADER_


#include "commons.h"


typedef enum {
    BUDGET,
    MAINTENANCE,
} SERICE_ORDER_REASON;

struct ServiceOrder {
    int id;
    SERICE_ORDER_REASON reason_order;
    char name_client[NAMESIZE];
    char issue_description[NAMESIZE];
};
#endif


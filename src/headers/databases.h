#ifndef _DATABASES_HEADER_
#define _DATABASES_HEADER_


#include "commons.h"


typedef enum {
    BUDGET,
    MAINTENANCE,
} SERVICE_ORDER_REASON;

struct ServiceOrder {
    int id         : 31;
    bool approved  : 1;
    SERVICE_ORDER_REASON reason_order;
    char name_client[NAMESIZE];
    char issue_description[NAMESIZE];
};
#endif


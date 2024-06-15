#ifndef _CLIENT_HEADER_
#define _CLIENT_HEADER_

#include "commons.h"
#include <inttypes.h>

class Client {
public:
    Client();
    id_t id;
    char nome[NAMESIZE];
    size_t veiculos[MAX_VEHICLE];
    Date data_registro;
    size_t orders[MAX_ORDERS];
};

#endif //_CLIENT_HEADER_

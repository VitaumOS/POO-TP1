/*  <src/header/seller.h>

*/


#ifndef _SELLER_HEADER_
#define _SELLER_HEADER_


#include "user.h"
#include "databases.h"


class Seller: public Worker {
private:
    
    
public:
    Seller(SO_Manager * so);
    ~Seller(void);

    bool RegisterClient(Client client);

    bool ServiceOrder_Approve(id_t &so_id);
    bool ServiceOrder_Close(id_t &so_id);
};


#endif // _SELLER_HEADER_

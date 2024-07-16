/*  <src/header/seller.h>

*/
#define ACTIVE_SELLER   false
#if ACTIVE_SELLER

#ifndef _SELLER_HEADER_
#define _SELLER_HEADER_


#include "user.h"
#include "databases.h"


class Seller: public User {
private:
    SO_Manager * so_manager;
    struct Client client_buffer;
    struct ServiceOrder so_buffer;

public:
    Seller(SO_Manager * so_manager) : so_manager(so_manager) { };
    ~Seller(void);

    bool RegisterClient();
    bool GenerateSO();
    bool ApproveOrders();
    bool CloseOrders();

    void Interface();
};


#endif // _SELLER_HEADER_

#endif

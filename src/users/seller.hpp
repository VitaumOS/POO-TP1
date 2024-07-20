/*  <src/header/seller.h>

*/

#define ACTIVE_SELLER true
#if ACTIVE_SELLER

#ifndef _SELLER_HEADER_
#define _SELLER_HEADER_

#include "../databases/so-db.hpp"
#include "user.hpp"


class Seller: public User {
private:
    SO_Manager * so_manager;
    bool client_is_loaded;
    struct Client client_buffer;
    struct ServiceOrder so_buffer;

    bool LoadClient();
public:
    Seller(SO_Manager * so_manager) { 
        this -> so_manager = so_manager;
    }

    ~Seller(void) { };

    bool RegisterClient();
    bool GenerateSO();
    bool ApproveOrders();
    bool CloseOrders();
    void Interface();
};


#endif // _SELLER_HEADER_

#endif

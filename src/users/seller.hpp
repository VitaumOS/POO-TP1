/*  <src/header/seller.h>

*/

#define ACTIVE_SELLER true
#if ACTIVE_SELLER


#ifndef _SELLER_HEADER_
#define _SELLER_HEADER_


#include "../databases/so-db.hpp"
#include "user.hpp"


enum REGISTERCLIENT {
    REGISTERCLIENT_SUCCESS,
    REGISTERCLIENT_ALREADY_EXIST,
    REGISTERCLIENT_CANCEL,
    REGISTERCLIENT_FAIL
};

class Seller: public User {
private:
    bool client_is_loaded;
    bool so_is_loaded;

    struct Client client_buffer;
    struct ServiceOrder so_buffer;

    bool LoadClient(void);

    void display_interaction_guide(void) const;

    enum REGISTERCLIENT register_client_menu();
    bool GenerateSO();
    bool approve_menu();
    bool close_menu();

public:
    Seller(id_t id, SO_Manager * so_manager);
    virtual ~Seller(void);

    void interact(void);
};


#endif // _SELLER_HEADER_

#endif

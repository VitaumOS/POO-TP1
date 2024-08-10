/*  <src/header/seller.h>

*/


#ifndef _SELLER_HEADER_
#define _SELLER_HEADER_


#include "../databases/so-db.hpp"
#include "../ui/db-vizualizers.hpp"
#include "user.hpp"


class Seller: public UserScreen {
private:    /*  Context buffers */

    bool client_is_loaded = false;
    struct ClientData client_buffer;
    std::list<struct ClientData> all_client_data;       // a list representing all associated data with a person...
    size_t focus_index = 0LLU;

    bool so_is_loaded = false;
    struct ServiceOrder so_buffer;

private:
    class SO_Vizualizer so_vizualizer;

    bool load_client(const char name[NAME_SIZE]);
    bool load_client_interface(void);

    void register_client(void);
    void new_so(void);
    void approve_menu();
    void close_menu(void);

    void manage_sos(void);

    int render(void);
    int process(void);

public:
    Seller(class SO_Manager * const, class UsersDatabase * const, const struct MinimalUserData &);
    virtual ~Seller(void);

    bool get_client(struct ClientData & client_buffer);
};


#endif // _SELLER_HEADER_


/*

*/

#ifndef _USER_ADMIN_HEADER_
#define _USER_ADMIN_HEADER_


#include "../databases/so-db.hpp"
#include "../databases/users-db.hpp"
#include "../databases/databases.hpp"
#include "user.hpp"
#include "seller.hpp"


using namespace std;


typedef enum{
    EDIT_SLR,
    EDIT_MCH,
    EDIT_ADMIN,
    EXIT_ADM_OPTIONS
} OPTION;


class Administrator : virtual public User {
private:

    class Users_DB * users_db;

    void SellerMenu(void);
    bool Update_Data_Seller(username_string_t, password_string_t);
    bool Register_Data_Seller(username_string_t , password_string_t );

    void MechanicMenu(void);
    bool Update_Data_Mechanic(username_string_t, password_string_t);
    bool Register_Data_Mechanic(username_string_t, password_string_t);

    void AdminMenu(void);
    bool Register_New_Admin(username_string_t, password_string_t);

public:
    Administrator(id_t id, SO_Manager *, Users_DB *);
    virtual ~Administrator(void);

    virtual void interact(void);
};


#endif // _USER_ADMIN_HEADER_

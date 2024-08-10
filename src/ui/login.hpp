/*	<src/headers/login.h>

    Where the definition of the login menu is given. */


#ifndef _LOGIN_HEADER_
#define _LOGIN_HEADER_


#include "ui.hpp"
#include "../databases/users-db.hpp"

/*  Each import each main user interface... */
#include "../users/seller.hpp"  
#include "../users/admin.hpp"
#include "../users/mechanic.hpp"


/*  A class representing the login screen for the workshop. 
    */
class LoginScreen : virtual public MenuScreen {
private:
    /*  State & Data */
    class UserScreen * user_buffer = nullptr;   //
    class SO_Manager * so_manager = nullptr;    //
    class UsersDatabase * users_db = nullptr;   //

    std::string username_buffer;                //
    std::string password_buffer;                //

    bool login_loop = true;                     //

    /*  Verifies brutely the initialization state of the screen's arguments:
        raises errors if some database isn't on a valid state. */
    void check_databases(void);

    /*  Attempts capturing the username and password from the user.
        Returns false either when stdin is interrupted or credentials won't match
        at the database; in both the login couldn't succeed.
        In case of success, loads a class instance of User at user_buffer. */
    bool capture_credentials(void);

    void render_invalid_credentials(void) const;

    int render(void) override;

public:
    LoginScreen(class SO_Manager * const so_manager, class UsersDatabase * const users_db);
    LoginScreen(int w, int h, class SO_Manager * const so_manager, class UsersDatabase * const users_db);
    virtual ~LoginScreen(void);

    virtual int interact(void);

    /*  Calls the interaction of the User buffer.
        Raises an error in case no User object is loaded. */
    int user_interact(void);
};

#endif // _LOGIN_HEADER_

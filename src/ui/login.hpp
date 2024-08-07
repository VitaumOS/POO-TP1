/*	<src/headers/login.h>

    Where the definition of the login menu is given. */


#ifndef _LOGIN_HEADER_
#define _LOGIN_HEADER_


#include "ui.hpp"
#include "../databases/users-db.hpp"
#include "../users/user.hpp"
#include "../users/seller.hpp"
#include "../users/admin.hpp"

// The steps there will be at login screen.
typedef enum {
    LS_NOTHING,
    LS_ENTERED_USERNAME,
    LS_ENTERED_PASSWORD,
    LS_REPEATING_PASSWORD,
    LS_LOGGED
} LOGIN_STEPS;


class LoginScreen : virtual public MenuScreen {
private:
    class User * user_buffer = nullptr;
    class SO_Manager * so_manager = nullptr;
    class Users_DB * users_db = nullptr;

    std::string username_buffer;
    std::string password_buffer;

    bool login_loop = true;

    bool capture_credentials(void);
    int render(void);

    void check_databases(void);
public:
    LoginScreen(class SO_Manager * const so_manager, class Users_DB * const users_db);
    LoginScreen(int w, int h, class SO_Manager * const so_manager, class Users_DB * const users_db);
    virtual ~LoginScreen(void);
    virtual int interact(void);
    int user_interact(void);
};

#endif // _LOGIN_HEADER_

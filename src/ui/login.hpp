/*	<src/headers/login.h>

    Where the definition of the login menu is given. */


#ifndef _LOGIN_HEADER_
#define _LOGIN_HEADER_


#include "ui.hpp"
#include "../databases/users-db.hpp"
#include "../users/user.hpp"


// The steps there will be at login screen.
typedef enum {
    LS_NOTHING,
    LS_ENTERED_USERNAME,
    LS_ENTERED_PASSWORD,
    LS_REPEATING_PASSWORD,
    LS_LOGGED
} LOGIN_STEPS;


class LoginScreen : virtual public Screen {
private:
    class User * user_buffer = nullptr;
    class Users_DB * users_db;

    std::string username_buffer;
    std::string password_buffer;

    bool login_loop = true;

    bool input_credentials(void);
    int render(void);

public:
    LoginScreen(Users_DB * users_db);
    virtual ~LoginScreen(void);

    class User * menu(void);
};


#if 0
class LoginScreen : public Screen {
private:
    int state = LS_NOTHING; // Tells in which state of processing is the login screen interaction.
    bool logged;

    // Text labels...
    Label username_label, password_label;

    std::string input_username; // Holds the username string the user types...
    std::string input_password; // Holds the password string the user types...

public:
    LoginScreen(int x, int y, rgb bg, rgb fg) :
        Screen(x, y, bg, fg)
    {
        username_label = Label(width >> 3, (height >> 1) - 2, "Username: ");
        password_label = Label(width >> 3, (height >> 1) - 1, "Password: ");

        input_username = "";
        input_password = "";
    }

    void render(void);
    void capture_username(void);
    void capture_password(void);
};
#endif

#endif // _LOGIN_HEADER_

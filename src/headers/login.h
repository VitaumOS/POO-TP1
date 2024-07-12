/*	<src/headers/login.h>

	@VitaumOS */

/*  Let it be a template... (Adapt whatever you feel the need for,
    as it was just a necessary testing example... 
    
    Here's a design choice: decide whether do you want to organize all the system
    into the screen object itself or as a separate thing... */


#ifndef _LOGIN_HEADER_
#define _LOGIN_HEADER_


#include "ui.h"


// The steps there will be at login screen.
typedef enum {
    LS_NOTHING,
    LS_ENTERED_USERNAME,
    LS_ENTERED_PASSWORD,
    LS_REPEATING_PASSWORD,
    LS_LOGGED
} LOGIN_STEPS;


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


#endif // _LOGIN_HEADER_

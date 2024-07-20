#ifndef _USER_HEADER_
#define _USER_HEADER_


#include <iostream>
#include "commons.h"


typedef enum {
    WRONG_PASSWORD,
    SUCCESSFUL_LOGIN,
} SYSTEM_LOGIN_CODE;


class User {
private:
    id_t id;    // Holds the ID for the user.

protected:
    bool logged;
    std::string string_buffer;

public:
    virtual SYSTEM_LOGIN_CODE login(void) { return SUCCESSFUL_LOGIN; };
    virtual void Interface(void) = 0;
};


#endif // _USER_HEADER_

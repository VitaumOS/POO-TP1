#ifndef _USER_HEADER_
#define _USER_HEADER_


#include <iostream>
#include "commons.h"


typedef enum {
    WRONG_PASSWORD,
    SUCCESSFUL_LOGIN,
} SYSTEM_LOGIN_CODE;


constexpr size_t username_string_length = 64ULL;
constexpr size_t password_string_length = 64ULL;

typedef char user_username_t[username_string_length];
typedef char user_password_t[password_string_length];


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

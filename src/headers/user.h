#ifndef _USER_HEADER_
#define _USER_HEADER_


#include "commons.h"


typedef enum {
    WRONG_PASSWORD,
    SUCCESSFUL_LOGIN,
} SYSTEM_LOGIN_CODE;


class User {
private:
    int id      : 31;
    bool logged : 1;

public:
    User(int id);
    ~ User(void) { };
    
    virtual SYSTEM_LOGIN_CODE login(void) {
        this -> logged = true;
        return SUCCESSFUL_LOGIN;
    }
};


#endif // _USER_HEADER_

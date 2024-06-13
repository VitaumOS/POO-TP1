#ifndef _WORKER_HEADER_
#define _WORKER_HEADER_


#include "commons.h"


typedef enum {
    WRONG_PASSWORD,
    SUCCESSFUL_LOGIN,
} SYSTEM_LOGIN_CODE;


class Worker {
private:
    int id      : 31;
    bool logged : 1;

public:
    Worker(int id); 
    ~ Worker(void);
    
    virtual SYSTEM_LOGIN_CODE login(void) {
        this -> logged = true;
        return SUCCESSFUL_LOGIN;
    }
};


#endif // _WORKER_HEADER_

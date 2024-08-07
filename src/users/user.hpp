#ifndef _USER_HEADER_
#define _USER_HEADER_


#include <iostream>
#include "commons.h"
#include "stdexcept"
#include "../databases/so-db.hpp"


constexpr size_t username_string_length = 64ULL;
constexpr size_t password_string_length = 64ULL;

typedef char username_string_t[username_string_length];
typedef char password_string_t[password_string_length];

/*

*/
class User {
private:
    Id_t id;    // Holds the ID for the user.

protected:
    bool logged;
    SO_Manager * so_manager;
    std::string string_buffer;

public:
    User(Id_t id, SO_Manager * const so_manager) : id(id), so_manager(so_manager) 
    {
        printf("+ user so_manager=%p\n", so_manager);

        if (so_manager == nullptr) {
            char string_buffer[64];
            sprintf(string_buffer, "Invalid SO_Manager passing to user (id:%llu) - nullptr.", id);

            throw std::runtime_error(string_buffer);
        }
    }
    
    virtual ~User(void) {
        // blank ~
    }

    // virtual SYSTEM_LOGIN_CODE login(void) { return SUCCESSFUL_LOGIN; };
    
    inline Id_t get_id(void) const { 
        return id; 
    }

    /*  Handles the interaction and UI with the user-type. */
    virtual void interact(void) = 0;
};














class Mechanic : virtual public User {
public:
    Mechanic(Id_t id, SO_Manager * so_manager) : User(id, so_manager) {
        // blank ~
    }

    virtual ~Mechanic(void) {
        // blank ~
    }

    virtual void interact(void) {
        // blank ~
    }
};


#endif // _USER_HEADER_

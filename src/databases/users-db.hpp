/*  <src/databases/users-db.hpp>

    Declares the users database. */

/*	Last update: 21/07/2024. */


#ifndef _USERS_DATABASE_HPP_INCLUDED_
#define _USERS_DATABASE_HPP_INCLUDED_


#include "../users/user.hpp"
#include "../users/admin.hpp"
#include "../users/seller.hpp"
#include "databases.hpp"
#include "so-db.hpp"


enum USER_TYPE {
    USER_TYPE_ADM,
    USER_TYPE_SLR,
    USER_TYPE_MCH
};


typedef union {
    id_t id;

    struct {
        id_t serial     : 62;
        id_t user_type  : 2;
    };
} user_id_t;


constexpr c_filepath users_DB_filename = "data/users.bin";

/*  Represents the data that is stored on the database for each user. */
struct UserData {
    user_id_t id;
    enum USER_TYPE type;

    char username[username_string_length];
    char password[password_string_length];

    struct _Date registry_date;
    struct _Date last_login;
};


class Users_DB : virtual public Database <struct UserData> {
private:
    
    void fprint_element(FILE * _OutputStream, const struct UserData * _User) const;

    /*  Stream-header */
    struct {
        id_t adm;
        id_t seller;
        id_t mechanic;
    } next_id; // Holds the next sequential IDs for each user type.

    bool reset_database(void);
    bool retrieve_stream_header(void);
    bool update_stream_header(void) const;

    SO_Manager * so_manager;

public:
    Users_DB(void);
    Users_DB(SO_Manager * const so_manager);
    ~Users_DB(void);

    // bool login(const username_string_t, const password_string_t, class User * const);

    bool register_user(enum USER_TYPE type, const username_string_t username, const password_string_t password);
    bool login(const username_string_t, const password_string_t, class User ** the_user);

    int64_t fetch_username(const username_string_t username, struct UserData * const return_data);
};



#endif // _USERS_DATABASE_HPP_INCLUDED_

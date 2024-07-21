/*  <src/databases/users-db.hpp>

    Declares the users database. */

/*	Last update: 21/07/2024. */


#ifndef _USERS_DATABASE_HPP_INCLUDED_
#define _USERS_DATABASE_HPP_INCLUDED_


#include "../users/user.hpp"
#include "databases.hpp"


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

    char username[username_string_length];
    char password[password_string_length];

    struct _Date registry_date;
    struct _Date last_login;
};


class Users_DB : virtual public Database <struct UserData> {
private:

    void fprint_element(FILE * _OutputStream, const struct UserData * _User);

    /*  Stream-header */
    struct {
        id_t adm;
        id_t seller;
        id_t mechanic;
    } next_id; // Holds the next sequential IDs for each user type.

    bool reset_database(void);
    bool retrieve_stream_header(void);
    bool update_stream_header(void) const;

    int64_t fetch_username(const user_username_t username, struct UserData * const return_data);
    
public:
    Users_DB(void);
    ~Users_DB(void);

    bool register_user(enum USER_TYPE type, const user_username_t username, const user_password_t password);
    bool login(const user_username_t username, const user_password_t);
};



#endif // _USERS_DATABASE_HPP_INCLUDED_

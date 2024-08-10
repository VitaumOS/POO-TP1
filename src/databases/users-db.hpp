/*  <src/databases/users-db.hpp>

    Declares the users database. */

/*	Last update: 21/07/2024. */


#ifndef _USERS_DATABASE_HPP_INCLUDED_
#define _USERS_DATABASE_HPP_INCLUDED_


// #include "../users/user.hpp"
#include "databases.hpp"
#include "so-db.hpp"


enum USER_TYPE {
    USER_TYPE_ADM,
    USER_TYPE_SLR,
    USER_TYPE_MCH,
    USER_TYPE_UNKOWN,
};


typedef union {
    Id_t id;

    struct {
        Id_t serial     : 62;
        Id_t user_type  : 2;
    };
} user_id_t;

inline bool operator==(const user_id_t & id_1, const user_id_t & id_2) { return id_1.id == id_2.id; }


constexpr c_filepath users_DB_filename = "data/users.bin";

struct MinimalUserData {
    /*  header-data */
    user_id_t id;
    enum USER_TYPE type;
    
    /*  inner data */
    bool active;
    username_string_t username;
};

/*  Represents the data that is stored on the database for each user. */
struct UserData {
    /* header data */
    user_id_t id;
    enum USER_TYPE type;
    
    /*  inner data */
    bool active;
    // bool logged;

    username_string_t username;
    password_string_t password;

    /*  meta-data */
    struct _Date registry_date;
    struct _Date last_login;
    struct _Date last_update;
    // struct _Date last_session_time;
    // struct _Date total_session_time;
    
    uint64_t interactions;
    uint8_t wrong_password_attempts;
};



class UsersDatabase : virtual public Database <struct UserData> {
private:
    
    void fprint_element(FILE * _OutputStream, const struct UserData * _User) const;

    /*  Stream-header */
    struct {
        Id_t adm;
        Id_t seller;
        Id_t mechanic;
    } next_id; // Holds the next sequential IDs for each user type.

    bool reset_database(void);
    bool retrieve_stream_header(void);
    bool update_stream_header(void) const;

    SO_Manager * so_manager;

    bool update_userdata(const struct UserData & new_user_data);

    int64_t fetch_userid(const user_id_t &, struct UserData &) const;
    int64_t fetch_username(const username_string_t, struct UserData &) const;
    int64_t fetch_username(const username_string_t) const;

    bool in_database(const Id_t &) const;
    bool in_database(const user_id_t &) const;

public:
    UsersDatabase(void);
    UsersDatabase(SO_Manager * const so_manager);
    ~UsersDatabase(void);

    bool register_user(enum USER_TYPE type, const username_string_t username, const password_string_t password);
    
    bool login(const username_string_t, const password_string_t, struct MinimalUserData &) const;
    bool logout(const struct MinimalUserData &) const;
    bool logout(const user_id_t &) const;


    bool fetch_id(const Id_t &, struct MinimalUserData &);

    friend class UsersEditor;   /*  Due to the editor needing more deep information about the users... */
    friend class Administrator;
    friend class Seller;
};


#endif // _USERS_DATABASE_HPP_INCLUDED_

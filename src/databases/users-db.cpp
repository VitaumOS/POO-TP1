/*  <src/databases/users-db.hpp>

    Defines the users database. */


#include "users-db.hpp"
#include <string.h>	// for strcpy, strcmp
#include <assert.h>
#include <stdexcept>


struct USERDB_stream_header {
    Id_t item_qtt;

    struct {
        Id_t adm;
        Id_t seller;
        Id_t mechanic;
    } next_id;
};

UsersDatabase::UsersDatabase(void) : Database(users_DB_filename, sizeof(item_qtt) + sizeof(next_id)) {
    if (! could_initialize()) {
        std::cerr << "Coudln't initialize UsersDatabase." << std::endl;
        return;
    }

    if (! UsersDatabase::retrieve_stream_header())
    {
        if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0) {
            UsersDatabase::reset_database();
        }
    }
    
    #if DEBUG_DATABASE_STATE
    std::cout << "Initial users database state:" << std::endl;
    print_database();
    #endif // DEBUG_DATABASE_STATE
}

UsersDatabase::UsersDatabase(SO_Manager * const so_manager) : UsersDatabase()
{
    if (so_manager == nullptr)
        throw std::runtime_error("Invalid SO_Manager on UsersDatabase.");

    this->so_manager = so_manager;
}

UsersDatabase::~UsersDatabase(void) {
    if (stream == nullptr) return; // the database is already closed.

    #if DEBUG_DATABASE_STATE
    std::cout << "Final users database state:" << std::endl;
    print_database();
    #endif // DEBUG_DATABASE_STATE

    if (! UsersDatabase::update_stream_header())
        std::cerr << "Stream header couldn't be written at UsersDatabase." << std::endl;

    finalize_stream();
}

bool UsersDatabase::reset_database(void) {
    next_id = { 0, 0, 0 };
    item_qtt = 0;

    if (! update_stream_header())
        return false;

    /*  Initializing the base admin login *
     *  --------------------------------- */

    struct _Date date_of_now;
    if (! get_date(date_of_now))
        return false;

    const struct UserData the_user = {
        .id = 0,
        .type = USER_TYPE_ADM,

        .active = true,
        // .logged = false,
        .username = "cmrd",
        .password = "dhr",

        .registry_date = date_of_now,
        .last_login = { 0, 0, 0, 0, 0, 0 },
        .last_update = date_of_now,
        // .last_session_time = { 0, 0, 0, 0, 0, 0 },
        // .total_session_time = { 0, 0, 0, 0, 0, 0 },

        .interactions = 0,
        .wrong_password_attempts = 0,
    };
    
    if (! write_element(item_qtt, &the_user)) {

        return false;
    }

    ++ item_qtt;
    ++ next_id.adm;
    
    return true;
}

bool UsersDatabase::retrieve_stream_header(void) {
    struct USERDB_stream_header _stream_header;

    rewind(stream);
    if (fread(&_stream_header, sizeof(_stream_header), 1, stream) < 1)
        return false;

    item_qtt            = _stream_header.item_qtt;
    next_id.adm         = _stream_header.next_id.adm;
    next_id.seller      = _stream_header.next_id.seller;
    next_id.mechanic    = _stream_header.next_id.mechanic;

    return true;
}

bool UsersDatabase::update_stream_header(void) const {
    struct USERDB_stream_header _stream_header = {
        .item_qtt = item_qtt,
        .next_id = {
            .adm =      next_id.adm,
            .seller =   next_id.seller,
            .mechanic = next_id.mechanic
        }
    };

    rewind(stream);
    return fwrite(&_stream_header, sizeof(_stream_header), 1, stream) > 0;
}

bool UsersDatabase::update_userdata(const struct UserData & new_user_data)
{
    /*  in case the user does not yet exist on the database */
    if (! UsersDatabase::in_database(new_user_data.id))
        return false;
    
    struct UserData old_data;

    /*  fetching the current data on the database */
    const int64_t index = UsersDatabase::fetch_userid(new_user_data.id, old_data);
    if (index < 0) // either IO error or couldn't match the id on the database...
        return false;

    /*  It cannot be the case that the user types are differents... */
    if (old_data.type != new_user_data.type)
        return false;

    /*  same for the registry and last-login dates... */
    if (old_data.last_login != new_user_data.last_login)        return false;
    if (old_data.registry_date != new_user_data.registry_date)  return false;

    /*  other meta-datas... */
    if (old_data.interactions != new_user_data.interactions)    return false;
    if (old_data.wrong_password_attempts != new_user_data.wrong_password_attempts)  return false;

    /*  as for the last_session_time, */

    /*  For control, only one change in the data is allowed at time...
        The data changing stands for either the username, password, or active situation. */
    bool had_change = false;

    std::string old_str_buffer(old_data.username);
    std::string cur_str_buffer(new_user_data.username);

    if (old_str_buffer != cur_str_buffer)
        had_change = true;

    old_str_buffer = old_data.password;
    cur_str_buffer = new_user_data.password;

    if (old_str_buffer != cur_str_buffer) {
        if (had_change)
            return false;
        had_change = true;
    }

    struct _Date date_of_now;
    if (! get_date(date_of_now))
        return false;


    struct UserData new_new_user_data = new_user_data;
    new_new_user_data.last_update = date_of_now;

    return write_element(static_cast<Id_t> (index), &new_new_user_data);
}

int64_t UsersDatabase::fetch_userid(const user_id_t & id, struct UserData & return_data) const {
    return Database::fetch_element(
        [id](const struct UserData & user_data)
        {
            return user_data.id == id;
        },
        return_data
    );
}

int64_t UsersDatabase::fetch_username(const username_string_t username, struct UserData & return_data) const {

    return Database::fetch_element(
        [username](const struct UserData & user_data) { 
            return ! strcmp(username, user_data.username); 
        }, 
        return_data );
}

int64_t UsersDatabase::fetch_username(const username_string_t username) const {
    struct UserData data_buffer;
    return fetch_username(username, data_buffer);
}

bool UsersDatabase::fetch_id(const Id_t & id, struct MinimalUserData & user_data) {
    std::cout << "Capturando o id <" << id << ">...\n";

    user_id_t user_id;
    user_id.id = id;

    struct UserData integral_user_data;
    if (UsersDatabase::fetch_userid(user_id, integral_user_data) < 0)
        return false;

    user_data.id = integral_user_data.id;
    user_data.type = integral_user_data.type;
    user_data.active = integral_user_data.active;
    strcpy(user_data.username, integral_user_data.username);

    return true;
}

bool UsersDatabase::register_user(enum USER_TYPE type, const username_string_t username, const password_string_t password) {
    struct UserData data_buffer;
    if (fetch_username(username, data_buffer) >= 0) {
        std::cerr << "O nome do usuário <" << std::string(username) << "> já existe..." << std::endl;
        return false;
    }

    user_id_t the_user_id = { 0 };

    switch (type)
    {
    case USER_TYPE_ADM: the_user_id.serial = next_id.adm;       break;
    case USER_TYPE_SLR: the_user_id.serial = next_id.seller;    break;
    case USER_TYPE_MCH: the_user_id.serial = next_id.mechanic;  break;
    default:            return false;
    }
    
    the_user_id.user_type = type;

    struct _Date date_of_now;
    if (! get_date(date_of_now))
        return false;
    
    struct UserData the_user = {
        .id = the_user_id,
        .type = type,
        
        .active = true,
        // .logged = false,

        .username = "undef",
        .password = "undef",

        .registry_date = date_of_now,
        .last_login = { 0, 0, 0, 0, 0, 0 },
        .last_update = date_of_now,
        // .last_session_time = { 0, 0, 0, 0, 0, 0 },
        // .total_session_time = { 0, 0, 0, 0, 0, 0 },

        .interactions = 0,
        .wrong_password_attempts = 0,
    };

    strcpy(the_user.username, username);
    strcpy(the_user.password, password);

    if (! write_element(item_qtt, &the_user)) {
        fprintf(stderr, "Erro ao escrever novo usuário registrado...\n");
        return false;
    }

    ++ item_qtt;

    switch (type)
    {
    case USER_TYPE_ADM: ++ next_id.adm;         break;
    case USER_TYPE_SLR: ++ next_id.seller;      break;
    case USER_TYPE_MCH: ++ next_id.mechanic;    break;
    default:                                    break;
    }

    return true;
}

bool UsersDatabase::in_database(const Id_t & id) const {
    const user_id_t user_id = { id };
    return UsersDatabase::in_database(user_id);
}

bool UsersDatabase::in_database(const user_id_t & user_id) const {
    uint64_t user_type_qtt = 0;
    switch (user_id.user_type)
    {
    case USER_TYPE_ADM:
        user_type_qtt = next_id.adm;
        break;

    case USER_TYPE_SLR:
        user_type_qtt = next_id.seller;
        break;

    case USER_TYPE_MCH:
        user_type_qtt = next_id.mechanic;
        break;

    default:    return false;
    }
    return user_id.serial < user_type_qtt;
}

/*  (...) */
bool UsersDatabase::login(const username_string_t username, const password_string_t password,
    struct MinimalUserData & m_data) const {
    // std::cout << "Attempting logging as " << std::string(username) << " (" << std::string(password) << ")" << std::endl;

    struct UserData the_user_data;
    int64_t user_index;
    if ((user_index = fetch_username(username, the_user_data)) < 0) {
        // the username isn't on the database.
        // std::cerr << "[UsersDatabase::login]: Username not found." << std::endl;
        return false;
    }

    /*  verifying if the user is inactive... */
    if (! the_user_data.active) {
        return false;
    }

    /*  verifying if it is already logged-in... */
    // if (the_user_data.logged) return false;

    // comparing credentials (checking the password)
    if (strcmp(password, the_user_data.password)) {
        // std::cerr << "[UsersDatabase::login]: Wrong Password." << std::endl;
        the_user_data.wrong_password_attempts ++;
        Database::write_element(static_cast<Id_t> (user_index), &the_user_data);
        return false;
    }

    struct _Date date_of_now;
    if (! get_date(date_of_now))
    {
        // std::cerr << "[UsersDatabase::login]: Couldn't get date..." << std::endl;
        return false;
    }

    /*  updating user's information on the database */
    the_user_data.last_login = date_of_now;
    // the_user_data.logged = true;
    // the_user_data.last_session_time = (struct _Date) { 0, 0, 0, 0, 0, 0 };

    if (! write_element(user_index, &the_user_data))
    {
        // std::cerr << "[UsersDatabase::login]: Couldn't update user." << std::endl;
        return false;
    }

    /*  Copying the reduced user-data into the return parameter buffer. */
    m_data = (struct MinimalUserData) {
        .id = the_user_data.id,
        .type = the_user_data.type,
        .active = the_user_data.active,
        .username = "",
    };
    strcpy(m_data.username, the_user_data.username);

    return true;
}

bool UsersDatabase::logout(const struct MinimalUserData & user_data) const {
    return UsersDatabase::logout(user_data.id);
}

bool UsersDatabase::logout(const user_id_t & id) const {
    if (! UsersDatabase::in_database(id))
        return false;

    struct UserData user_data;
    int64_t user_index = fetch_userid(id, user_data);
    if (user_index < 0)
        return false;

    // if (! user_data.logged) return false;

    // struct _Date date_of_now;
    // if (! get_date(date_of_now))
    //     return false;

    // struct _Date session_time = date_of_now - user_data.last_login;
    // user_data.last_session_time += session_time;
    // user_data.total_session-time += session_time;

    if (! write_element(user_index, &user_data))
        return false;
    
    /*  (...) */

    return true;
}


// db repr

inline void UsersDatabase::fprint_element(FILE * _OutputStream, const struct UserData * _User) const
{
    fprintf(_OutputStream, "[%llu | %06llu:%02d]: ",
        _User->id.id, _User->id.serial, _User->id.user_type);

    fprint_date(_OutputStream, _User->registry_date);
    fprintf(_OutputStream, ":");
    fprint_date(_OutputStream, _User->last_login);

    fprintf(_OutputStream, " <%s:%s>",
        _User->username,
        _User->password
    );
}

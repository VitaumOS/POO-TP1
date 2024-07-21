/*  <src/databases/users-db.hpp>

    Defines the users database. */

/*	Last update: 21/07/2024. */


#include "users-db.hpp"
#include <string.h>	// for strcpy, strcmp
#include <assert.h>


struct USERDB_stream_header {
    id_t item_qtt;
    
    struct {
        id_t adm;
        id_t seller;
        id_t mechanic;
    } next_id;
};

Users_DB::Users_DB(void) : Database(users_DB_filename, sizeof(item_qtt) + sizeof(next_id)) {
    if (! could_initialize()) {
        std::cerr << "Coudln't initialize Users_DB." << std::endl;
        return;
    }

    if (! Users_DB::retrieve_stream_header())
    {
        if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0) {
            Users_DB::reset_database();
        }
    }

    std::cout << "Initial users database state:" << std::endl;
    print_database();
}

Users_DB::~Users_DB(void) {
    if (stream == nullptr) return; // the database is already closed.

    std::cout << "Final users database state:" << std::endl;
    print_database();

    if (! Users_DB::update_stream_header())
        std::cerr << "Stream header couldn't be written at Users_DB." << std::endl;

    finalize_stream();
}

bool Users_DB::reset_database(void) {
    next_id = { 0, 0, 0 };
    item_qtt = 0;

    if (! update_stream_header())
        return false;

    /*  Initializing the base admin login *
     *  --------------------------------- */

    const user_username_t username = "cmrd";
    const user_password_t password = "dhr";




    return true;
}

bool Users_DB::retrieve_stream_header(void) {
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

bool Users_DB::update_stream_header(void) const {
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




int64_t Users_DB::fetch_username(const user_username_t username, struct UserData * const return_data) {

    struct UserData ud_buffer;
    size_t iterator = 0;
    while ((iterator < item_qtt) && read_element(iterator, &ud_buffer))
    {
        if (! strcmp(ud_buffer.username, username))
        {
            * return_data = ud_buffer;
            return (int64_t) iterator;
        }
        iterator ++;
    }
    return -1;
}

bool Users_DB::register_user(enum USER_TYPE type, const user_username_t username, const user_password_t password) {
    printf(">> [%s(%d, %s, %s)]\n", __func__, (int) type, username, password);

    struct UserData data_buffer;
    if (fetch_username(username, &data_buffer) >= 0) {
        std::cerr << "The username <" << std::string(username) << "> does already exists..." << std::endl;
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
        .username = "undef",
        .password = "undef",

        .registry_date = date_of_now,
        .last_login = { 0, 0, 0, 0, 0, 0 },
    };

    strcpy(the_user.username, username);
    strcpy(the_user.password, password);

    if (! write_element(item_qtt, &the_user)) {
        fprintf(stderr, "error writing new registering user...\n");
        return false;
    }

    ++ item_qtt;

    switch (type)
    {
    case USER_TYPE_ADM: ++ next_id.adm;         break;
    case USER_TYPE_SLR: ++ next_id.seller;      break;
    case USER_TYPE_MCH: ++ next_id.mechanic;    break;
    }

    return true;
}



bool Users_DB::login(const user_username_t username, const user_password_t password) {
    std::cout << "Attempting logging as " << std::string(username) << " (" <<
        std::string(password) << ")" << std::endl;

    struct UserData the_user;
    int64_t user_index;
    if ((user_index = fetch_username(username, &the_user)) < 0) {
        // the username isn't on the database.
        std::cerr << "[Users_DB::login]: Username not found." << std::endl;
        return false;
    }

    // comparing credentials (checking the password)
    if (strcmp(password, the_user.password)) {
        std::cerr << "[Users_DB::login]: Wrong Password." << std::endl;
        return false;
    }

    struct _Date date_of_now;
    if (! get_date(date_of_now))
    {
        std::cerr << "[Users_DB::login]: Couldn't get date..." << std::endl;
        return false;
    }

    the_user.last_login = date_of_now;
    if (! write_element(user_index, &the_user))
    {
        std::cerr << "[Users_DB::login]: Couldn't update user." << std::endl;
        return false;
    }

    return true;
}










// db repr

inline void Users_DB::fprint_element(FILE * _OutputStream, const struct UserData * _User)
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
/*
std::ostream & operator<<(std::ostream & stream, const client_id_t & ID)
{
    char string_buffer[16];
    sprintf(string_buffer, "[%06llu:%02d]", ID.person_id, (int) ID.vehicle_id);
    stream << string_buffer;
    return stream;
}
*/
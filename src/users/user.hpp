/*  <src/suers/user.hpp>

    (...) */


#ifndef _USER_HEADER_
#define _USER_HEADER_


#include "commons.h"
#include "../ui/ui.hpp"
#include "../databases/so-db.hpp"
#include "../databases/users-db.hpp"

#include <stdexcept>


class UserScreen : virtual public MenuScreen
{
protected:
    struct MinimalUserData user_data;   // minimal data representing the user of actuation.
    class SO_Manager * so_manager;  // SO database
    class UsersDatabase * users_db; // Users database

    struct _Date interaction_begin; // tracks when the interaction with the user began.

    /*  TODO: quit_loop method... This attribute -> private.*/
    bool main_loop = true;  // tracks the main loop on the screen's interaction.

    char * menu_title = nullptr;
    void render_menu_header(void) const;

protected:  /*  pure virtual methods  */
    virtual int render(void) const  = 0;
    virtual int process(void)       = 0;

public:
    UserScreen(class SO_Manager * const, class UsersDatabase * const, const struct MinimalUserData &);

    int interact(void) override;
    inline Id_t get_id(void) const { return static_cast<Id_t> (user_data.id.id); }

};

#endif // _USER_HEADER_

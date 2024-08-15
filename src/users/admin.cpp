

#include "admin.hpp"
#include "../ui/db-vizualizers.hpp"
#include <string>


// Construtor do Admnistrador
Administrator::Administrator(class SO_Manager * const so_manager, class UsersDatabase * const users_db, 
    const struct MinimalUserData & user_data) : UserScreen(so_manager, users_db, user_data) {

    constexpr const char * title = "Menu de Administrador";
    UserScreen::menu_title = const_cast<char *> (title);
}

// Destrutor do Admnistrador
Administrator::~Administrator(void) {

}

int Administrator::render(void) const
{
    clean_screen();

    /*  header */
    UserScreen::render_menu_header();

    /*  footer */
    constexpr const char * footer_title = "O que desejas fazer?";
    //print_n_char('-', literal_string_length(footer_title) - 1);
    print_n_char('=', 50);
    std::cout << std::endl << footer_title << std::endl;
    std::cout << "1\t->\tSair" << std::endl;
    std::cout << "2\t->\tGerenciar usuários" << std::endl;

    fflush(stdout);
    return 0;
}

int Administrator::process(void)
{
    int option = -1;
    std::cin >> option;
    MenuScreen::clean_stdin();

    switch (option) {
        /*  Exit */
    case 1:
        UserScreen::main_loop = false;
        break;

    case 2:
        Administrator::edit_users();
        break;

    default:
        /*  Invalid input... */
        break;
    }
    return 0;
}

void Administrator::edit_users(void)
{
    struct UserData self;

    // TODO: add more control and prevention at this fetching...?
    users_db->fetch_userid(user_data.id, self);

    UsersEditor edit_users_menu(users_db, self);
    edit_users_menu.interact();
}


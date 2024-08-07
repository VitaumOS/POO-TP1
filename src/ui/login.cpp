/*	<src/login.cpp>

    Where the definition of the login menu is given. */


#include "login.hpp"
#include <stdexcept>

/*
22, 12, 40
239, 203, 104
225, 239, 230
174, 183, 179
0, 4, 17
*/

void LoginScreen::check_databases(void)
{

    if (so_manager == nullptr)
        throw std::runtime_error("ASD");

    if (! so_manager->could_initialize())
        throw std::runtime_error("ASD2");

    if (users_db == nullptr)
        throw std::runtime_error("ASD3");
    if (! users_db->could_initialize())
        throw std::runtime_error("Error initializing LoginScreen due to improper users database passing...");
}

LoginScreen::LoginScreen(class SO_Manager * const so_manager,  class Users_DB * const users_db) : 
    MenuScreen(), so_manager(so_manager), users_db(users_db)
{
    LoginScreen::check_databases();
}

LoginScreen::LoginScreen(int w, int h, 
    class SO_Manager * const so_manager, class Users_DB * const users_db) : 
    MenuScreen(w, h), so_manager(so_manager), users_db(users_db)
{
    LoginScreen::check_databases();

    MenuScreen::bg = (struct rgb) { 22, 12, 40 };
    MenuScreen::fg = (struct rgb) {174, 183, 179};
}

LoginScreen::~LoginScreen(void)
{
    if (user_buffer == nullptr)
        return;
    delete user_buffer;
    user_buffer = nullptr;
}

bool LoginScreen::capture_credentials(void) {
    std::cout << "Nome de usuário:\t";
    if (! (std::cin >> username_buffer))
        return false;

    std::cout << "Senha:\t\t\t";
    if (! (std::cin >> password_buffer))
        return false;
    
    struct MinimalUserData return_user_data;
    if (! users_db->login(username_buffer.c_str(), password_buffer.c_str(), return_user_data)) {
        std::cout << "Credenciais inválidas!" << std::endl;
        return false;
    }
    
    switch (return_user_data.type)
    {
    case USER_TYPE_ADM:     user_buffer = new Administrator(return_user_data.id.id, so_manager, users_db);  break;
    case USER_TYPE_SLR:     user_buffer = new Seller(return_user_data.id.id, so_manager);                   break;
    case USER_TYPE_MCH:     user_buffer = new Mechanic(return_user_data.id.id, so_manager);                 break;
    default:                return false;
    }

    return true;
}

int LoginScreen::render(void) {
    clean_screen();
    /*
    MenuScreen::set_bg();
    MenuScreen::set_fg();
    MenuScreen::fill_char(' ');
    
    const int username_label_x = MenuScreen::width >> 3;
    const int username_label_y = MenuScreen::height >> 2;

    MenuScreen::pos_string(username_label_x, username_label_y, "Nome de usuário: ");
    MenuScreen::pos_string(username_label_x, username_label_y + 1, "Senha: ");
    MenuScreen::to_ending();
    */

    std::cout << "Login\n\n\n";

    return 0;
}

int LoginScreen::interact(void) { //TODO: criar um meio de fazer a pessoa quitar daqui
    user_buffer = nullptr;

    login_loop = true;
    while (login_loop)
    {
        LoginScreen::render();

        if (! LoginScreen::capture_credentials()) {
            std::cout << "Deseja continuar? ";
            login_loop = input_verification();
        }
        else login_loop = false;
    }

    if (user_buffer != nullptr)
        return 1;
    return 0;
}

int LoginScreen::user_interact(void)
{
    if (user_buffer == nullptr)
        throw std::runtime_error("Interaction with invalid user at login-screen...");
    user_buffer->interact();
    return 0;
}

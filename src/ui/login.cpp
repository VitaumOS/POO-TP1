/*	<src/login.cpp>

    Where the definition of the login menu screen takes place. */


#include <stdexcept> // for std::runtime_error
#include "login.hpp"

/*  Verifies brutely the initialization state of the screen's arguments:
    raises errors if some database isn't on a valid state. */
void LoginScreen::check_databases(void)
{
    if (LoginScreen::so_manager == nullptr)
        throw std::runtime_error("Objeto <SO_Manager> inválido para <LoginScreen>");

    if (! LoginScreen::so_manager->could_initialize())
        throw std::runtime_error("SO_Manager não pôde ser inicializado. Verificado em LoginScreen");

    if (LoginScreen::users_db == nullptr)
        throw std::runtime_error("Objeto UsersDatabase inválido para LoginScreen");

    if (! LoginScreen::users_db->could_initialize())
        throw std::runtime_error("Não foi possível inicializar o UsersDatabase. Verificado em LoginScreen");
}

LoginScreen::LoginScreen(class SO_Manager * const so_manager,  class UsersDatabase * const users_db) : 
    MenuScreen(), so_manager(so_manager), users_db(users_db) {
    LoginScreen::check_databases();
}

LoginScreen::LoginScreen(int w, int h, 
    class SO_Manager * const so_manager, class UsersDatabase * const users_db) : 
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
    case USER_TYPE_ADM:     user_buffer = new Administrator(so_manager, users_db, return_user_data);    break;
    case USER_TYPE_SLR:     user_buffer = new Seller(so_manager, users_db, return_user_data);           break;
    case USER_TYPE_MCH:     user_buffer = new Mechanic(so_manager, users_db, return_user_data);         break;
    default:                return false;
    }

    return true;
}

int LoginScreen::render(void) {
    clean_screen();

    /*  TODO: Ponder this...
    MenuScreen::set_bg();
    MenuScreen::set_fg();
    MenuScreen::fill_char(' ');
    
    const int username_label_x = MenuScreen::width >> 3;
    const int username_label_y = MenuScreen::height >> 2;

    MenuScreen::pos_string(username_label_x, username_label_y, "Nome de usuário: ");
    MenuScreen::pos_string(username_label_x, username_label_y + 1, "Senha: ");
    MenuScreen::to_ending();
    */

    std::cout << "Oficina: Login\n";
    std::cout << "-------  -----\n\n\n";

    return 0;
}

int LoginScreen::interact(void) {
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
        throw std::runtime_error("Interação com usuário inválido na tela de login...");
    user_buffer->interact();
    return 0;
}

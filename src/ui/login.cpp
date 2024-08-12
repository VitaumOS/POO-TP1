/*	<src/login.cpp>

    Where the definition of the login menu screen takes place. */


#include <stdexcept> // for std::runtime_error
#include "login.hpp"

/*  Verifies brutely the initialization state of the screen's arguments:
    raises errors if some database isn't on a valid state. */
void LoginScreen::check_databases(void)
{
    if (LoginScreen::so_manager == nullptr)
        throw std::runtime_error("Objeto <SO_Manager> inv�lido para <LoginScreen>");

    if (! LoginScreen::so_manager->could_initialize())
        throw std::runtime_error("SO_Manager n�o p�de ser inicializado. Verificado em LoginScreen");

    if (LoginScreen::users_db == nullptr)
        throw std::runtime_error("Objeto UsersDatabase inv�lido para LoginScreen");

    if (! LoginScreen::users_db->could_initialize())
        throw std::runtime_error("N�o foi poss�vel inicializar o UsersDatabase. Verificado em LoginScreen");
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

    MenuScreen::bg = (struct rgb) { 21, 21, 25 };
    MenuScreen::fg = (struct rgb) { 174, 183, 179 };
}

LoginScreen::~LoginScreen(void)
{
    if (user_buffer == nullptr)
        return;
    delete user_buffer;
    user_buffer = nullptr;
}

void LoginScreen::render_invalid_credentials(void) const
{
    go_to_abs(25 + (MenuScreen::width >> 3), 4 + (MenuScreen::height >> 2));
    set_bg();
    std::cout << "Credenciais invalidas!" << std::endl;
    go_to_abs(25 + (MenuScreen::width >> 3), 5 + (MenuScreen::height >> 2));
}

bool LoginScreen::capture_credentials(void) {
    aec_bg_rgb_l(21, 21, 25);
    go_to_abs(18 + (MenuScreen::width >> 3), MenuScreen::height >> 2);
    if (! (std::cin >> username_buffer))
        return false;
    
    aec_reset();
    aec_fg_rgb_l(0, 0, 0);
    aec_bg_rgb_l(21, 21, 25);
    go_to_abs(18 + (MenuScreen::width >> 3), 1 + (MenuScreen::height >> 2));
    if (! (std::cin >> password_buffer))
        return false;
    aec_reset();
    /*
    std::cout << "Nome de usuario:\t";

    std::cout << "Senha:\t\t\t";
    */
    struct MinimalUserData return_user_data;
    if (! users_db->login(username_buffer.c_str(), password_buffer.c_str(), return_user_data)) {
        render_invalid_credentials();
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

int LoginScreen::render(void) const {
    clean_screen();

    MenuScreen::set_bg();
    MenuScreen::set_fg();
    MenuScreen::fill_char(' ');
    to_beggining();
    print_n_char('=', width);
    constexpr const char * title = "Oficina: Login";
    std::cout << std::endl;
    aec_crs_right(((width / 2) - (literal_string_length(title) / 2)));
    std::cout << title << std::endl;
    print_n_char('=', width);

    const int username_label_x = MenuScreen::width >> 3;
    const int username_label_y = MenuScreen::height >> 2;
    
    MenuScreen::pos_string(username_label_x, username_label_y, "Nome de usuario: ");
    MenuScreen::pos_string(username_label_x, username_label_y + 1, "Senha: ");
    MenuScreen::to_ending();
    print_n_char('=', width);

    aec_reset();
    fflush(stdout);
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
            aec_reset();
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
        throw std::runtime_error("Interacao com usuario invalido na tela de login...");
    user_buffer->interact();
    return 0;
}

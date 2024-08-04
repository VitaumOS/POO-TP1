/*	<src/login.cpp>

    Where the definition of the login menu is given. */


#include "login.hpp"
#include <stdexcept>


LoginScreen::LoginScreen(class Users_DB * users_db) : Screen(), users_db(users_db)
{
    if (! users_db->could_initialize())
        throw std::runtime_error("VTMNC.");
}

LoginScreen::~LoginScreen(void)
{
    
}

bool LoginScreen::input_credentials(void) {
    std::cout << "Nome de usuário:\t";
    if (! (std::cin >> username_buffer))
        return false;

    std::cout << "Senha:\t\t\t";
    if (! (std::cin >> password_buffer))
        return false;
    
    if (! users_db->login(username_buffer.c_str(), password_buffer.c_str(), &user_buffer)) {
        std::cout << "Credenciais inválidas!" << std::endl;
        return false;
    }

    return true;
}

int LoginScreen::render(void) {
    clean_screen();

    printf("Login\n\n");
    
    std::cout << "\n\n\n\nCALA A BOCA\n" << std::endl;

    return 0;
}

User * LoginScreen::menu(void) { //TODO: criar um meio de fazer a pessoa quitar daqui
    user_buffer = nullptr;

    login_loop = true;
    while (login_loop)
    {
        LoginScreen::render();

        if (! LoginScreen::input_credentials()) {
            std::cout << "Deseja continuar? ";

            login_loop = input_verification();
        }
        else login_loop = false;
    }
    // Users_DB :: bool login(const username_string_t, const password_string_t, class User ** the_user);

    std::cout << "Logado!" << std::endl;
    
    return user_buffer;
}


#if 0
void LoginScreen::capture_username(void) {
    std::cin >> input_username;
    input_username.resize(16);
    printf("input_username: <%s>\n", input_username.c_str());
}

void LoginScreen::capture_password(void) {
    std::cin >> input_password;
    input_password.resize(16);
    printf("input_password: <%s>\n", input_password.c_str());
}

void LoginScreen::render(void)
{
    clean_screen();

    // Coloring the background...
    set_bg(); 
    fill_char(' ');

    set_fg();

    // marking a big x at screen's center (demonstration)
    go_to_rel(CENTER);
    printf("X");
    
    centralize_string_at(width >> 3, (height >> 3), "Login")
        && centralize_string_at(width >> 3, (height >> 3) + 1, "=====");

    username_label.render_at_screen(this);
    password_label.render_at_screen(this);

    to_ending();
    aec_reset();
}
#endif

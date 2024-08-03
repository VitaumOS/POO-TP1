/*	<src/login.cpp>

	@VitaumOS */

/*  Let it be a template... (Adapt whatever you feel the need for,
    as it was just a necessary testing example... */


#include "headers/login.h"

#if LOGIN


bool Login::InputUserPass(Users_DB * user_db){

    std::string username_buffer;
    std::string password_buffer;
    
    if (! (std::cin >> username_buffer >> password_buffer)) 
        continue;

    if (! user_db.login(username_buffer.c_str(), password_buffer.c_str(), &user)){
        std::cout << "Usuário ou Senha Inválido!" << std::endl;
        return true;
    }

    return false;
    
}

User * Login::LoginMenu(void){ //TODO: criar um meio de fazer a pessoa quitar daqui
#if 0
    main_loop = true;  // Sinaliza o inicio do loop da função

    SO_Manager so_manager;
    Users_DB user_db(&so_manager);


    if (! user_db.could_initialize())
        return -1;
    
    login_loop = true;

    // LOGGANDO COM O CARA CMRD
    while (login_loop) //TODO: transformar isso em um método
    {
        login_loop = InputUserPass(user_db);
        if(login_loop){
            input_verification();
        }
    }
    // Users_DB :: bool login(const username_string_t, const password_string_t, class User ** the_user);

    std::cout << "Logado!" << std::endl;
    user->logged=true;
        
    return user;
#endif
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
#endif

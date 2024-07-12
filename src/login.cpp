/*	<src/login.cpp>

	@VitaumOS */

/*  Let it be a template... (Adapt whatever you feel the need for,
    as it was just a necessary testing example... */


#include "headers/login.h"


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

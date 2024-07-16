/*  <src/workshop.cpp>

    The entry point for the system. */


#include <stdio.h>
#include <iostream>
#include <login.h>
#include <databases.h>
#include <string.h>


//
/*  (Program specs.) */
int main(void) {

    /*  Poor login screen testing... */
#   define LOGIN_SCREEN_TEST		false
#   if LOGIN_SCREEN_TEST
    LoginScreen login_screen(50, 25, (rgb) { 43, 43, 43 }, (rgb) { 200, 200, 225 });
    login_screen.render();

    login_screen.go_to_abs((50 >> 3) + 12, (25 >> 1) - 2);
    login_screen.capture_username();
    login_screen.capture_password();
#   endif // LOGIN_SCREEN_TEST
	
#	define CLIENT_GENERATION_TEST	false
#	if CLIENT_GENERATION_TEST

	ClientManager client_manager;
	client_manager.register_client("Hebert", (struct Vehicle) { .type = "tipo-doido", .model = "modelo-doido", .mileage = 100 }, 9);


#	endif // CLIENT_GENERATION_TEST

#	define SO_GENERATION_TEST		true
#	if SO_GENERATION_TEST
    SO_Manager so_manager;

	struct ServiceOrder SO;
    if (! so_manager.new_order("quebrou ne fi", { (2 << VEHICLE_ID_BITS) + 0 }, & SO))
        fprintf(stderr, "FALHOU TUDO\n");

#	endif // SO_GENERATION_TEST




    return 0;
}
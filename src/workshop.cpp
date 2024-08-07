/*  <src/workshop.cpp>

    The entry point for workshop the system. */


#include <workshop.h>
#include <string>


static int __main(void)
{   

    class SO_Manager so_manager;
    if (! so_manager.could_initialize())    return -1;

    class Users_DB users_db(&so_manager);
    if (! users_db.could_initialize())      return -2;
    
    class LoginScreen login_screen(50, 25, &so_manager, &users_db);

    bool main_loop = true;
    while (main_loop)
    {   
        if (login_screen.interact() > 0)
        {
            login_screen.user_interact();
            continue;
        }

        std::cout << "Deseja encerrar o programa? ";
        if (input_verification())   main_loop = false;
    }
    
    return 0;
}

/*  (Program specs.) */
int main(void) {
    /*  Language setup */
    setlocale(LC_ALL, "portuguese");

#   define  TESTSOMENU                  false
#   if      TESTSOMENU

    SO_Manager so_manager;
    so_manager.so_vizualizer();

#endif

#   define MAIN_SERIA                   true
#   if MAIN_SERIA

    return __main();

#   endif // MAIN_SERIA

#   define USERS_DB_TEST                false
#   if USERS_DB_TEST
    
    // TODO: 

    Users_DB users_manager;
    users_manager.register_user(USER_TYPE_ADM, "cmrd-admin", "123");
    users_manager.register_user(USER_TYPE_SLR, "cmrd-seller", "123");
    users_manager.register_user(USER_TYPE_MCH, "cmrd-mechanic", "123");
    users_manager.login("cmrd-admin", "123");


#   endif // USERS_DB_TEST

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

	ClientsManager client_manager;
    struct Client client_buffer;
    if (! client_manager.register_client("Hebert", (struct Vehicle) { .type = "tipo-doido", .model = "modelo-doido", .mileage = 100 }, & client_buffer, 2)) fprintf(stderr, "COULDN'T REGISTRY CLIENT\n");
    //if (! client_manager.register_client("Andrd", (struct Vehicle) { .type = "tipo-maluco", .model = "modelo-doidera", .mileage = 150 }, & client_buffer,  0)) fprintf(stderr, "COULDN'T REGISTRY CLIENT\n");
    if (! client_manager.register_client("Gabriel", (struct Vehicle) { .type = "tipo-crazy", .model = "modelo-crazy", .mileage = 1001 }, & client_buffer, 3)) fprintf(stderr, "COULDN'T REGISTRY CLIENT\n");

#	endif // CLIENT_GENERATION_TEST

#	define SO_GENERATION_TEST		false
#	if SO_GENERATION_TEST
    SO_Manager so_manager;
    
	struct ServiceOrder SO;
    if (! so_manager.new_order("quebrou ne fi", { (2 << VEHICLE_ID_BITS) + 0 }, & SO))
        fprintf(stderr, "FALHOU TUDO\n");
    
    struct PartsBudget parts = {
        2, { PIECE_TIRE, PIECE_WHEEL }
    };

    if (! so_manager.budget_order(6, parts, &SO)) {
        fprintf(stderr, "FALHOU EM ORCAR A ORDEM Z\n");
    }

    // if (! so_manager.operate_order(2, &SO)) { fprintf(stderr, "FALHOU EM ABRIR PRA MANUTENCAO\n"); }

    //if (! so_manager.close_order(4, &SO)) { fprintf(stderr, "FALHOU EM FECHAR A MANUTENCAO\n"); }

    std::list<struct ServiceOrder> LA_ELE = so_manager.so_category(SO_BUDGET);

    for (const auto & so_it : LA_ELE) {
        std::cout << so_it;
    }


#	endif // SO_GENERATION_TEST
    
    return 0;
}
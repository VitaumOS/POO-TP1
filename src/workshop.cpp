/*  <src/workshop.cpp>

    The entry point for the workshop system. */


#include <workshop.h>
#include <string>


/*  The main screen object representing the end application. 
    The method <interact> of this screen will then be responsible for everything. */
class MainScreen : virtual public MenuScreen
{
public:
    MainScreen(void) { }
    ~MainScreen(void) { }

    int interact(void)
    {
        class SO_Manager so_manager;
        if (! so_manager.could_initialize())    return -1;

        class UsersDatabase users_db(&so_manager);
        if (! users_db.could_initialize())      return -2;
        
        class LoginScreen login_screen(50, 25, &so_manager, &users_db);

        bool main_loop = true;
        while (main_loop)
        {   
            if (login_screen.interact() > 0)
            {
                login_screen.user_interact();
            }

            clean_screen();
            print_n_char('\n', 3);
            std::cout << "\tDeseja encerrar o programa? ";
            print_n_char('\n', 3);
            printf("\t\t");

            if (input_verification())   main_loop = false;
        }
    
        return 0;
    }
};

/*  NO SPECS DECLARED */
int main(void) {
    /*  Language setup */
    setlocale(LC_ALL, "portuguese");
    
    MainScreen m_screen;
    return m_screen.interact();
}

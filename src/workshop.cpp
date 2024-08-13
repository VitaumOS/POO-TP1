/*  <src/workshop.cpp>

    The entry point for the workshop system. */


#include <workshop.h>
#include <string>


/*  The main screen object representing the end application. 
    The method <interact> of this screen will then be responsible for everything. */
class MainScreen : virtual public MenuScreen
{
private:
    void render_exit_program(void)
    {
        aec_reset();
        clean_screen();
        //print_n_char('=', width);
        aec_bg_rgb(150, 150, 150);
        fill_char(' ');
        to_beggining();
        print_n_char('\n', 3);
        //print_n_char('=', width);
        constexpr const char * message = "Deseja encerrar o programa?";
        constexpr size_t message_size = literal_string_length(message);

        aec_crs_right((width - message_size) >> 1);
        std::cout << message;
        print_n_char('\n', 3);
        aec_crs_right((width - 8) >> 1);
    }
    
    int render(void) const override {
        return 1;
    }

    int process(void) override {
        return 1;
    }

    void final_screen(void)
    {
        aec_reset();
        aec_clean();
        std::cout << "Sistema de oficina finalizado com sucesso.\n";
    }

public:
    MainScreen(int width, int height) : MenuScreen(width, height) { 
        
    }

    ~MainScreen(void) { 
        
    }

    int interact(void)
    {
        class SO_Manager so_manager;
        if (! so_manager.could_initialize())    return -1;

        class UsersDatabase users_db(&so_manager);
        if (! users_db.could_initialize())      return -2;
        
        class LoginScreen login_screen(width, height, &so_manager, &users_db);
        
        bool main_loop = true;
        while (main_loop)
        {   
            if (login_screen.interact() > 0)
            {
                login_screen.user_interact();
            }

            render_exit_program();

            if (input_verification())   main_loop = false;
        }

        MainScreen::final_screen();
        return 0;
    }

};

/*  NO SPECS DECLARED */
int main(void) {
    /*  Language setup */
    // setlocale(LC_ALL, "portuguese")
    setlocale(LC_ALL, "pt_BR.UTF-8");
    
    MainScreen m_screen(100, 30);
    return m_screen.interact();
}

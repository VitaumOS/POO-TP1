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


public:
    MainScreen(void) { 
        width = 100;
        height = 30;
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

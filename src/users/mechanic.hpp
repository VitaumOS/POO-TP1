

#ifndef _MECHANIC_HPP_
#define _MECHANIC_HPP_


#include "user.hpp"
#include "../databases/users-db.hpp"


class Mechanic : virtual public UserScreen {
private:

    int render(void)
    {
        clean_screen();

        /*	header */
        std::cout << "Mechanic Menu\n";
        std::cout << "-------------\n\n";

        /*	footer */
        std::cout << "\tO que deseja fazer a seguir? " << std::endl;
        std::cout << "\t0\t->\tSair\n";
        std::cout << "\t1\t->\tOrçar\n";
        std::cout << "\t2\t->\tManutençar\n";

        fflush(stdout);
        return 0;
    }

    int process(void)
    {
        int x;
        std::cin >> x;
        switch (x)
        {
        case 0:
            UserScreen::main_loop = false;
            break;

        case 1:
            Mechanic::budget();
            break;

        case 2:
            Mechanic::maintenance();
            break;

        case 3:


        default:
            break;
        }

        return 0;
    }

    void budget(void);
    void maintenance(void);

public:
    Mechanic(class SO_Manager * const so_manager, class UsersDatabase * const users_db, const struct MinimalUserData & user_data) : 
        UserScreen(so_manager, users_db, user_data) {
        if (users_db == nullptr)
            throw std::runtime_error("VTMNC");

    }

    virtual ~Mechanic(void) {
    
    }
};

#endif // _MECHANIC_HPP_

#ifndef _MECHANIC_HEADER_
#define _MECHANIC_HEADER_

#include "../databases/so-db.hpp"
#include "user.hpp"

class Mechanic: public User{
private:
    SO_Manager * so_manager;
    // Inserir atributos
public:
    Mechanic(id_t id, SO_Manager * SO_Manager) : User(id, so_manager) {
        this -> so_manager = so_manager;
    }
    ~Mechanic(void){};

    void interact(void);
    // Inserir metodos
};

#endif // _MECHANIC_HEADER_
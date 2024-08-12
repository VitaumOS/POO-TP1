

#ifndef _MECHANIC_HPP_
#define _MECHANIC_HPP_


#include "user.hpp"
#include "../databases/users-db.hpp"


class Mechanic : virtual public UserScreen {
private:

    void budget(void);
    void maintenance(void);

    int render(void) const override;
    int process(void) override;

public:
    Mechanic(class SO_Manager * const, class UsersDatabase * const, 
        const struct MinimalUserData &);
    virtual ~Mechanic(void);
};

#endif // _MECHANIC_HPP_

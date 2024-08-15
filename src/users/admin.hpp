/*

*/

#ifndef _USER_ADMIN_HEADER_
#define _USER_ADMIN_HEADER_


#include "../databases/so-db.hpp"
#include "../databases/users-db.hpp"
#include "../databases/databases.hpp"
#include "user.hpp"
#include "seller.hpp"



class Administrator : virtual public UserScreen {
private:
    friend class UsersEditor;

    void edit_users(void);

    int render(void) const override;
    int process(void) override;

public:
    Administrator(class SO_Manager * const, class UsersDatabase * const, const struct MinimalUserData &);
    virtual ~Administrator(void);
};



#endif // _USER_ADMIN_HEADER_

/*	<src/ui/seller-ui.hpp
	
	(...) */


#ifndef _SELLER_UI
#define _SELLER_UI


#include "ui.hpp"
#include "../databases/so-db.hpp"
#include <stdexcept>


class SO_MenuScreen : virtual public MenuScreen {
protected:
    so_id_t id = 0;
    class SO_Manager * so_manager = nullptr;
    struct ServiceOrder so_buffer;
    
    std::string output_buffer;
    

public:
    SO_MenuScreen(class SO_Manager * const so_manager);
    virtual ~SO_MenuScreen(void);

};

class SO_ApprovalMenu : virtual public SO_MenuScreen {
private:
    so_id_t id = 0;
    struct ServiceOrder so_buffer;

    std::string output_buffer;

    virtual int render(void);
    void interact_so(void);

public:
    SO_ApprovalMenu(class SO_Manager * const so_manager);
    virtual ~SO_ApprovalMenu(void);

    virtual int interact(void);
    inline so_id_t get_id(void) const { return id; };
};


class SO_ClosingMenu : virtual public SO_MenuScreen {
private:

    virtual int render(void);

public:
    SO_ClosingMenu(class SO_Manager * const so_manager);
    virtual ~SO_ClosingMenu(void);

    virtual int interact(void);
};





#endif //_SELLER_UI
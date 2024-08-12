/*	<src/ui/db-vizualizers.hpp> 
    
    (...) */

/*	Declares the (base) objects responsible for the vizualization and
	direct interactions with the databases. */


#ifndef _SO_VIZUALIZER_HEADER_
#define _SO_VIZUALIZER_HEADER_


#include <vector>
#include "ui.hpp"
#include "../databases/so-db.hpp"
#include "../databases/users-db.hpp"


/*  Color pallet:
22, 12, 40
239, 203, 104
225, 239, 230
174, 183, 179
0, 4, 17
*/


/*  An abstract class representing the UI with a database.
    Defines the menu structure for the purpose. 
    At the end it is a static navigator window over them. */
class DatabaseInterface : virtual public MenuScreen {
private:
    /*  */
    std::string cmd_input_buffer;   //
    std::string cmd_output_buffer;  //
    
    /*  common logical inner methods */
    int process(void);          // 
    int render(void) const; // 

protected:

    /* paging */
    size_t page_size            = 10LLU;    //
    size_t page_index           = 0LLU;     //
    size_t last_page_loaded     = 1LLU;     //
    size_t last_qtt_read        = 0LLU;     //
    virtual void load_page(void);           //
    void reload_page(void);

    /*  page vizualization inner parameters */
    size_t focus_index          = 0LLU;     //
    size_t page_index_max       = 0LLU;     //
    
    bool running_menu           = true;     //

    virtual void inspect(void);

    /*  abstract rendering methods */
    virtual void render_header(void) const; //
    virtual void render_page(void) const;   //
    virtual void render_footer(void) const; //

    /*  stylization */
    virtual void color_selected(void) const;

    virtual void event_n_key(void);
    virtual void event_f_key(void);

public:
    DatabaseInterface(void);
    virtual ~DatabaseInterface(void);
    
    int interact(void);
};


class SO_Vizualizer : virtual public DatabaseInterface {
protected:
    class SO_Manager * so_manager;
    std::list<struct ServiceOrder> SOs;

    char base_title_name[64] = "UNDEFINED";

private:
    std::vector<struct ServiceOrder> vpage;

    SERVICE_ORDER_STAGE category;

    void load_page(void) override;
    
    inline void represent_so(const struct ServiceOrder & so) const;

    /*  rendering methods */
    void render_header(void) const override;
    void render_page(void)  const override;
    virtual void render_footer(void) const override;

    so_id_t inspect_id;

    virtual void inspect(void) override;                                

protected:
    void print_so(const struct ServiceOrder & so) const;
    void get_service_order(struct ServiceOrder & so) const;

public:
    SO_Vizualizer(SO_Manager * const so_manager);
    virtual ~SO_Vizualizer(void);

    void set_category(SERVICE_ORDER_STAGE category);
};


class UsersEditor : virtual public DatabaseInterface {
private:
    class UsersDatabase * users_db = nullptr;
    struct UserData admin_data;

    struct UserData * page_buffer = nullptr;
    std::vector<struct UserData> vpage;
    
    bool update_userdata(const struct UserData &);
    bool fetch_username(const username_string_t) const;

    void load_page(void);

    inline void color_selected(void) const;
    void represent_userdata(const struct UserData &) const;
    
    void render_header(void) const override;
    void render_footer(void) const override;
    void render_page(void) const override;

    void event_n_key(void) override;
    void inspect(void) override;

    void new_user(void);

public:
    UsersEditor(class UsersDatabase * const, const struct UserData & admin_data);
    ~UsersEditor(void);

    friend class InspectUser;
};


#endif // _SO_VIZUALIZER_HEADER_

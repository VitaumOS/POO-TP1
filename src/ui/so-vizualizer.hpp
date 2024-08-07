/*	<src/ui/so-vizualizer.hpp>

	(...) */


#ifndef _SO_VIZUALIZER_HEADER_
#define _SO_VIZUALIZER_HEADER_


#include "ui.hpp"
#include "../databases/so-db.hpp"


/*  Color pallet:
22, 12, 40
239, 203, 104
225, 239, 230
174, 183, 179
0, 4, 17
*/


class SO_Vizualizer : virtual public MenuScreen {
private:
    class SO_Manager * so_manager;


    std::string cmd_input_buffer;
    std::string cmd_output_buffer;

    SERVICE_ORDER_STAGE category;

    size_t vpage_index = 0;
    size_t focus_index = 0;
    size_t vpage_item_qtt = 0;
    
    bool running_menu = true;

    inline void repr_el(const struct ServiceOrder & so) const;

    size_t print_lpage(std::list<struct ServiceOrder> & the_list, size_t lpage_index, size_t focus_index) const;          
    void render_header(void) const;           
    void render_footer(void)    const;                      
    
    void so_inspect(so_id_t id);                                
    void process_io(const size_t & vpage_index_max);        

public:
    SO_Vizualizer(SO_Manager * so_manager);
    virtual int interact(void);
    void set_category(SERVICE_ORDER_STAGE category);

};



#endif // _SO_VIZUALIZER_HEADER_

/*  <src/databases/so-db.hpp>

    Declares the service-orders (SOs) database. */

/*	Last update: 20/07/2024. */


#ifndef _SERVICEORDER_DB_HPP_INCLUDED_
#define _SERVICEORDER_DB_HPP_INCLUDED_


#include "clients-db.hpp"


typedef Id_t so_id_t;

/*	The string-size a SO's description have. */
constexpr uint64_t SO_DESCRIPTION_SIZE = 64ULL;

/*  The stream over which the service-orders will be located at. */
constexpr c_filepath SODB_filename = "data/service-orders.bin";


/*  The states a service-order can be at.

    A SO will always start as "OPEN".
    The lifetime of a SO can have the following sequences running tree:
    SO_OPEN -> SO_BUDGET -> SO_MAINTENANCE -> SO_CLOSED
      \                 \
       --> SO_CANCELED   \
                          ---> SO_CLOSED_BUDGET                         */
typedef enum _SOS {
    SO_OPEN,            // SO is open.
    SO_BUDGET,          // SO is (open and) budgeted.
    SO_MAINTENANCE,     // SO is (open, budget and) over maintenance.

    SO_CLOSED,          // Closed SO. SO closed from "maintenance" state.
    SO_CANCELED,        // Closed SO. SO closed from "open" state.
    SO_CLOSED_BUDGET,   // Closed SO. SO closed from "budget" state.
    SO_ALL,
} SERVICE_ORDER_STAGE;

typedef Id_t so_id_t;

/*  Represents a service-order item. */
struct ServiceOrder {
    // Identification.
    so_id_t id;                     // SO's sequential id.
    SERVICE_ORDER_STAGE stage : 3;  // The stage at which the SO is at.
    client_id_t client_id;          // The client's ID for the SO.

    char issue_description[SO_DESCRIPTION_SIZE];

    // Budget and pricing.
    struct PartsBudget budget;  // Contains the budget information for all the parts.
    currency_t labor_price;     // The labor price for the service.
    currency_t hardware_price;  // The total hardware price for the service.

    // SO's Meta-data.
    Date creation_date; // The date at which the SO was created.
    Date update_date;   // The date at which the SO was last updated.
};

typedef enum _SOH {
    SOH_SUCCESS
} SO_HANDLING;

/*  SOV ATTRIBUTE. 
    TODO: move. */
constexpr size_t vpage_size = 10;


class SO_Manager : virtual public Database <struct ServiceOrder> {
private:
    bool retrieve_stream_header(void);
    bool update_stream_header(void) const;
    bool reset_database(void);

    /*  Stream-header */
    Id_t first_active; // The oldest active element in the database.

    inline void fprint_element(FILE * _OutputStream, const struct ServiceOrder * _SO) const;

    currency_t sum_hardware_budget(const struct PartsBudget & budget) const {
        currency_t sum = 0;
        for (uint8_t i = 0; i < budget.n_pieces; ++ i)
            sum += hash_piece_price(budget.pieces[i]);
        return sum;
    }

    currency_t calculate_labor_price(const struct ServiceOrder & SO) const {
    #define DEFAULT_LABOR_PRICE     ((currency_t) 10000)
    #define LINEAR_COEFFICIENT      ((double) 0.1)

        return DEFAULT_LABOR_PRICE + ((currency_t) (LINEAR_COEFFICIENT * SO.hardware_price));
    }

public:
    ClientsManager client_manager;

    SO_Manager(void);
    ~SO_Manager(void);
    
    /*  Attempts creating a new service-order into the database. */
    bool new_order(const char issue[SO_DESCRIPTION_SIZE], const client_id_t & client_id,
        struct ServiceOrder * return_so);
    
    /*  Attempts budgeting an open service-order in the database. */
    bool budget_order(const so_id_t id, const struct PartsBudget & parts_budget, struct ServiceOrder * return_so);

    /*  Attempts opening a service-order in the database as over maintenance. */
    bool operate_order(const so_id_t id, struct ServiceOrder * return_so);

    /*  Attempts closing a service-oder in the database. */
    bool close_order(const so_id_t id, struct ServiceOrder * return_so);

    /*  Attempts advancing a service-oder to its next (natural) stage, in the database.
        It bases upon the the source SO to the updating. */
    bool advance_order(Id_t id, const struct ServiceOrder * src_so);

    /*  Attempts getting an existing service-order on the database.
        Returns success, failing in case of an invalid ID or, generally, IO issues. */
    bool get_order(Id_t id, struct ServiceOrder * return_so) const;
    
    /*  Lists all SOs in the database that fits a certain stage category. */
    std::list<struct ServiceOrder> so_category(SERVICE_ORDER_STAGE category);

    friend class SO_Vizualizer;
};

std::ostream & operator<<(std::ostream & stream, const struct ServiceOrder & so);


#endif // _SERVICEORDER_DB_HPP_INCLUDED_

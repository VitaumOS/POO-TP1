#ifndef _DATABASES_HEADER_
#define _DATABASES_HEADER_


#include "commons.h"
#include "vehicle.h"
#include <stdio.h.>
#include <stdio.h>


/*  Database
    ======== */

/*  A class representing a homogeneous database manager. */
class Database {
private:
    bool init_succeeded;
    bool create_stream(void);
    
protected:
    bool write_stream_header(void);
    bool read_stream_header(void);

    const char * filename;
    FILE * stream;

    struct {
        id_t next_id;   // The next sequential ID to be filled on the database.
        id_t item_qtt;  // How many items does is the database holding.
    } stream_header;

public:
    Database(const char * filename);
    ~Database(void);

    inline bool could_initialize(void) const { return init_succeeded; }

    void reset_database(void) {
        printf("resetting <%s>:(%p)\n", filename, stream);
        rewind(stream);
        stream_header.next_id = 0;
        fwrite(&stream_header, sizeof(stream_header), 1, stream);
    }
};


/*  Client
    ====== */

constexpr const char * CLIENTDB_FILENAME = "data/clients.bin";

// In the address space of 64 bits, "how many there are for identifying the client's (person) id?"
#define VEHICLE_ID_BITS     4u

typedef uint32_t small_id_t;

typedef union {
    id_t id;

    struct {
        id_t vehicle_id : VEHICLE_ID_BITS;
        id_t person_id  : (64u - VEHICLE_ID_BITS);
    };

} client_id_t;


typedef id_t so_id_t;


struct Client {
    client_id_t id;
    char name[NAME_SIZE];
    struct Vehicle vehicle;
    Date registry_date;
};

class ClientManager : Database {
    /*  stream_header.last_id from <Database> will hold in this context
        the last person's id. */

private:
    friend class SO_Manager;
    
    inline bool read_item(id_t index, struct Client * return_client) const {
        fseek(stream, sizeof(stream_header) + index * sizeof(struct Client), SEEK_SET);
        return fread(return_client, sizeof(struct Client), 1, stream) > 0;
    }

    inline bool write_item(id_t index, const struct Client * client) const {
        printf("[%s] index=%llu\n", __func__, index);
        fseek(stream, sizeof(stream_header) + index * sizeof(struct Client), SEEK_SET);
        return fwrite(client, sizeof(struct Client), 1, stream) > 0;
    }

public:
    ClientManager(void);
    ~ClientManager(void);

    inline uint64_t get_next_person_id(void) const { return stream_header.next_id; };

    void print_database(FILE * _PrintStream = stdout, size_t _From = 0, size_t _To = 0) const;

    int64_t fetch_person_id(id_t person_id, size_t _From = 0);


    bool register_new_person(const char name[NAME_SIZE], const struct Vehicle vehicle);
    bool register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, id_t person_id = ((id_t) - 1));
};





/*  Service Orders
    ============== */

/*  The stream over which the service-orders will be located at. */
constexpr const char * SO_FILENAME = "data/service-orders.bin";


typedef enum {
    SO_OPEN,        // If a SO was open.
    SO_BUDGET,      // If a SO is budgeted.
    SO_MAINTENANCE, // If a SO is over maintenance.
    SO_CLOSED,      // If a SO is closed.
} SERVICE_ORDER_STAGES;

struct PartsBudget {
    uint8_t n_pieces;               
    PIECE_ID pieces[MAX_PIECES];    
    currency_t prices[MAX_PIECES];       
};


typedef id_t so_id_t;

/*  Represents a service-order item. */
struct ServiceOrder {
    // Identification.
    so_id_t id;                     // SO's sequential id.
    SERVICE_ORDER_STAGES stage : 2; // The stage at which the SO is at.
    client_id_t client_id;          // The client's ID for the SO.

    char issue_description[DESCRIPTION_SIZE];
    
    // Budget and pricing.
    struct PartsBudget budget;  // Contains the budget information for all the parts.
    currency_t labor_price;     // The labor price for the service.
    currency_t hardware_price;  // The total hardware price for the service.

    // SO's Meta-data.
    Date creation_date; // The date at which the SO was created.
    Date update_date;   // The date at which the SO was last updated.
};


class SO_Manager : Database {
private:
    /*  File stream */

    ClientManager client_manager;

    currency_t calculate_labor_price(const struct PartsBudget & budget) const {
        currency_t S = 0;

        for (uint8_t i = 0; i < budget.n_pieces; i ++)
        {
            S += budget.prices[i];
        }

        return S >> 2;
    }

    inline bool read_item(id_t index, struct ServiceOrder * return_so) const {
        fseek(stream, sizeof(stream_header) + index * sizeof(struct ServiceOrder), SEEK_SET);
        return fread(return_so, sizeof(struct ServiceOrder), 1, stream) > 0;
    }

    inline bool write_item(id_t index, const struct ServiceOrder * so) {
        fseek(stream, sizeof(stream_header) + index * sizeof(struct ServiceOrder), SEEK_SET);
        return fwrite(so, sizeof(struct ServiceOrder), 1, stream) > 0;
    }

    
public:
    SO_Manager(void);
    ~SO_Manager(void);

    bool get_new_order(struct ServiceOrder * return_so);
    bool set_new_order(const struct ServiceOrder * so);

    bool update_order(id_t id, const struct ServiceOrder * so);
    bool close_order(id_t id);
};

#endif
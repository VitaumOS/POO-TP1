#ifndef _DATABASES_HEADER_
#define _DATABASES_HEADER_


#include "commons.h"
#include "vehicle.h"
#include <stdio.h.>
#include <stdio.h>


/*  A class representing a database manager. */
class Database {
private:
    bool init_succeeded;
    bool create_stream(void);
    
protected:
    bool write_stream_header(void) { };
    bool read_stream_header(void) { };

    const char * filename;
    FILE * stream;

    struct {
        id_t last_id;
    } stream_header;

public:
    Database(const char * filename);
    ~Database(void);

    inline bool could_initialize(void) const { return init_succeeded; }

    void reset_database(void) {
        printf("resetting <%s>:(%p)\n", filename, stream);
        rewind(stream);
        stream_header.last_id = 9;
        fwrite(&stream_header, sizeof(stream_header), 1, stream);
    }
};


/*  Client
    ====== */

constexpr const char * CLIENTDB_FILENAME = "data/clients.bin";

// In the address space of 64 bits, "how many there are for identifying the client's (person) id?"
#define VEHICLE_ID_BITS     4

typedef struct {
    uint64_t person_id  : (64 - VEHICLE_ID_BITS);
    uint64_t vehicle_id : VEHICLE_ID_BITS;
} client_id_t;


typedef uint64_t so_id_t;


struct Client {
    client_id_t id;
    char name[NAME_SIZE];
    struct Vehicle vehicle;
    Date registry_date;
};

class ClientManager : Database {
private:
    friend class SO_Manager;


public:
    ClientManager(void);
    ~ClientManager(void);

    inline bool read_item(id_t index, struct Client * return_client) const {
        fseek(stream, sizeof(stream_header) + index * sizeof(struct Client), SEEK_SET);
        return fread(return_client, sizeof(struct Client), 1, stream) > 0;
    }

    inline bool write_item(id_t index, const struct Client * client) const {
        fseek(stream, sizeof(stream_header) + index * sizeof(struct Client), SEEK_SET);
        return fwrite(client, sizeof(struct Client), 1, stream) > 0;
    }

    // stream header -> last_person_id
    bool does_client_exists(const client_id_t * id) const { return false; };

};


/*  Service Order
    ============= */

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


/*  A service-order item. */
struct ServiceOrder {
    id_t id;                        // SO's sequential id.
    SERVICE_ORDER_STAGES stage : 2; // The stage at which the SO is at.

    client_id_t client_id;          // The client's ID for the SO.

    char issue_description[DESCRIPTION_SIZE];
    
    struct PartsBudget budget;
    currency_t labor_price;
    currency_t hardware_price;

    Date creation_date;
    Date update_date;   
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
    bool get_order(id_t id, struct ServiceOrder * return_so);
    bool update_order(id_t id, const struct ServiceOrder * so);
    bool close_order(id_t id);
};

#endif
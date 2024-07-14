/*  <src/headers/databases.h>
    
    Defines the databases for the main application. */

/*  Assignment part for @HexagonalUniverse.
    
    Last update: 14/07/2024. */


#ifndef _DATABASES_HEADER_
#define _DATABASES_HEADER_


#include "commons.h"
#include "vehicle.h"
#include <stdio.h.> // FILE


/*  Database Template
    ================= */

/*  A template class representing a homogeneous database manager. 
    The homogeneous data are its elements, and they're specified by their index. 
    
    The database is associated with a headed file-stream tracking the indexes. */
template <typename ElementType> class Database {
private:
    bool init_succeeded;    // Tracks if the database's initialization was successful.
    bool init_stream(void); // Initializes (from empty) the file-stream - overwriting its occurrence.
    void reset_database(void);  // Resets the file-stream.
    
    // The database's stream filename.
    const char * filename;

protected:
    // The database's file-stream.
    FILE * stream;

    struct {    // Keeps overall information of the stream.
        id_t next_id;   // The next sequential ID to be filled on the database.
        id_t item_qtt;  // How many items does is the database holding.
    } stream_header;

    /*  Reads a single element in the homogeneous database data-space.
        The element is specified by its index; the element is written by reference.
        Returns success; fails in case of IO sequencing. In case of fail, the stream
        pointer state is undeterminated. */
    inline bool read_element(id_t index, ElementType * _DstItem) const {
        return (fseek(stream, sizeof(stream_header) + index * sizeof(ElementType), SEEK_SET) == 0) && 
            (fread(_DstItem, sizeof(ElementType), 1, stream) > 0);
    }

    /*  Writes a single element in the homogeneous database data-space.
        The element is specified by its index; the element is written by reference.
        Returns success; fails in case of IO sequencing. In case of fail, the stream
        pointer state is undeterminated. */
    inline bool write_element(id_t index, const ElementType * _SrcItem) const {
        return (fseek(stream, sizeof(stream_header) + index * sizeof(ElementType), SEEK_SET) == 0) &&
            (fwrite(_SrcItem, sizeof(ElementType), 1, stream) > 0);
    }

    bool read_stream_header(void);          // Reads the internal stream-header structure.
    bool write_stream_header(void) const;   // Writes the internal stream-header structure.

    /*  Database Representation
        ----------------------- */
    
    virtual inline void fprint_element(FILE * _OutputStream, const ElementType * _Element) { 
        fprintf(_OutputStream, "NONE%p", _Element); 
    };

    /*  Represents the database onto an output stream, sectioned inclusively, from a start to an end
        - (_From) and (_To) respectively.
        Case for what (_To = 0) actually will means that the entire database is target of printing.
        Prints it on its entirety, and on stdout, by default. */
    bool print_database(FILE * _OutputStream = stdout, size_t _From = 0, size_t _To = 0);

public:
    Database(const char * filename);
    ~Database(void);

    /*  Returns if the database initialization was successfully done. 
        In case of fail, the object shall not be used. */
    inline bool could_initialize(void) const { return init_succeeded; }
};



/*  Clients Database
    ================ */

constexpr const char * CLIENTDB_FILENAME = "data/clients.bin";

// In the address space of 64 bits:
// "how many bits there are reserved for identifying the client's (vehicle) id?"
#define VEHICLE_ID_BITS     4u


/*  A type that represents the ID of the client.
    Structurally it is "id_t" number, onto which the bit-space is divided
    into the person's and the vehicle's id. */
typedef union {
    id_t id;    // The overall numerical ID.
    
    struct {
        id_t vehicle_id : VEHICLE_ID_BITS;
        id_t person_id  : (64u - VEHICLE_ID_BITS);
    };
} client_id_t;


/*  Represents a Client.
    
    Here, "client" doesn't refer only to the person associated with it,
    but rather to it AND its vehicle. */
struct Client {
    client_id_t id; // Client's ID.

    // Client-associated person's name.
    // TODO: struct Person to contain it. (Scalability)
    char name[NAME_SIZE];

    // Client-associated vehicle's information.
    struct Vehicle vehicle;

    // The date at which the client was registered in the database.
    Date registry_date;
};


/*  A homogeneous database for the clients. */
class ClientManager : Database <struct Client> {
    /*  stream_header.next_id from <Database> will hold in this context
        the last person's id. */

private:
    friend class SO_Manager;
    inline void fprint_element(FILE * _OutputStream, const struct Client *);

public:
    ClientManager(void);
    ~ClientManager(void);

    // Returns what should be the next person's id on the client's database sequence.
    inline uint64_t get_next_person_id(void) const { return stream_header.next_id; };

    /*	Returns the index of first occurrence of the person's id on the database, from a given
    starting index (_From).
        The return is (-1) in case of not founding; and (-2) in case of IO errors. */
    int64_t fetch_person_id(id_t person_id, size_t _From = 0);

    bool register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, id_t person_id = ((id_t) - 1));
};



/*  Service Orders Database
    ======================= */

typedef id_t so_id_t;

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

class SO_Manager : Database <ServiceOrder> {
private:
    /*  File stream */

    inline void fprint_element(FILE * _OutputStream, const struct ServiceOrder * _SO);

    ClientManager client_manager;

    currency_t calculate_labor_price(const struct PartsBudget & budget) const {
        currency_t S = 0;

        for (uint8_t i = 0; i < budget.n_pieces; i ++)
        {
            S += budget.prices[i];
        }

        return S >> 2;
    }

    inline bool read_element(id_t index, struct ServiceOrder * return_so) const {
        fseek(stream, sizeof(stream_header) + index * sizeof(struct ServiceOrder), SEEK_SET);
        return fread(return_so, sizeof(struct ServiceOrder), 1, stream) > 0;
    }

    inline bool write_element(id_t index, const struct ServiceOrder * so) {
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
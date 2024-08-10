/*  <src/databases/clients-db.hpp>

    Declares the client's database. */


#ifndef _CLIENTS_DB_HPP_INCLUDED_
#define _CLIENTS_DB_HPP_INCLUDED_


#include "../headers/vehicle.h"
#include "../headers/date.hpp"
#include "databases.hpp"
#include <string.h>


constexpr c_filepath people_DB_filename = "data/persons.bin"; // TODO
constexpr c_filepath clients_DB_filename = "data/clients.bin";

// In the address space of 64 bits:
// "how many bits there are reserved for identifying the client's (vehicle) id?"
constexpr uint64_t VEHICLE_ID_BITS = 4ULL;

constexpr uint64_t VEHICLES_PER_PERSON = (1ULL << VEHICLE_ID_BITS);


/*  A type that represents the ID of the client.
    Structurally it is "Id_t" number, onto which the bit-space is divided
    into the person's and the vehicle's id. */
typedef union {
    Id_t id;    // The overall numerical ID.

    struct {
        Id_t vehicle_id : VEHICLE_ID_BITS;
        Id_t person_id : (64ULL - VEHICLE_ID_BITS);
    };
} client_id_t;

struct PersonData {
    char name[NAME_SIZE];
};


inline bool operator==(const struct PersonData & x, const struct PersonData & y) { return ! strcmp(x.name, y.name); }
inline bool operator==(const client_id_t & x, const client_id_t & y) { return x.id == y.id;  }

/*  Represents a client's data on its database. */
struct ClientData {
    client_id_t id; // ClientData's ID.

    struct PersonData person;   // The client's associated person's data.
    
    // uint8_t vehicle_qtt = (unsigned char) 0;
    // struct VehicleData vehicle[VEHICLES_PER_PERSON]; 
    struct VehicleData vehicle; // The client's associated vehicle's data.

    /*  metadata */

    // The date at which the client was registered in the database.
    Date registry_date;

    /*  navigation data */
    int64_t previous    = - 1LL;
    int64_t next        = - 1LL;
};

/*  A homogeneous database for the clients. */
class ClientsManager : virtual public Database <struct ClientData> {
private: 
    friend class SO_Manager;
    friend class Seller;
       
    /*  Stream-header */
    Id_t next_id = 0;   // The next sequential person ID to be filled on the database.

    bool reset_database(void);
    bool retrieve_stream_header(void) override;
    bool update_stream_header(void) const;

    inline void fprint_element(FILE * _OutputStream, const struct ClientData *) const;

    int64_t fetch_person(const struct PersonData &, struct ClientData &, size_t _From = 0) const;
    int64_t fetch_client_id(const client_id_t &, struct ClientData &, size_t _From = 0) const;
    
public:
    ClientsManager(void);
    ~ClientsManager(void);
    
    bool register_client(const struct PersonData & person, const struct VehicleData & vehicle, struct ClientData & return_client);

    // Returns what should be the next person's id on the client's database sequence.
    inline uint64_t get_next_person_id(void) const { return next_id; };

    bool get_client(const client_id_t & c_id, struct ClientData &);
    bool get_client(char person_name[NAME_SIZE], struct ClientData &);

    std::list<struct ClientData> get_person_clients(const PersonData &);
    std::list<struct ClientData> get_person_clients(const char name[NAME_SIZE]);
};


std::ostream & operator<<(std::ostream & stream, const client_id_t & ID);


#endif // _CLIENTS_DB_HPP_INCLUDED_

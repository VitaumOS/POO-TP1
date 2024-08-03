/*  <src/databases/clients-db.hpp>

    Declares the client's database. */

/*	Last update: 20/07/2024. */


#ifndef _CLIENTS_DB_HPP_INCLUDED_
#define _CLIENTS_DB_HPP_INCLUDED_


#include "../headers/vehicle.h"
#include "../headers/date.hpp"
#include "databases.hpp"


constexpr c_filepath people_DB_filename = "data/persons.bin"; // TODO
constexpr c_filepath clients_DB_filename = "data/clients.bin";

// In the address space of 64 bits:
// "how many bits there are reserved for identifying the client's (vehicle) id?"
constexpr uint64_t VEHICLE_ID_BITS = 4ULL;

constexpr uint64_t VEHICLES_PER_PERSON = (1ULL << VEHICLE_ID_BITS);


/*  A type that represents the ID of the client.
    Structurally it is "id_t" number, onto which the bit-space is divided
    into the person's and the vehicle's id. */
typedef union {
    id_t id;    // The overall numerical ID.

    struct {
        id_t vehicle_id : VEHICLE_ID_BITS;
        id_t person_id : (64ULL - VEHICLE_ID_BITS);
    };
} client_id_t;

/*  Represents a Client.

    Here, "client" doesn't refer only to the person associated with it,
    but rather to it AND his vehicle. */
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
class ClientsManager : virtual public Database <struct Client> {
private: 
    friend class SO_Manager;
       
    /*  Stream-header */
    id_t next_id;   // The next sequential person ID to be filled on the database.

    bool reset_database(void);
    bool retrieve_stream_header(void);
    bool update_stream_header(void) const;

    inline void fprint_element(FILE * _OutputStream, const struct Client *) const;

public:
    ClientsManager(void);
    ~ClientsManager(void);
    
    bool register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, struct Client * const return_client, id_t person_id = ((id_t) - 1));

    // Returns what should be the next person's id on the client's database sequence.
    inline uint64_t get_next_person_id(void) const { return next_id; };

    bool get_client(const client_id_t & c_id, struct Client * const return_client);

    /*	Returns the index of first occurrence of the person's id on the database, from a given
    starting index (_From).
        The return is (-1) in case of not founding; and (-2) in case of IO errors. */
    int64_t fetch_person_id(id_t person_id, size_t _From = 0) const;
    int64_t fetch_client_id(const client_id_t & client_id_t, size_t _From = 0) const;
    
    int64_t fetch_person_name(const char person_name[NAME_SIZE], size_t _From = 0) const;
    int64_t fetch_person_name(const char person_name[NAME_SIZE], struct Client *, size_t _From = 0) const;
};


std::ostream & operator<<(std::ostream & stream, const client_id_t & ID);


#endif // _CLIENTS_DB_HPP_INCLUDED_

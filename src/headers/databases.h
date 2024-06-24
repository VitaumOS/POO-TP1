#ifndef _DATABASES_HEADER_
#define _DATABASES_HEADER_

#include "commons.h"
#include "vehicle.h"
#include <stdio.h.>


typedef enum {
    BUDGET, MAINTENANCE,
} SERVICE_ORDER_REASON;


struct PartsBudget {
    uint8_t n_pieces;           // 1 byte
    PIECE_ID pieces[MAX_PIECES];   // 8 * 4 = 32 bytes
    float prices[MAX_PIECES];   // 8 * 4 = 32 bytes
};  // 68 bytes.

struct ServiceOrder {

    id_t id;

    SERVICE_ORDER_REASON reason_order;
    bool approved_by_client;
    bool approved_by_mechanic;
    bool open;

    id_t client_id;
    int vehicle_id;

    char issue_description[DESCRIPTIONSIZE];

    struct PartsBudget budget;
    float labor_price, total_price;
};


#define SO_FILENAME     "data/service-orders.bin"

class SO_Manager {
private:
    /*  The file stream
        --------------- */

    FILE * stream;

    struct {
        id_t last_id;
    } stream_header;

    float calculate_labor_price(struct PartsBudget budget) const {
        return -1.1;
    }

    inline bool read_order(id_t index, struct ServiceOrder * return_so) const {
        fseek(stream, index * sizeof(struct ServiceOrder), SEEK_SET);
        return fread(return_so, sizeof(struct ServiceOrder), 1, stream) > 0;
    }

    inline bool write_order(id_t index, const struct ServiceOrder * so) {
        fseek(stream, index * sizeof(struct ServiceOrder), SEEK_SET);
        return fwrite(return_so, sizeof(struct ServiceOrder), 1, stream) > 0;
    }

    size_t inline index_so(id_t index) { 
        return sizeof(stream_header) + sizeof(struct ServiceOrder) * index 
    }

public:
    bool init_succeeded;

    SO_Manager(void);
    ~SO_Manager(void);

    bool get_new_order(struct ServiceOrder * return_so);
    bool get_order(id_t id, struct ServiceOrder * return_so);

    bool update_order(id_t id, const struct ServiceOrder * so);

    bool close_order(id_t id);
};

#endif
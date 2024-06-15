/*  <src/header/seller.h>

*/



#ifndef _SELLER_HEADER_
#define _SELLER_HEADER_


#include "worker.h"
#include "client.h"
#include "vehicle.h"


class Seller: public Worker {
private:
    void RegisterClient(Client client, Vehicle vehicle);
    void GenerateWorkOrder(void);
    void WorkOrder_Aprove(void);
    void WorkOrder_Close(void);
    
public:
    Seller(void);
    ~Seller(void);
};


#endif // _SELLER_HEADER_

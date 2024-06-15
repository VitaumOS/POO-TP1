
#include <iostream>
#include "headers/databases.h"

#define d1_filename     "data/d1.bin"
#define d2_filename     "data/d2.bin"
#define d3_filename     "data/d3.bin"

FILE    * d1 = nullptr, 
        * d2 = nullptr, 
        * d3 = nullptr;


static void __InitializeStreams(void) {
    d1 = fopen(d1_filename, "w+b");
    if (d1 == NULL) {
        printf("failed initiliazing d1...\n");
    }

    d2 = fopen(d1_filename, "w+b");
    if (d2 == NULL) {
        printf("failed initiliazing d2...\n");
    }
    
    d3 = fopen(d1_filename, "w+b");
    if (d3 == NULL) {
        printf("failed initiliazing d3...\n");
    }
}

static void __TerminateStreams(void) { fclose(d1); fclose(d2); fclose(d3 ); };


static void __SellerInterface(void) {
      

}


/*  primitive... */
int main(void) {
    
    //Login
    //----------------------
    
    __InitializeStreams();

    //Seller case
    __SellerInterface();
    
    //Client register and check if it exists
    //Generate an order and add in global archive
    //Check if client agree with pending orders

    //----------------------
    
    //Mechanic case


    //----------------------

    //Admin case

    __TerminateStreams();
    //----------------------
    return 0;
}

/*
    //Global archive -----------------------
    FILE* global = fopen("global.bin", "wb+");

    //Insert a new order fwrite(& order, sizeof(SERVICE_ORDER_REASON), 1, global);

    fclose(global);
    //--------------------------------------
*/

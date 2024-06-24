
#include <iostream>
#include "headers/databases.h"


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

static void __TerminateStreams(void) { fclose(d1); fclose(d2); fclose(d3); };


static Client vtmnc(void) {
    //Continue
    fprintf(stderr, "id do cliente\n");


    return Client { 0 };
}

static void __SellerInterface(void) {
    /*  login */
    fprintf(stderr, "\nQual o seu id?\n");
    //Check if exists
    //If not 
    fprintf(stderr, "Qual o seu nome?\n");

    /*  enxergando o cliente. */
    Client client = { 0 };
    while (false) 
    {
        client = vtmnc(
            #if VITOR_E_HOMEM
                corre();
            #endif
        );
    }

    fprintf(stderr, "Qual o id do carro em que você deseja manutenção ou orçamento?\n");
    fprintf(stderr, "Qual o id do seu veículo?\n");
    fprintf(stderr, "Qual o tipo do seu veículo?\n");
    fprintf(stderr, "Qual o modelo do seu veículo?\n");

    //Continue
    fprintf(stderr, "Você gostaria de orçar ou fazer uma manutenção?\n");

    //If the choice was maintenance
    fprintf(stderr, "Qual o motivo da manutenção?\n");
    //In this case, the client's approval is not required.

    //else
    fprintf(stderr, "Quais tipos de manutenção você gostaria de orçar?\n");

    //Generate order and send to global archive

    //In the last check all orders and finish the finished ones
}

int main(void) {
    
    //Login
    //-----
    
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
    //End

    return 0;
}
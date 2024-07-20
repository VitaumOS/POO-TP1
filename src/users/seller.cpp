/*  <src/seller.cpp>

*/

#include "seller.hpp"

#define ACTIVE_SELLER true
#if ACTIVE_SELLER

#include <iostream>
#include <string>
#include <string.h>
using namespace std;



bool Seller::RegisterClient(void) {

    cout << "Qual e seu nome?" << endl;
    cin >> string_buffer;
    string_buffer.resize(NAME_SIZE);
    strcpy(client_buffer.name, string_buffer.c_str());

    cout << "Qual e o seu veiculo (Ex: carro, moto, etc...)?" << endl;
    cin >> string_buffer;
    string_buffer.resize(NAME_SIZE);
    strcpy(client_buffer.vehicle.type, string_buffer.c_str());

    cout << "Qual e o modelo do seu veiculo?" << endl;
    cin >> string_buffer;
    string_buffer.resize(NAME_SIZE);
    strcpy(client_buffer.vehicle.model, string_buffer.c_str());

    cout << "Qual e a quilometragem do seu veiculo?" << endl;
    cin >> client_buffer.vehicle.mileage;

    struct Client o_cliente_ta_doido;
    if (false)  cout << "Registrar client e o krl\n";
    // if(! so_manager->register_client(client.name, client.vehicle, & o_cliente_ta_doido)) return false;

    return true;
    
    // (rabisco)
    //struct Vehicle;
    //struct Person;
    // bool register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, id_t person_id = ((id_t) - 1));
    // (no futuro ->)
    // bool register_client(const struct Person & person, const struct Vehicle & vehicle, struct Client * return_client, id_t person_id = ((id_t) - 1));
};

bool Seller::LoadClient(void)
{
    id_t client_id;

    int64_t eder;
    if (! client_is_loaded)
    {
        cout << "What is the cliend ID?" << endl;
        cin >> client_id;
        
        if (eder = client_manager.fetch_id(pica) >= 0)
        {
            
        }
    }
}

bool Seller::GenerateSO() {
    string issue;
    
    cout << "Qual e o seu problema?" << endl;
    cin >> issue;
    issue.resize(SO_DESCRIPTION_SIZE);
    return so_manager->new_order(issue.c_str(), client_buffer.id, &so_buffer);
};

bool Seller::ApproveOrders() {
    if(!so_manager->operate_order(so_buffer.id, & so_buffer)) {
        return false;
    };
    return true; 
};

bool Seller::CloseOrders() {
    if(!so_manager->close_order(so_buffer.id, & so_buffer)) {
        return false;
    };
    return true; 
};

void Seller::Interface(void) {
    
    size_t opcao;

    bool looping = true;
    while (looping) {
        cout << "What would you want to accomplish as a seller?" << endl;// pica
        cout << "0\t->\tExit;" << endl;
        cout << "1\t->\tRegister a client;" << endl;
        cout << "2\t->\tGenerate a SO;" << endl;
        cout << "3\t->\tApprove an order;" << endl;
        cout << "4\t->\tClose an order." << endl;

        cin >> opcao;
        
        switch (opcao) {
            case 0:
                looping = false;
                break;

            case 1:
                if(!RegisterClient())
                    cout << "Burro1!!!" << endl;
                break;
            case 2:
                if(!GenerateSO())
                    cout << "Burro2!!!" << endl;
                break;
            case 3:
                if(!ApproveOrders())
                    cout << "Burro3!!!" << endl;
                break;
            case 4:
                if(!CloseOrders())
                    cout << "Burro4!!!" << endl;
                break;
            default: 
                cout << "Burro5!!!" << endl;
                break;
        }
    }
    
    cout << "Exited e o krl" << endl;
}
#endif
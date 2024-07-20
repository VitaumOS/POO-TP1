
#include <seller.h>

#if ACTIVE_SELLER
#include <iostream>
using namespace;

bool RegisterClient() {
    cout << "Qual é seu nome?" << endl;
    cin >> client.name;
    cout << "Qual é o seu veículo (Ex: carro, moto, etc...)?" << endl;
    cin >> client.vehicle.type;
    cout << "Qual é o modelo do seu veículo?" << endl;
    cin >> client.vehicle.model;
    cout << "Qual é a quilometragem do seu veículo?" << endl;
    cin >> client.vehicle.mileage;

    struct Client o_cliente_ta_doido;
    if(!register_client(client.name, client.vehicle, & o_cliente_ta_doido)) {
        return false;
    };

    return true;

    // (rabisco)
    //struct Vehicle;
    //struct Person;
    // bool register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, id_t person_id = ((id_t) - 1));
    // (no futuro ->)
    // bool register_client(const struct Person & person, const struct Vehicle & vehicle, struct Client * return_client, id_t person_id = ((id_t) - 1));
};

bool GenerateSO() {
    string x;
    cout << "Qual é o seu problema?" << endl;
    cin >> x;
    x.set_size(SO_DESCRIPTION_SIZE);
    if(!so_buffer.new_order(x.cstr(), client_buffer.client_id_t), & so_buffer) {
        return false;
    };
    
    /*struct PartsBudget parts_budget_buffer;
    cout << "Qual é o número de peças que você irá trocar?" << endl;
    cin >> parts_budget_buffer.n_pieces;

    for(int i = 0; i > parts_budget_buffer.n_pieces; i++ ) {
        cout << "Qual é a peça que você irá trocar?" << endl;
        cin >> parts_budget_buffer.pieces[i]
    }   

    if(!so_buffer.budget_order(so_buffer.id, & parts_budget_buffer, & so_buffer)) {
        return false;
    };*/

    return true; 
};

bool ApproveOrders() {
    if(!operate_order(so_buffer.id, & so_buffer)) {
        return false;
    };
    return true; 
};

bool CloseOrders() {
    if(!close_order(so_buffer.id, & so_buffer)) {
        return false;
    };
    return true; 
};


void Interface() {
    size_t opcao;
    cout << "O que você deseja fazer?" << endl;
    cin >> opcao;
    cout << "1 -> Register client;" << endl;
    cout << "2 -> Generate SO;" << endl;
    cout << "3 -> Approve Orders;" << endl;
    cout << "4 -> Close orders." << endl;

    switch(opcao) {
        case 1:
            if(!RegisterClient())
                cout << "Burro!!!" << endl;
            break;
        case 2:
            if(!GenerateSO())
                cout << "Burro!!!" << endl;
            break;
        case 3:
            if(!ApproveOrders())
                cout << "Burro!!!" << endl;
            break;
        case 4:
            if(!CloseOrders())
                cout << "Burro!!!" << endl;
            break;
        default: 
            cout << "Burro!!!" << endl;
            break;
    }

#endif
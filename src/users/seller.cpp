/*  <src/seller.cpp>

*/

#include "seller.hpp"
#include "../ui/ui.hpp"
#include "../ui/seller-ui.hpp"


#define ACTIVE_SELLER true
#if ACTIVE_SELLER


#include <iostream>
#include <string>
#include <string.h>


using namespace std;



Seller::Seller(id_t id, SO_Manager * so_manager) : User(id, so_manager)
{
    client_is_loaded = false;
    so_is_loaded = 0;

    client_buffer = { 0 };
    so_buffer = { 0 };
}

Seller::~Seller(void)
{

}

enum REGISTERCLIENT Seller::register_client_menu(void) {

    cout << "Digite o nome do cliente:\t\t\t\t";
    
    cin >> string_buffer;
    string_buffer.resize(NAME_SIZE);
    strcpy(client_buffer.name, string_buffer.c_str());

    //if (so_manager->fetch_person_name(client_buffer.name) >= 0) 
    //        return REGISTERCLIENT_ALREADY_EXIST;

    cout << "Qual é o tipo do veículo (Ex: carro, moto, etc...)?\t";
    
    cin >> string_buffer;
    strcpy(client_buffer.vehicle.type, string_buffer.c_str());
    string_buffer.resize(NAME_SIZE);

    cout << "Qual é o modelo do veículo?\t\t\t\t";
    
    cin >> string_buffer;
    string_buffer.resize(NAME_SIZE);
    strcpy(client_buffer.vehicle.model, string_buffer.c_str());

    cout << "Qual é a quilometragem do seu veículo?\t\t\t";
    
    do {
        cin.clear();
        cin.ignore(INT64_T_MAX, '\n');
        client_buffer.vehicle.mileage = input_numeral();
        
    } while ((client_buffer.vehicle.mileage < 0) && (cout << "Entrada inválida para kilometragem. Redigite-a:\t\t"));

    // TODO: deixar meió
    printf("Pessoa\t| Nome: %-64s;\nVeículo\t| Modelo: %s, tipo: %s, kilometragem: %d\n",
        client_buffer.name,
        client_buffer.vehicle.model, client_buffer.vehicle.type, (int) client_buffer.vehicle.mileage);
    
    cout << "Deseja continuar com o registro? ";
    if (! input_verification())
        return REGISTERCLIENT_CANCEL;

    client_is_loaded = false;

    if (! so_manager->client_manager.register_client(client_buffer.name, client_buffer.vehicle, &client_buffer))
        return REGISTERCLIENT_FAIL;

    client_is_loaded = true;
    return REGISTERCLIENT_SUCCESS;
}

bool Seller::GenerateSO(void) {
    if ((! client_is_loaded) && (! Seller::LoadClient())) {
        cout << "Não foi possível carregar o cliente.\n";
        return false;
    }
    
    cout << "Qual é o seu problema?\t";
    
    string issue;
    cin >> issue;
    issue.resize(SO_DESCRIPTION_SIZE);

    cout << "Deseja abrir uma SO para <" << string(client_buffer.name) << ">? ";

    if (! input_verification())
    {
        cerr << "cancl\n";
        return false;
    }

    return so_manager->new_order(issue.c_str(), client_buffer.id, &so_buffer);
}

bool Seller::LoadClient(void) {
    string name_buffer;
    int64_t index;

    cout << "Nenhum cliente está atualmente carregado no sistema." << endl;

    while (true) {
        cout << "Digite o nome do cliente:\t";
        
        cin >> name_buffer;
        name_buffer.resize(NAME_SIZE);

        if ((index = so_manager->client_manager.fetch_person_name(name_buffer.c_str(), &client_buffer)) >= 0) {
            client_is_loaded = true;
            break;
        }
        else if (index == -1) 
        {   
            cout << "Não foi possível encontrar um cliente com o nome <" << name_buffer << ">." << endl;
            cout << "Deseja prosseguir? ";

            /*  Veryfing whether the user wants to keep the load-client cycle. */
            if (! input_verification())
                return false;
        } 
        else
        {
            cerr << "VTMNC COMPUTADOR" << endl;
        }
    }
    return true;
}

bool Seller::approve_menu(void) {
    SO_ApprovalMenu approval_menu(so_manager);
                                    
    if (approval_menu.interact() >= 0)
    {
        so_id_t the_id = approval_menu.get_id();
    }

    return true; 
}

bool Seller::close_menu(void) {
    SO_ClosingMenu closing_menu(so_manager);

    if (closing_menu.interact() >= 0)
    {
        
    }

    return true; 
}

#include <list>

void Seller::interact(void) {
    
    size_t opcao = 0;
    bool looping = true;
    
    while (looping) 
    {
        display_interaction_guide();

        // When the input by <cin> fails it is default at 0.
        cin >> opcao;
        cin.clear();
        cin.ignore(INT64_T_MAX,'\n');

        list<struct ServiceOrder> budget_orders;

        switch (opcao) {
        case 0: // Exiting the menu
            looping = false; 
            break;
        
        case 1: // Registering a new client
            switch (register_client_menu())
            {
            case REGISTERCLIENT_ALREADY_EXIST:
                cerr << "VTMNC EXISTENTE" << endl;
                break;
                       
            case REGISTERCLIENT_FAIL:
                cerr << "VTMNC REGISTRANTE" << endl;
                break;
                 
            default:
                break;
            }
            
            break;

        case 2: // Generating a new SO
            if (! GenerateSO()) {
                cout << "Não foi possível gerar uma nova SO." << endl;
            }
            break;

        case 3:
            budget_orders = so_manager->so_category(SO_BUDGET);
            for (struct ServiceOrder order : budget_orders)
            {
                printf("SO: #%llu\n", order.id);
            }

            cout << "Gostaria de entrar no menu de navegações de SOs? ";
            if (input_verification())
            {
                so_manager->so_category_vizualizer(SO_BUDGET);
            }

            if (! approve_menu())
                cerr << "Erro ao aprovar ordem" << endl;
            break;

        case 4:

            if (! close_menu())    
                cerr << "Burro4!!!" << endl;
            break;

        default: 
            cerr << "Burro5!!!" << endl;
            break;
        }
    }
    
    cerr << "Exited" << endl;
}
#endif

void Seller::display_interaction_guide(void) const {
    cout << "Qual alteração você deseja fazer?" << endl;
    cout << "0\t->\tSair;" << endl;
    cout << "1\t->\tRegistrar um cliente;" << endl;
    cout << "2\t->\tGerar uma Ordem de Serviço;" << endl;
    cout << "3\t->\tAprovar uma Ordem de Serviço;" << endl;
    cout << "4\t->\tConcluir uma Ordem de Serviço." << endl;
    fflush(stdout);
}

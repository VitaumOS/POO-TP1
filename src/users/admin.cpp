


#include "admin.hpp"
#include <string>

//Construtor do Admnistrador
Administrator::Administrator(Id_t id, SO_Manager * so_manager, Users_DB * users_db) 
    : User(id, so_manager), users_db(users_db) {

}

// Destrutor do Admnistrador
Administrator::~Administrator(void) {

}

//Função que registra um novo vendedor. Retorna true se conseguir e falso caso contrário
bool Administrator::Register_Data_Seller(username_string_t username,
    password_string_t password) { 

    if (! users_db->register_user(USER_TYPE_SLR, username, password))
        return false; 
    return true;
}

// Função que atualiza as informações de um vendedor já existente. Retorna true se conseguir e falso caso contrário
bool Administrator::Update_Data_Seller(username_string_t username,
    password_string_t password) { 

    struct UserData user_data;
    if (users_db->fetch_username(username, &user_data) >= 0){

        cout << "Digite o novo usuário e senha do vendedor \" " << username<< "\"";
        cin >> username >> password;

        /*
        update_db(user);  TODO: fazer essa função que atualiza a informação de um usuário (usuário e senha)
        */
    }
}


// Função que define o menu para a criação/alteração do vendedor pelo Admnistrador
void Administrator::edit_seller(void) { 
    int option = 0;
#if 0
    username_string_t username[username_string_length];
    password_string_t password[password_string_length];

    bool running_menu = true;
    while (running_menu)
    {
        cout << "Qual alteração você deseja fazer?"<<endl;
        cout << "0\t->\tCriar Novo Vendedor;" << endl;
        cout << "1\t->\tEditar Vendedor Existente;" << endl;
        cout << "2\t->\tVoltar;" << endl;

        cin >> option;
        cin.clear();
        cin.ignore(INT64_T_MAX, '\n');

        

        switch(option){
            case 0:

                cout << "Digite o usuário e senha do novo vendedor: ";
                cin >> username >> password;
                if(!Register_Data_Seller(username, password))
                    cout << "Erro ao criar o Vendedor!"<<endl;
                else
                    cout << "Vendedor criado com sucesso!"<<endl;
                
            break;
            case 1:

                cout << "Digite o usuário do vendedor que queira alterar: ";
                cin >> username;

                if(!Update_Data_Seller(username, password))
                    cout << "Erro ao atualizar o Vendedor!"<<endl;
                else
                    cout << "Vendedor atualizado com sucesso!"<<endl;
            break;
            case 2:

            break;
            default:
                cout <<"Valor inválido!"<<endl;
            break;
        }
    }
#endif
}

//Função que registra um novo Mecânico. Retorna true se conseguir e falso caso contrário
bool Administrator::Register_Data_Mechanic(username_string_t username,
    password_string_t password){ 

    if (! users_db->register_user(USER_TYPE_MCH, username, password))
        return false; 
    return true;
}


// Função que atualiza as informações de um mecânico já existente. Retorna true se conseguir e falso caso contrário
bool Administrator::Update_Data_Mechanic(username_string_t username,
    password_string_t password) { 

    struct UserData user_data;
    if (users_db->fetch_username(username, &user_data) >= 0){

        cout << "Digite o novo usuário e senha do Mecânico \" "<< username<<"\"";
        cin >> username >> password;

        /*
        update_db(user);  TODO: fazer essa função que atualiza a informação de um usuário (usuário e senha)
        */
    }
        
}

void Administrator::edit_mechanic(void) { // Função que define o menu para a criação/alteração do mecânico pelo Admnistrador
#if 0    
    int option;
    do{
        cout<< "Qual alteração você deseja fazer?"<<endl;
        cout << "0\t->\tCriar Novo Mecânico;" << endl;
        cout << "1\t->\tEditar Mecânico Existente;" << endl;
        cout << "2\t->\tVoltar;" << endl;

        cin>>option;

        username_string_t username[username_string_length];
        password_string_t password[password_string_length];

        switch(option){
            case 0:

                cout << "Digite o usuário e senha do novo Mecânico: ";
                cin >> username >> password;
                if(!Register_Data_Mechanic(username, password))
                    cout << "Erro ao criar o Mecânico!"<<endl;
                else
                    cout  << "Mecânico criado com sucesso!"<<endl;
                
            break;
            case 1:

                cout << "Digite o usuário do Mecânico que queira alterar: ";
                cin >> username;

                if(!Update_Data_Mechanic(username, password))
                    cout << "Erro ao atualizar o Mecânico!"<<endl;
                else
                    cout  << "Mecânico atualizado com sucesso!"<<endl;
            break;
            case 2:
                return;
            break;
            default:
                cout<<"Valor inválido!"<<endl;
            break;
        }
    }while(option!=2);
#endif
}

bool Administrator::Register_New_Admin(username_string_t username, password_string_t password) {

    if (!users_db->register_user(USER_TYPE_ADM, username, password))
        return false; 
    return true;
    
}

void Administrator::edit_admin(void){

    username_string_t username;
    password_string_t password;
    
    cout << "Digite o usuário e senha do novo Administrador: ";
    cin >> username >> password;
    if(!Register_New_Admin(username, password))
        cout << "Erro ao criar o Administrador!"<<endl;
    else
        cout  << "Administrador criado com sucesso!"<<endl;
}

// Menu geral do administrador
void Administrator::interact(void) { 
    int option = -1;
    
    bool running_menu = true;
    while (running_menu) {
        cout << "O que desejas fazer?" << endl;
        cout << "0\t->\tSair;" << endl;
        cout << "1\t->\tGerenciar Vendedores;" << endl;
        cout << "2\t->\tGerenciar Mecânicos;" << endl;
        cout << "3\t->\tGerenciar Administradores;" << endl;
        cout << "4\t->\tGerenciar usuários" << endl;
        
        cin >> option;
        cin.clear();
        cin.ignore(INT64_T_MAX, '\n');

        switch (option) {
        /*  Exit */
        case 0: 
            running_menu = false;
            break;
        
        case 1:
            Administrator::edit_seller();
            break;

        case 2: 
            Administrator::edit_mechanic();            
            break;

        case 3:
            Administrator::edit_admin();
            break;

        case 4:
            Administrator::edit_users();
            break;
            
        default:
            /*  Invalid input... */
            break;
        }
    }
}

void Administrator::edit_users(void)
{


}


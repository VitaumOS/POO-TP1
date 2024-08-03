


#include "admin.hpp"
#include <string>

//Construtor do Admnistrador
Administrator::Administrator(id_t id, SO_Manager * so_manager, Users_DB * users_db) 
    : User(id, so_manager), users_db(users_db) { 
    cout << "ADM+" << endl;    
}

// Destrutor do Admnistrador
Administrator::~Administrator(void){}

//Fun��o que registra um novo vendedor. Retorna true se conseguir e falso caso contr�rio
bool Administrator::Register_Data_Seller(username_string_t username,
    password_string_t password) { 

    if (! users_db->register_user(USER_TYPE_SLR, username, password))
        return false; 
    return true;
}

// Fun��o que atualiza as informa��es de um vendedor j� existente. Retorna true se conseguir e falso caso contr�rio
bool Administrator::Update_Data_Seller(username_string_t username,
    password_string_t password) { 

    struct UserData user_data;
    if (users_db->fetch_username(username, &user_data) >= 0){

        cout << "Digite o novo usu�rio e senha do vendedor \" " << username<< "\"";
        cin >> username >> password;

        /*
        update_db(user);  TODO: fazer essa fun��o que atualiza a informa��o de um usu�rio (usu�rio e senha)
        */
    }
}


// Fun��o que define o menu para a cria��o/altera��o do vendedor pelo Admnistrador
void Administrator::SellerMenu(void) { 
#if 0    
    int option;
    do{
        cout << "Qual altera��o voc� deseja fazer?"<<endl;
        cout << "0\t->\tCriar Novo Vendedor;" << endl;
        cout << "1\t->\tEditar Vendedor Existente;" << endl;
        cout << "2\t->\tVoltar;" << endl;

        cin>>option;

        username_string_t username[username_string_length];
        password_string_t password[password_string_length];

        switch(option){
            case 0:

                cout << "Digite o usu�rio e senha do novo vendedor: ";
                cin >> username >> password;
                if(!Register_Data_Seller(username, password))
                    cout << "Erro ao criar o Vendedor!"<<endl;
                else
                    cout << "Vendedor criado com sucesso!"<<endl;
                
            break;
            case 1:

                cout << "Digite o usu�rio do vendedor que queira alterar: ";
                cin >> username;

                if(!Update_Data_Seller(username, password))
                    cout << "Erro ao atualizar o Vendedor!"<<endl;
                else
                    cout << "Vendedor atualizado com sucesso!"<<endl;
            break;
            case 2:

            break;
            default:
                cout <<"Valor inv�lido!"<<endl;
            break;
        }
    }while(option!=2);
    // int64_t fetch_username(const username_string_t username, struct UserData * const return_data);
#endif
}

//Fun��o que registra um novo Mec�nico. Retorna true se conseguir e falso caso contr�rio
bool Administrator::Register_Data_Mechanic(username_string_t username,
    password_string_t password){ 

    if (! users_db->register_user(USER_TYPE_MCH, username, password))
        return false; 
    return true;
}


// Fun��o que atualiza as informa��es de um mec�nico j� existente. Retorna true se conseguir e falso caso contr�rio
bool Administrator::Update_Data_Mechanic(username_string_t username,
    password_string_t password) { 

    struct UserData user_data;
    if (users_db->fetch_username(username, &user_data) >= 0){

        cout << "Digite o novo usu�rio e senha do Mec�nico \" "<< username<<"\"";
        cin >> username >> password;

        /*
        update_db(user);  TODO: fazer essa fun��o que atualiza a informa��o de um usu�rio (usu�rio e senha)
        */
    }
        
}

void Administrator::MechanicMenu(void) { // Fun��o que define o menu para a cria��o/altera��o do mec�nico pelo Admnistrador
#if 0    
    int option;
    do{
        cout<< "Qual altera��o voc� deseja fazer?"<<endl;
        cout << "0\t->\tCriar Novo Mec�nico;" << endl;
        cout << "1\t->\tEditar Mec�nico Existente;" << endl;
        cout << "2\t->\tVoltar;" << endl;

        cin>>option;

        username_string_t username[username_string_length];
        password_string_t password[password_string_length];

        switch(option){
            case 0:

                cout << "Digite o usu�rio e senha do novo Mec�nico: ";
                cin >> username >> password;
                if(!Register_Data_Mechanic(username, password))
                    cout << "Erro ao criar o Mec�nico!"<<endl;
                else
                    cout  << "Mec�nico criado com sucesso!"<<endl;
                
            break;
            case 1:

                cout << "Digite o usu�rio do Mec�nico que queira alterar: ";
                cin >> username;

                if(!Update_Data_Mechanic(username, password))
                    cout << "Erro ao atualizar o Mec�nico!"<<endl;
                else
                    cout  << "Mec�nico atualizado com sucesso!"<<endl;
            break;
            case 2:
                return;
            break;
            default:
                cout<<"Valor inv�lido!"<<endl;
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

void Administrator::AdminMenu(void){

    username_string_t username;
    password_string_t password;
    
    cout << "Digite o usu�rio e senha do novo Administrador: ";
    cin >> username >> password;
    if(!Register_New_Admin(username, password))
        cout << "Erro ao criar o Administrador!"<<endl;
    else
        cout  << "Administrador criado com sucesso!"<<endl;
}

//Menu do Administrador
void Administrator::interact(void) { 
    int option;
    do {
            
        cout << "Qual altera��o voc� deseja fazer?"<<endl;
        cout << "0\t->\tEditar dados de Vendedores;" << endl;
        cout << "1\t->\tEditar dados de Mec�nicos;" << endl;
        cout << "2\t->\tCriar novo Administrador;" << endl;
        cout << "3\t->\tSair;" << endl;
            
        cin >> option;

        switch(option){ // seller_origin, edit_seller <- arg solto
            case EDIT_SLR: SellerMenu();              break;

            case EDIT_MCH: MechanicMenu();            break;

            case EDIT_ADMIN: 
                // Register_New_Admin();    
                break;

            case EXIT_ADM_OPTIONS: return;            break;

            default: cout<<"aaaa vou me matar"<<endl; break;
        }
    } while (option != EXIT_ADM_OPTIONS);
}



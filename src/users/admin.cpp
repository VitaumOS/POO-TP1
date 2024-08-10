

#include "admin.hpp"
#include "../ui/db-vizualizers.hpp"
#include <string>


// Construtor do Admnistrador
Administrator::Administrator(class SO_Manager * const so_manager, class UsersDatabase * const users_db, 
    const struct MinimalUserData & user_data) : UserScreen(so_manager, users_db, user_data) {

    UserScreen::menu_title = "Menu de Administrador";
}

// Destrutor do Admnistrador
Administrator::~Administrator(void) {

}


#if 0
// Função que define o menu para a criação/alteração do vendedor pelo Admnistrador
void Administrator::edit_seller(void) { 
    int option = 0;
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
    if (users_db->fetch_username(username, user_data) >= 0){

        cout << "Digite o novo usuário e senha do Mecânico \" "<< username<<"\"";
        cin >> username >> password;

        /*
        update_db(user);  TODO: fazer essa função que atualiza a informação de um usuário (usuário e senha)
        */
    }
    return false;
}

void Administrator::edit_mechanic(void) { // Função que define o menu para a criação/alteração do mecânico pelo Admnistrador
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
    if(! Register_New_Admin(username, password))
        cout << "Erro ao criar o Administrador!"<<endl;
    else
        cout  << "Administrador criado com sucesso!"<<endl;
}
#endif

int Administrator::render(void)
{
    clean_screen();

    /*  header */
    UserScreen::render_menu_header();

    /*  footer */
    constexpr const char * footer_title = "O que desejas fazer?";
    print_n_char('-', literal_string_length(footer_title) - 1);
    std::cout << std::endl << footer_title << std::endl;
    std::cout << "1\t->\tSair" << std::endl;
    std::cout << "2\t->\tGerenciar usuários" << std::endl;

    fflush(stdout);
    return 0;
}

int Administrator::process(void)
{
    int option = -1;
    std::cin >> option;
    MenuScreen::clean_stdin();

    switch (option) {
        /*  Exit */
    case 1:
        UserScreen::main_loop = false;
        break;

    case 2:
        Administrator::edit_users();
        break;

    default:
        /*  Invalid input... */
        break;
    }
    return 0;
}

void Administrator::edit_users(void)
{
    struct UserData self;

    // TODO: add more control and prevention at this fetching...?
    users_db->fetch_userid(user_data.id, self);

    UsersEditor edit_users_menu(users_db, self);
    edit_users_menu.interact();
}


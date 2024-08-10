/*  <src/seller.cpp>

    (...) */
 

#include "seller.hpp"


class SO_MenuScreen : virtual public MenuScreen {
protected:
    so_id_t id = 0;
    class SO_Manager * so_manager = nullptr;
    struct ServiceOrder so_buffer;

    std::string output_buffer;


public:
    SO_MenuScreen(class SO_Manager * const so_manager);
    virtual ~SO_MenuScreen(void);

};

class SO_ApprovalMenu : virtual public SO_MenuScreen {
private:
    so_id_t id = 0;
    struct ServiceOrder so_buffer;

    std::string output_buffer;

    virtual int render(void);
    void interact_so(void);

public:
    SO_ApprovalMenu(class SO_Manager * const so_manager);
    virtual ~SO_ApprovalMenu(void);

    virtual int interact(void);
    inline so_id_t get_id(void) const { return id; };
};

class SO_ClosingMenu : virtual public SO_MenuScreen {
private:

    virtual int render(void);

public:
    SO_ClosingMenu(class SO_Manager * const so_manager);
    virtual ~SO_ClosingMenu(void);

    virtual int interact(void);
};

SO_MenuScreen::SO_MenuScreen(class SO_Manager * const so_manager) : MenuScreen(), so_manager(so_manager) {
    if (so_manager == nullptr)
        throw std::runtime_error("VTMNC\n");
}

SO_MenuScreen::~SO_MenuScreen(void) {

}

SO_ApprovalMenu::SO_ApprovalMenu(class SO_Manager * const so_manager) : SO_MenuScreen(so_manager) {

}

SO_ApprovalMenu::~SO_ApprovalMenu(void) {

}

int SO_ApprovalMenu::render(void)
{
    clean_screen();

    constexpr const char * menu_base_title = "Menu de aprovações de ordens";
    constexpr size_t title_length = literal_string_length(menu_base_title);

    print_n_char('=', 50); putchar('\n');
    printf("%s\n", menu_base_title);
    print_n_char('*', title_length - 1); putchar('\n');
    print_n_char('\n', 5);

    std::cout << "*: " << output_buffer;
    print_n_char('\n', 5);

    return 0;
}

void SO_ApprovalMenu::interact_so(void)
{
    int64_t input_so_id;
    bool menu_loop = true;

    while (menu_loop)
    {
        /*  Asking for the SO id. */
        std::cin >> input_so_id;
        clean_stdin();

        if (input_so_id < 0)
        {
            std::cout << "Entrada inválida. Deseja tentar inserir novamente? ";
            if (! input_verification())
                menu_loop = false;
            continue;
        }

        SO_ApprovalMenu::id = static_cast<so_id_t> (input_so_id);

        if (! SO_ApprovalMenu::so_manager->get_order(SO_ApprovalMenu::id, &so_buffer))
        {
            std::cout << "Entrada inválida. Não existe nenhuma SO com o id \"" <<
                static_cast<unsigned long long> (SO_ApprovalMenu::id) << "\". Tente novamente ou dê o bumbum." << std::endl;
        }

        std::cout << "After get_order..." << std::endl;

        // SO_BUDGET
        if (SO_ApprovalMenu::so_buffer.stage != SO_BUDGET)
        {
            SO_ApprovalMenu::output_buffer +=
                "VTMNC DIGITA O TREM CERTO VÉI NÓOOOOO (a ordem não está sob o estágio de orçamento).\n";
            break;
        }

        /*  Asking for verification on the approving the order. */
        std::cout << "Deseja aprovar ordem de id \"" << static_cast<unsigned long long> (SO_ApprovalMenu::id) << "\"? ";
        if (! input_verification())
        {
            std::cout << "Deseja cancelá-la? ";
            if (input_verification())
            {
                if (SO_ApprovalMenu::so_manager->close_order(SO_ApprovalMenu::id, &(SO_ApprovalMenu::so_buffer))) {
                    SO_ApprovalMenu::output_buffer +=
                        "Ordem NSQLA foi cancelada...\n";
                }
                else {
                    SO_ApprovalMenu::output_buffer +=
                        "Ordem não pôde ser cancelada...\n";
                }
            }

            menu_loop = false;
            break;
        }

        if (SO_ApprovalMenu::so_manager->operate_order(SO_ApprovalMenu::id, &(SO_ApprovalMenu::so_buffer))) {
            SO_ApprovalMenu::output_buffer +=
                "Ordem SLAOQ aprovada com sucesso...";
        }
        else {
            SO_ApprovalMenu::output_buffer +=
                "A ordem SLAOQ não pôde ser aprovada...";
        }
        menu_loop = false;
    }
}

int SO_ApprovalMenu::interact(void)
{
    char command_buffer;

    bool menu_loop = true;
    while (menu_loop) {
        /*  Rendering the screen... */
        SO_ApprovalMenu::render();
        output_buffer.clear();

        /*  Inputting the command. */
        std::cout << "[i] para entrar com a ordem [q] para sair\n";
        std::cin >> command_buffer;
        std::cin.clear();
        std::cin.ignore(INT64_MAX, '\n');

        switch (command_buffer)
        {
        case 'i':
            SO_ApprovalMenu::interact_so();
            break;

        case 'q':
            menu_loop = false;
            break;

        default: continue;
        }
    }

    return 0;
}

SO_ClosingMenu::SO_ClosingMenu(class SO_Manager * const so_manager) : SO_MenuScreen(so_manager)
{


}

SO_ClosingMenu::~SO_ClosingMenu(void)
{

}

int SO_ClosingMenu::render(void)
{
    clean_screen();

    constexpr const char * menu_base_title = "Menu de conclusão de ordens";
    constexpr size_t title_length = literal_string_length(menu_base_title);

    print_n_char('=', 50); putchar('\n');
    printf("%s\n", menu_base_title);
    print_n_char('*', title_length - 1); putchar('\n');
    print_n_char('\n', 5);


    std::cout << "*: " << output_buffer;
    print_n_char('\n', 5);

    return 0;
}

int SO_ClosingMenu::interact(void)
{
    char command_buffer;

    char temp_bf;

    bool menu_loop = true;
    while (menu_loop) {
        /*  Rendering the screen... */
        SO_ClosingMenu::render();
        output_buffer.clear();

        /*  Inputting the command. */
        std::cout << "[i] para entrar com a ordem [q] para sair\n";
        std::cin >> command_buffer;
        std::cin.clear();
        std::cin.ignore(INT64_MAX, '\n');

        switch (command_buffer)
        {
        case 'i':
            std::cout << "Temporariamente em branco; não implementado ainda... (PRESSIONE ALGO PARA CONTINUAR)" << std::endl;
            std::cin >> temp_bf;
            break;

        case 'q':
            menu_loop = false;
            break;

        default: continue;
        }
    }

    return 0;
}






/*  -------------------------- */

class SellerInspectSO : virtual public MenuScreen
{
private:
    class SO_Manager * so_manager;
    struct ServiceOrder so;

    bool menu_loop = true;
    bool altered = false;
    std::string feedback_err_buffer;

    int render(void) override {
        clean_screen();

        // header
        std::cout << "Editor de SOs (Vendedor)" << std::endl;
        std::cout << "------------------------" << std::endl;

        // user information
        printf("\n\n\n");
        printf("so: ");

        // footer
        print_n_char('\n', 2);
        print_n_char('-', 10); putchar('\n');

        std::cout << "\tq:\tSair da inspeção\n";
        print_n_char('=', 50);
        print_n_char('\n', 2);

        return 0;
    }

    int process(void) override {
        char command_buffer;
        std::cin >> command_buffer;

        switch (command_buffer)
        {
        case 'q':
            SellerInspectSO::menu_loop = false;
            break;

        default:
            break;
        }
        return 0;
    }

public:
    SellerInspectSO(class SO_Manager * const so_manager, struct ServiceOrder & so) :
        MenuScreen(), so_manager(so_manager), so(so)
    {
        if (so_manager == nullptr)
            throw std::runtime_error("AEIOU");
    }

    virtual ~SellerInspectSO(void) {

    }

    int interact(void) {

        /*	creating the event loop */
        while (SellerInspectSO::menu_loop)
        {
            /*	rendering the screen */
            SellerInspectSO::render();

            /*	processing the input */
            SellerInspectSO::process();
        }
        SellerInspectSO::menu_loop = true;

        if (SellerInspectSO::altered)
            return 1;
        return 0;
    }
};

class SellerSO_Vizualizer : virtual public SO_Vizualizer {
private:
    struct UserData user_data;
    class Seller * seller;

    /*  Tracks the category iterator for the vizualizer.
        That is, represents the current type of filtering over SO database. */
    int cat = 0;

    void render_footer(void) const {
        print_n_char('\n', 2);
        print_n_char('-', 10); putchar('\n');
        std::cout << "\tw:\tItem anterior\n";
        std::cout << "\ts:\tPróximo item\n";
        std::cout << "\ta:\tPágina anterior\n";
        std::cout << "\td:\tPróxima página\n";
        std::cout << "\ti:\tInspecionar ordem\n";
        
        std::cout << "\tf:\t";

        switch (cat) {
        case 0:
            std::cout << "Filtrar por ordens abertas";
            break;

        case 1:
            std::cout << "Filtrar por ordens orçamentadas";
            break;

        case 2:
            std::cout << "Mostrar todas as ordens";
            break;
            
        default:
            break;
        }
        
        std::cout << "\n\tn:\tGerar ordem\n";
        std::cout << "\tq:\tSair do visualizador\n";
        print_n_char('=', 50);
        print_n_char('\n', 2);
    }

    void event_n_key(void) {    /*  Add SO menu */
        
        struct ClientData client_buffer;
        if (! seller->get_client(client_buffer)) {
            std::cerr << "Falha em associar cliente à nova SO.\n";
            press_anything_to_continue();
            return;
        }

        std::cout << "Qual é o seu problema?\t";

        std::string issue;
        std::cin >> issue;
        issue.resize(SO_DESCRIPTION_SIZE);

        std::cout << "Deseja abrir uma SO para <" << client_buffer.person.name << ">? ";

        if (! input_verification())
        {
            std::cerr << "cancl\n";
            press_anything_to_continue();
            return;
        }

        struct ServiceOrder so_buffer;
        if (! so_manager->new_order(issue.c_str(), client_buffer.id, &so_buffer))
        {
            std::cout << "falhow...\n";
            press_anything_to_continue();
            return;
        }

        cat = (cat - 1) % 3;
        event_f_key();
    }

    void event_f_key(void) {
        cat = (cat + 1) % 3;
        switch (cat)
        {
        case 0: 
            set_category(SO_ALL);
            break;

        case 1:
            set_category(SO_OPEN);
            break;

        case 2:
            set_category(SO_BUDGET);
            break;

        default:    
            break;
        }

        reload_page();
    }

public:
    SellerSO_Vizualizer(class SO_Manager * const so_manager, const struct UserData & user_data,
        class Seller * const seller) : 
        SO_Vizualizer(so_manager), user_data(user_data), seller(seller) {

        if (seller == nullptr)
            throw std::runtime_error("ASD");
        SO_Vizualizer::set_category(SO_ALL);
    }

    ~SellerSO_Vizualizer(void) {

    }

    void inspect(void) override {
        struct ServiceOrder so;
        SO_Vizualizer::get_service_order(so);
        
        SellerInspectSO inspect_so(so_manager, so);

        if (inspect_so.interact() == 0)
            return;

        const size_t sos_size = SOs.size();
        if ((sos_size > 0) && (focus_index > 0) && (focus_index == (sos_size - 1)))
            focus_index --;

        SOs.erase(std::next(SOs.begin(), SO_Vizualizer::focus_index));
        SO_Vizualizer::reload_page();
    }
};




/*  The main seller interface *
 *  ------------------------- */

Seller::Seller(class SO_Manager * const so_manager, class UsersDatabase * const users_db, const struct MinimalUserData & user_data) : 
    UserScreen(so_manager, users_db, user_data), so_vizualizer(so_manager) {
    client_is_loaded = false;
    so_is_loaded = 0;

    constexpr const char * MENU_TITLE = "Menu de Vendedor";
    UserScreen::menu_title = const_cast<char *> (MENU_TITLE);
}

Seller::~Seller(void) {

}

int Seller::render(void) 
{
    clean_screen();

    /*  header */
    UserScreen::render_menu_header();

    if (client_is_loaded)
    {
        std::cout << "Cliente ";
        printf("\t[%llu:%llu]:\n",
            (unsigned long long) client_buffer.id.person_id,
            (unsigned long long) client_buffer.id.vehicle_id);

        printf("\tPessoa\t|\tnome: %s\n", client_buffer.person.name);
        std::cout << "\tVeículo\t|\ttipo: " << client_buffer.vehicle.type << ", modelo: "
            << client_buffer.vehicle.model << ", kilometragem: "
            << client_buffer.vehicle.mileage << std::endl;
        std::cout << "\tdata de registro: ";
        std::cout << client_buffer.registry_date << std::endl << std::endl;

    }
    else {
        printf("(cliente não carregado...)\n\n");
    }

    /*  footer: interaction guide */
    constexpr const char * footer_title = "O que desejas fazer?";
    print_n_char('-', literal_string_length(footer_title) - 1);
    std::cout << std::endl << footer_title << std::endl;
    std::cout << "1\t->\tSair" << std::endl;
    std::cout << "2\t->\tRegistrar um cliente" << std::endl;
    std::cout << "3\t->\tCarregar um cliente" << std::endl;
    std::cout << "4\t->\tNavegar sobre ordens de serviço" << std::endl;

    std::cout << "5\t->\tAprovar uma ordem de serviço" << std::endl;
    std::cout << "6\t->\tConcluir ordem de serviço" << std::endl;
    
    fflush(stdout);
    return 0;
}

void Seller::manage_sos(void)
{
    struct UserData self;

    // TODO: add more control and prevention at this fetching...?
    users_db->fetch_userid(user_data.id, self);

    class SellerSO_Vizualizer so_vizualizer(so_manager, self, this);
    so_vizualizer.interact();
}

int Seller::process(void)
{
    int opcao;
    std::cin >> opcao;
    clean_stdin();

    switch (opcao) {
    case 1: // Exiting the menu
        UserScreen::main_loop = false;
        break;
        
    case 2: // Registering a new client
        Seller::register_client();
        break;
    
    case 3:
        Seller::load_client_interface();
        break;

    case 4:
        Seller::manage_sos();
        break;
    
    case 5:
        std::cout << "Gostaria de entrar no menu de navegações de SOs orçamentadas antes? ";
        if (input_verification()) {
            so_vizualizer.set_category(SO_BUDGET);
            so_vizualizer.interact();
        }

        Seller::approve_menu();
        break;

    case 6:

        std::cout << "Gostaria de entrar no menu de navegações de SOs antes? ";
        if (input_verification())
        {
            so_vizualizer.set_category(SO_ALL);
            so_vizualizer.interact();
        }

        Seller::close_menu();
        break;

    default:
        break;
    }
    return 0;
}

// * updated
void Seller::register_client(void) {
    /*  warning */
    std::cout << "(Cuide a entrada dos dados a seguir)" << std::endl;
    press_anything_to_continue();

    /*  person's name */
    std::cout << "Digite o nome do cliente:\t\t\t\t";
    std::cin.getline(client_buffer.person.name, NAME_SIZE);
    
    if (Seller::load_client(client_buffer.person.name)) {
        std::cout << "Ocorrência da pessoa <" << client_buffer.person.name << "> encontrada no banco de dados de clientes ("
            << all_client_data.size() << ") clientes." << std::endl;
    }

    /*  vehicle's type */
    std::cout << "Qual é o tipo do veículo (Ex: carro, moto, etc...)?\t";
    std::cin.getline(client_buffer.vehicle.type, NAME_SIZE);
    
    /*  vehicle's model */
    std::cout << "Qual é o modelo do veículo?\t\t\t\t";
    std::cin.getline(client_buffer.vehicle.model, NAME_SIZE);

    /*  vehicl's mileage */
    std::cout << "Qual é a quilometragem do seu veículo?\t\t\t";
    do {
        std::cin >> client_buffer.vehicle.mileage;
        clean_stdin();
        
    } while ((client_buffer.vehicle.mileage < 0) && (std::cout << "Entrada inválida para kilometragem. Redigite-a:\t\t"));


    // TODO: deixar meió
    printf("Pessoa\t| Nome: %-64s;\nVeículo\t| Modelo: %s, tipo: %s, kilometragem: %d\n",
        client_buffer.person.name,
        client_buffer.vehicle.model, client_buffer.vehicle.type, (int) client_buffer.vehicle.mileage);
    
    std::cout << "Deseja mesmo registar um novo cliente para <" << client_buffer.person.name << ">? ";
    if (! input_verification()) 
        return; // canceling registering
       
    if (! so_manager->client_manager.register_client(client_buffer.person, client_buffer.vehicle, client_buffer))
    {
        // fail
        std::cerr << "Algo deu errado no prcesso de registrar o cliente...\n";
        press_anything_to_continue();
        return;
    }
       
    all_client_data.push_back(client_buffer);
}

bool Seller::get_client(struct ClientData & client_buffer) {
    if (! Seller::client_is_loaded) { 
        clean_stdin();

        if (! Seller::load_client_interface())
        {
            std::cout << "Não foi possível carregar o cliente.\n";
            MenuScreen::press_anything_to_continue();
            return false;
        }
    }
    client_buffer = this->client_buffer;
    return true;
}

// * old
void Seller::new_so(void) {
    
    if ((! client_is_loaded) && (! Seller::load_client_interface())) {
        std::cout << "Não foi possível carregar o cliente.\n";
        press_anything_to_continue();
        return;
    }
    
    std::cout << "Qual é o seu problema?\t";
    std::string issue;
    std::cin >> issue;
    issue.resize(SO_DESCRIPTION_SIZE);

    std::cout << "Deseja abrir uma SO para <" << client_buffer.person.name << ">? ";

    if (! input_verification())
    {
        std::cerr << "cancl\n";
        press_anything_to_continue();
        return;
    }

    if (! so_manager->new_order(issue.c_str(), client_buffer.id, &so_buffer))
    {
        std::cout << "falhow...\n";
        press_anything_to_continue();
        return;
    }
}

bool Seller::load_client_interface(void) {

    if (client_is_loaded)
    {
        std::cout << "Deseja carregar uma nova pessoa? ";
        if (input_verification())
            client_is_loaded = false;

        clean_stdin();
    }

    struct PersonData person_data;
    if (! client_is_loaded) {
        clean_screen();
        printf("\n\n\tCarregando clientes...\n\n\n");

        std::cout << "Digite o nome da pessoa: ";
        std::cin.getline(person_data.name, NAME_SIZE);

        if (! load_client(person_data.name))
        {
            std::cout << "Falha em carregar a pessoa <" << person_data.name << ">...\n";
            press_anything_to_continue();
            return false;
        }
    }
    else {
        strcpy(person_data.name, client_buffer.person.name);
    }

    /*  selecting the vehicle's index */
    bool vehicles_menu = true;
    char command;

    struct ClientData the_client_data;

    while (vehicles_menu)
    {
        clean_screen();
        printf("\nSeleção de Cliente\n\n\tPessoa: %s\n\n", person_data.name);

        the_client_data = static_cast<struct ClientData> (* std::next(all_client_data.begin(), focus_index));

        printf("\t\tVeículo #%llu/%llu:\n", (unsigned long long) (focus_index + 1), (unsigned long long) all_client_data.size());
        printf("\t\tmodelo: %s, tipo: %s, kilometragem: %llu\n",
            the_client_data.vehicle.model, the_client_data.vehicle.type,
            (unsigned long long) the_client_data.vehicle.mileage);

        printf("\n\na: Veículo anterior\n");
        printf("d: Veículo posterior\n");
        printf("q: Sair (confirmar seleção)\n");

        std::cin >> command;
        switch (command){
        case 'a':
            if (focus_index > 0)
                focus_index --;
            break;
        
        case 'd':
            focus_index ++;
            if (focus_index >= all_client_data.size())
                focus_index --;
            break;

        case 'q':
            vehicles_menu = false;
            break;
        }
    }
    
    client_buffer = the_client_data;
    return true;
}

bool Seller::load_client(const char name[NAME_SIZE]) {
    all_client_data = so_manager->client_manager.get_person_clients(name);
    
    Seller::client_is_loaded = all_client_data.size() > 0;
    return Seller::client_is_loaded;
}


void Seller::approve_menu(void) {
    SO_ApprovalMenu approval_menu(so_manager);
                                    
    if (approval_menu.interact() >= 0)
    {
        so_id_t the_id = approval_menu.get_id();
    }
}

void Seller::close_menu(void) {
    SO_ClosingMenu closing_menu(so_manager);

    if (closing_menu.interact() >= 0)
    {
        
    }
}

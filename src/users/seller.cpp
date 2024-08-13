/*  <src/seller.cpp>

    (...) */
 

#include "seller.hpp"


class SellerInspectSO : virtual public MenuScreen
{
private:
    class SO_Manager * so_manager = nullptr;
    struct ServiceOrder so;

    bool menu_loop = true;
    bool altered = false;
    std::string feedback_err_buffer;
    bool positive_highlight = false;

    int render(void) const override {
        clean_screen();

        // header
        std::cout << "Inspeção de SOs (Vendedor)" << std::endl;
        std::cout << "--------------------------" << std::endl;

        // so information
        printf("\n\n\n");
        std::cout << so;

        // footer
        print_n_char('\n', 2);
        print_n_char('-', 10); putchar('\n');
        
        if (so.stage == SO_BUDGET)
            std::cout << "\ti:\tAprovar ordem\n";

        if (so.stage < SO_CLOSED)
            std::cout << "\tc:\tFechar ordem\n";

        std::cout << "\tq:\tSair da inspe��o\n";
        print_n_char('=', 50);
        print_n_char('\n', 2);
        
        if (positive_highlight) aec_fg_rgb(100, 150, 150);
        else                    aec_fg_rgb(150, 100, 100);

        std::cerr << feedback_err_buffer << std::endl;
        aec_reset();

        return 0;
    }

    void advance_so(void) {
        if (so.stage != SO_BUDGET)
            return;

        std::cout << "Deseja aprovar ordem de id \"" << so.id << "\"?";
        std::cout << "(Em caso positivo, a ordem ser� encaminhada para os mec�nicos para a manuten��o) ";
        if (! input_verification())
            return;

        if (SellerInspectSO::so_manager->operate_order(so.id, &so)) 
        {
            SellerInspectSO::feedback_err_buffer +=
                "A ordem p�de ser encaminhada com sucesso para o estado de manuten��o...";
            positive_highlight = true;
            altered = true;
        }
        else {
            SellerInspectSO::feedback_err_buffer +=
                "A ordem n�o p�de ser encaminhada com sucesso para o estao de manute��o...";
        }
    }
    
    void close_so(void) {
        if (so.stage >= SO_CLOSED)
            return;

        if (so.stage == SO_MAINTENANCE)
        {
            std::cout << "Somente o mecânico poderá fechar uma ordem de serviço sob o estado de manutenção...\n";
            press_anything_to_continue();
            return;
        }
        
        std::cout << "Sobre a SO de id \"" << so.id << "\": deseja realmente cancelá-la? ";
        clean_stdin();

        if (input_verification())
        {
            if (SellerInspectSO::so_manager->close_order(so.id, &so)) {
                SellerInspectSO::feedback_err_buffer +=
                    "A ordem pôde ser fechada com sucesso...";
                positive_highlight = true;
                altered = true;
            }
            else {
                SellerInspectSO::feedback_err_buffer +=
                    "A ordem não pôde ser fechada com sucesso...";
            }
        }
    }

    int process(void) override {
        char command_buffer;
        std::cin >> command_buffer;

        feedback_err_buffer.clear();
        positive_highlight = false;

        switch (command_buffer)
        {
        case 'q':
            SellerInspectSO::menu_loop = false;
            break;
        
        case 'c':
            SellerInspectSO::close_so();
            break;

        case 'i':
            SellerInspectSO::advance_so();
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

    inline struct ServiceOrder get_so(void)
    {
        return so;
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
        std::cout << "\ts:\tProximo item\n";
        std::cout << "\ta:\tPagina anterior\n";
        std::cout << "\td:\tProxima pagina\n";
        std::cout << "\ti:\tInspecionar ordem\n";
        
        std::cout << "\tf:\t";

        switch (cat) {
        case 0:
            std::cout << "Filtrar por ordens abertas";
            break;

        case 1:
            std::cout << "Filtrar por ordens orcamentadas";
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
            std::cerr << "Falha em associar cliente a nova SO.\n";
            press_anything_to_continue();
            return;
        }

        std::cout << "Qual e o seu problema?\t";

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
            std::cout << "falhou...\n";
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

    void inspect(void) override {
        struct ServiceOrder so;
        SO_Vizualizer::get_service_order(so);
        
        SellerInspectSO inspect_so(so_manager, so);

        if (inspect_so.interact() == 0)
            return;

        struct ServiceOrder _so_buffer = inspect_so.get_so();
        (* std::next(SOs.begin(), focus_index + page_index * page_size)) = _so_buffer;

        const size_t sos_size = SOs.size();
        if ((sos_size > 0) && (focus_index > 0) && (focus_index == (sos_size - 1)))
            focus_index --;

        if (so.stage != SO_ALL)
            SOs.erase(std::next(SOs.begin(), SO_Vizualizer::focus_index));
        SO_Vizualizer::reload_page();
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
};

class ClientSO_HistoryVizualizer : virtual public SO_Vizualizer {
private:
    struct ClientData client_data;

    void render_footer(void) const override {
        print_n_char('\n', 2);
        print_n_char('-', 10); putchar('\n');
        std::cout << "\tw:\tItem anterior\n";
        std::cout << "\ts:\tProximo item\n";
        std::cout << "\ta:\tPagina anterior\n";
        std::cout << "\td:\tProxima pagina\n";
        std::cout << "\tq:\tSair do visualizador\n";
        print_n_char('=', 50);
        print_n_char('\n', 2);
    }
    
    void inspect(void) override {
        
    }

public:
    ClientSO_HistoryVizualizer(class SO_Manager * const so_manager,
        const struct ClientData & client_data) :
        SO_Vizualizer(so_manager) {
        
        set_category(SO_UNDEF);

        SO_Vizualizer::SOs = so_manager->so_client(client_data);
        strcpy(SO_Vizualizer::base_title_name, "Histórico de SOs de Cliente");
    }

    ~ClientSO_HistoryVizualizer(void) {

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

int Seller::render(void) const
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
        std::cout << "\tVeiculo\t|\ttipo: " << client_buffer.vehicle.type << ", modelo: "
            << client_buffer.vehicle.model << ", kilometragem: "
            << client_buffer.vehicle.mileage << std::endl;
        std::cout << "\tdata de registro: ";
        std::cout << client_buffer.registry_date << std::endl << std::endl;

    }
    else {
        printf("(cliente nao carregado...)\n\n");
    }

    /*  footer: interaction guide */
    constexpr const char * footer_title = "O que desejas fazer?";
    print_n_char('-', literal_string_length(footer_title) - 1);
    std::cout << std::endl << footer_title << std::endl;
    std::cout << "1\t->\tSair" << std::endl;
    std::cout << "2\t->\tRegistrar um cliente" << std::endl;
    std::cout << "3\t->\tCarregar um cliente" << std::endl;
    std::cout << "4\t->\tVizualizer histórico do cliente" << std::endl;
    std::cout << "5\t->\tNavegar sobre ordens de servico" << std::endl;

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
        Seller::show_so_history();
        break;

    case 5:
        Seller::manage_sos();
        break;

    default:
        break;
    }
    return 0;
}

void Seller::show_so_history(void) {
    if (! Seller::client_is_loaded)
    {
        std::cout << "É necessário, para tanto, carregar um cliente primeiramente...\n";
        std::cout << "Deseja carregar um cliente? ";
        if (input_verification()) {
            clean_stdin();
            Seller::load_client_interface();
        }
        
        return;
    }

    ClientSO_HistoryVizualizer history_vizualizer(so_manager, client_buffer);
    history_vizualizer.interact();
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
    std::cout << "Qual é o tipo do ve�culo (Ex: carro, moto, etc...)?\t";
    std::cin.getline(client_buffer.vehicle.type, NAME_SIZE);
    
    /*  vehicle's model */
    std::cout << "Qual é o modelo do ve�culo?\t\t\t\t";
    std::cin.getline(client_buffer.vehicle.model, NAME_SIZE);

    /*  vehicle's mileage */
    std::cout << "Qual é a quilometragem do seu veículo?\t\t\t";
    do {
        std::cin >> client_buffer.vehicle.mileage;
        clean_stdin();
        
    } while ((client_buffer.vehicle.mileage < 0) && (std::cout << "Entrada inv�lida para kilometragem. Redigite-a:\t\t"));


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
            std::cout << "Nao foi possivel carregar o cliente.\n";
            MenuScreen::press_anything_to_continue();
            return false;
        }
    }
    client_buffer = this->client_buffer;
    return true;
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

        printf("\t\tVeiculo #%llu/%llu:\n", (unsigned long long) (focus_index + 1), (unsigned long long) all_client_data.size());
        printf("\t\tmodelo: %s, tipo: %s, kilometragem: %llu\n",
            the_client_data.vehicle.model, the_client_data.vehicle.type,
            (unsigned long long) the_client_data.vehicle.mileage);

        printf("\n\na: Veiculo anterior\n");
        printf("d: Veiculo posterior\n");
        printf("q: Sair (confirmar selecao)\n");

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
    if (Seller::client_is_loaded)
        client_buffer = all_client_data.front();

    return Seller::client_is_loaded;
}

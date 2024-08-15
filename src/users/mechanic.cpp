/*	<src/users/mechanic.cpp>

	(...) */


#include "mechanic.hpp"
#include "../ui/db-vizualizers.hpp"


class InspectBudgetSO : virtual public MenuScreen
{
private:
	class SO_Manager * so_manager;
	struct ServiceOrder so;

	bool menu_loop = true;
	bool altered = false;
	std::string feedback_err_buffer;

	int render(void) const override {
		clean_screen();

		// header
		print_n_char('=', width); putchar('\n');
		std::cout << "\t\tBudget SO Editor" << std::endl;
		print_n_char('=', width); putchar('\n');

		// so information
		printf("\n\n");
		printf("so: ");
		std::cout << so << std::endl;
		
		// footer
		print_n_char('\n', 2);
		print_n_char('-', 50); putchar('\n');

		if (so.stage == SO_OPEN)
			std::cout << "\tb:\tOrçamentar ordem\n";

		std::cout << "\tq:\tSair da inspeção\n";
		print_n_char('=', 50);
		print_n_char('\n', 2);

		return 0;
	}

	void budget(void)
	{
		/*	the input won't match anymore if the so is once updated... */
		if (so.stage != SO_OPEN)
			return;

		struct PartsBudget parts;
		parts.n_pieces = 0;

		int piece_code;

		std::cout << "Entre com os códigos dos componentes (0 à " << PIECE_MAX << ", inclusivo) - digite(-1) para interromper entrada.\n";

		while (parts.n_pieces < MAX_PIECES) {
			std::cout << "Código do #" << parts.n_pieces + 1 << " componente: " << std::endl;
			std::cin >> piece_code;
			std::cin.clear();
			std::cin.ignore(INT64_T_MAX, '\n');

			if ((piece_code < 0) || (piece_code > PIECE_MAX))
				break;

			parts.pieces[parts.n_pieces ++] = static_cast<PIECE_ID> (piece_code);
		}

		std::cout << "Tem certeza de que deseja fazer um orcamento SO <" << so.id << ">? ";
		if (! input_verification())
			return;

		if (! so_manager->budget_order(so.id, parts, &so))
		{
			std::cerr << "Nao conseguiu orçar...\n";
			press_anything_to_continue();
			return;
		}

		InspectBudgetSO::menu_loop = false;
		InspectBudgetSO::altered = true;
	}

	int process(void) override {
		char command_buffer;
		std::cin >> command_buffer;

		switch (command_buffer)
		{
		case 'b':	// Budget
			InspectBudgetSO::budget();
			break;

		case 'q':	// Quit of the inspection
			InspectBudgetSO::menu_loop = false;
			break;

		default:
			break;
		}
		return 0;
	}

public:
	InspectBudgetSO(class SO_Manager * const so_manager, struct ServiceOrder & so) :
		MenuScreen(), so_manager(so_manager), so(so)
	{
		if (so_manager == nullptr)
			throw std::runtime_error("AEIOU");
	}

	virtual ~InspectBudgetSO(void) {

	}

	int interact(void) {

		/*	creating the event loop */
		while (InspectBudgetSO::menu_loop)
		{
			/*	rendering the screen */
			InspectBudgetSO::render();

			/*	processing the input */
			InspectBudgetSO::process();
		}
		InspectBudgetSO::menu_loop = true;

		if (InspectBudgetSO::altered)
			return 1;
		return 0;
	}
};

class BudgetSO_Editor : virtual public SO_Vizualizer {
public:
	BudgetSO_Editor(class SO_Manager * const so_manager) :
		SO_Vizualizer(so_manager)
	{
		SO_Vizualizer::set_category(SO_OPEN);
	}

	void inspect(void) override {

		struct ServiceOrder so;
		SO_Vizualizer::get_service_order(so);

		InspectBudgetSO inspect_budget(so_manager, so);
		if (inspect_budget.interact() == 0)
			return;

		if ((focus_index > 0) && (focus_index == (SOs.size() - 1)))
			focus_index --;

		SOs.erase(std::next(SOs.begin(), SO_Vizualizer::focus_index));
		SO_Vizualizer::reload_page();
	}
};

class InspectMaintenanceSO : virtual public MenuScreen
{
private:
	class SO_Manager * so_manager;
	struct ServiceOrder so;

	bool menu_loop = true;
	bool altered = false;
	std::string feedback_err_buffer;

	int render(void) const override {
		clean_screen();

		// header
		constexpr const char * title = "Inspeção de SOs sob manutenção";
		std::cout << title << std::endl;
		print_n_char('-', literal_string_length(title));
		
		// so information
		printf("\n\n\n");
		std::cout << so;

		// footer
		print_n_char('\n', 2);
		print_n_char('-', 10); putchar('\n');

		if (so.stage == SO_MAINTENANCE)
			std::cout << "\tc:\tConcluir manutenção da SO\n";

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
			InspectMaintenanceSO::menu_loop = false;
			break;

		case 'c':
			InspectMaintenanceSO::close();
			break;

		default:
			break;
		}
		return 0;
	}

	void close(void) 
	{
		/*	the key won't respond anymore in case of change of states... */
		if (so.stage != SO_MAINTENANCE)
			return;
		
		std::cout << "Deseja realmente fechar a so #" << so.id << " ? ";
		if (! input_verification())
			return;

		if (! so_manager->close_order(so.id, &so))
		{
			std::cerr << "Por algum erro no sistema, a ordem #" << so.id << " não pôde ser atualizada apropriadamente..."
				<< std::endl;
		}
		else {
			std::cout << "Ordem concluída com sucesso." << std::endl;
			altered = true;
		}
		press_anything_to_continue();
	}

public:
	InspectMaintenanceSO(class SO_Manager * const so_manager, struct ServiceOrder & so) :
		MenuScreen(), so_manager(so_manager), so(so)
	{
		if (so_manager == nullptr)
			throw std::runtime_error("AEIOU");
	}

	virtual ~InspectMaintenanceSO(void) {

	}

	int interact(void) {

		/*	creating the event loop */
		while (InspectMaintenanceSO::menu_loop)
		{
			/*	rendering the screen */
			InspectMaintenanceSO::render();

			/*	processing the input */
			InspectMaintenanceSO::process();
		}
		InspectMaintenanceSO::menu_loop = true;

		if (InspectMaintenanceSO::altered)
			return 1;
		return 0;
	}
};

class MaintenanceSO_Editor : virtual public SO_Vizualizer {
public:
	MaintenanceSO_Editor(class SO_Manager * const so_manager) : SO_Vizualizer(so_manager)
	{
		SO_Vizualizer::set_category(SO_MAINTENANCE);
	}

	void inspect(void) override {
		struct ServiceOrder so;
		SO_Vizualizer::get_service_order(so);

		InspectMaintenanceSO inspect_budget(so_manager, so);

		if (inspect_budget.interact() == 0)
			return;

		if ((focus_index > 0) && (focus_index == (SOs.size() - 1)))
			focus_index --;

		SOs.erase(std::next(SOs.begin(), SO_Vizualizer::focus_index));
		SO_Vizualizer::reload_page();
	}
};


Mechanic::Mechanic(class SO_Manager * const so_manager, class UsersDatabase * const users_db,
	const struct MinimalUserData & user_data) : UserScreen(so_manager, users_db, user_data) {
	
	constexpr const char * title = "Menu de Mecânico";
	UserScreen::menu_title = const_cast<char *> (title);
}

Mechanic::~Mechanic(void) {

}

void Mechanic::budget(void)
{
	BudgetSO_Editor budget_so_editor(so_manager);
	budget_so_editor.interact();
}

void Mechanic::maintenance(void)
{
	MaintenanceSO_Editor maintenance_so_editor(so_manager);
	maintenance_so_editor.interact();
}

int Mechanic::render(void) const
{
	clean_screen();

	/*  header */
	UserScreen::render_menu_header();

	/*  footer: interaction guide */
	constexpr const char * footer_title = "O que desejas fazer?";
	print_n_char('-', literal_string_length(footer_title) - 1);
	std::cout << std::endl << footer_title << std::endl;
	std::cout << "1\t->\tSair" << std::endl;
	std::cout << "2\t->\tFazer orçamento de uma SO" << std::endl;
	std::cout << "3\t->\tConcluir a manutenção de uma SO" << std::endl;

	fflush(stdout);
	return 0;
}

int Mechanic::process(void)
{
	int opcao;
	std::cin >> opcao;
	clean_stdin();

	switch (opcao) {
	case 1: // Exiting the menu
		UserScreen::main_loop = false;
		break;

	case 2: // Registering a new client
		Mechanic::budget();
		break;

	case 3:
		Mechanic::maintenance();
		break;

	default:
		break;
	}
	return 0;
}


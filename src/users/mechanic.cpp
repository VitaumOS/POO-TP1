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

	int render(void) override {
		clean_screen();

		// header
		std::cout << "==================================================" << std::endl;
		std::cout << "\t\tBudget SO Editor" << std::endl;
		std::cout << "==================================================" << std::endl;

		// so information
		printf("\n\n");
		printf("so: ");
		std::cout << so << std::endl;
		
		// footer
		print_n_char('\n', 2);
		print_n_char('-', 50); putchar('\n');
		std::cout << "\tb:\tOrcar\n";
		std::cout << "\tq:\tSair da inspecao\n";
		print_n_char('=', 50);
		print_n_char('\n', 2);

		return 0;
	}

	void budget(void)
	{
		if (so.stage != SO_OPEN)
		{
			std::cout << "A SO nao esta aberta...\n";
			press_anything_to_continue();
			return;
		}

		struct PartsBudget parts;
		parts.n_pieces = 0;

		int piece_code;

		std::cout << "Entre com os c�digos dos componentes - digite (-1) para sair.\n";

		while (parts.n_pieces < MAX_PIECES) {
			std::cout << "C�digo do #" << parts.n_pieces + 1 << " componente: " << std::endl;
			std::cin >> piece_code;
			std::cin.clear();
			std::cin.ignore(INT64_T_MAX, '\n');

			if ((piece_code < 0) || (piece_code > PIECE_MAX))
				break;

			parts.pieces[parts.n_pieces ++] = static_cast<PIECE_ID> (piece_code);
		}
		// std::cout << "Deseja adicionar a pe�a de c�digo <" << piece_code << "> ao or�amento? ";

		std::cout << "Tem certeza de que deseja fazer um orcamento SO <" << so.id << ">? ";
		if (! input_verification())
			return;

		if (! so_manager->budget_order(so.id, parts, &so))
		{
			std::cerr << "Nao conseguiu orcar...\n";
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

	int render(void) override {
		clean_screen();

		// header
		std::cout << "Editor de <Manutencao SO>" << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		// user information
		printf("\n\n\n");
		printf("so: ");

		// footer
		print_n_char('\n', 2);
		print_n_char('-', 10); putchar('\n');

		std::cout << "\tq:\tSair da inspecao\n";
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

		default:
			break;
		}
		return 0;
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

		InspectBudgetSO inspect_budget(so_manager, so);

		if (inspect_budget.interact() == 0)
			return;

		if ((focus_index > 0) && (focus_index == (SOs.size() - 1)))
			focus_index --;

		SOs.erase(std::next(SOs.begin(), SO_Vizualizer::focus_index));
		SO_Vizualizer::reload_page();
	}
};



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



/*	<src/ui/db-vizualizers.cpp>

	Defines the objects responsible for the vizualization and
	direct interactions with the databases. */


#include "db-vizualizers.hpp"


DatabaseInterface::DatabaseInterface(void) {

}

DatabaseInterface::~DatabaseInterface(void) {

}

void DatabaseInterface::event_n_key(void) { }
void DatabaseInterface::event_f_key(void) { }

void DatabaseInterface::process_io(void)
{
	char cstring_buffer[16];

	std::cout << cmd_output_buffer;
	std::cout << ": ";
	std::cin >> cmd_input_buffer;

	cmd_output_buffer.clear();
	cmd_output_buffer += "<" + cmd_input_buffer + ">\n";
	for (char command : cmd_input_buffer) {

		switch (command)
		{
		case 'a':
			if (page_index > 0) {
				-- page_index;
				load_page();
			}
			else {
				cmd_output_buffer += "* Nao ha pagina anterior para mostrar.\n";
			}
			break;

		case 'd':
			if (page_index < page_index_max) {
				++ page_index;
				load_page();

				if (focus_index >= last_qtt_read)
				{
					focus_index = last_qtt_read - 1;
				}

			}
			else {
				cmd_output_buffer += "* Nao ha pagina posterior para mostrar.\n";
			}
			break;

		case 'w':
			if (focus_index > 0) {
				focus_index --;
			}
			else {
				if (page_index > 0) {
					-- page_index;
					load_page();
					focus_index = page_size - 1;
				}
			}
			break;

		case 's':
			++ focus_index;
			if (focus_index >= page_size) {
				if (page_index < page_index_max) {
					page_index ++;
					load_page();
				}
				focus_index = 0;
			}
			else if (focus_index >= last_qtt_read)
			{
				-- focus_index;
			}
			break;

		case 'i':
			if (last_qtt_read > 0)
				inspect();
			break;

		case 'q':
			running_menu = false;
			break;
		
		case 'n':
			event_n_key();
			break;

		case 'f':
			event_f_key();
			break;

		default:
			sprintf(cstring_buffer, "%c", command);
			cmd_output_buffer += "* \"" + std::string(cstring_buffer) + "\" nao e reconhecido.\n";
			break;
		}
	}
}

void DatabaseInterface::render_screen(void) const {
	clean_screen();

	render_header();	/*  rendering the header */
	render_page();		/*  rendering the body */
	render_footer();	/*	rendering the footer */

	fflush(stdout);
}

void DatabaseInterface::load_page(void) {

}

void DatabaseInterface::reload_page(void) {
	/*	forcing page re-loading... */
	last_page_loaded ++; 
	load_page();
}

int DatabaseInterface::interact(void)
{
	/*	Cleaning IO buffers. */
	cmd_input_buffer.clear();
	cmd_output_buffer.clear();

	/*	pre-loading the initial page. */
	load_page();

	while (DatabaseInterface::running_menu) {
		render_screen();	/*	rendering */
		process_io();		/*	input */
	}
	DatabaseInterface::running_menu = true;

	clean_screen();
	return 0;
}

void DatabaseInterface::render_header(void) const {
	printf("\n\tHEADER\n\n\n\n\n");
}

void DatabaseInterface::render_footer(void) const {
	print_n_char('\n', 2);
	print_n_char('-', 160); putchar('\n');
	std::cout << "\tw:\tItem anterior\n";
	std::cout << "\ts:\tProximo item\n";
	std::cout << "\ta:\tPagina anterior\n";
	std::cout << "\td:\tProxima pagina\n";
	std::cout << "\ti:\tInspecionar elemento\n";
	std::cout << "\tq:\tSair do visualizador\n";
	print_n_char('=', 160);
	print_n_char('\n', 2);

}

void DatabaseInterface::render_page(void) const {
	print_n_char('\n', 10);
	printf("PAGI EM BRANCO!\n");
	print_n_char('\n', 10);
}

void DatabaseInterface::color_selected(void) const {
	aec_bg_rgb(239, 203, 104);
	aec_fg_rgb(22, 12, 40);
}

void DatabaseInterface::inspect(void) {
	std::cout << "NAO ESTA IMPLEMENTADO AINDA!\n";
	char c;
	std::cin >> c;
}




/*	SO_Vizualizer *
 *	============= */

SO_Vizualizer::SO_Vizualizer(SO_Manager * const so_manager) : so_manager(so_manager), vpage(page_size) {
	if (so_manager == nullptr)
		throw std::runtime_error("Invalido SO_Manager para SO_Vizualizer.");
}

SO_Vizualizer::~SO_Vizualizer(void) {

}

void SO_Vizualizer::load_page(void)
{
	/*	the page buffer is only update whenever it urges to change */
	if (last_page_loaded == page_index)
		return;

	last_page_loaded = page_index;

	const size_t index = page_index * page_size;
	last_qtt_read = page_size;
	if ((index + page_size) > SOs.size())
		last_qtt_read = SOs.size() - index;
	
	size_t iterator = 0;
	std::list<struct ServiceOrder>::const_iterator list_ptr = std::next(SOs.begin(), index);

	/*	erasing the previous content. */
	vpage.clear();

	struct ServiceOrder so;
	for (; iterator < last_qtt_read; iterator ++)
	{
		so = static_cast<struct ServiceOrder> (* list_ptr);
		vpage.push_back(so);
		list_ptr = std::next(list_ptr, 1);
	}
}

void SO_Vizualizer::set_category(SERVICE_ORDER_STAGE category) {
	this->category = category;
	
	SOs = so_manager->so_category(category);
	page_index_max = SOs.size() / page_size;
}

inline void SO_Vizualizer::represent_so(const struct ServiceOrder & _SO) const {
	switch (_SO.stage) {
	case SO_OPEN:			fprintf(stdout, "%-4s", "OPEN"); break;
	case SO_BUDGET:			fprintf(stdout, "%-4s", "BDGT"); break;
	case SO_MAINTENANCE:	fprintf(stdout, "%-4s", "MNTC"); break;
	case SO_CLOSED:			fprintf(stdout, "%-4s", "CLOS"); break;
	case SO_CANCELED:		fprintf(stdout, "%-4s", "CANC"); break;
	case SO_CLOSED_BUDGET:	fprintf(stdout, "%-4s", "CBUD"); break;
	default:				fprintf(stdout, "%-4s", "UNKW"); break;
	}

	fprintf(stdout, "\t[%06llu:%02d]\t",
		_SO.client_id.person_id, _SO.client_id.vehicle_id);

	fprintf(stdout, "R$%05.2lf R$%06.2lf",
		((double) _SO.hardware_price) / ((double) 100.0), ((double) _SO.labor_price) / ((double) 100.0));

	// Dates
	fprintf(stdout, "\t+ ");
	fprint_date(stdout, _SO.creation_date);
	fprintf(stdout, "\t* ");
	fprint_date(stdout, _SO.update_date);
	fprintf(stdout, "\t");
}

void SO_Vizualizer::render_header(void) const
{
	constexpr const char * menu_base_title = "SO vizualizer: ";
	constexpr size_t title_length = literal_string_length(menu_base_title);

	print_n_char('=', 160); putchar('\n');

	printf("%s ", menu_base_title);
	switch (category) {
	case SO_OPEN:				printf("Abrir SOs\n");									break;
	case SO_BUDGET:				printf("Orcar SOs\n");									break;
	case SO_MAINTENANCE:		printf("Manutencao SOs\n");								break;
	case SO_CLOSED:				printf("Fechar os SOs (em manutencao)\n");				break;
	case SO_CANCELED:			printf("SOs canceladas (fechado depois abrir)\n");		break;
	case SO_CLOSED_BUDGET:		printf("SOs fechados apos ja terem sidos orcados\n");	break;
	case SO_ALL:				printf("Todos SOs\n");									break;
	default:					printf("DESCONHECIDO\n");								break;
	}

	print_n_char('*', title_length - 1); putchar('\n');

	fflush(stdout);
}

void SO_Vizualizer::render_footer(void) const
{
	print_n_char('\n', 2);
	print_n_char('-', 160); putchar('\n');
	std::cout << "\tw:\tItem anterior\n";
	std::cout << "\ts:\tProximo item\n";
	std::cout << "\ta:\tPagina anterior\n";
	std::cout << "\td:\tProxima pagina\n";
	std::cout << "\ti:\tInspecionar elemento\n";
	std::cout << "\tq:\tSair do visualizador\n";
	print_n_char('=', 160);
	print_n_char('\n', 2);

	fflush(stdout);
}

void SO_Vizualizer::render_page(void) const
{
	print_n_char('\n', 2);

	printf("Page: #%03llu\n", page_index);
	fprintf(stdout, "SO-ID\tESTADO\tCLIENT-ID\tHRD\t LBR\t\tDATA DE CRIACAO\t\tDATA UPADA\t\n");
	
	const size_t index = page_index * page_size;
	size_t iterator_limit = page_size;
	if ((index + page_size) > SOs.size())
		iterator_limit = SOs.size() - index;

	size_t iterator = 0;
	for (struct ServiceOrder so : SO_Vizualizer::vpage)
	{
		if (iterator == focus_index)
		{
			DatabaseInterface::color_selected();
			fprintf(stdout, "[%03llu]\t", so.id);

			SO_Vizualizer::represent_so(so);

			aec_reset();
			putchar('\n');
		}
		else {
			fprintf(stdout, "[%03llu]\t", so.id);

			SO_Vizualizer::represent_so(so);
			putchar('\n');
		}

		iterator ++;
	}

	while ((iterator ++) < page_size) {
		fprintf(stdout, "[---] ");
		print_n_char('-', 3);
		printf(" / * / ");
		print_n_char('-', 3);
		putchar('\n');
	}
}

void SO_Vizualizer::print_so(const struct ServiceOrder & so) const 
{
	std::cout << "SO:\t";
	switch (so.stage)
	{
	case SO_OPEN:			std::cout << "aberto";					break;
	case SO_BUDGET:			std::cout << "orcado";					break;
	case SO_MAINTENANCE:	std::cout << "manutencao";				break;
	case SO_CLOSED:			std::cout << "fechado";					break;
	case SO_CANCELED:		std::cout << "cancelado";				break;
	case SO_CLOSED_BUDGET:	std::cout << "fechado apos orcamento";	break;
	default:				std::cout << "estado desconhecido";		break;
	}
	std::cout << " order\n";

	printf("\tid: %019llu\n", (unsigned long long) so.id);
	printf("\tid do cliente:\n");
	printf("\tpessoa:\t %llu\n", (unsigned long long) so.client_id.person_id);
	printf("\tveiculo:\t %llu\n", (unsigned long long) so.client_id.vehicle_id);
	printf("\nproblema: %-64s\n", so.issue_description);
	// printf("address: %-64s\n", so.budget_description);
	printf("\thardware-budget: ");
	std::cout << so.hardware_price;
	printf("\tlabor-budget: ");
	std::cout << so.labor_price;
	printf("\n\n\tcdata de cria��o: ");
	std::cout << so.creation_date;
	printf("\n\tdata upada: ");
	std::cout << so.update_date;
	printf("\n\n");
}

void SO_Vizualizer::get_service_order(struct ServiceOrder & so) const {
	so = SO_Vizualizer::vpage[SO_Vizualizer::focus_index];
}

void SO_Vizualizer::inspect(void)
{
	aec_clean();

	struct ServiceOrder so = vpage[focus_index];
	SO_Vizualizer::print_so(so);

	printf("NAO ESTA IMPLEMENTADO AINDA\n(DIGITE QUALQUER COISA PARA SAIR)\n");
	char c;
	std::cin >> c;
}


/*	UsersEditor *
 *	=========== */

class InspectUser : virtual public MenuScreen
{
private:
	class UsersEditor * users_editor;
	struct UserData user_data;

	bool menu_loop = true;
	std::string feedback_err_buffer;
	
	bool show_password = false;

	int render(void) override {
		clean_screen();

		// header
		std::cout << "==================================================" << std::endl;
		std::cout << "\t\tInspecionar usuario" 				  				  << std::endl;
		std::cout << "--------------------------------------------------" << std::endl;

		// user information
		printf("\n\n\n");
		printf("usuario: ");

		if (! user_data.active)
		{
			aec_fg_rgb(250, 150, 150);
			printf("%s", user_data.username);
			aec_reset();
		}
		else {
			printf("%s", user_data.username);
		}

		printf("\nsenha: ");
		printf("%s\n", show_password ? user_data.password : "***");

		// footer
		print_n_char('\n', 2);
		print_n_char('-', 50); putchar('\n');
		std::cout << "\tu:\tmudar o nome de usuario\n";
		std::cout << "\tp:\tmudar a senha\n";
		std::cout << "\td:\tDeletar usuario\n";
		std::cout << "\tv:\tExibir senha\n";
		std::cout << "\tq:\tSair da inspecao\n";
		print_n_char('=', 50);
		print_n_char('\n', 2);

		return 0;
	}

	inline void toggle_show_password(void) { show_password = ! show_password; }

	void change_username(void)
	{
		std::cout << "\nRenomeando <" << user_data.username << ">." << std::endl;

		if (! user_data.active)
		{
			std::cout << "Voce nao pode alterar o nome de usuario de um usuario inativo no banco de dados...\n";
			press_anything_to_continue();
			return;
		}

		std::string new_username_buffer;
		bool c_username_loop = true;

		while (c_username_loop)
		{
			std::cout << "Digite o novo nome de usuario: ";
			std::cin >> new_username_buffer;
			new_username_buffer.resize(username_string_length);

			clean_stdin();

			/*	checking if it is attempting to change to the same... */
			if (! strcmp(new_username_buffer.c_str(), user_data.username))
			{
				std::cout << "O nome de usuario ja esta <" << user_data.username << ">..." << std::endl;
				press_anything_to_continue();
				continue;
			}

			/*	checking if the user already exists on the databse... */
			if (InspectUser::users_editor->fetch_username(new_username_buffer.c_str()))
			{
				std::cout << "Ja existe um usuario com o nome de usuario <" <<  new_username_buffer << ">." << std::endl;
				std::cout << "Deseja cancelar a renomeacao? ";
				if (input_verification())
					c_username_loop = false;

				continue;
			}

			std::cout << "Voce tem certeza que quer mudar <" << user_data.username << "> to <" << new_username_buffer << ">? ";
			if (input_verification()) {
				struct UserData new_user_data = InspectUser::user_data;
				strcpy(new_user_data.username, new_username_buffer.c_str());

				if (! users_editor->update_userdata(new_user_data)) {
					std::cerr << "NAO FOI POSSIVEL ATUALIZAR OS DADOS DO USUARIO!!!!\n";
					
					char c;
					std::cout << "PRESSIONE QUALQUER COISA PARA SAIR\n";
					std::cin >> c;
				}
				else InspectUser::user_data = new_user_data;

				c_username_loop = false;
				
			} else {
				std::cout << "Cancelar alteracao de nome de usuario? ";
				if (input_verification()) {
					c_username_loop = false;
					break;
				}
			}
		}
	}

	void change_password(void)
	{
		std::cout << "\nMudar a senha para <" << user_data.username << ">." << std::endl;

		if (! user_data.active)
		{
			std::cout << "Voce nao pode alterar a senha de um usuario inativo no banco de dados...\n";
			press_anything_to_continue();
			return;
		}

		std::string new_password_buffer;
		std::string confirmation_password_buffer;

		bool c_password_loop = true;
		while (c_password_loop)
		{
			std::cout << "Digite a nova senha: ";
			std::cin >> new_password_buffer;
			new_password_buffer.resize(password_string_length);

			std::cin.clear();
			std::cin.ignore(INT64_T_MAX, '\n');

			std::cout << "Confirme a senha: ";
			std::cin >> confirmation_password_buffer;
			confirmation_password_buffer.resize(password_string_length);

			std::cin.clear();
			std::cin.ignore(INT64_T_MAX, '\n');

			if (new_password_buffer != confirmation_password_buffer)
			{
				std::cout << "As duas senhas passadas nao correspondem...\n";
				continue;
			}

			std::cout << "Voce tem certeza que deseja mudar a senha? ";
			if (input_verification())
			{
				struct UserData new_user_data = InspectUser::user_data;
				strcpy(new_user_data.password, new_password_buffer.c_str());

				if (! users_editor->update_userdata(new_user_data)) {
					std::cerr << "NAO FOI POSSEVEL ATUALIZAR OS DADOS DO USUARIO!!!!\n";
					press_anything_to_continue();
				}
				else InspectUser::user_data = new_user_data;

				c_password_loop = false;
			}
			else
			{
				std::cout << "Cancelar a mudanca de senha? ";
				if (input_verification()) {
					c_password_loop = false;
					break;
				}
			}
		}
	}

	void delete_user(void)
	{
		if (user_data.id.id == 0)
		{
			std::cout << "Nao e possivel excluir a chave admin do banco de dados...\n";
			press_anything_to_continue();
			return;
		} 
		else if (user_data.id.id == users_editor->admin_data.id.id) 
		{
			std::cout << "Voce nao pode deletar a si mesmo...\n";
			press_anything_to_continue();
			return;
		}

		if (! user_data.active)
		{
			std::cout << "O usuario ja foi excluido do banco de dados...\n";
			press_anything_to_continue();
			return;
		}

		std::cout << "Tem certeza de que deseja excluir o usuario <" <<
			user_data.username << ">? ";
		if (! input_verification())
			return;

		user_data.active = false;
		if (! users_editor->update_userdata(user_data))
		{
			std::cerr << "Nao foi possavel excluir o usuario do banco de dados...\n";
			user_data.active = true;

			press_anything_to_continue();
		}
	}

	int process(void) override {
		char command_buffer;
		std::cin >> command_buffer;

		switch (command_buffer) {
		case 'u':	// Change the username
			InspectUser::change_username();
			break;

		case 'p':	// Change the password
			InspectUser::change_password();
			break;
		
		case 'd':	// Delete user
			InspectUser::delete_user();
			break;

		case 'q':	// Quit
			InspectUser::menu_loop = false;
			break;

		case 'v':	// Show / hide password
			InspectUser::toggle_show_password();
			break;

		default:
			break;
		}
		return 0;
	}

public:
	InspectUser(class UsersEditor * const users_editor_ptr, struct UserData & user_data) : 
		MenuScreen(), users_editor(users_editor_ptr), user_data(user_data) 
	{
		if (users_editor == nullptr)
			throw std::runtime_error("AEIOU");
	}

	virtual ~InspectUser(void) {

	}

	int interact(void) {

		/*	creating the event loop */
		while (InspectUser::menu_loop)
		{
			/*	rendering the screen */
			InspectUser::render();

			/*	processing the input */
			InspectUser::process();
		}
		InspectUser::menu_loop = true;
		return 0;
	}
};

UsersEditor::UsersEditor(class UsersDatabase * const users_db, const struct UserData & admin_data) : vpage(page_size), users_db(users_db), admin_data(admin_data) {
	if (users_db == nullptr)
		throw std::runtime_error("Objeto invalido <UsersDatabase> passado para <UsersEditor UI screen>...");

	if (admin_data.type != USER_TYPE_ADM)
		throw std::runtime_error("<UserData> invalido para <UsersEditor>...");

	UsersEditor::page_buffer = new struct UserData[UsersEditor::page_size];
	if (page_buffer == nullptr)
		throw std::runtime_error("UsersEditor nao conseguiu alocar buffer de pagina...\n");
}

UsersEditor::~UsersEditor(void) {
	if (page_buffer != nullptr)
		delete page_buffer;
}

bool UsersEditor::update_userdata(const struct UserData & new_user_data) {
	return users_db->update_userdata(new_user_data);
}

bool UsersEditor::fetch_username(const username_string_t username) const {
	return users_db->fetch_username(username) >= 0;
}

void UsersEditor::load_page(void)
{
	if (last_page_loaded == page_index)
		return;
	last_page_loaded = page_index;

	UsersEditor::page_buffer = new struct UserData[UsersEditor::page_size];
	if (page_buffer == nullptr)
		throw std::runtime_error("UsersEditor nao conseguiu alocar buffer de pagina...\n");
	
	if (fseek(users_db->stream, users_db->stream_header_size, SEEK_SET) == 0) {
		last_qtt_read = fread(UsersEditor::page_buffer, sizeof(struct UserData), page_size, users_db->stream);

	}
	else {

	}

	const size_t db_size = users_db->get_size();
	if (last_qtt_read > db_size)
		last_qtt_read = db_size;
	
	vpage.clear();
	for (size_t i = 0; i < last_qtt_read; ++i)
		vpage.push_back(UsersEditor::page_buffer[i]);
}

void UsersEditor::color_selected(void) const {
	aec_bg_rgb(239, 203, 104);
	aec_fg_rgb(22, 12, 40);
}

void UsersEditor::represent_userdata(const struct UserData & user_data) const {
	
	if (! user_data.active)
	{
		aec_fg_rgb(250, 150, 150);
	}
	
	switch (user_data.type)
	{
	case USER_TYPE_ADM: fprintf(stdout, "%-9s", "ADMIN");       break;
	case USER_TYPE_SLR: fprintf(stdout, "%-9s", "VENDEDOR");      break;
	case USER_TYPE_MCH: fprintf(stdout, "%-9s", "MECANICO");    break;
	default:		    fprintf(stdout, "%-9s", "DESCONHECIDO");      break;
	}

	fprintf(stdout, "\t[%019llu]\t", (unsigned long long) user_data.id.id);
	fprintf(stdout, "\t%-16s\t", user_data.username);

	// Dates
	fprintf(stdout, "\t+ ");
	fprint_date(stdout, user_data.registry_date);
	fprintf(stdout, "\t* ");
	fprint_date(stdout, user_data.last_login);
	fprintf(stdout, "\t");

	aec_reset();
}

void UsersEditor::render_header(void) const {
	constexpr const char * menu_base_title = "Visualizador de banco de dados de usu�rios";
	constexpr size_t title_length = literal_string_length(menu_base_title);

	print_n_char('=', 160);
	putchar('\n');
	putchar('\n');
}

void UsersEditor::render_footer(void) const {
	print_n_char('\n', 2);
	print_n_char('-', 160); putchar('\n');
	std::cout << "\tw:\tItem anterior\n";
	std::cout << "\ts:\tProximo item\n";
	std::cout << "\ta:\tPagina anterior\n";
	std::cout << "\td:\tProxima pagina\n";
	std::cout << "\ti:\tInspecionar elemento\n";
	std::cout << "\tn:\tRegistrar novo usuario\n";
	std::cout << "\tq:\tSair do visualizador\n";
	print_n_char('=', 160);
	print_n_char('\n', 2);
}

void UsersEditor::render_page(void) const {
	print_n_char('\n', 2);

	printf("Page: #%03llu\n", page_index);
	fprintf(stdout, "INDEX\tTIPO\t\tRELATIVE-ID\t\t\tNOME DE USUARIO\t\t\tDATA DE CRIACAO\t\tDATA UPADA\t\n");
	
	/*  current's page first index. */
	const size_t index = page_index * page_size;

	size_t iterator = 0;

	for (struct UserData ud : UsersEditor::vpage)
	{
		if (iterator == focus_index)
		{
			UsersEditor::color_selected();
			fprintf(stdout, "[%03llu]\t", index + iterator);

			UsersEditor::represent_userdata(ud);

			aec_reset();
			putchar('\n');

		}
		else {
			fprintf(stdout, "[%03llu]\t", index + iterator);

			UsersEditor::represent_userdata(ud);
			putchar('\n');
		}

		iterator ++;
	}

	while ((iterator ++) < UsersEditor::page_size) {
		fprintf(stdout, "[---] ");
		print_n_char('-', 3);
		printf(" / * / ");
		print_n_char('-', 3);
		putchar('\n');
	}
}

void UsersEditor::event_n_key(void) { UsersEditor::new_user(); }

void UsersEditor::inspect(void) 
{

	/*	gettting the user. */
	struct UserData the_user = vpage[focus_index % page_size];
	
	InspectUser iu_menu(this, the_user);
	iu_menu.interact();

	UsersEditor::reload_page();
}

void UsersEditor::new_user(void)
{
	std::string type_string_buffer;
	std::string username_string_buffer;
	std::string password_string_buffer;

	USER_TYPE the_user_type;

	bool new_user_loop = true;
	while (new_user_loop)
	{
		std::cout << "Qual e o tipo de usuario? ";
		std::cin >> type_string_buffer;
		std::cin.clear(); std::cin.ignore(INT64_T_MAX, '\n');

		if (type_string_buffer == "admin")
		{
			the_user_type = USER_TYPE_ADM;
		}
		else if (type_string_buffer == "vendedor")
		{
			the_user_type = USER_TYPE_SLR;
		}
		else if (type_string_buffer == "mecanico")
		{
			the_user_type = USER_TYPE_MCH;
		}
		else {
			std::cerr << "Irreconhecido tipo de usuario...";
			continue;
		}

		std::cout << "Nome de usuario: ";
		std::cin >> username_string_buffer;
		username_string_buffer.resize(username_string_length);

		std::cin.clear(); std::cin.ignore(INT64_T_MAX, '\n');

		std::cout << "Senha: ";
		std::cin >> password_string_buffer;
		password_string_buffer.resize(password_string_length);

		std::cin.clear(); std::cin.ignore(INT64_T_MAX, '\n');

		std::cout << "Voce deseja registrar um novo usuario <" << username_string_buffer << ">? ";
		if (! input_verification()) {
			std::cout << "Sair do registramento? ";
			if (input_verification()) {
				new_user_loop = false;
			}
			continue;
		}

		if (! users_db->register_user(the_user_type, username_string_buffer.c_str(), password_string_buffer.c_str()))
		{
			std::cerr << "FALHA AO REGISTRAR NOVO USUARIO!\n";
			press_anything_to_continue();
		}

		new_user_loop = false;
	}

	UsersEditor::reload_page();
}

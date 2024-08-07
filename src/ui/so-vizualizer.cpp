/*	<src/ui/so-vizualizer.cpp>

	(...) */


#include "so-vizualizer.hpp"


SO_Vizualizer::SO_Vizualizer(SO_Manager * so_manager) : so_manager(so_manager)
{
	if (so_manager == nullptr)
		throw std::runtime_error("Invalid SO_Manager to SO_Vizualizer.");
}

void SO_Vizualizer::set_category(SERVICE_ORDER_STAGE category)
{
	this->category = category;
}

int SO_Vizualizer::interact(void)
{
	/*	Cleaning IO buffers. */
	cmd_input_buffer.clear();
	cmd_output_buffer.clear();

	std::list<struct ServiceOrder> SOs = so_manager->so_category(category);
	const size_t vpage_index_max = SOs.size() / vpage_size;

	while (running_menu) {
		/*	Rendering: header */
		clean_screen();
		SO_Vizualizer::render_header();

		/*	Rendering: body */
		print_n_char('\n', 2);

		printf("Page: #%03llu\n", vpage_index);
		fprintf(stdout, "SO-ID\tSTATE\tCLIENT-ID\tHRD\t LBR\t\tCREATION DATE\t\tUPDATE DATE\t\n");
		vpage_item_qtt = SO_Vizualizer::print_lpage(SOs, vpage_index, focus_index);

		/*	Rendering: footer */
		SO_Vizualizer::render_footer();

		/*	Input / Output	*/
		SO_Vizualizer::process_io(vpage_index_max);
	}
	running_menu = true;

	clean_screen();
	return 0;
}

inline void SO_Vizualizer::repr_el(const struct ServiceOrder & _SO) const {
	switch (_SO.stage)
	{
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

size_t SO_Vizualizer::print_lpage(std::list<struct ServiceOrder> & the_list,
	size_t lpage_index, size_t focus_index) const
{
	const size_t index = lpage_index * vpage_size;
	size_t iterator_limit = vpage_size;
	if ((index + vpage_size) > the_list.size())
		iterator_limit = the_list.size() - index;

	size_t iterator = 0;
	std::list<struct ServiceOrder>::iterator list_ptr = std::next(the_list.begin(), index);

	struct ServiceOrder so;
	for (; iterator < iterator_limit; iterator ++)
	{
		if (iterator == focus_index)
		{
			so = static_cast<struct ServiceOrder> (* list_ptr);

			aec_bg_rgb(239, 203, 104);
			aec_fg_rgb(22, 12, 40);
			fprintf(stdout, "[%03llu]\t", so.id);

			SO_Vizualizer::repr_el(so);

			aec_reset();
			putchar('\n');

			list_ptr = std::next(list_ptr, 1);
		}
		else {
			so = static_cast<struct ServiceOrder>(* list_ptr);

			fprintf(stdout, "[%03llu]\t", so.id);

			SO_Vizualizer::repr_el(so);
			putchar('\n');

			list_ptr = std::next(list_ptr, 1);
		}
	}

	while ((iterator ++) < vpage_size) {
		fprintf(stdout, "[---] ");
		print_n_char('-', 3);
		printf(" / * / ");
		print_n_char('-', 3);
		putchar('\n');
	}

	return the_list.size() - index;
}

void SO_Vizualizer::render_header(void) const
{
	constexpr const char * menu_base_title = "SO vizualizer: ";
	constexpr size_t title_length = literal_string_length(menu_base_title);

	print_n_char('=', 50); putchar('\n');

	printf("%s ", menu_base_title);
	switch (category) {
	case SO_OPEN:				printf("Open SOs\n");							break;
	case SO_BUDGET:				printf("Budgeted SOs\n");						break;
	case SO_MAINTENANCE:		printf("Maintenanced SOs\n");					break;
	case SO_CLOSED:				printf("Closed SOs (by maintenance)\n");		break;
	case SO_CANCELED:			printf("Canceled SOs (closed after open)\n");	break;
	case SO_CLOSED_BUDGET:		printf("Closed SOs after being budgeted\n");	break;
	case SO_ALL:				printf("All SOs\n");							break;
	default:					printf("UNKOWN\n");								break;
	}

	print_n_char('*', title_length - 1); putchar('\n');

	fflush(stdout);
}

void SO_Vizualizer::render_footer(void) const
{
	print_n_char('\n', 2);
	print_n_char('-', 10); putchar('\n');
	std::cout << "\tw:\tPrevious item\n";
	std::cout << "\ts:\tNext item\n";
	std::cout << "\ta:\tPrevious page\n";
	std::cout << "\td:\tNext page\n";
	std::cout << "\ti:\tInspect SO\n";
	std::cout << "\tq:\tQuit vizualizer\n";
	print_n_char('=', 50);
	print_n_char('\n', 2);

	fflush(stdout);
}

void SO_Vizualizer::so_inspect(so_id_t id)
{
	aec_clean();

	printf("NOT YET IMPLEMENTED\n(ENTER ANYTHING TO LEAVE)\n");
	char c;
	std::cin >> c;
}

void SO_Vizualizer::process_io(const size_t & vpage_index_max)
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
			if (vpage_index > 0) {
				-- vpage_index;
			}
			else {
				cmd_output_buffer += "* There's no previous page to show.\n";
			}
			break;

		case 'd':
			if (vpage_index < vpage_index_max) {
				++ vpage_index;
			}
			else {
				cmd_output_buffer += "* There's no next page to show.\n";
			}
			break;

		case 'w':
			if (focus_index > 0) {
				focus_index --;
			}
			else {
				if (vpage_index > 0) {
					-- vpage_index;
					focus_index = vpage_size - 1;
				}
			}
			break;

		case 's':
			++ focus_index;
			if (focus_index >= vpage_size) {
				if (vpage_index < vpage_index_max) {
					vpage_index ++;
				}
				focus_index = 0;
			}
			else if (focus_index >= vpage_item_qtt)
			{
				-- focus_index;
			}
			break;

		case 'i':
			SO_Vizualizer::so_inspect(static_cast<so_id_t> (focus_index + vpage_index * vpage_size));
			break;

		case 'q':
			running_menu = false;
			break;

		default:
			sprintf(cstring_buffer, "%c", command);
			cmd_output_buffer += "* \"" + std::string(cstring_buffer) + "\" is not recognized.\n";
			break;
		}
	}
}


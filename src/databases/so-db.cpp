/*  <src/databases/so-db.cpp>

	Defines the service-orders (SOs) database. */

/*	Last update: 03/08/2024. */


#include "so-db.hpp"
#include <string.h>	// for strcpy, strcmp
#include "../headers/ui.hpp"
#include <stdexcept>
#include <assert.h>
#include <iterator>

struct SOM_stream_header {
	id_t item_qtt = 0;
	id_t first_active = 0;
};


SO_Manager::SO_Manager(void) : Database(SODB_filename, sizeof(first_active) + sizeof(item_qtt)), client_manager() {
	// stream_header_size += sizeof(first_active);

	if (! SO_Manager::retrieve_stream_header()) {

		if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0) {
			SO_Manager::reset_database();
		}
	}

	std::cout << "Initial SO-DB state:" << std::endl;
	print_database();
}

SO_Manager::~SO_Manager(void) {
	if (stream == nullptr)	
		return;

	std::cout << "Final SO-DB state:" << std::endl;
	print_database();

	if (! SO_Manager::update_stream_header())
		std::cerr << "Stream header couldn't be written at SO_Manager." << std::endl;

	finalize_stream();
}

bool SO_Manager::reset_database(void) {
	std::cerr << "Reseting SO database." << std::endl;

	item_qtt = 0;
	first_active = 0;
	return update_stream_header();
}

bool SO_Manager::retrieve_stream_header(void) {
	struct SOM_stream_header _stream_header;

	rewind(stream);
	if (fread(&_stream_header, sizeof(_stream_header), 1, stream) < 1)
		return false;

	item_qtt		= _stream_header.item_qtt;
	first_active	= _stream_header.first_active;

	return true;
}

bool SO_Manager::update_stream_header(void) const {
	struct SOM_stream_header _stream_header = {
		.item_qtt		= item_qtt,
		.first_active	= first_active
	};

	rewind(stream);
	return fwrite(&_stream_header, stream_header_size, 1, stream) > 0;
}

/*  Attempts creating a new service-order into the database.

	\param issue:       The SO description text.
	\param client_id:   The id of the client the order will be associated with.
	\param return_so:   (Return) In case of success, it will match the new SO.

	\return The function's return is the success in creating the SO. Fails in case of data inconsistency
	or IO problems. In case of success, <return_so> will be returned matching the new created SO.
	Its state is unchanged from input in case of failure. */
bool SO_Manager::new_order(const char issue[SO_DESCRIPTION_SIZE], const client_id_t & client_id, struct ServiceOrder * return_so)
{
	// Validating the issue-description:
	// The description has to be bigger enough.
	int i = 0;
	while (issue[i ++] && (i <= 4));
	if (i < 4)
	{
		fprintf(stderr, "[%s] (false): Issue description doesn't have even 4 valid characters (%d)...\n",
			__func__, i);
		return false;
	}

	// Validating the SO's client's ID.
	int64_t client_pos;
	if ((client_pos = client_manager.fetch_client_id(client_id)) < 0)
	{
		fprintf(stderr, "[%s] (false): Client's ID (%llu) is inexistent.\n", __func__, client_id.id);
		return false;
	}

	Date date_of_now;
	if (! get_date(date_of_now)) {
		fprintf(stderr, "[%s] (false): Couldn't get date.\n", __func__);
		return false;
	}

	struct ServiceOrder so = {
		.id = item_qtt,
		.stage = SO_OPEN,
		.client_id = client_id,
		.issue_description = "undef",
		.budget = {
			.n_pieces = 0,  .pieces = { (PIECE_ID) 0 }
			},

		.labor_price = 0,
		.hardware_price = 0,

		.creation_date = date_of_now,
		.update_date = date_of_now,
	};
	strcpy(so.issue_description, issue);

	if (write_element(item_qtt, &so))
	{
		fprintf(stderr, "[%s] Apparently could write element #%llu\n", __func__, item_qtt);
		++ item_qtt;
		// ++ item_qtt;
		* return_so = so;
		return true;
	}

	fprintf(stderr, "[%s] (false): Couldn't write new-order.\n", __func__);
	return false;
}

/*  Attempts budgeting an open service-order in the database.

	\param id:              The SO's id.
	\param parts_budget:    The budget of the vehicle's parts information to be added.
	\param return_so:		(Return) In case of success, it will match the newly update SO.

	\return The function's return is the success of updating the SO. Fails in case of data inconsistency
	or IO problems. In case of success, <return_so> will be returned matching the newly updated SO.
	Its state is unchanced from input in case of failure. */
bool SO_Manager::budget_order(const so_id_t id, const struct PartsBudget & parts_budget, struct ServiceOrder * return_so)
{
	// Validating the SO ID.
	if (id >= item_qtt)
		return false;

	// Validating the parts-budget structure.
	if (parts_budget.n_pieces > MAX_PIECES)	return false;
	for (uint8_t i = 0; i < parts_budget.n_pieces; ++i)
	{
		if (parts_budget.pieces[i] > LAST_PIECE_ID)
			return false;
	}

	// Retrieving the SO.
	ServiceOrder target_so;
	if (! read_element(id, &target_so))
		return false;

	// Validating tis currente stage.
	if (target_so.stage != SO_OPEN)
		return false;

	Date date_of_now;
	if (! get_date(date_of_now))
		return false;

	target_so.update_date = date_of_now;
	target_so.stage = SO_BUDGET;
	target_so.budget = parts_budget;
	target_so.hardware_price = sum_hardware_budget(parts_budget);
	target_so.labor_price = calculate_labor_price(target_so);

	if (write_element(id, &target_so))
	{
		* return_so = target_so;
		return true;
	}

	fprintf(stderr, "[%s] Couldn't update SO.\n", __func__);
	return false;
}

/*  Attempts opening a service-order in the database as over maintenance.

	\param id:              The SO's id.
	\param return_so:		(Return) In case of success, it will match the newly update SO.

	\return The function's return is the success of updating the SO. Fails in case of data inconsistency
	or IO problems. In case of success, <return_so> will be returned matching the newly updated SO.
	Its state is unchanced from input in case of failure. */
bool SO_Manager::operate_order(const so_id_t id, struct ServiceOrder * return_so)
{
	// Validating the SO ID.
	if (id >= item_qtt)
		return false;

	// Retrieving the SO.
	ServiceOrder target_so;
	if (! read_element(id, &target_so))
		return false;

	// Validating the currente stage.
	if (target_so.stage != SO_BUDGET)
		return false;

	Date date_of_now;
	if (! get_date(date_of_now))
		return false;

	target_so.update_date = date_of_now;
	target_so.stage = SO_MAINTENANCE;

	if (write_element(id, &target_so))
	{
		* return_so = target_so;
		return true;
	}

	fprintf(stderr, "[%s] Couldn't update SO.\n", __func__);
	return false;
}

/*  Attempts closing a service-oder in the database.

	(...) */
bool SO_Manager::close_order(const so_id_t id, struct ServiceOrder * return_so)
{
	// Validating the SO ID.
	if (id >= item_qtt)
		return false;

	// Retrieving the SO.
	ServiceOrder target_so;
	if (! read_element(id, &target_so))
		return false;

	// Validating and skipping the currente stage.
	switch (target_so.stage)
	{
	case SO_OPEN:			target_so.stage = SO_CANCELED; break;
	case SO_BUDGET:			target_so.stage = SO_CLOSED_BUDGET; break;
	case SO_MAINTENANCE:	target_so.stage = SO_CLOSED; break;
	default:				return false;
	}

	Date date_of_now;
	if (! get_date(date_of_now))
		return false;

	target_so.update_date = date_of_now;

	if (write_element(id, &target_so))
	{
		* return_so = target_so;
		return true;
	}

	fprintf(stderr, "[%s] Couldn't update SO.\n", __func__);
	return false;

}

/*	Advances a SO to its next stage.

	\param src_so: The new SO's state.
	\param nasd: asd

	\return Returns success in advancing the SO to its next stage in the database.
	Fails in case of IO-related operations, and in case the <src_so> state
	being incoherent. */
bool SO_Manager::advance_order(id_t id, const struct ServiceOrder * src_so)
{
	// Pre-validating <src_so> state.
	if (src_so->id != id)	// same id...
		return false;

	else if (id >= item_qtt) // in this case, this should rather be a new-order.
		return false;

	struct ServiceOrder old_so;
	if (! read_element(id, &old_so))
		return false;

	Date src_so_creation_date = src_so->creation_date;
	if (src_so_creation_date != old_so.creation_date)
		return false;

	else if (src_so->client_id.id != old_so.client_id.id)
		return false;

	else if (src_so->stage != old_so.stage)
		return false;

	Date date_of_now;
	if (! get_date(date_of_now))
		return false;


	return false;
}

/*

*/
bool SO_Manager::get_order(id_t id, struct ServiceOrder * return_so) const
{
	if (id >= item_qtt)
		return false;
	
	return read_element(id, return_so);
}

/*  Lists all SOs in the database that fits a certain stage category. */
std::list<struct ServiceOrder> SO_Manager::so_category(SERVICE_ORDER_STAGE category) {
	return list_filter([=](const struct ServiceOrder & so) { return so.stage == category; }, 0, ((size_t) -1));
}

/*

*/
std::ostream & operator<<(std::ostream & stream, const struct ServiceOrder & so)
{
	// ID.
	char string_buffer[16];
	sprintf(string_buffer, "[%03llu]", so.id);
	stream << string_buffer << " ";

	// SO stage.
	char stage[5];
	#define STAGE_STRING_FORMATTING	"%-4s"

	switch (so.stage)
	{
	case SO_OPEN:			sprintf(stage, STAGE_STRING_FORMATTING, "OPEN"); break;
	case SO_BUDGET:			sprintf(stage, STAGE_STRING_FORMATTING, "BDGT"); break;
	case SO_MAINTENANCE:	sprintf(stage, STAGE_STRING_FORMATTING, "MNTC"); break;
	case SO_CLOSED:			sprintf(stage, STAGE_STRING_FORMATTING, "CLOS"); break;
	case SO_CANCELED:		sprintf(stage, STAGE_STRING_FORMATTING, "CANC"); break;
	case SO_CLOSED_BUDGET:	sprintf(stage, STAGE_STRING_FORMATTING, "CBUD"); break;
	default:				sprintf(stage, STAGE_STRING_FORMATTING, "UNKW"); break;
	}
	stream << std::string(stage);

	// The dates.
	stream << " +" << so.creation_date << " *" << so.update_date << " | ";

	// Client-ID.
	stream << so.client_id << ", ";

	// Budgeting.
	sprint_currency(string_buffer, so.hardware_price);
	stream << "hardware=" << string_buffer;

	sprint_currency(string_buffer, so.labor_price);
	stream << ", labor=" << string_buffer << "\t";

	// Description.
	stream << std::string(so.issue_description);

	return stream;
}	

inline void SO_Manager::fprint_element(FILE * _OutputStream, const ServiceOrder * _SO) const
{
	fprintf(_OutputStream, ": ");
	switch (_SO->stage)
	{
	case SO_OPEN:			fprintf(_OutputStream, "%-4s", "OPEN"); break;
	case SO_BUDGET:			fprintf(_OutputStream, "%-4s", "BDGT"); break;
	case SO_MAINTENANCE:	fprintf(_OutputStream, "%-4s", "MNTC"); break;
	case SO_CLOSED:			fprintf(_OutputStream, "%-4s", "CLOS"); break;
	case SO_CANCELED:		fprintf(_OutputStream, "%-4s", "CANC"); break;
	case SO_CLOSED_BUDGET:	fprintf(_OutputStream, "%-4s", "CBUD"); break;
	default:				fprintf(_OutputStream, "%-4s", "UNKW"); break;
	}

	// Date
	fprintf(_OutputStream, " +"); // _SO->id
	fprint_date(_OutputStream, _SO->creation_date);
	fprintf(_OutputStream, ", *");
	fprint_date(_OutputStream, _SO->update_date);
	fprintf(_OutputStream, " | ");

	fprintf(_OutputStream, "[%06llu:%02d], ",
		_SO->client_id.person_id, _SO->client_id.vehicle_id);

	fprintf(_OutputStream, "hardware=R$%05.2lf, labor=R$%06.2lf",
		((double) _SO->hardware_price) / ((double) 100.0), ((double) _SO->labor_price) / ((double) 100.0));

	fprintf(_OutputStream, " [%-16s]", _SO->issue_description);
}

inline void SO_Manager::repr_el(const struct ServiceOrder & _SO) const {
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




static constexpr size_t literal_string_length(const char * _String)
{
	size_t iterator = 0;
	while (_String[iterator ++]);
	return iterator;
}

static inline void print_n_char(char c, size_t n) {
	while (n --)
		putchar(c);
}

/*
22, 12, 40
239, 203, 104
225, 239, 230
174, 183, 179
0, 4, 17
*/

size_t SO_Manager::print_vpage(size_t vpage_index, size_t focus_index)
{
	constexpr size_t vpage_size = 10;
	size_t index = vpage_size * vpage_index;

	if (index >= item_qtt)	// exceeded
		return 0;

	struct ServiceOrder so_buffer[vpage_size];

	size_t qtt_read = read_elements(index, vpage_size, so_buffer);
	size_t iterator = 0;
	for (; iterator < qtt_read; iterator ++)
	{
		if (iterator == focus_index)
		{
			aec_bg_rgb(0, 4, 17);
			aec_fg_rgb(174, 183, 179);
			fprintf(stdout, "[%03llu] ", index + iterator);
			fprint_element(stdout, &so_buffer[iterator]);
			aec_reset();
			putchar('\n');
		}
		else {
			fprintf(stdout, "[%03llu] ", index + iterator);
			fprint_element(stdout, &so_buffer[iterator]);
			putchar('\n');
		}
	}

	while (iterator < vpage_size) {
		fprintf(stdout, "[%03llu] ", index + (iterator ++));
		print_n_char('-', 3);
		printf(" / * / ");
		print_n_char('-', 3);
		putchar('\n');
	}

	return qtt_read;
}

size_t SO_Manager::print_lpage(std::list<struct ServiceOrder> & the_list, 
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

			aec_bg_rgb(0, 4, 17);
			aec_fg_rgb(174, 183, 179);
			fprintf(stdout, "[%03llu]\t", so.id);

			SO_Manager::repr_el(so);

			aec_reset();
			putchar('\n');

			list_ptr = std::next(list_ptr, 1);
		}
		else {
			so = static_cast<struct ServiceOrder>(* list_ptr);
			
			fprintf(stdout, "[%03llu]\t", so.id);

			SO_Manager::repr_el(so);
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

void SO_Manager::sov_render_header(SERVICE_ORDER_STAGE category) const 
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

void SO_Manager::sov_render_footer(void) const
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

void SO_Manager::sov_process_io(const size_t & vpage_index_max)
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
			SO_Manager::so_inspect(static_cast<so_id_t> (focus_index + vpage_index * vpage_size));
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

void SO_Manager::so_vizualizer(void) 
{
	cmd_input_buffer.clear();
	cmd_output_buffer.clear();
	
	const size_t vpage_index_max = item_qtt / vpage_size;

	while (running_menu) {
		/*	Rendering: header */
		clean_screen();
		SO_Manager::sov_render_header();

		/*	Rendering: body */
		print_n_char('\n', 2);

		printf("Page: #%03llu\n", vpage_index);
		fprintf(stdout, "SO-ID\tSTATE\t\tCREATION DATE\tUPDATE DATE\t\n");
		vpage_item_qtt = SO_Manager::print_vpage(vpage_index, focus_index);

		/*	Rendering: footer */
		SO_Manager::sov_render_footer();

		/*	Input / Output	*/
		SO_Manager::sov_process_io(vpage_index_max);
	}
	running_menu = true;

	clean_screen();
}

void SO_Manager::so_category_vizualizer(SERVICE_ORDER_STAGE category)
{
	cmd_input_buffer.clear();
	cmd_output_buffer.clear();

	std::list<struct ServiceOrder> SOs = SO_Manager::so_category(category);

	const size_t vpage_index_max = SOs.size() / vpage_size;

	while (running_menu) {
		/*	Rendering: header */
		clean_screen();
		SO_Manager::sov_render_header(category);

		/*	Rendering: body */
		print_n_char('\n', 2);

		printf("Page: #%03llu\n", vpage_index);
		fprintf(stdout, "SO-ID\tSTATE\tCLIENT-ID\tHRD\t LBR\t\tCREATION DATE\t\tUPDATE DATE\t\n");
		vpage_item_qtt = SO_Manager::print_lpage(SOs, vpage_index, focus_index);

		/*	Rendering: footer */
		SO_Manager::sov_render_footer();

		/*	Input / Output	*/
		SO_Manager::sov_process_io(vpage_index_max);
	}
	running_menu = true;

	clean_screen();
}

/*

*/
void SO_Manager::so_inspect(so_id_t id)
{
	aec_clean();

	printf("NOT YET IMPLEMENTED\n(ENTER ANYTHING TO LEAVE)\n");
	char c;
	std::cin >> c;
}



/*	class SO_Vizualizer *
 *	=================== */

SO_Vizualizer::SO_Vizualizer(SO_Manager * so_manager) : so_manager(so_manager)
{
	if (so_manager == nullptr)
		throw std::runtime_error("Invalid SO_Manager to SO_Vizualizer.");
}

void SO_Vizualizer::sov_menu(void)
{
	printf("\n\n>\t\tBLANK\n\n");
	// BLANK
}


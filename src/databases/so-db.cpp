/*  <src/databases/so-db.cpp>

	Defines the service-orders (SOs) database. */

/*  Assignment part for @HexagonalUniverse.

	Last update: 20/07/2024. */


#include "so-db.hpp"
#include <iostream>	// cin & cout
#include <string.h>	// for strcpy, strcmp
#include <assert.h>


struct SOM_stream_header {
	id_t item_qtt = 0;
	id_t first_active = 0;
};


SO_Manager::SO_Manager(void) : Database(SO_FILENAME, sizeof(first_active) + sizeof(item_qtt)), client_manager() {
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
	if (stream == nullptr)	return;

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

bool SO_Manager::get_order(id_t id, struct ServiceOrder * return_so) const
{
	return false;
}

std::list<struct ServiceOrder> SO_Manager::so_category(SERVICE_ORDER_STAGE category)
{
	return list_filter([=](const struct ServiceOrder & so) { return so.stage == category; }, 0, ((size_t) -1));
}


// db repr


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

inline void SO_Manager::fprint_element(FILE * _OutputStream, const ServiceOrder * _SO)
{
	// SO "header"
	// fprintf(_OutputStream, "[%03llu]: ", _SO->id);

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


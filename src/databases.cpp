/*  <src/databases.cpp>

	Defines the databases for the main application. */

/*  Assignment part for @HexagonalUniverse.

	Last update: 14/07/2024. */


#include "databases.h"
#include <iostream>	// cin / cout
#include <time.h>	// time, localtime
#include <string.h>	// for strcpy
#include <assert.h>


/*	Date
	====
	
	(TODO: Move out) */

static bool get_date(Date & the_date) {

	time_t current_time = time(nullptr);
	struct tm * time_components = localtime(&current_time);
	if (time_components == nullptr)
		return false;

	the_date.year =		time_components->tm_year;
	the_date.month =	time_components->tm_mon;
	the_date.day =		time_components->tm_mday;
	the_date.hour =		time_components->tm_hour;
	the_date.minutes =	time_components->tm_min;
	the_date.seconds =	time_components->tm_sec;

	return true;
}

static void fprint_date(FILE * const _PrintStream, const Date & the_date)
{
	fprintf(_PrintStream, "(%02hhu/%02hhu/%02d %02hhu:%02hhu:%02hhu)",
		the_date.day, the_date.month + 1, ((int) the_date.year) + 1900,
		the_date.hour, the_date.minutes, the_date.seconds);
}


/*	Database Template
	================= */

template <typename ElementType> 
Database<ElementType>::Database(const char * filename) : filename(filename) {
	init_succeeded = true;
	stream_header.next_id = 0;
	stream_header.item_qtt = 0;
	stream = nullptr;

	if (((stream = fopen(filename, "r+b")) == nullptr) && (! init_stream())) {
		init_succeeded = false;
		std::cerr << "Couldn't initialize stream" << std::endl;
		return;
	}

	/*	If reading the stream-header fails, then probably the database is empty. 
		In that case, it is reset. */
	if (! read_stream_header())
	{
		// Checking if the stream is actually empty.
		if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0)
			reset_database();
	}

	printf("<%s> last id: %llu\n", filename, stream_header.next_id);
};

template <typename ElementType>
Database<ElementType>::~Database(void) {
	if (! write_stream_header())
		std::cerr << "[%s]: STREAM HEADER COULDN'T BE WRITTEN!" << std::endl;

	fclose(stream);
}

template <typename ElementType>
inline bool Database<ElementType>::init_stream(void) {
	return (stream = fopen(filename, "w+b")) != nullptr; 
}

template <typename ElementType>
void Database<ElementType>::reset_database(void) {
	rewind(stream);
	stream_header.next_id = 0;
	fwrite(&stream_header, sizeof(stream_header), 1, stream);
}

template <typename ElementType>
bool Database<ElementType>::read_stream_header(void) {
	rewind(stream);
	return fread(&stream_header, sizeof(stream_header), 1, stream) > 0;
};

template <typename ElementType>
bool Database<ElementType>::write_stream_header(void) const {
	rewind(stream);
	return fwrite(&stream_header, sizeof(stream_header), 1, stream) > 0;
};

/*  Database Representation
	----------------------- */

template <typename ElementType>
bool Database<ElementType>::print_database(FILE * _OutputStream, size_t _From, size_t _To)
{
	// _To = 0 will force printing the entire database.
	if ((_To == 0) && (stream_header.next_id > 0))
		_To = stream_header.next_id;

	ElementType element_buffer;
	
	if (fseek(stream, sizeof(stream_header), SEEK_SET) != 0)
		return false;
	
	size_t iterator = _From;
	while ((iterator <= _To) && read_element(iterator ++, &element_buffer)) {
		// not safe inside the loop yet*

		fprintf(_OutputStream, "[%03llu] ", iterator - 1);
		fprint_element(_OutputStream, &element_buffer);
		fprintf(_OutputStream, "\n");
	}

	return iterator == (_To + 1);
}



/*	Clients Database
	================ */

inline void ClientManager::fprint_element(FILE * _OutputStream, const struct Client * _Client)
{
	fprintf(_OutputStream, "[%06llu:%02d]: ",
		_Client->id.person_id, _Client->id.vehicle_id);

	fprint_date(_OutputStream, _Client->registry_date);
	fprintf(_OutputStream, " <%s> com <%s:%s>",
		_Client->name,
		_Client->vehicle.type,
		_Client->vehicle.model
	);
}

ClientManager::ClientManager(void) : Database(CLIENTDB_FILENAME) {
	std::cout << "Initial client database state:" << std::endl;
	print_database();
};

ClientManager::~ClientManager(void) {
	std::cout << "Final client database state:" << std::endl;
	print_database();
}

/*	Returns the index of first occurrence of the person's id on the database, from a given
	starting index.
	The return is (-1) in case of not founding; and (-2) in case of IO errors. */
int64_t ClientManager::fetch_person_id(id_t person_id, size_t _From) {
	struct Client client_buffer;

	size_t iterator = _From;
	while (iterator < stream_header.item_qtt)
	{
		if (! read_element(iterator, &client_buffer))
			return -2;

		if (client_buffer.id.person_id == person_id)
			return (int64_t) iterator;

		iterator ++;
	}
	return -1;
}
//

/*	Register a new client - with a person registered or yet not - on the database.
	Returns success; in case of fail, the state of the database won't change. */
bool ClientManager::register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, id_t person_id) {
	if (person_id == ((id_t) -1))
		person_id = stream_header.next_id;
	
	// Determining the vehicle's id.
	int64_t index = -1;	// tracks the index from fetching the person's id.
	uint8_t vehicle_index = 0;	// "counts" occurrences of the person on the DB.
	while ((index = fetch_person_id(person_id, (size_t) (index + 1))) >= 0)
		vehicle_index ++;

	// IO error on fetching;
	if (index == -2) {
		fprintf(stderr, "[%s]: Error on fetching\n", __func__);
		return false;
	}

	// the client (person) does not exists yet on the database;
	else if ((index == -1) && (vehicle_index == 0)) {
		printf("Person [%llu] doesn't exists\n", person_id);

		// Beyond not existing, the person's ID is not even the next on the sequence.
		if (person_id != stream_header.next_id)
		{
			fprintf(stderr, "Invalid person id... (Expected %llu)\n", stream_header.next_id);
			return false;
		}
	}

	// exceeded the allowed number of vehicles for the person.
	else if (vehicle_index >= (1 << VEHICLE_ID_BITS)) {
		printf("MUITA COISA\n");
		return false;
	}

	/* Invariant: 
		Person can allocate a new vehicle. */

	// Marking the time of action.
	Date date_of_now;
	if (! get_date(date_of_now))
	{
		fprintf(stderr, "[%s] <- (false): Date...\n", __func__);
		return false;
	}

	// Initializing the client.
	struct Client client;
	client.id.person_id = person_id;
	strcpy(client.name, name);
	client.vehicle = vehicle;
	client.registry_date = date_of_now;

	client.id.vehicle_id = vehicle_index;

	printf("client id: %d (%llu:%llu)\n", (int) client.id.id,
		client.id.person_id, (unsigned long long) client.id.vehicle_id);

	/*	Sequentiating the actions of writing and increment the 
	data stream properties. */
	return write_element(stream_header.item_qtt, &client) && 
		(++ stream_header.item_qtt) && 
		((stream_header.next_id == person_id) && (++ stream_header.next_id));
}


/*	Service Orders Database
	======================= */


inline void SO_Manager::fprint_element(FILE * _OutputStream, const ServiceOrder * _SO)
{
	// SO "header"
	// fprintf(_OutputStream, "[%03llu]: ", _SO->id);

	fprintf(_OutputStream, ": ");
	switch (_SO->stage)
	{
	case SO_OPEN:			fprintf(_OutputStream, "%-7s", "OPEN"); break;
	case SO_CLOSED:			fprintf(_OutputStream, "%-7s", "CLOSED"); break;
	case SO_BUDGET:			fprintf(_OutputStream, "%-7s", "BUDGET"); break;
	case SO_MAINTENANCE:	fprintf(_OutputStream, "%-7s", "MNTNC"); break;
	default:				fprintf(_OutputStream, "%-7s", "UNKOWN"); break;
	}
	
	// Date
	fprintf(_OutputStream, " +", _SO->id);
	fprint_date(_OutputStream, _SO->creation_date);
	fprintf(_OutputStream, ", *");
	fprint_date(_OutputStream, _SO->update_date);
	fprintf(_OutputStream, "\t| ");

	fprintf(_OutputStream, "client=[%06llu:%02d], ",
		_SO->client_id.person_id, _SO->client_id.vehicle_id);

	fprintf(_OutputStream, "hardware=R$%05.2lf, labor=R$%05.2lf",
		((double) _SO->hardware_price) / 100.0L, ((double) _SO->labor_price) / 100.0L);
}

SO_Manager::SO_Manager(void) : Database(SO_FILENAME), client_manager() {
	std::cout << "Initial SO-DB state:" << std::endl;
	print_database();
}

SO_Manager::~SO_Manager(void) {
	std::cout << "Final SO-DB state:" << std::endl;
	print_database();
}


/*	Gets a new order on the database. 
	In that sense, the passed SO is pre-loaded with the new order template, to be
	filled elsewhere.

	Once filled, it will be followed by <set_new_order> so it can correctly go to the
	database. */
bool SO_Manager::get_new_order(struct ServiceOrder * return_so)
{
	Date date_of_now;
	if (! get_date(date_of_now))
		return false;

	struct ServiceOrder so = {
		.id = stream_header.next_id,
		.stage = SO_OPEN,
		.client_id = { 0 },
		.issue_description = "undef",
		.budget = {
			.n_pieces = 0,  .pieces = { (PIECE_ID) 0 }, .prices = { 0 } 
			},

		.labor_price = 0,
		.hardware_price = 0,

		.creation_date = date_of_now,
		.update_date = date_of_now,
	};
	
	* return_so = so;
	return true;
}



/*	Sets / writes a new order on the database. 
	For that, first it is analysed to check if it can properly be set as a 
	new SO. And that involves:
		. SO's ID being the next in the database sequence;
		. SO's stage to be on open;
		. SO's client's ID to be valid;
		. SO's issue-description to be large enough (>4).
		
	Returns success; fails in case of the SO validity and IO operations. */
bool SO_Manager::set_new_order(const struct ServiceOrder * so)
{
	// Validating the SO's id:
	// It has to have the next id in the database.
	if (so->id != stream_header.next_id)
	{
		fprintf(stderr, "[%s] (false): ID is not the next.\n", __func__);
		return false;
	}

	// Validating the SO stage:
	// It has to be on OPEN stage.
	if (so->stage != SO_OPEN)
	{
		fprintf(stderr, "[%s] (false): Can only set new orders as OPEN.\n", __func__);
		return false;
	}

	// Validating the issue-description:
	// The description has to be bigger enough.
	volatile int i = 0;
	while (so->issue_description[i ++] && (i <= 4));
	if (i < 4)
	{
		fprintf(stderr, "[%s] (false): Issue description doesn't have even 4 valid characters (%d)...\n", 
			__func__, i);
		return false;
	}

	// Validating the SO's client's ID
	if (client_manager.fetch_person_id(so->client_id.person_id) < 0)
	{
		fprintf(stderr, "[%s] (false): Client's ID is inexistent.\n", __func__);
		return false;
	}

	// Updating the date.

	Date date_of_now;
	if (! get_date(date_of_now))
	{
		fprintf(stderr, "[%s] (false): Couldn't get the date...\n", __func__);
		return false;
	}
	
	struct ServiceOrder updated_so = * so;
	updated_so.update_date = date_of_now;
	
	if (! write_element(updated_so.id, &updated_so))
	{
		fprintf(stderr, "[%s] (false): IO...", __func__);
		return false;
	}

	stream_header.next_id ++;

	return true;
}




bool SO_Manager::update_order(id_t id, const struct ServiceOrder * so)
{
	printf("[%s] id=%llu, so=%p\n", __func__, id, so);
	return false;
}

bool SO_Manager::close_order(id_t id)
{
	printf("[%s] id=%llu, so=%p\n", __func__, id, (void*)NULL);
	return false;
}

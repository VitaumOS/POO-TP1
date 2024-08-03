/*  <src/databases/clients-db.cpp>

	Defines the client's database.. */

/*	Last update: 20/07/2024. */


#include "clients-db.hpp"
#include <string.h>	// for strcpy, strcmp
#include <assert.h>


struct CM_stream_header {
	id_t next_id = 0;
	id_t item_qtt = 0;
};


ClientsManager::ClientsManager(void) : Database(clients_DB_filename, sizeof(next_id) + sizeof(item_qtt)) {
	// stream_header_size += sizeof(next_id);
	if (! could_initialize()) {
		std::cerr << "Couldn't initialize ClientsManager." << std::endl;
		return;
	}

	if (! ClientsManager::retrieve_stream_header()) {

		if ((! fseek(stream, 0, SEEK_END)) && ftell(stream) == 0) {
			ClientsManager::reset_database();
		}
	}

	std::cout << "Initial client database state:" << std::endl;
	print_database();
};

ClientsManager::~ClientsManager(void) {
	if (stream == nullptr)	return;	// the database is already closed.

	std::cout << "Final client database state:" << std::endl;
	print_database();

	if (! ClientsManager::update_stream_header())
		std::cerr << "Stream header couldn't be written at ClientsManager." << std::endl;

	finalize_stream();
}

bool ClientsManager::reset_database(void) {
	std::cerr << "Resetting Clients database." << std::endl;

	next_id = 0;
	item_qtt = 0;
	return update_stream_header();
}

bool ClientsManager::retrieve_stream_header(void) {
	struct CM_stream_header _stream_header;

	rewind(stream);
	if (fread(&_stream_header, sizeof(_stream_header), 1, stream) < 1)
		return false;

	next_id		= _stream_header.next_id;
	item_qtt	= _stream_header.item_qtt;

	return true;
}

bool ClientsManager::update_stream_header(void) const {
	struct CM_stream_header _stream_header = {
		.next_id	= next_id,
		.item_qtt	= item_qtt
	};

	rewind(stream);
	return fwrite(&_stream_header, stream_header_size, 1, stream) > 0;
}

/*	Returns the index of first occurrence of the person's id on the database, from a given
	starting index.
	The return is (-1) in case of not founding; and (-2) in case of IO errors. */
int64_t ClientsManager::fetch_person_id(id_t person_id, size_t _From) const {
	struct Client client_buffer;

	size_t iterator = _From;
	while (iterator < item_qtt)
	{
		if (! read_element(iterator, &client_buffer)) {

			return -2;
		}

		if (client_buffer.id.person_id == person_id)
			return (int64_t) iterator;

		iterator ++;
	}
	return -1;
}

int64_t ClientsManager::fetch_client_id(const client_id_t & ID, size_t _From) const {
	printf("fetch client id: [%llu]\n", ID.id);

	struct Client client_buffer;

	size_t iterator = _From;
	while (iterator < item_qtt)
	{
		if (! read_element(iterator, &client_buffer))
			return -2;

		if (client_buffer.id.id == ID.id)
			return (int64_t) iterator;

		iterator ++;
	}
	return -1;
}

int64_t ClientsManager::fetch_person_name(const char person_name[NAME_SIZE], size_t _From) const {
	struct Client client_buffer;

	size_t iterator = _From;
	while (iterator < item_qtt)
	{
		if (! read_element(iterator, &client_buffer))
			return -2LL;

		if (! strcmp(person_name, client_buffer.name))
			return (int64_t) iterator;

		iterator ++;
	}
	return -1LL;
}

int64_t ClientsManager::fetch_person_name(const char person_name[NAME_SIZE], struct Client * client_buffer, size_t _From) const {
	const int64_t index = fetch_person_name(person_name, _From);

	if ((index >= 0) && (! ClientsManager::read_element(index, client_buffer)))
		return -3;
	return index;
}

bool ClientsManager::get_client(const client_id_t & ID, struct Client * const return_client)
{
	if (ID.person_id >= next_id)
		return false;

	int64_t index = fetch_client_id(ID, 0);
	if (index == -1)
		return false;

	return read_element((id_t) index, return_client);
}

/*	Register a new client - with a person registered or yet not - on the database.
	Returns success; in case of fail, the state of the database won't change. */
bool ClientsManager::register_client(const char name[NAME_SIZE], const struct Vehicle vehicle, struct Client * const return_client, id_t person_id) {
	if (person_id == ((id_t) -1))
		person_id = next_id;
	
	printf("[%s] person-id: %llu\n", __func__, person_id);

	struct Client person_registry;	// will track the a first client's for the person.
	
	// Determining the vehicle's id.
	int64_t index = -1;	// tracks the index from fetching the person's id.
	uint8_t vehicle_index = 0;	// "counts" occurrences of the person on the DB.
	while ((index = fetch_person_id(person_id, (size_t) (index + 1))) >= 0) {
		if (vehicle_index == 0)
		{	// Checking the person's credentials.

			client_id_t c_id;
			c_id.person_id = person_id;
			c_id.vehicle_id = 0;

			if (! get_client(c_id, &person_registry))
				return false;

			// The names doesn't match.
			if (strcmp(name, person_registry.name) != 0)
			{
				fprintf(stderr, "[%s]: The registered person's name for the client does not match.\n", __func__);
				return false;
			}
		}
		vehicle_index ++;
	}

	// IO error on fetching;
	if (index == -2) {
		fprintf(stderr, "[%s]: Error on fetching\n", __func__);
		return false;
	}

	// the client (person) does not exists yet on the database;
	else if ((index == -1) && (vehicle_index == 0)) {
		fprintf(stderr, "Person [%llu] doesn't exists\n", person_id);

		// Beyond not existing, the person's ID is not even the next on the sequence.
		if (person_id != next_id)
		{
			fprintf(stderr, "Invalid person id... (Expected %llu)\n", next_id);
			return false;
		}
	}

	// exceeded the allowed number of vehicles for the person.
	else if (vehicle_index >= (1 << VEHICLE_ID_BITS)) {
		fprintf(stderr, "MUITA COISA\n");
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

	printf("\n>WRITING AT %llu\n", item_qtt);
	if (write_element(item_qtt, &client))
	{
		item_qtt ++;
		if (person_id == next_id)	next_id ++;

		* return_client = client;
		return true;
	}
	return false;
}

inline void ClientsManager::fprint_element(FILE * _OutputStream, const struct Client * _Client) const 
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

std::ostream & operator<<(std::ostream & stream, const client_id_t & ID)
{
	char string_buffer[16];
	sprintf(string_buffer, "[%06llu:%02d]", ID.person_id, (int) ID.vehicle_id);
	stream << string_buffer;
	return stream;
}

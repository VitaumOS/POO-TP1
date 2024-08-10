/*  <src/databases/clients-db.cpp>

	Defines the client's database.. */


#include "clients-db.hpp"
#include <string.h>	// for strcpy, strcmp
#include <assert.h>


struct CM_stream_header {
	Id_t next_id = 0;
	Id_t item_qtt = 0;
};


ClientsManager::ClientsManager(void) : Database(clients_DB_filename, sizeof(item_qtt) + sizeof(next_id))
{
	// stream_header_size += sizeof(next_id);
	if (! could_initialize())
		return;

	if (! ClientsManager::retrieve_stream_header()) {

		if ((! fseek(stream, 0, SEEK_END)) && (ftell(stream) == 0)) {
			ClientsManager::reset_database();
		}
	}

	std::cout << "Estado incial do <client database>:" << std::endl;
	print_database();

	printf("next-id: %llu\n", next_id);
	printf("item-qtt: %llu\n", item_qtt);

	std::cout << "nothing: "; char c; std::cin >> c;
};

ClientsManager::~ClientsManager(void) {
	if (stream == nullptr)	return;	// the database is already closed.

	std::cout << "Estado final do <client database> :" << std::endl;
	print_database();

	if (! ClientsManager::update_stream_header())
		std::cerr << "N�o foi poss�vel gravar o cabe�alho do stream em ClientsManager." << std::endl;

	finalize_stream();
}

bool ClientsManager::reset_database(void) {
	std::cerr << "Resetando o Clients database." << std::endl;

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

int64_t ClientsManager::fetch_person(const struct PersonData & person_data, struct ClientData & client_data, size_t _From) const
{
	return Database::fetch_element(
		[person_data](const struct ClientData & client) {
			return client.person == person_data;
		}, client_data, _From);
}

int64_t ClientsManager::fetch_client_id(const client_id_t & id, struct ClientData & return_data, size_t _From) const {
	return Database::fetch_element(
		[id](const struct ClientData & client) {
			return id.id == client.id.id;
		},
		return_data, _From);
}

bool ClientsManager::get_client(const client_id_t & ID, struct ClientData & return_data)
{
	if (ID.person_id >= next_id)
		return false;

	int64_t index = fetch_client_id(ID, return_data, 0);
	if (index == -1)
		return false;

	return read_element((Id_t) index, &return_data);
}

std::list<struct ClientData> ClientsManager::get_person_clients(const PersonData & person_data) {
	return Database::list_filter(
		[person_data](const struct ClientData & client_data) {
			return client_data.person == person_data;
		}, 0, ((size_t) - 1));
}

std::list<struct ClientData> ClientsManager::get_person_clients(const char person_name[NAME_SIZE]) {
	struct PersonData person_data = { .name = "undef" };
	strcpy(person_data.name, person_name);

	return ClientsManager::get_person_clients(person_data);
}


/*	Register a new client - with a person registered or yet not on the database.
	Returns success; in case of fail, the state of the database won't change. */
bool ClientsManager::register_client(const struct PersonData & person, const struct VehicleData & vehicle, 
	struct ClientData & return_data) {
	
	Id_t person_id = next_id;			// tracks person's id on the database, once registered.
	struct ClientData last_client_data;	// tracks the last client data associated with the person.

	// tracks the index from fetching the person's id.
	int64_t index = fetch_person(person, last_client_data);
	uint8_t vehicle_index = 0;

	/*	if the person is already on the database, skips the
		list formed by it onto the last node. */
	if (index >= 0) {
		/*	skipping to the last last_client_data position */
		while ((printf(">>> next: %lld\n", last_client_data.next) >= 0) && (last_client_data.next >= 0) && read_element(last_client_data.next, &last_client_data) &&
			(++ vehicle_index)) {
			
		}

		vehicle_index ++;

		/*	If such, the clients will be associated, and so no new-person is introduced at db. */
		person_id = last_client_data.id.person_id;
	}

	/*	IO error case */
	else if (index == -2) {
		std::cerr << "Error gathering client data on [register_client]...\n";
		return false;
	}

	/*	there's no occurrence of the person on the database */
	else if (index == -1)
	{
		std::cerr << "[ClientsManager] Pessoa in�dita...\n";
	}

	Date date_of_now;
	if (! get_date(date_of_now))
		return false;

	client_id_t new_c_id;
	new_c_id.person_id = person_id;
	new_c_id.vehicle_id = vehicle_index;

	const struct ClientData new_client = {
		.id = new_c_id,

		.person = person,
		.vehicle = vehicle,

		.registry_date = date_of_now,

		.previous	= index,
		.next		= -1LL,
	};

	if (index >= 0)
	{
		last_client_data.next = item_qtt;
		if (! write_element(index, &last_client_data))
			return false;
	}

	printf("\n> ESCREVENDO EM %llu\n", item_qtt);
	if (write_element(item_qtt, &new_client))
	{
		item_qtt ++;
		if (person_id == next_id)	
			next_id ++;


		return_data = new_client;
		return true;
	}
	
	if (index >= 0)
	{
		last_client_data.next = -1;
		write_element(index, &last_client_data); // at this point it'll be sad if this failed...
	}

	return false;
}

inline void ClientsManager::fprint_element(FILE * _OutputStream, const struct ClientData * _Client) const 
{
	fprintf(_OutputStream, "[%06llu:%02d]: ",
		_Client->id.person_id, _Client->id.vehicle_id);

	fprint_date(_OutputStream, _Client->registry_date);
	fprintf(_OutputStream, " <%s> com <%s:%s>",
		_Client->person.name,
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

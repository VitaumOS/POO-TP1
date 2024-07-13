/*	</databases.cpp>

*/

#include "databases.h"
#include <stdio.h>
#include <time.h>


/*	Date
	==== */

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


/*	Database
	======== */

#define DEBUG_DS_CONSTRUCTOR_DECONSTRUCTOR	true

#if DEBUG_DS_CONSTRUCTOR_DECONSTRUCTOR
#define D_DS_CONSTRUCTOR()		printf("[%s]+\n", __func__)
#define D_DS_DECONSTRUCTOR()	printf("[%s]-\n", __func__)

#else
#define D_DS_CONSTRUCTOR()
#define D_DS_DECONSTRUCTOR()

#endif // DEBUG_DS_CONSTRUCTOR_DECONSTRUCTOR

bool Database::write_stream_header(void) {
	rewind(stream);
	return fwrite(&stream_header, sizeof(stream_header), 1, stream) > 0;
};

bool Database::read_stream_header(void) {
	rewind(stream);
	return fread(&stream_header, sizeof(stream_header), 1, stream) > 0;
};

Database::Database(const char * filename) : filename(filename) {
	D_DS_CONSTRUCTOR();

	init_succeeded = true;
	stream_header.next_id = 0;
	stream_header.item_qtt = 0;
	stream = nullptr;

	if (((stream = fopen(filename, "r+b")) == nullptr) && (! create_stream())) {
		init_succeeded = false;
		fprintf(stderr, "COULDN'T INITIALIZE STREAM <%s>\n", filename);
		return;
	}

	/*	If reading the stream-header fails, resets the database. */
	if (! read_stream_header())
	{
		reset_database();
	}

	printf("<%s> last id: %llu\n", filename, stream_header.next_id);
};

Database::~Database(void) {
	D_DS_DECONSTRUCTOR();

	if (! write_stream_header())
	{
		fprintf(stderr, "[%s]: STREAM HEADER COULDN'T BE WRITTEN!\n", __func__);
	}

	fclose(stream);
}

bool Database::create_stream(void) {
	return (stream = fopen(filename, "w+b")) != nullptr; 
}





/*	Client
	====== */

ClientManager::ClientManager(void) : Database(CLIENTDB_FILENAME) {
	D_DS_CONSTRUCTOR();

	print_database();

	/*
	fseek(stream, 0, SEEK_END);
	long pos = ftell(stream);
	if (pos < 0)
	{
		printf("SOMETHING'S VERY WRONG!!!\n");
	}
	last_index = (((id_t) pos) - sizeof(stream_header)) / sizeof(struct Client);
	printf("LAST_INDEX: %llu\n", last_index);
	*/
};

ClientManager::~ClientManager(void)
{
	D_DS_DECONSTRUCTOR();

	print_database();
}


/*	Prints a database part into an output stream.
	Prints it on its entirety, and on stdout, by default. */
void ClientManager::print_database(FILE * _PrintStream, size_t _From, size_t _To) const {
	if (_To == 0)
		_To = stream_header.next_id;

	struct Client client_buffer;
	fseek(stream, sizeof(stream_header), SEEK_SET);
	size_t iterator = _From;
	while (read_item(iterator ++, &client_buffer))
	{
		fprintf(_PrintStream, "[%03llu] ", iterator - 1);
		fprintf(_PrintStream, "[%06llu:%02d]: ",
			client_buffer.id.person_id, client_buffer.id.vehicle_id);

		fprint_date(_PrintStream, client_buffer.registry_date);
		fprintf(_PrintStream, " <%s> com <%s:%s>\n",
			client_buffer.name,
			client_buffer.vehicle.type,
			client_buffer.vehicle.model
		);
	}
}

/*	Returns the index of first occurrence of the person's id on the database, from a given
	starting index.
	The return is (-1) in case of not founding; and (-2) in case of IO errors. */
int64_t ClientManager::fetch_person_id(id_t person_id, size_t _From) {
	struct Client client_buffer;

	size_t iterator = _From;
	while (iterator < stream_header.item_qtt)
	{
		if (! read_item(iterator, &client_buffer))
			return -2;

		if (client_buffer.id.person_id == person_id)
			return (int64_t) iterator;

		iterator ++;
	}
	return -1;
}

#include <string.h>
/*	Register a new client - with a person not yet registered - on the database.
	Returns success; in case of fail, the state of the database won't change. */
bool ClientManager::register_new_person(const char name[NAME_SIZE], const struct Vehicle vehicle)
{
	// Marking the time of action.
	Date date_of_now;
	if (! get_date(date_of_now))
	{
		fprintf(stderr, "[%s] <- (false): Date...\n", __func__);
		return false;
	}

	// Initializing the client.
	struct Client client;
	client.id.person_id = stream_header.next_id;	// receives the next person's id.
	strcpy(client.name, name);
	client.vehicle = vehicle;
	client.registry_date = date_of_now;

	// Determining the vehicle's id.
	int64_t index;	// tracks the index from fetching the person's id.
	uint8_t vehicle_index = 0;	// "counts" occurrences of the person on the DB.
	while ((index = fetch_person_id(client.id.person_id)) >= 0)
		vehicle_index ++;
	
	if (index == -2) {
		fprintf(stderr, "Error on fetching\n");
		return false;
	}

	client.id.vehicle_id = vehicle_index;
	return write_item(client.id.id, &client) && 
		(++ stream_header.next_id);
}

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

		if (person_id != stream_header.next_id)
		{
			fprintf(stderr, "Invalid person id... (Expected %llu)\n", stream_header.next_id);
			return false;
		}
		//return register_new_person(name, vehicle);
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
	return write_item(stream_header.item_qtt, &client) && 
		(++ stream_header.item_qtt) && 
		((stream_header.next_id == person_id) && (++ stream_header.next_id));
}


/*	Service Orders
	============== */

SO_Manager::SO_Manager(void) : Database(SO_FILENAME), client_manager() {
	D_DS_CONSTRUCTOR();
}

SO_Manager::~SO_Manager(void)
{
	D_DS_DECONSTRUCTOR();
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

	/*
	if (! write_item(stream_header.last_id, &so)) {
		fprintf(stderr, "[%s] <- (false): Couldn't write SO.\n", __func__);

		// stream_header.last_id --;
		return false;
	}
	*/


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
	
	if (! write_item(updated_so.id, &updated_so))
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

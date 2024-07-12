/*	</databases.cpp>

*/

#include "databases.h"
#include <stdio.h>
#include <time.h>


static bool get_date(Date & the_date) {

	time_t current_time = time(nullptr);
	struct tm * time_components = localtime(&current_time);
	if (time_components == nullptr)
		return false;

	the_date.year = time_components->tm_year;
	the_date.month = time_components->tm_mon;
	the_date.day = time_components->tm_mday;
	the_date.hour = time_components->tm_hour;
	the_date.minutes = time_components->tm_min;
	the_date.seconds = time_components->tm_sec;

	return true;
}

#define DEBUG_DS_CONSTRUCTOR_DECONSTRUCTOR	false

#if DEBUG_DS_CONSTRUCTOR_DECONSTRUCTOR
#define D_DS_CONSTRUCTOR()		printf("[%s]+\n", __func__)
#define D_DS_DECONSTRUCTOR()	printf("[%s]-\n", __func__)

#else
#define D_DS_CONSTRUCTOR()
#define D_DS_DECONSTRUCTOR()

#endif // DEBUG_DS_CONSTRUCTOR_DECONSTRUCTOR



void Database::write_stream_header(void) {
	
};

void Database::read_stream_header(void) {

};


Database::Database(const char * filename) : filename(filename) {
	D_DS_CONSTRUCTOR();

	init_succeeded = true;
	stream_header.last_id = 0;
	stream = nullptr;

	if (((stream = fopen(filename, "r+b")) == nullptr) && (! create_stream())) {
		init_succeeded = false;
		fprintf(stderr, "COULDN'T INITIALIZE STREAM <%s>\n", filename);
		return;
	}

	/*	If reading the stream-header fails, resets the database. */
	rewind(stream);
	if (fread(&stream_header, sizeof(stream_header), 1, stream) < 1)
	{
		reset_database();
	}
};

Database::~Database(void) {
	D_DS_DECONSTRUCTOR();
	fclose(stream);
}

bool Database::create_stream(void) {
	return (stream = fopen(filename, "w+b")) != nullptr; 
}




ClientManager::ClientManager(void) : Database(CLIENTDB_FILENAME) {
	D_DS_CONSTRUCTOR();
};

ClientManager::~ClientManager(void)
{
	D_DS_DECONSTRUCTOR();
}

SO_Manager::SO_Manager(void) : Database(SO_FILENAME), client_manager() {
	D_DS_CONSTRUCTOR();
	printf("stream_header: %d\n", (int) stream_header.last_id);
}

SO_Manager::~SO_Manager(void)
{
	D_DS_DECONSTRUCTOR();
	stream_header.last_id = 123;
	rewind(stream);
	fwrite(&stream_header, sizeof(stream_header), 1, stream);
}



/* The SO file stream. */

bool SO_Manager::get_new_order(struct ServiceOrder * return_so, char issue_description[static NAME_SIZE]) {

	Date date_of_now;
	if (! get_date(date_of_now))
		return false;

	struct ServiceOrder so = {
		.id = ++ stream_header.last_id,
		.stage = SO_OPEN,
		.client_id = { 0, 0 },
		.issue_description = "undef",
		.budget = { .n_pieces=0,  .pieces={ (PIECE_ID) 0 }, .prices={ 0 } },
		
		.labor_price = 0,
		.hardware_price = 0,

		.creation_date = date_of_now,
		.update_date = date_of_now,
	};

	if () {

	}

	
	* return_so = so;
	return true;
}	

bool SO_Manager::get_order(id_t id, struct ServiceOrder * return_so)
{
	printf("[%s] id=%llu, so=%p\n", __func__, id, return_so);
	return false;
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

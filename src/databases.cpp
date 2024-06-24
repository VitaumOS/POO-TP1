/*	</databases.cpp>

*/

#include "databases.h"


SO_Manager::SO_Manager(void) {
	init_succeeded = true;

	if ((stream = fopen(SO_FILENAME, "w+b")) == NULL) {
		init_succeeded = false;
		return;
	}

	if (fread(&stream_header, sizeof(stream_header), 1, stream) < 1) {
		init_succeeded = false;
		return;
	}
}

SO_Manager::~SO_Manager(void) { 
	fclose(stream); 
}


/* The SO file stream. */

bool SO_Manager::get_new_order(struct ServiceOrder * return_so) {
	struct ServiceOrder so = { 0 };
	


	* return_so = so;
	return true;
}


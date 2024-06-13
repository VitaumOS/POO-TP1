
#include "worker.h"


Worker::Worker(int id) {
    this -> id = id;
    logged = false;   
    std::printf("worker init; id: %d\n", id);
}

Worker::~Worker(void) {
    std::printf("destruindo worker\n");
}

#include "library.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <sys/timeb.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char** argv) {
    if (argc < 6) {
        std::cout << "Error, usage must be:\n";
        std::cout << "./update <heapfile> <record_id> <attribute_id> <new_value> <page_size>\n";
        return 1;
    }

    int record_id = atoi(argv[2]);
    int attribute_id = atoi(argv[3]);
    int page_size = atoi(argv[5]);

    FILE *heapfile = fopen(argv[1], "rb+");

    if (!heapfile) {
        std::cout << "Could not find file " << argv[1] << "\n";
        return 1;
    }

    Heapfile *heap = new Heapfile();
    heap->page_size = page_size;
    heap->file_ptr = heapfile;

    int entry = page_size/8-1;
    //each record is of size 1000 so num of records will be as such
    int numRecord = page_size/1000;
    int slot;
    int free;
    int finalpage;
    int currentDirectory = 0;
    int nxtDirectory;
    int recLeft = record_id;
    bool exists = true;

    while (exists) {

    }

}



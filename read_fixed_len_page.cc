#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <sys/timeb.h>
#include "library.h"
#include <time.h>
#include <sstream>


int main (int argc, char** argv) {
    if (argc < 3 || argc > 4) {
        std::cout << "Usage: read_fixed_len_pages <page_file> <page_size>\n";
        return 1;
    }

    // initialize variables from parameters
    std::string page_filename(argv[1]);
    int page_size = std::stoi(argv[2]);

    std::ifstream pageFile;
    pageFile.open(page_filename, std::ios::in | std::ios::binary);

    // start timer
    struct timeb t;
    ftime(&t);
    unsigned long start_ms = t.time * 1000 + t.millitm;

    FILE *null = fopen("tuples.csv", "w");
    
    // 10*100
    int record_size = 1000;
    int numPages = 0;
    int numRecords = 0;

    // run until we hit end of file
    while (!pageFile.eof()) {
        Page page;

        init_fixed_len_page(&page, page_size, record_size);
        pageFile.read((char *)page.data, page_size);

        int capacity = fixed_len_page_capacity(&page);
        
        for (int i = 0; i < capacity; i++) {
            Record record;
            
            read_fixed_len_page(&page, i, &record);

            for (unsigned int j = 0; j < num_attributes; j++) {
                
                if (j < num_attributes-1) {
                    std::cout << record.at(j);
                    std::cout << ",";
                }
                else {
                    std::cout << record.at(j);
                }
            }
            
            fputs("\n", null);
            numRecords++;
        }
        numPages++;
    }

    fclose(null);
    pageFile.close();

    ftime(&t);
    unsigned long stop_ms = t.time * 1000 + t.millitm;


    std::cout << "NUMBER OF RECORDS: " << numRecords << "\n";
    std::cout << "NUMBER OF PAGES: " << numPages - 1 << "\n";
    std::cout << "TIME: " << stop_ms - start_ms << " milliseconds\n";

    return 0;

}
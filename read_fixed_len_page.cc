#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <sys/timeb.h>
#include "library.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Error, usage must be:\n";
        std::cout << "./read_fixed_len_page <page_file> <page_size>\n";
        return 1;
    }

    // page_size taken from argument
    int page_size = atoi(argv[2]);
    int recordSize = num_attributes * attribute_size;
    // printf("%d\n", recordSize);

    std::ifstream page_file;
    page_file.open(argv[1]);
    


    if (!page_file)
    {
        std::cout << "Incorrect file" << argv[1] << "\n";
        return 1;
        //printf("no file")
    }

    Page page;
    char page_copy[page_size];

    int numRecords = 0;
    int numPages = 0;

    struct timeb t;
    ftime(&t);
    unsigned long start_ms = t.time * 1000 + t.millitm;
    

    
    // copying characters to page_copy array from page_file until page_size is reached 
    while (page_file.read(page_copy, page_size)) {

        // initialzing the page
        init_fixed_len_page(&page, page_size, recordSize);

        for (int i = 0; i < fixed_len_page_capacity(&page); ++i) {
            Record *record = new Record;

            int offset = i*recordSize;
            fixed_len_read(page_copy + offset, recordSize, record);

            write_fixed_len_page(&page, i, record);

            for (Record::iterator header = record->begin(); header!=record->end(); ++header) {
                if (header == record->end() - 1) {
                        std::cout << *header;
                } else {
                        std::cout << *header << ",";
                }
            }
            std::cout << "\n";
            numRecords++;
        }
        std::cout << "\n";
        std::cout << "\n";
    }
    

    numPages = (numRecords*(num_attributes*attribute_size))/page_size;
    
    ftime(&t);
    unsigned long stop_ms = t.time * 1000 + t.millitm;
    

    page_file.close();

    std::cout << "NUMBER OF PAGES: " << numPages << "\n";
    std::cout << "NUMBER OF RECORDS: " << numRecords << "\n";
    std::cout << "TIME: " << stop_ms - start_ms << " milliseconds\n";
    
    return 0;
}
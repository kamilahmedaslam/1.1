#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <cstring>
#include "library.h"

int main(int argc, const char * argv[]) {
    if (argc < 4) {
        std::cout << "Usage: write_fixed_len_pages <csv_file> <page_file> <page_size>";
        return 1;
    }

    // initialize variables from parameters
    std::string csv_filename(argv[1]);
    std::string page_filename(argv[2]);
    int page_size = std::stoi(argv[3]);

    // opening the csv file
    std::ofstream pageFile;
    pageFile.open(page_filename);

    std::ifstream csv_file(csv_filename);
    std::string r;

    Page page;

    int numPages = 0;
    int numRecords = 0;
    int new_page = 1;


    // start timer
    struct timeb t;
    ftime(&t);
    unsigned long start_ms = t.time * 1000 + t.millitm;

    while (std::getline(csv_file, r)) {
        Record record;
        std::stringstream line(r);
        std::string attr;

        while (std::getline(line, attr, ',')) {
            char *read = (char *)malloc(attribute_size + 1);
            std::strncpy(read, attr.c_str(), attribute_size + 1);
            record.push_back(read); 
        }
        
        // for the first iteration, we have to initialize the page to be written into

        if (numPages == 0) {
            init_fixed_len_page(&page, page_size, fixed_len_sizeof(&record));
            numPages++;
        }
    

        int r_val = add_fixed_len_page(&page, &record);
        numRecords++;

        if (r_val < 0) {
            new_page = 1;
        }

        if (new_page) {
            pageFile.write((const char*)page.data, page.page_size);
            init_fixed_len_page(&page, page_size, fixed_len_sizeof(&record));
            add_fixed_len_page(&page, &record);
            numPages++;
            new_page = 0;
        }
    }

    if (!new_page) {
        pageFile.write((const char*)page.data, page.page_size);
    }

    pageFile.close();
    csv_file.close();

    ftime(&t);

    unsigned long stop_ms = t.time * 1000 + t.millitm;

    std::cout << "NUMBER OF RECORDS: " << numRecords << "\n";
    std::cout << "NUMBER OF PAGES: " << numPages - 1 << "\n";
    std::cout << "TIME: " << stop_ms - start_ms << " milliseconds\n";

    return 0;
}

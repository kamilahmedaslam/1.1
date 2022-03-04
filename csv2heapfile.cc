#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <cstring>
#include "library.h"

int main(int argc, const char * argv[]) {
    if (argc < 4) {
        std::cout << "Usage: csv2heapfile <csv_file> <heap_file> <page_size>";
        return 1;
    }

    std::ifstream csv_file;
    csv_file.open(argv[1]);

    if (!csv_file) {
        std::cout << "No file exists: " << argv[1] << "\n";
    }

    Page page;
    int numRecords = 0;
    int numPages = 0;

    int page_size = std::stoi(argv[3]);
    int record_size = num_attributes * attribute_size;
    init_fixed_len_page(&page, page_size, record_size);

    Heapfile *heap_file = (Heapfile *)malloc(sizeof(Heapfile));
    FILE* open_heap = fopen(argv[2], "wb+");

    if (!open_heap) {
         std::cout << "No file exists: " << argv[2] << "\n";
    }

    init_heapfile(heap_file, page_size, open_heap);
    std::string rec;

    // start timer
    struct timeb t;
    ftime(&t);
    unsigned long start_ms = t.time * 1000 + t.millitm;

    while (std::getline(csv_file, rec)) {
        Record record;

        rec.erase(std::remove(rec.begin(), rec.end(), ','), rec.end());
        fixed_len_read((char *)rec.c_str(), page_size, &record);

        // checking if slot can be added to the current page
        int slot = add_fixed_len_page(&page, &record);

        // page is full so we have to add a new page
        if (slot == -1) {
            PageID newPageID = alloc_page(heap_file);
            write_page(&page, heap_file, newPageID);
            init_fixed_len_page(&page, page_size, record_size);
            slot = add_fixed_len_page(&page, &record);
            numPages++;
        }

        write_fixed_len_page(&page, slot, &record);
        numRecords++;
    }

    if (fixed_len_page_freeslots(&page) != fixed_len_page_capacity(&page)) {
        PageID newPageID = alloc_page(heap_file);
        write_page(&page, heap_file, newPageID);
        numPages++;
    }

    ftime(&t);
    unsigned long stop_ms = t.time * 1000 + t.millitm;

    csv_file.close();

    std::cout << "NUMBER OF RECORDS: " << numRecords << "\n";
    std::cout << "NUMBER OF PAGES: " << numPages << "\n";
    std::cout << "TIME: " << stop_ms - start_ms << " milliseconds\n";

    return 0;
}


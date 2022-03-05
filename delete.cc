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
    if (argc < 4) {
        std::cout << "Error, usage must be:\n";
        std::cout << "Usage: ./delete <heapfile> <record_id> <page_size>\n";
        return 1;
    }

    int record_id = atoi(argv[2]);
    int page_size = atoi(argv[3]);

    FILE *heapfile = fopen(argv[1], "rb+");
    Page *page = (Page *)malloc(sizeof(Page));
    page->data = (char *)malloc(page_size);
    Heapfile *heap = (Heapfile *)malloc(sizeof(Heapfile));
    heap->page_size = page_size;
    heap->file_ptr = heapfile;
    
    if (!heapfile) {
        std::cout << "Could not find file " << argv[1] << "\n";
        return 1;
    }
    
    int rec_size = num_attributes * attribute_size;

    init_fixed_len_page(page, page_size, rec_size);

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
    int moreFree;

    while (exists) {
        int line = 0;
        fread(&nxtDirectory, sizeof(int), 1, heap->file_ptr);
        fread(&free, sizeof(int), 1, heap->file_ptr);
        
        while(line < entry and exists) {
                line++;
                fread(&finalpage, sizeof(int), 1, heap->file_ptr);
                fread(&free, sizeof(int), 1, heap->file_ptr);
                int rec_size = page_size/(num_attributes * attribute_size);
                int rec_free = free/(num_attributes * attribute_size);
                int rec_num = rec_size - rec_free;

                if (recLeft >= rec_num) {
                    recLeft -= rec_num;
                }
                else {
                    exists = false;
                    fseek(heap->file_ptr, -4, SEEK_CUR);
                    moreFree = free + 1000;
                    fwrite(&moreFree, sizeof(int), 1, heap->file_ptr);
                }
        }
        fseek(heap->file_ptr, nxtDirectory*page_size, SEEK_SET);
    }
    read_page(heap, finalpage, page);

    int capacity = fixed_len_page_capacity(page);

    for (int i = 0; i < capacity; i++) {
        Record record;
        read_fixed_len_page(page, i, &record);

        if (record.size() > 0) {
            if (recLeft == 0) {
                for (int j = 0; j < record.size(); j++) {
                    memset((char *)record[j],'0',10);
                }

                write_fixed_len_page(page, i, &record);
                write_page(page, heap, finalpage);
                return 0;
            }
            else {
                recLeft-=1;
            }
        }
    }
    return 0;
}


//
//  insert.cpp
//  
//
//  Created by Steven Li on 3/4/22.
//
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <cstring>

#include "library.h"

using namespace std;

int main(int argc, char* argv[])
{
    FILE *given_heap = fopen(argv[1], "rb+");
    ifstream csv_file(argv[2]);
    int page_size = atoi(argv[3]);
//    FILE *csv_page = fopen("128_tuples.csv", "r");
    
    Heapfile *heap = (Heapfile *)malloc(sizeof(Heapfile));
    init_heapfile(heap, page_size, given_heap);
    
    string line;
//    char tuples[num_attributes * attribute_size];
    
    int r_size = num_attributes;
    Page *temp_page = (Page *)malloc(sizeof(Page));
    init_fixed_len_page(temp_page, page_size, r_size);
    while(getline(csv_file, line)){
        Record temp_record;

        line.erase(remove(line.begin(), line.end(), ','), line.end());
//        printf("%s", line.c_str());
        fixed_len_read((char*)line.c_str(), r_size, &temp_record);
        int slot_index = add_fixed_len_page(temp_page, &temp_record);
        
        if (slot_index==-1){
            PageID add_me = alloc_page(heap);
            write_page(temp_page,heap,add_me);
            init_fixed_len_page(temp_page, page_size, r_size);
            add_fixed_len_page(temp_page, &temp_record);
        }
    }
        
    printf("\nfinished\n");
    return 0;

    
 }

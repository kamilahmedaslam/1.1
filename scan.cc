#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/timeb.h>
#include <cstring>
#include "library.h"

int main(int argc, char* argv[])
{
    FILE *given = fopen(argv[1], "r");
    int p_size = atoi(argv[2]);

    Heapfile *heapfile = (Heapfile *)malloc(sizeof(Heapfile));
    init_heapfile(heapfile, p_size, given);

    char printed_record[attribute_size + 1];
    bzero(printed_record, attribute_size + 1);

    char buf_offset[(num_attributes * attribute_size) + 1];
    bzero(buf_offset, (num_attributes * attribute_size) + 1);

    RecordIterator iterator(heapfile);

    Record temp;
    while(iterator.hasNext()){
        temp = iterator.next();
        fixed_len_write(&temp, buf_offset);

        for(int i = 0; i < num_attributes; i++){
            strncpy(printed_record, buf_offset + (i * attribute_size), attribute_size);
            printf("%s", printed_record);
        }
    }


    return 0;
 }
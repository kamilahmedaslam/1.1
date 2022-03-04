#include "library.h"

/**
 * Compute the number of bytes required to serialize record
 */
int fixed_len_sizeof(Record *record)
{
    return num_attributes * attribute_size;
}

/**
 * Serialize the record to a byte array to be stored in buf.
 */
void fixed_len_write(Record *record, void *buf)
{
    for (int i = 0; i < record->size(); i++)
    {
        int position = i * attribute_size;
        // since record length is fixed,
        // we can use attribute_size for all
        memcpy(((char *)buf + position), record->at(i), attribute_size);
    }
}

/**
 * Deserializes `size` bytes from the buffer, `buf`, and
 * stores the record in `record`.
 */
void fixed_len_read(void *buf, int size, Record *record)
{
    for (int i = 0; i < size / attribute_size; i++)
    {
        char *attr = new char[attribute_size + 1];
        int attribute_idx = i * attribute_size;
        memcpy(attr, (char *)buf + attribute_idx, attribute_size);
        attr[attribute_size] = '\0';
        (*record).push_back(attr);
    }
}
/**
 * Initializes a page using the given slot size
 */
void init_fixed_len_page(Page *page, int page_size, int slot_size) {
    page->page_size = page_size;
    page->slot_size = slot_size;
    page->data = malloc(page_size);
    
    // adding '0' keeps track of unused slots
    memset(page->data, 0, page_size);
}

/**
 * Calculates the maximal number of records that fit in a page
 */
int fixed_len_page_capacity(Page *page) {
    // number of maximum records = pageSize / sizeofSlot 
    return page->page_size / page->slot_size;
}

/**
 * Calculate the free space (number of free slots) in the page
 */
int fixed_len_page_freeslots(Page *page) {
    int cap = fixed_len_page_capacity(page);
    int free = 0;

    for (int i = 0; i < cap; i++) {
        int slot_offset = i*page->slot_size;

        if (((char *)page->data)[slot_offset] == 0) {
            free += 1;
        }
    }
    return free;
}

/**
 * Add a record to the page
 * Returns:
 *   record slot offset if successful,
 *   -1 if unsuccessful (page full)
 */
int add_fixed_len_page(Page *page, Record *r) {
    int res = -1;
    int cap = fixed_len_page_capacity(page);
    if (cap > 0) {
        for (int i = 0; i < cap; i++) {
            int slot_offset = i*page->slot_size;
            if (((char *)page->data)[slot_offset] == 0) {
                write_fixed_len_page(page, i, r);
                return i;
            }
        }
    }
    return res;
}

/**
 * Write a record into a given slot.
 */
void write_fixed_len_page(Page *page, int slot, Record *r) {
    int slot_offset = slot*page->slot_size;
    char *buf = (char *)page->data + slot_offset;
    fixed_len_write(r, buf);
}

/**
 * Read a record from the page from a given slot.
 */
void read_fixed_len_page(Page *page, int slot, Record *r) {

    //slot size = record_size
    int slot_offset = slot*page->slot_size;
    int record_size = fixed_len_sizeof(r); // or = page->slot_size
    char *curr_slot = (char *)page->data + slot_offset;
    fixed_len_read(curr_slot, record_size, r);
    printf("%s\n", curr_slot);
}


void init_heapfile(Heapfile *heapfile, int page_size, FILE *file){
    heapfile->file_ptr = file;
    heapfile->page_size = page_size;
}

/**
 * Allocate another page in the heapfile.  This grows the file by a page.
 */
PageID alloc_page(Heapfile *heapfile){
    fseek(heapfile->file_ptr, 0, SEEK_SET);
    bool foundPage = true;
    int offset = 0;
    int last_id = 0;
    
    while(foundPage){
        fseek(heapfile->file_ptr, last_id * (heapfile->page_size), SEEK_SET);
        fread(&offset, sizeof(int), 1, heapfile->file_ptr);
        //if equal to one another, we know we've found the last page
        if(offset == last_id){
            foundPage = false;
        }
        else{
            last_id = offset;
        }
    }
    
    // BE CAREFUL ABOUT RESETTING THE POINTERS WHERE YOU NEED TO BE RESET
    
    //CHECK THIS
//    fseek(heapfile->file_ptr,0,SEEK_CUR);
    int last_page=offset;
//    fseek(heapfile->file_ptr, last_id * (heapfile->page_size),SEEK_SET);
    
    

    int new_page = last_page+1;
    fwrite(&new_page, sizeof(int), 1, heapfile->file_ptr);

    fseek(heapfile->file_ptr,(last_page+1)*heapfile->page_size,SEEK_SET);
    
    for (int i = 0; i <heapfile->page_size/8; ++i) {
        int offset = i+last_page+1;
        fwrite(&offset, sizeof(int), 1, heapfile->file_ptr);
        fwrite(&heapfile->page_size, sizeof(int), 1, heapfile->file_ptr);
    }
    
    return last_page+2;
    
}

/**
 * Read a page into memory
 */
void read_page(Heapfile *heapfile, PageID pid, Page *page){
    fseek(heapfile->file_ptr, pid* (heapfile->page_size) , SEEK_SET);
    fread(page->data, heapfile->page_size, 1, heapfile->file_ptr);
    fseek(heapfile->file_ptr, 0, SEEK_SET);

}

/**
 * Write a page from memory to disk
 */
void write_page(Page *page, Heapfile *heapfile, PageID pid){
    fseek(heapfile->file_ptr, pid * heapfile->page_size, SEEK_SET);
    fwrite(page->data, page->page_size, 1, heapfile->file_ptr);
    
    //any unwritten data in its output buffer is written to the file. But do we need this?
    fflush(heapfile->file_ptr);
    fseek(heapfile->file_ptr, 0, SEEK_SET);

}


RecordIterator::RecordIterator(Heapfile *heapfile){

    //initializing and allocating memory for heapfile
    this->heapfile = (Heapfile *) malloc(sizeof(Heapfile));
    this->heapfile = heapfile;

    //initializing records and pages
    this->rid->page_id = 0;
    this->rid->slot = 0;
    this->curPage = (Page *) malloc(sizeof(heapfile->page_size));
    this->rid = (RecordID*) malloc(sizeof(RecordID));

    //read an empty page into memory
    read_page(heapfile, 0, this->curPage);
}

Record RecordIterator::next() {
    Record record;
    int capacity = fixed_len_page_capacity(this->curPage);
    int currSlot = this->rid->slot;

    if (currSlot >= capacity) {
        this->rid->slot = 0;
        this->rid->page_id += 1;
        read_page(this->heapfile, this->rid->page_id, this->curPage);
        return next();
    }

    else {
        read_fixed_len_page(this->curPage, this->rid->slot, &record);
        this->rid->slot += 1;
        return record;
    }

}

bool RecordIterator::hasNext() {
    Record record;
    int capacity = fixed_len_page_capacity(this->curPage);
    int currSlot = this->rid->slot;
    bool exists = false;
    //checking if slot is on the page
    // if not we check a different page with the record id
    if (currSlot >= capacity) {
        Page *page = (Page *) malloc(this->heapfile->page_size);
        read_page(this->heapfile, (this->rid->page_id + 1), page);
        read_fixed_len_page(page, currSlot, &record);
        free(page);
        if (record.empty()) {
            return exists;
        }
    }

    else {
        read_fixed_len_page(this->curPage, this->rid->slot, &record);
        if (record.empty()) {
            return exists;
        }
    }   

    exists = true;
    return exists;
}

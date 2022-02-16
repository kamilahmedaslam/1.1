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
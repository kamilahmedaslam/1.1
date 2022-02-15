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
        // we can use attribute_len for all
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

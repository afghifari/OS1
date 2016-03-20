#ifndef PageTable_H
#define PageTable_H

#define true 1
#define false 0
#define boolean unsigned char

typedef struct {
    int LRU;
    int Valid;
    int Frame;
    int Dirty;
    int Requested;
} page_table_entry;
typedef page_table_entry* page_table_pointer;


#endif
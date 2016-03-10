#ifndef PageTable_H
#define PageTable_H

#define  NOT_READY  -1
#define  FILLED     0
#define  TAKEN      1

//-----------------------------------------------------------------------------
typedef struct {
	int statusOS;
    int Valid;
    int Frame;
    int Dirty;
    int Requested;
} page_table_entry;

typedef page_table_entry* page_table_pointer;
//-----------------------------------------------------------------------------

#endif
#include "types_constants.h"
#include <string.h>


void init_TLB(TLBEntry *tlb) {
    for (int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = false;
    }
}

void init_physical_memory(unsigned char physical_memory[NUM_FRAMES][FRAME_SIZE]) {
    memset(physical_memory, 0, NUM_FRAMES * FRAME_SIZE * sizeof(unsigned char));
}

void init_page_table(PageTableEntry *page_table) {
    for (int i = 0; i < MAX_PAGES; i++) {
        page_table[i].valid = false;
        page_table[i].referenced = false;  // initializing the referenced bit as false which means 0 in terms of algorithm
    }
}
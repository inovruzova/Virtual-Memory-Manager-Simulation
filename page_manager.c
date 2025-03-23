#include "types_constants.h"
#include <stdio.h>
#include <stdlib.h>

extern int read_page_from_disk(unsigned int page_number, unsigned char physical_memory[NUM_FRAMES][FRAME_SIZE], bool *frame_occupied);
extern void update_page_table(PageTableEntry *page_table, unsigned int page_number, unsigned int frame_number);
extern int handle_page_fault(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults);
extern int handle_page_fault_second_chance(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults);
extern int handle_page_fault_lru(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults);
extern void update_TLB(TLBEntry *tlb, unsigned int page_number, unsigned int frame_number);
extern void update_TLB_lru(TLBEntry *tlb, unsigned int page_number, unsigned int frame_number);

int frame_queue[NUM_FRAMES];  // This holds the indices of pages in the order they were added.
int front = 0;               // Points to the page to be replaced.

void update_page_table(PageTableEntry *page_table, unsigned int page_number, unsigned int frame_number) {
    page_table[page_number].frame_number = frame_number;
    page_table[page_number].valid = true;
    page_table[page_number].referenced = true;
}

void update_frame_queue(int frame_number) {
    frame_queue[front] = frame_number;
    front = (front + 1) % MAX_PAGES;  // Move the front page to point to the oldest page now
}

int handle_page_fault(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults) {
    (*total_page_faults)++;
    int frame_number = read_page_from_disk(page_number, physical_memory, frame_occupied);
    if (frame_number != -1) {
        update_page_table(page_table, page_number, frame_number);
        update_TLB(tlb, page_number, frame_number);
    }
    return frame_number;
}

int handle_page_fault_second_chance(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults) {
    (*total_page_faults)++;
    int frame_number = -1;
    
    while (true) {
        int idx = frame_queue[front];
        if (!page_table[idx].referenced) {
            // If R bit is 0, replace this page.
            frame_number = idx;
            break;
        } else {
            // If R bit is 1, give it a second chance.
            page_table[idx].referenced = false;
            // Move this page to the end of the queue.
            update_frame_queue(idx);
        }
    }

    // After finding, we make a place for that frame
    frame_occupied[frame_number] = true;

    // Now read the frame into the selected page
    frame_number = read_page_from_disk(page_number, physical_memory, frame_occupied);
    if (frame_number != -1) {
        update_page_table(page_table, page_number, frame_number);
        update_TLB(tlb, page_number, frame_number);
    }
    return frame_number;
}

int handle_page_fault_lru(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults) {
    (*total_page_faults)++;
    int frame_number = read_page_from_disk(page_number, physical_memory, frame_occupied);
    if (frame_number != -1) {
        update_page_table(page_table, page_number, frame_number);
        update_TLB_lru(tlb, page_number, frame_number);
    }
    return frame_number;
}
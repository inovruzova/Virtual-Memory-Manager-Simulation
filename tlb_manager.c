#include "types_constants.h"

int tlb_next_index = 0;
int tlb_usage_counter[TLB_SIZE] = {0};

void update_TLB(TLBEntry *tlb, unsigned int page_number, unsigned int frame_number) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].page_number == page_number) {
            tlb[i].frame_number = frame_number;
            return;
        }
    }

    tlb[tlb_next_index].page_number = page_number;
    tlb[tlb_next_index].frame_number = frame_number;
    tlb[tlb_next_index].valid = true;
    tlb_next_index = (tlb_next_index + 1) % TLB_SIZE;
}

int search_TLB(const TLBEntry *tlb, unsigned int page_number, int *total_tlb_hits) {
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].page_number == page_number) {
            (*total_tlb_hits)++;
            return tlb[i].frame_number;
        }
    }
    
    return -1;
}

void update_TLB_lru(TLBEntry *tlb, unsigned int page_number, unsigned int frame_number) {
    // Update TLB with new entry
    tlb[tlb_next_index].page_number = page_number;
    tlb[tlb_next_index].frame_number = frame_number;
    tlb[tlb_next_index].valid = true;

    // Increment usage counter for the updated entry
    tlb_usage_counter[tlb_next_index] = 0;

    // Increment usage counters for all other entries
    for (int i = 0; i < TLB_SIZE; i++) {
        if (i != tlb_next_index && tlb[i].valid) {
            tlb_usage_counter[i]++;
        }
    }

    // Move to the next index for next update
    tlb_next_index = (tlb_next_index + 1) % TLB_SIZE;
}

int search_TLB_lru(const TLBEntry *tlb, unsigned int page_number, int *total_tlb_hits) {
    // Find the least recently used entry
    int least_used_index = 0;
    int least_used_count = tlb_usage_counter[0];
    for (int i = 1; i < TLB_SIZE; i++) {
        if (tlb_usage_counter[i] < least_used_count) {
            least_used_index = i;
            least_used_count = tlb_usage_counter[i];
        }
    }

    // Search for the page in the TLB
    for (int i = 0; i < TLB_SIZE; i++) {
        if (tlb[i].valid && tlb[i].page_number == page_number) {
            (*total_tlb_hits)++;
            // Update usage counters for all entries
            for (int j = 0; j < TLB_SIZE; j++) {
                if (j != i && tlb[j].valid) {
                    tlb_usage_counter[j]++;
                }
            }
            // Reset usage counter for the hit entry
            tlb_usage_counter[i] = 0;
            return tlb[i].frame_number;
        }
    }

    // If the page is not found in the TLB, update least recently used entry
    tlb_usage_counter[least_used_index]++;
    return -1;
}
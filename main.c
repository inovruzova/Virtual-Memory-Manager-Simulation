#include "types_constants.h"
#include <stdio.h>
#include <stdlib.h>

extern void init_TLB(TLBEntry *tlb);
extern void init_physical_memory(unsigned char physical_memory[NUM_FRAMES][FRAME_SIZE]);
extern void init_page_table(PageTableEntry *page_table);
extern void update_page_table(PageTableEntry *page_table, unsigned int page_number, unsigned int frame_number);
extern int handle_page_fault(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults);
extern int handle_page_fault_second_chance(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults);
extern int handle_page_fault_lru(PageTableEntry *page_table, TLBEntry *tlb, unsigned int page_number, bool *frame_occupied, int *total_page_faults);
extern int search_TLB(const TLBEntry *tlb, unsigned int page_number, int *total_tlb_hits);
extern int search_TLB_lru(const TLBEntry *tlb, unsigned int page_number, int *total_tlb_hits);
extern void update_frame_queue(int frame_number);

int total_requests = 0;
int total_page_faults = 0;
int total_tlb_hits = 0;

TLBEntry tlb[TLB_SIZE];
PageTableEntry page_table[MAX_PAGES];
unsigned char physical_memory[NUM_FRAMES][FRAME_SIZE];
bool frame_occupied[NUM_FRAMES] = {false};

int main(int argc, char *argv[]) {
    int choice;
    printf("Choose algorithm (1 for FIFO, 2 for Second Chance, 3 for LRU): ");
    scanf("%d", &choice);

    const char *virtual_addr_filename = "addresses.txt";
    FILE *virtual_addr_file = fopen(virtual_addr_filename, "r");
    if (virtual_addr_file == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    unsigned int logical_address;
    unsigned int page_number;
    unsigned int offset;
    int frame_number;

    init_TLB(tlb);
    init_physical_memory(physical_memory);
    init_page_table(page_table);

    FILE *log_file = fopen("log.txt", "w");
    if (!log_file) {
        perror("Failed to open log file");
        return EXIT_FAILURE;
    }

    fprintf(log_file, "- - - - - System information - - - - -\n");
    fprintf(log_file, "Page Size: %d bytes\n", PAGE_SIZE);
    fprintf(log_file, "Number of Frames: %d\n", NUM_FRAMES);
    fprintf(log_file, "TLB Size: %d entries\n", TLB_SIZE);
    fprintf(log_file, "Physical Memory Size: %.0f KB\n", NUM_FRAMES * ((float)PAGE_SIZE / 1024));
    fprintf(log_file, "- - - - - - - - - - -\n\n");

    // Reading the logical addresses from the file
    while (fscanf(virtual_addr_file, "%u", &logical_address) != EOF) {
        total_requests++;

        page_number = logical_address / PAGE_SIZE;
        offset = logical_address % PAGE_SIZE;

        switch (choice) {
            case 1:
                frame_number = search_TLB(tlb, page_number, &total_tlb_hits);
                
                if (frame_number == -1) { // TLB Miss
                    if (page_table[page_number].valid) 
                        frame_number = page_table[page_number].frame_number;
                    else
                        frame_number = handle_page_fault(page_table, tlb, page_number, frame_occupied, &total_page_faults);   
                }
                break;
            case 2:
                frame_number = search_TLB(tlb, page_number, &total_tlb_hits);

                if (frame_number == -1) { // TLB Miss
                    if (page_table[page_number].valid) {
                        frame_number = page_table[page_number].frame_number;
                        update_frame_queue(frame_number);
                        page_table[page_number].referenced = true;
                    } else {
                        frame_number = handle_page_fault_second_chance(page_table, tlb, page_number, frame_occupied, &total_page_faults);
                    }
                }
                break;
            case 3:
                frame_number = search_TLB_lru(tlb, page_number, &total_tlb_hits);

                if (frame_number == -1) { // TLB Miss
                    if (page_table[page_number].valid) 
                        frame_number = page_table[page_number].frame_number;
                    else {
                        frame_number = handle_page_fault_lru(page_table, tlb, page_number, frame_occupied, &total_page_faults);
                        // Increment total TLB hits when TLB is updated after a TLB miss
                        total_tlb_hits++;
                    }
                }
                break;
            default:
                fprintf(stderr, "Invalid choice.\n");
                return EXIT_FAILURE;
        }
            
    }

        // Physical address calculation
        unsigned int physical_address = (frame_number * PAGE_SIZE) + offset;

        // Reading the value from the physical memory
        unsigned char value = physical_memory[frame_number][offset];

        fprintf(log_file, "Virtual Address: %u, Physical Address: %u, Value: %u\n",
                                        logical_address, physical_address, value);

    float page_fault_rate = (float)total_page_faults / total_requests * 100;
    float tlb_hit_rate = (float)total_tlb_hits / total_requests * 100;

    fprintf(log_file, "\nPage Fault Rate: %.3f%%\n", page_fault_rate);
    fprintf(log_file, "TLB Hit Rate: %.3f%%\n", tlb_hit_rate);

    fclose(virtual_addr_file);
    fclose(log_file);
    return EXIT_SUCCESS;
}
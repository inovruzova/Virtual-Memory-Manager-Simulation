#ifndef TYPES_CONSTANTS_H
#define TYPES_CONSTANTS_H

#include <stdbool.h>

#define TLB_SIZE 16
#define PAGE_SIZE 256
#define FRAME_SIZE 256 // the same as the page size
#define NUM_FRAMES 256 // memory size divided by frame size
#define MAX_PAGES 256

typedef struct {
    unsigned int page_number;
    unsigned int frame_number;
    bool valid;
    int counter; // Add counter field for LRU replacement
} TLBEntry;

typedef struct {
    unsigned int frame_number;
    bool valid;
    bool referenced;  // adding R bit for tracking usage of pages
} PageTableEntry;

extern unsigned char physical_memory[NUM_FRAMES][FRAME_SIZE];

extern bool frame_occupied[NUM_FRAMES];

#endif
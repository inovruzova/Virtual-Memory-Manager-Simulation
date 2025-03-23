#include "types_constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void update_frame_queue(int frame_number);

int read_page_from_disk(unsigned int page_number, unsigned char physical_memory[NUM_FRAMES][FRAME_SIZE], bool *frame_occupied) {
    const char *disk_filename = "disk_sim";
    FILE *disk = fopen(disk_filename, "rb");
    if (disk == NULL) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    unsigned char BUFFER[PAGE_SIZE];
    int frame_number = -1;

    long offset = (long) page_number * PAGE_SIZE;
    fseek(disk, offset, SEEK_SET);

    if(fread(BUFFER, 1, PAGE_SIZE, disk) == PAGE_SIZE) {
        for (int i = 0; i < NUM_FRAMES; i++) {
            if (!frame_occupied[i]) {
                memcpy(physical_memory[i], BUFFER, PAGE_SIZE);
                frame_occupied[i] = true;
                frame_number = i;
                update_frame_queue(frame_number);
                break;
            }
        }
    }

    fclose(disk);
    return frame_number;
}
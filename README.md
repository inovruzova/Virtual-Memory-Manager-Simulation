This project implements a simulated virtual memory management system that translates logical addresses into physical addresses using a page table and a Translation Lookaside Buffer (TLB). The simulation includes address translation, page fault handling, and memory management using replacement strategies.

Key components:

- Address translation using page tables and TLB
- Page fault handling by reading pages from a simulated disk file
- TLB and page table updates with FIFO replacement
- Output generation including page fault rate and TLB hit rate
- Modular, readable, and function-oriented code structure
- Optional extensions including Second-Chance and LRU replacement strategies

The simulator uses a 16-bit virtual address space, 64KB physical memory, and models realistic paging behavior with support for configurable memory parameters.

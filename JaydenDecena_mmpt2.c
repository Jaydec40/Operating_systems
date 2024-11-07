/*
Jayden Decena 
Project 2 Pt2
due:2/24/24
Virtual Memory Manager
Submission 2: forgot to comment a test line out
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//constant definitions for Pt.2 
const int PAGE_TABLE_SIZE = 128;
const int BUFFER_SIZE = 128;
const int PHYS_MEM_SIZE = 128;
const int TLB_SIZE = 16;

//Structure for TLB
struct TranslationBuffer {
    unsigned char page[16];
    unsigned char frame[16];
    int index;
};


//Read disk into PM
int readFromDisk(int pageNum, char *physicalMemory, int *openFrame) {
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    FILE *backingStore;
    backingStore = fopen("BACKING_STORE.bin", "rb");
    if (backingStore == NULL) {
        printf("File failed to open\n");
        exit(0);
    }

    if (fseek(backingStore, pageNum * PHYS_MEM_SIZE, SEEK_SET) != 0)
        printf("error in fseek\n");

    if (fread(buffer, sizeof(char), PHYS_MEM_SIZE, backingStore) == 0)
        printf("error in fread\n");

    int i = 0;
    for (i; i < PHYS_MEM_SIZE; i++) {
        *((physicalMemory + (*openFrame) * PHYS_MEM_SIZE) + i) = buffer[i];
        //printf("buffer[%d]=%d\n", i + pageNum * 256, buffer[i]);
    }

    (*openFrame)++;

    return (*openFrame) - 1;
}
// translate logical address to physical address
int translateAddress(int logicalAddr, char *pageTable, struct TranslationBuffer *tlb, char *physicalMemory,
                      int *openFrame, int *pageFaults, int *tlbHits) {
    unsigned char mask = 0xFF;
    unsigned char offset;
    unsigned char pageNum;
    bool TLBhit = false;
    int frame = 0;
    int value;
    int newFrame = 0;

    printf("Virtual address: %d\t", logicalAddr);

    pageNum = (logicalAddr >> 8) & mask;

    offset = logicalAddr & mask;

    int i = 0;
    //check for translation
    for (i; i < TLB_SIZE; i++) {
        if (tlb->page[i] == pageNum) {
            frame = tlb->frame[i];
            TLBhit = true;
            (*tlbHits)++;
        }
    }
		// catch if no translation found in TLB
    if (TLBhit == false) {
        if (pageTable[pageNum] != -1) {
            // Page hit 
        } else {
        	//page fault
            newFrame = readFromDisk(pageNum, physicalMemory, openFrame);
            pageTable[pageNum] = newFrame;
            (*pageFaults)++;
        }
        // lru for modification 
        frame = pageTable[pageNum];
        tlb->page[tlb->index] = pageNum;
        tlb->frame[tlb->index] = pageTable[pageNum];
        tlb->index = (tlb->index + 1) % TLB_SIZE;
    }
	//Calculate physical address and get value
    int index = ((unsigned char) frame * PHYS_MEM_SIZE) + offset;
    value = *(physicalMemory + index);
    printf("Physical address: %d\t Value: %d\n", index, value);

    return 0;
}

int main(int argc, char *argv[]) {
    int val;
    FILE *file;
    int openFrame = 0;
    int pageFaults = 0;
    int tlbHits = 0;
    int inputCount = 0;
    float pageFaultRate;
    float tlbHitRate;

    unsigned char pageTable[PAGE_TABLE_SIZE];
    memset(pageTable, -1, sizeof(pageTable));

    struct TranslationBuffer tlb;
    memset(tlb.page, -1, sizeof(tlb.page));
    memset(tlb.frame, -1, sizeof(tlb.frame));
    tlb.index = 0;

    char physMemory[PHYS_MEM_SIZE][PHYS_MEM_SIZE];
//CHeck for CL args
    if (argc < 2) {
        printf("Not enough arguments\nProgram Exiting\n");
        exit(0);
    }

    file = fopen(argv[1], "r");
    if (file == NULL) {
        printf("File failed to open\n");
        exit(0);
    }
//read logical addresses and translate 
    while (fscanf(file, "%d", &val) == 1) {
        translateAddress(val, pageTable, &tlb, (char *) physMemory, &openFrame, &pageFaults, &tlbHits);
        inputCount++;
    }
	//calculations
    pageFaultRate = (float) pageFaults / (float) inputCount;
    tlbHitRate = (float) tlbHits / (float) inputCount;
    printf("Page Fault Rate = %.4f\nTLB hit rate= %.4f\n", pageFaultRate, tlbHitRate);

    fclose(file);
    return 0;
}



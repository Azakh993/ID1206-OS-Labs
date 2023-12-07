#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FILE_SIZE 1000
#define TLB_ENTRIES 16
#define PAGE_ENTRIES 256
#define PAGE_SIZE 256
#define FRAME_ENTRIES 256
#define FRAME_SIZE 256
#define BUFFER_SIZE 16
#define DUMMY_VALUE -1

typedef struct TlbEntry {
    int pageNumber;
    int frameNumber;
} TlbEntry;

int pageTable[PAGE_ENTRIES];
int physicalMemory[FRAME_ENTRIES][FRAME_SIZE];
TlbEntry tlb[TLB_ENTRIES];

int frameNumberPointer;
int tlbPointer;
int pageFaultCounter;
int tlbHitCounter;

int extractOffset(int logicalAddress);

int extractPageOrFrameNumber(int logicalOrPhysicalAddress);

FILE* getFile(char* path, char* type);

int getLogicalAddress(char* unprocessedLogicalAddress);

int getPhysicalAddress(int logicalAddress);

int getValue(int physicalAddress);

void initCountersAndPointers();

void initGlobalVariables();

void loadPageIntoBuffer(FILE* storage, int pageNumber, char* buffer);

char* loadUnprocessedLogicalAddressIntoBuffer(char* buffer, FILE* logicalAddresses);

int getFrameNumberFromTlb(int pageNumber);

int parsePhysicalAddress(int frameNumber, int offset);

void populatePageTableWithDummyValues();

void populateTlbWithDummyValues();

void printPageFaultAndTlbHitResults();

void printVirtualPhysicalValueResults(int logicalAddress, int physicalAddress, int value);

void setPageTableEntry(int pageNumber, int frameNumber);

void setTlbEntry(int index, int pageNumber, int frameNumber);

void swapPageIntoPhysicalMemory(char* buffer);

void swapPageIntoPhysicalMemoryFromStorage(int pageNumber);

void updateTlb(int pageNumber, int frameNumber);

void validateFile(FILE* fileToBeValidated, char* message);

int main(int argc, char* argv[]) {
    initGlobalVariables();

    FILE *unprocessedLogicalAddresses = getFile(argv[1], "r");

    char buffer[BUFFER_SIZE];

    while(loadUnprocessedLogicalAddressIntoBuffer(buffer, unprocessedLogicalAddresses) != NULL) {
        int logicalAddress = getLogicalAddress(buffer);
        int physicalAddress = getPhysicalAddress(logicalAddress);
        int value = getValue(physicalAddress);

        printVirtualPhysicalValueResults(logicalAddress, physicalAddress, value);
    }

    printPageFaultAndTlbHitResults();

    fclose(unprocessedLogicalAddresses);

    return 0;
}

int extractOffset(int logicalAddress) {
    return logicalAddress & 0xFF;
}

int extractPageOrFrameNumber(int logicalOrPhysicalAddress) {
    return logicalOrPhysicalAddress >> 8;
}

FILE* getFile(char* path, char* type) {
    FILE* file = fopen(path, type);
    validateFile(file, path);

    return file;
}

int getFrameNumberFromTlb(int pageNumber) {
    for(int i = 0; i < TLB_ENTRIES; i++) {
        TlbEntry tlbEntry = tlb[i];

        if(tlbEntry.pageNumber == pageNumber) {
            return tlbEntry.frameNumber;
        }
    }

    return -1;
}

int getLogicalAddress(char* unprocessedLogicalAddress) {
    return atoi(unprocessedLogicalAddress) & 0xFFFF;
}

int getValue(int physicalAddress) {
    int frameNumber = extractPageOrFrameNumber(physicalAddress);
    int offset = extractOffset(physicalAddress);
    
    return physicalMemory[frameNumber][offset];
}

int getPhysicalAddress(int logicalAddress) {
    int pageNumber = extractPageOrFrameNumber(logicalAddress);
    int offset = extractOffset(logicalAddress);
    
    int frameNumber = getFrameNumberFromTlb(pageNumber);

    int nonExistent = -1;

    if(frameNumber != nonExistent) {
        tlbHitCounter++;
    } else {
        if(pageTable[pageNumber] != nonExistent) {
            frameNumber = pageTable[pageNumber];
        } else {
            swapPageIntoPhysicalMemoryFromStorage(pageNumber);
            setPageTableEntry(pageNumber, frameNumberPointer);
            frameNumber = frameNumberPointer++;
            pageFaultCounter++;
        }

        updateTlb(pageNumber, frameNumber);
    }
    
    return parsePhysicalAddress(frameNumber, offset);
}

void initCountersAndPointers() {
    frameNumberPointer = 0;
    tlbPointer = 0;
    pageFaultCounter = 0;
    tlbHitCounter = 0;
}

void initGlobalVariables() {
    initCountersAndPointers();
    populatePageTableWithDummyValues();
    populateTlbWithDummyValues();
}

char* loadUnprocessedLogicalAddressIntoBuffer(char* buffer, FILE* logicalAddresses) {
    return fgets(buffer, BUFFER_SIZE - 1, logicalAddresses);
}

void loadPageIntoBuffer(FILE* storage, int pageNumber, char* buffer) {
    fseek(storage, pageNumber * FRAME_SIZE, SEEK_SET);
    fread(buffer, 1, FRAME_SIZE, storage);
}

int parsePhysicalAddress(int frameNumber, int offset) {
    return (frameNumber << 8) | offset;
}

void printVirtualPhysicalValueResults(int logicalAddress, int physicalAddress, int value) {
    printf("Virtual address: %d Physical address: %d Value: %d\n", logicalAddress, physicalAddress, value);
}

void printPageFaultAndTlbHitResults() {
    printf("Page-fault rate: %f\n", pageFaultCounter / 1000.0);
    printf("TLB hit rate: %f\n", tlbHitCounter / 1000.0);
}

void populatePageTableWithDummyValues() {
    for(int i = 0; i < PAGE_ENTRIES; i++) {
        setPageTableEntry(i, DUMMY_VALUE);
    }
}

void populateTlbWithDummyValues() {
    for(int i = 0; i < TLB_ENTRIES; i++) {
        setTlbEntry(i, DUMMY_VALUE, DUMMY_VALUE);
    }
}

void setPageTableEntry(int pageNumber, int frameNumber) {
    pageTable[pageNumber] = frameNumber;
}

void setTlbEntry(int index, int pageNumber, int frameNumber) {
    tlb[index].pageNumber = pageNumber;
    tlb[index].frameNumber = frameNumber;
}

void swapPageIntoPhysicalMemory(char* buffer) {
    for(int i = 0; i < FRAME_SIZE; i++) {
        physicalMemory[frameNumberPointer][i] = buffer[i];
    }
}

void swapPageIntoPhysicalMemoryFromStorage(int pageNumber) {
    FILE* storage = getFile("BACKING_STORE.bin", "rb");
    char buffer[FRAME_SIZE];
    loadPageIntoBuffer(storage, pageNumber, buffer);
    swapPageIntoPhysicalMemory(buffer);
    fclose(storage);
}

void updateTlb(int pageNumber, int frameNumber) {
    int theLastEntry = TLB_ENTRIES - 1;
    int indexToInsertInto;

    if(tlbPointer == theLastEntry) {
        indexToInsertInto = theLastEntry;
        tlbPointer = 0;
    } else {
        indexToInsertInto = tlbPointer++;
    }

    setTlbEntry(indexToInsertInto, pageNumber, frameNumber);
}

void validateFile(FILE* fileToBeValidated, char* message) {
    if(fileToBeValidated == NULL) {
        perror(message);
        exit(1);
    }
}
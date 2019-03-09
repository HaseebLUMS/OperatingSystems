#include "stdio.h"
#include "stdlib.h"
#define specialCharacter '*'
#define pageTableSize 256
#define mainMemorySize 62 //as 2 are for page table
#define frameSize 256
#define inMemoryMask 0x00008000 //0000 0000 0000 0000 1000 0000 0000 0000
#define referenceBitMask 0x00004000 //0000 0000 0000 0000 0100 0000 0000 0000
#define modifyBitMask 0x00002000 //0000 0000 0000 0000 0010 0000 0000 0000
#define offInMemory 0xFFFF7FFF //1111 1111 0111 1111 1111 1111
#define offReferenceBit 0xBFFF
#define okayToRemoveMask 0x9FFF
#define okay1 0x9FFF
#define okay2 0xBFFF
#define okay3 0xDFFF
#define okay4 0xFFFF
#define okay2toOkay1 0xDFFF
#define rmFirst4 0x0FFF
#define okay3toOkay2 0xA000
#define okay4toOkay3 0xDFFF
#define turnOnInMemory 0x8000


int getLogicalAddress(int addr) {
	return (addr & 0xFFFF);
}


int getPageNumber(int logicalAddress) {
	return ((logicalAddress & 0xFF00) >> 8) & 0xFF;
}


int getOffset(int logicalAddress) {
	return (logicalAddress & 0xFF);
}


int giveLen(char* fileName) {
	FILE *fp = fopen(fileName, "r");

	int count = 0;char isN;
	while((isN = fgetc(fp)) != EOF){if(isN == '\n'){count++;}}

	fclose(fp);
	return count;
}
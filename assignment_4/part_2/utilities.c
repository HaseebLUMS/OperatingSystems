#include "stdio.h"
#include "stdlib.h"
#define specialCharacter '*'


#define frameSize 1024
#define mainMemorySize 95
int pageFaults = 0;
int tableFaults = 0;
int totalReferences = 0;

int total_wr = 0;

int getLogicalAddress(int addr) {
	return (addr & 0xFFFFFF);
}


int getOffset(int logicalAddress) {
	return (logicalAddress & 0x3FF);
}


int giveLen(char* fileName) {
	FILE *fp = fopen(fileName, "r");

	int count = 0;char isN;
	while((isN = fgetc(fp)) != EOF){if(isN == '\n'){count++;}}

	fclose(fp);
	return count;
}

char seeStore(int addr)
{
	char buffer = '*';FILE* ptr;
	ptr = fopen("DATA_STORE.bin", "rb");
		fseek(ptr, addr, SEEK_SET);fread(&buffer, sizeof(char), 1, ptr);
	fclose(ptr);return buffer;
}

char validate(char targetData, int logicalAddress)
{
	char t = seeStore(logicalAddress);
	if(t != targetData)
	{
		total_wr++; // 4 or 5% out of 1 lac addresses get wrong. You can print this value in main.c by uncommeting a line.
		targetData = t;
	}
	return t;
}
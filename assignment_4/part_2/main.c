#include "translate.c"


int main(int argc, char *argv[])
{
	//if(argc < 2){printf("Provide fileName as argument.\n"); return 0;}
	char* fileName = "addresses.txt";
	int fileLen = giveLen(fileName);


	FILE * fp = fopen(fileName, "r");
	if(fp == NULL) {printf("File could not be opened.\n");return 0;}
	//printf("Logical Address, Physical Address, Value, Page Fault\n");

	/*Initialing Data STructures in translate.c*/
	int a = 0, b = 0;
	for(a = 0; a< 64; a++){outerPageTable[a] = 0;} //Initializing pageTable to 0s
	for(a = 0; a< 32; a++){for(b = 0; b< frameSize; b++){innerPageTable[a][b] = -1;}}
	for(a = 0; a< mainMemorySize; a++){for(b = 0; b< frameSize; b++){mainMemory[a][b] = specialCharacter;}}



	int i = 0;
	for(i = 0; i< fileLen; i++){
		int addr = -1, flag = -1;
		fscanf(fp, "%x %d", &addr, &flag);

		/*Starting the translation business*/
		int logicalAddress = getLogicalAddress(addr);
		processLogicalAddress(logicalAddress, flag);
	}

	printf("%d %d %d\n", pageFaults, tableFaults, totalReferences);
	//printf("%d\n", total_wr);
	return 0;
}
#include "translate.c"


int main(int argc, char *argv[])
{
	//if(argc < 2){printf("Provide fileName as argument.\n"); return 0;}
	char* fileName = "addresses.txt";
	int fileLen = giveLen(fileName);


	FILE * fp = fopen(fileName, "r");
	if(fp == NULL) {printf("File could not be opened.\n");return 0;}
	printf("Logical Address, Physical Address, Value, Page Fault\n");

	/*Initialing Data STructures in translate.c*/
	int a = 0, b = 0;
	for(a = 0; a< pageTableSize; a++){pageTable[a] = 0;} //Initializing pageTable to 0s
	for(a = 0; a< mainMemorySize; a++){for(b = 0; b< frameSize; b++){mainMemory[a][b] = specialCharacter;}} //


	int total_faults = 0;
	int total_references = 0;


	int i = 0;
	for(i = 0; i< fileLen; i++){
		int addr = -1, flag = -1;
		fscanf(fp, "%x %d", &addr, &flag);

		/*Starting the translation business*/
		int logicalAddress = getLogicalAddress(addr);
		//printf("%d -> %x\n", addr, logicalAddress);
		total_references++;
		total_faults += processLogicalAddress(logicalAddress, flag);
	}

	float rate = (total_faults + 0.0)/(total_references+ 0.0);
	printf("Fault percentage = %f\n", rate*100);
	//printf("Done.\n");
	return 0;
}

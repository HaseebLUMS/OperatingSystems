#include "utilities.c"



int pageTable[pageTableSize]; //each entry required to be of 2 Bytes ==>So always use last 2 Bytes
char mainMemory[mainMemorySize][frameSize]; 
int counter = 0;


int getFreeIndexOfMainMemory();
void placeDataInMemory(char* data, int index);
char* retrieveFromBackingStore(int frameIndex);
void updatePageTable(int pageNumber, int frameNumber);
int findFrameToReplace();
int enhancedPolicy();









/*A page Fault* => 
1) get data from Backing Store 
2) place it in mainMemory 
3) update pageTable accordingly 
No free Frame =>
1) Find a frame to replace and (nextFreeIndexOfMainMemory = found frame's index) 
2) If modify 1, write the frame to backing store 
3) set the in memoery bit to 0 of to be replaced frame 
4) Set nextFreeIndexOfMainMemory*/


int processLogicalAddress(int logicalAddress, int flag) {
	int isFault = 0;
	int pageNumber = getPageNumber(logicalAddress);
	int frameNumber = 0;
	int offset = getOffset(logicalAddress);
	char targetData = ' ';
	int physicalAddress = -1;
	if(!(pageTable[pageNumber & 0xFF] & inMemoryMask)) {
		isFault = 1;
		counter = (counter + 1) % pageTableSize;
		char* retrievedData = retrieveFromBackingStore((pageNumber&0xFF)*frameSize);
		int nextFreeIndexOfMainMemory = getFreeIndexOfMainMemory();
		if(nextFreeIndexOfMainMemory == -1){
			int toBeEvictedPageNumber = enhancedPolicy();
			if(pageTable[(toBeEvictedPageNumber)] & modifyBitMask){ /*write the frame back to backing store*/}
			pageTable[(toBeEvictedPageNumber)] &= offInMemory;
			nextFreeIndexOfMainMemory = pageTable[(toBeEvictedPageNumber)] & 0xFF;
		}
		frameNumber = nextFreeIndexOfMainMemory;
		placeDataInMemory(retrievedData, nextFreeIndexOfMainMemory);
		updatePageTable(pageNumber, frameNumber);
	}


	frameNumber = (pageTable[(pageNumber & 0xFF)])&0xFF;
	physicalAddress = ((frameNumber) << 8) | (offset);
	if(flag == 1){pageTable[(pageNumber & 0xFF)] |= modifyBitMask;} //setting modify bit
	pageTable[(pageNumber & 0xFF)] |= referenceBitMask; //setting reference bit
	

	targetData = mainMemory[(frameNumber)&0xFF][offset];
	printf("0x%x 0x%x 0x%x %s\n", logicalAddress, physicalAddress & 0xFFFF, targetData, isFault?"Yes":"NO");
	return isFault;
}















int findFrameToReplace() {
	int i = 0;

	int turn = 1;
	while(turn){
		if((pageTable[i & 0xFF] & inMemoryMask)) {
			
			if((pageTable[i&0xFF] | okayToRemoveMask) == okay1){
				turn = 0;
				//printf("Found By Policy\n");
				return i&0xFF;
			}

			else if((pageTable[i&0xFF] | okayToRemoveMask) == okay2) {
				//printf("Converting 2 to 1\n");
				(pageTable[i&0xFF] &= okay2toOkay1);
			}
			
			else if((pageTable[i&0xFF] | okayToRemoveMask) == okay3) {
				//printf("Converting 3 to 2\n");
				pageTable[i&0xFF] &= rmFirst4; //0000 ---- ---- ----
				pageTable[i&0xFF] |= okay3toOkay2;
			}
			
			else if((pageTable[i&0xFF] | okayToRemoveMask) == okay4) {
				//printf("Converting 4 to 3\n");
				pageTable[i&0xFF] &= okay4toOkay3;
			}else {
				printf("Ye Kya\n");
			}
		}else{
			//printf("na yaaaaaaaaaaaaar\n");
		}
		//pageTable[i & 0xFF] &= offReferenceBit; 
		i = (i + 1) % pageTableSize;
		turn++;
	}
	/*Traverse the table and apply second chance and find victim frame*/

	return 1;
}











/*	it will find the frame to replace and return the page number of page table at which that frame exists
	This Policy's code was taken from a friend as I needed to compare the results of my own policy
	findFrameToReplace() (above function). My Policy also works but that is a bit different policy than
	usual enhanced second chance.
	My policy also also changes the dirty bits while this one does not.
*/
int enhancedPolicy(){
	while(1) {
		int i = counter;
		do{
			if ((pageTable[i]& inMemoryMask) != 0){if (pageTable[i]&0x6000 == 0){return i;}}
			i = (i+1)%frameSize;
		}while(i != counter);



		int j = counter;
		do{
			if ((pageTable[j]& inMemoryMask) != 0){if (! (pageTable[j]& referenceBitMask)){return j;}
													else {pageTable[j] = pageTable[j]& okay2;}}
			j = (j+1)%frameSize;
		}while(j != counter);
	}
}














char* retrieveFromBackingStore(int frameIndex){
	char* buffer = malloc(sizeof(char)* frameSize);
	FILE* ptr;
	ptr = fopen("BACKING_STORE.bin", "rb");
	for(int i = 0; i< frameSize; i++){
		fseek(ptr, frameIndex+i, SEEK_SET);
		fread(&buffer[i], sizeof(char), 1, ptr);
	}

	fclose(ptr);
	return buffer;
}













int getFreeIndexOfMainMemory(){
	int i = 0;
	for(i = 0; i< mainMemorySize; i++){
		if(mainMemory[i][0] == specialCharacter){return i;}
	}
	return -1;
}




void placeDataInMemory(char* data, int index) {
	int i = 0;
	for(i = 0; i< frameSize; i++){
		mainMemory[index][i] = data[i];
	}
}



void updatePageTable(int pageNumber, int frameNumber){
	pageTable[pageNumber &0xFF] = frameNumber;
	pageTable[pageNumber &0xFF] |= turnOnInMemory;
}








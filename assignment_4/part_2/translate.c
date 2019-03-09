#include "utilities.c"



char mainMemory[mainMemorySize][frameSize];
int BackingStore[64][frameSize];
int outerPageTable[64]; //simulated in one page
int innerPageTable[32][frameSize]; // 32 will be on and 32 will be off at any moment, simulated in 32 frames
int innerCounter = 0;
int outerCounter = 0;


char* retrieveFromBackingStore(int frameIndex);
int getFreeIndexOfMainMemory();
int enhancedPolicy();
int findFreeIndexOfInnerPageTable();
void placeData(int* data, int innerPageTableIndex);
void placeDataInMainMemory(char* data, int index);
void updatePageTable(int* pageTable, int offsetToInnerPageTable, int freeIndexOfMainMemory);


int processLogicalAddress(int logicalAddress, int flag)
{
	totalReferences++;
	int pageTableFault = 0;
	int memoryFault = 0;
	//	-Get FIRST 6 BITS
	int indexToOuterPageTable = ((logicalAddress & 0xFC0000) >> 18) & (0x3F);

	//	-Get Next 8 BITS
	int offsetToInnerPageTable = ((logicalAddress & 0x3FC00) >> 10) & (0xFF);

	//	-Get indexToInnerPageTable
	int indexToInnerPageTable = -1; //indexToInnerPageTable is like Page Number

	//	--Check if innerPageTable 's required page is in memory or not
	if(!(outerPageTable[indexToOuterPageTable] & 0x8000))
	{
		pageTableFault = 1; tableFaults++;
		int freeIndexOfInnerPageTable = findFreeIndexOfInnerPageTable();


		//	---set in-memory bit off
		//outerPageTable[indexToOuterPageTable] = outerPageTable[indexToOuterPageTable] & 0x40FF;

		int* fetchedFrameFromBackingStore = BackingStore[indexToOuterPageTable];
		placeData(fetchedFrameFromBackingStore, freeIndexOfInnerPageTable);
		
		outerPageTable[freeIndexOfInnerPageTable] = freeIndexOfInnerPageTable;
		outerPageTable[freeIndexOfInnerPageTable] = outerPageTable[freeIndexOfInnerPageTable] | 0x8000;
		outerPageTable[freeIndexOfInnerPageTable] = outerPageTable[freeIndexOfInnerPageTable] | 0x4000;

		indexToInnerPageTable = freeIndexOfInnerPageTable & 0xFF; //it is actually 6 bits
	}
	else
	{
		indexToInnerPageTable = outerPageTable[indexToOuterPageTable] & 0xFF;
		outerPageTable[indexToOuterPageTable] = outerPageTable[indexToOuterPageTable] | 0x4000;
	}



								/*NOW CONSIDER IT AS A SINGLE PAGE TABLE
								  AND ACCESSIGN DATA FROM MEMORY JUST L-
								  IKE PART ONE.
								*/


	int* targetPageOfInner = innerPageTable[indexToInnerPageTable];
	
	//	Now Deal It like single level page table and do business
	int frameNumber = targetPageOfInner[offsetToInnerPageTable];

	if(frameNumber == -1 || !(frameNumber & 0x80))
	{
		// Page Fault Due to page not being in memory
		memoryFault = 1; pageFaults++;
		char * retrievedData = retrieveFromBackingStore(1024* (logicalAddress/1024));
		//char * retrievedData = retrieveFromBackingStore(1024* offsetToInnerPageTable);
		int freeIndexOfMainMemory = getFreeIndexOfMainMemory(targetPageOfInner);

		placeDataInMainMemory(retrievedData, freeIndexOfMainMemory);


		innerPageTable[indexToInnerPageTable][offsetToInnerPageTable] |= 0x80;
		
		updatePageTable(targetPageOfInner, offsetToInnerPageTable, freeIndexOfMainMemory);
		frameNumber = freeIndexOfMainMemory;
	}

	frameNumber = frameNumber & (0xFFFF00);
	int finalOffset = getOffset(logicalAddress);
	char targetData = mainMemory[frameNumber][finalOffset];
	targetPageOfInner[offsetToInnerPageTable] |= 0x40;
	targetData = validate(targetData, logicalAddress);
	if(flag){targetPageOfInner[offsetToInnerPageTable] |= 0x20;}

	int physicalAddress = ((indexToInnerPageTable << 10) | (finalOffset));

	printf("0x%x 0x%x 0x%x 0x%x %s %s\n", logicalAddress, physicalAddress,indexToInnerPageTable, targetData, pageTableFault > 0? "Yes" : "No", memoryFault > 0 ? "Yes" : "No");



	return 0;
}


void updatePageTable(int* pageTable, int offsetToInnerPageTable, int freeIndexOfMainMemory)
{
	pageTable[offsetToInnerPageTable] = freeIndexOfMainMemory;
	pageTable[offsetToInnerPageTable] |= 0x80;
}


void placeDataInMainMemory(char* data, int index)
{
	for(int i = 0; i< 1024; i++)
	{
		mainMemory[index][i] = data[i];
	}
}


int getFreeIndexOfMainMemory(int* pageTable)
{
	for(int i = 0; i< 95; i++){
		if(mainMemory[i][0] == specialCharacter)
		{
			return i;
		}
	}


	//need to get a victim frame
	int go = 3;
	while(go--)
	{
		int i = 0;
		for(i = 0; i< 32; i++)
		{
			if(pageTable[i] & 0x80)
			{
				if(pageTable[i] & 0x40) //reference bit on ??
				{
					//turn it off
					pageTable[i] = pageTable[i] & 0xBF;
				}
				else
				{
					go = 0;
					pageTable[i] &= 0x7F;
					return pageTable[i] & 0xFFFF00;
				}
			}
		}
	}

	return 0;
}

char* retrieveFromBackingStore(int frameIndex)
{
	char* buffer = (char*)malloc(sizeof(char)* 1024);
	FILE* ptr;
	ptr = fopen("DATA_STORE.bin", "rb");
	for(int i = 0; i< 1024; i++){
		fseek(ptr, frameIndex+i, SEEK_SET);
		fread(&buffer[i], sizeof(char), 1, ptr);
	}

	fclose(ptr);
	return buffer;
}


void placeData(int* data, int innerPageTableIndex)
{
	for(int i = 0; i< 1024; i++)
	{
		innerPageTable[innerPageTableIndex][i] = data[i];
	}
}


int findFreeIndexOfInnerPageTable()
{
	int i = 0;
	int freeIndex = -1;
	for(i = 0; i< 32; i++)
	{
		if(innerPageTable[i][0] == -1){freeIndex = i;}
	}

	if(freeIndex == -1)
	{
		freeIndex = enhancedPolicy();
		for(int i = 0; i < 64; i++)
		{
			if(outerPageTable[i] == freeIndex)
			{
				outerPageTable[i] &= 0x40FF;
				break;
			}
		}
	}

	return freeIndex;
}


void retrieveInnerPageTable(int victim, int ind)
{
	int i = 0;
	for(i = 0; i< 1024; i++)
	{
		innerPageTable[victim][i] = BackingStore[ind][i];
	}
}


int enhancedPolicy()
{
	//policy on outerTable
	int go = 3;
	while(go--)
	{
		int i = 0;
		for(i = 0; i< 64; i++)
		{
			if(outerPageTable[i] & 0x8000)
			{
				if(outerPageTable[i] & 0x4000) //reference bit on ??
				{
					//turn it off
					outerPageTable[i] = outerPageTable[i] & 0x80FF;
				}
				else
				{
					go = 0;
					return outerPageTable[i] & 0xFF;
				}
			}
		}
		
	}

	return 0;
}
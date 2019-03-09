#include "stdio.h"
#include "stdlib.h"


char retrieveFromBackingStore(int frameIndex)
{
	char buffer = '*';
	FILE* ptr;
	ptr = fopen("DATA_STORE.bin", "rb");
	//for(int i = 0; i< 1024; i++){
		fseek(ptr, frameIndex, SEEK_SET);
		fread(&buffer, sizeof(char), 1, ptr);
	//}

	fclose(ptr);
	return buffer;
}


int main(){

	int index = 0xc414f7;
	printf("%d\n", retrieveFromBackingStore(index));
}
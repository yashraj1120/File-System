#include<stdio.h>
#include<stdlib.h>
#include"diskIO.h"
#include"hardDisk.h"


void* readBlock(size_t block_pos,FILE* fphd,size_t block_size)
{
	//FILE* fphd = fopen(hardDisk, "rb+");
	void* buffer = malloc(block_size);
	fseek(fphd, block_pos * block_size, SEEK_SET);

	if (fread(buffer, block_size, 1, fphd) == 0)
	{
		fclose(fphd);
		printf("Unable to read disk\n");
		return NULL;
	}
	//fclose(fphd);
	return buffer;
}

void writeBlock(FILE* fphd,void* block, size_t block_pos, size_t block_size)
{
	//FILE* fphd = fopen(hardDisk, "rb+");
	fseek(fphd, block_pos * block_size, SEEK_SET);
	fwrite(block, 1, block_size, fphd);
	//fclose(fphd);
}

void initNewHardDisk(char* hardDiskName, size_t block_size)
{
	FILE* newHardDisk = fopen(hardDiskName, "rb+");
	metaData *newHeader = createHeader(BLOCK_SIZE);
	fwrite(newHeader, sizeof(metaData), 1, newHardDisk);
	fclose(newHardDisk);
}

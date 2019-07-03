#ifndef __hardDisk__h
#define  __hardDisk__h

#include<stdio.h>
#include<stdlib.h>


#define BLOCK_SIZE 16384												//16 bytes each block
#define MB_100 (104857600)											//100 MB 104857600
#define TABLE_ENTRY_SIZE 32											//filename, start_block, no_of_blocks, file_size 
#define MAX_TABLE_ENTRIES 32
#define START_BLOCK 3
#define METADATA_SIZE (20+MAX_BLOCKS + (MAX_TABLE_ENTRIES*TABLE_ENTRY_SIZE))	//magic number, noOfFiles, freeSpace, bit_vectorOf, table_entries[max_table_entries]
#define FILE_STORAGE_START  (6512+1024)								//16+1024   //metaData + TABLE 
#define MAX_BLOCKS (MB_100/BLOCK_SIZE)								//MAX BLOCKS for blocksize
#define MAGIC_NUMBER 0x444E524D										//MRND	
#define HARD_DISK "hardDisk.hdd"									//hard disk name
//#define META_DATA_BLOCKS METADATA_SIZE/BLOCK_SIZE
size_t meta_data_size;

typedef struct tabledata
{
	char filename[20];
	size_t blocks;
	size_t start_block;
	size_t file_size;
}tableData;

typedef struct metaData
{
	size_t magicNumber;
	size_t block_size;
	size_t number_of_blocks;
	size_t free_space;
	size_t filesInDisk;
	char block_vector[MAX_BLOCKS];
	tableData files[MAX_TABLE_ENTRIES];
}metaData;

/*
typedef struct metaData
{
	size_t magicNumber;
	size_t block_size;
	size_t number_of_blocks;
	size_t free_blocks;
	char block_vector[4];
}metaData;


typedef struct file_meta_data
{
	size_t filesInDisk;
	tableData files[1];
}file_meta_data;
*/


tableData* createTableData(char*, size_t, size_t, size_t);
metaData *createHeader(size_t);

void hardDiskManagement_block(char* hardDiskName);
FILE* openHardDisk(char* hardDiskName);
void initNewHardDisk(char* hardDiskName,size_t);

void listTable(FILE*);
void listFiles(FILE*);

size_t sizeOfFile(FILE*);
int numberOfFiles(FILE*);

void copyToFileSys(char* file, FILE* fphd);
void writeToDisk(char* hardDisk, FILE* fp, size_t,size_t* blockStorage,size_t start_block);

void copyFromFileSys(FILE* fphd);

//HAVE TO IMPLEMENT!!!!!!!
void format(char* hardDiskName);
void deleteFile();

void debugInfo();


#endif  

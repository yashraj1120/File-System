#include<stdio.h>
#include<stdlib.h>
#include"hardDisk.h"
#include"diskIO.h"


/*tableData createTableData(char* filename, size_t start, size_t blocks, size_t file_size)
{
	tableData newTableEntry; //= (tableData*)malloc(sizeof(tableData));
	strcpy(newTableEntry.filename, filename);
	newTableEntry.start_block = start;
	newTableEntry.blocks = blocks;
	newTableEntry.file_size = file_size;
	return newTableEntry;
}*/

metaData *createHeader(size_t block_size)
{
	metaData* temp = (metaData*)malloc(METADATA_SIZE);
	//(metaData*)malloc(meta_data_size);
	temp->magicNumber = MAGIC_NUMBER;
	temp->block_size = block_size;
	temp->number_of_blocks = MB_100 / block_size;
	temp->filesInDisk = 0;
	//size_t header_size =  52 + MAX_TABLE_ENTRIES * TABLE_ENTRY_SIZE + temp->number_of_blocks;
	
	//meta_data_size = 52 + MAX_TABLE_ENTRIES * TABLE_ENTRY_SIZE + temp->number_of_blocks;
	int i = 0;
	for (i = 0; i < temp->number_of_blocks; i++)
	{
		temp->block_vector[i] = 0;
	}
	temp->block_vector[i] = '\0';
	for ( i = 0; i < 3; i++)
	{
		temp->block_vector[i] = 1;
	}
	temp->free_space = MB_100 - METADATA_SIZE ;
	return temp;
}

//---------------------Driver Function------------------------------------
void hardDiskManagement_block(char* hardDiskName)
{
	FILE* fphd = openHardDisk(hardDiskName);
	if (fphd == NULL)
	{
		return;
	}
	char* command = (char*)malloc(20);
	printf("\n");
	while (1)
	{
		fseek(fphd, 0, SEEK_SET);
		printf("Enter command : ");
		scanf("%s", command);
		if (!strcmp(command, "init"))
		{
			//printf(" : ");
			//size_t bs=0;
			//scanf("%d", &bs);
			initNewHardDisk(hardDiskName,BLOCK_SIZE);
		}
		else if (!strcmp(command, "copytofilesys"))
		{
			char* filename = (char*)malloc(20);
			printf("Enter Filename : ");
			scanf("%s", filename);
			copyToFileSys(filename, fphd);
		}
		else if (!strcmp(command, "copyfromfilesys"))
		{
			char* filename = (char*)malloc(20);
			printf("Enter Filename : ");
			scanf("%s", filename);
			copyFromFileSys(filename, fphd);
		}
		else if (!strcmp(command, "listfiles"))
		{
			listFiles(fphd);
		}
		else if (!strcmp(command, "listtable"))
		{
			listTable(fphd);
		}
		else if (!strcmp(command, "exit"))
		{
			return;
		}
		else if (!strcmp(command, "format"))
		{
			char* response = (char*)malloc(20);
			printf("The disk content will be cleared. Do you want to proceed(y/n) ? : \n");
			scanf("%s", response);
			if (strcmp(response, "no") || strcmp(response, "n") || strcmp(response, "NO"))
			{
				format(hardDiskName);
			}
			else
			{
				printf("Format cancelled\n");
			}
		}
		/*else
		{
			printf("Invalid command\n");
		}*/
	}
}
//----------------------------------------------


//--------------COPY_TO_DISK----------------------
void copyToFileSys(char* filename, FILE* fphd)
{
	FILE* fptr = fopen(filename, "rb");
	if (fptr == NULL)
	{
		printf("File Not Found!\n");
		return NULL;
	}

	int size_file = sizeOfFile(fptr);

	metaData meta_data;
	fread(&meta_data, sizeof(metaData), 1, fphd);
	size_t block_size = meta_data.block_size;
	size_t noOfFiles = meta_data.filesInDisk;

	size_t blocks_req = size_file / (block_size) + (size_file % (block_size) ? 1 : 0);
	
	if (size_file<0 || size_file > meta_data.free_space)
	{
		printf("sizefile=%d  blocks req=%d\n", size_file, blocks_req);
		printf("Cannot insert File, no Space available\n");
		return;
	}

	size_t start_block = 0;
	for (start_block = 0; meta_data.block_vector[start_block] != 0; start_block++);
	size_t* blockStorage = NULL;
	meta_data.block_vector[start_block] = 1;
	if(blocks_req > 1)
	{
		blockStorage = (size_t*)malloc((blocks_req)* sizeof(size_t));
		size_t insert_block = 0;
		for (int i = start_block+1; i<MAX_BLOCKS && insert_block < blocks_req ; i++)
		{
			if (meta_data.block_vector[i] == 0) 
			{
				blockStorage[insert_block++] = i;
				meta_data.block_vector[i] = 1;
			}
		}
		if (insert_block < blocks_req)
		{
			printf("Cannot insert File, no Space available\n");
			return;
		}
	}

	fclose(fptr);
	fptr = NULL;
	//---------------------write file to disk--------------------
	writeToDisk(fphd,filename, blocks_req,blockStorage,start_block,block_size);
	
	//-------------------------------------TABLE INSERT IMPLEMENT 
	//tableData tableEntry = createTableData(filename,blocks_req, start_block,size_file);
	strcpy(meta_data.files[noOfFiles].filename, filename);
	meta_data.files[noOfFiles].start_block = start_block;
	meta_data.files[noOfFiles].blocks = blocks_req;
	meta_data.files[noOfFiles].file_size = size_file;

	fseek(fphd, 0, SEEK_SET);
	meta_data.filesInDisk++;
	meta_data.free_space -= size_file;
	fwrite(&meta_data, 1, sizeof(metaData), fphd);
	return;
}

void writeToDisk(FILE* fphd, char* filename, size_t blocks_req, size_t* blockStorage, size_t start_block, size_t block_size)
{
	FILE* fp = fopen(filename, "rb");
	int size_file = sizeOfFile(fp);
	void* buffer = (char*)malloc(block_size);
	
	if (blocks_req <=1)
	{
		fread(buffer, 1, size_file, fp);
		writeBlock(fphd, buffer, start_block, size_file);
	}
	else
	{
		size_t count = 0;
		writeBlock(fphd, (void*)blockStorage, start_block, block_size);
		size_t blocks_pos = 0;
		while (count < blocks_req-1)
		{
			fread(buffer, 1, block_size, fp);
			writeBlock(fphd, buffer, blockStorage[blocks_pos++], block_size);
			count++;
		}
		fread(buffer, 1, size_file % block_size, fp);
		writeBlock(fphd, buffer, blockStorage[blocks_pos++], block_size);
	}
	fclose(fp);
	return;
}
//--------------------------------------------------


//--------------COPY_FROM_DISK----------------------
void copyFromFileSys(char* filename, FILE * fphd)
{

	metaData meta_data;
	fread(&meta_data, sizeof(metaData), 1, fphd);
	size_t block_size = meta_data.block_size;
	size_t noOfFiles = meta_data.filesInDisk;

	int file_count = 0;
	while (file_count < noOfFiles)
	{
		if (!strcmp(meta_data.files[file_count].filename, filename))
		{
			tableData file_data;
			memcpy(&file_data, &meta_data.files[file_count], sizeof(tableData));
			char* newfilename[20];
			printf("Enter new filename : ");
			scanf("%s", newfilename);
			FILE* output_file = fopen(newfilename, "wb");

			size_t blocks_of_file = file_data.blocks;
			size_t start_block = file_data.start_block;
			size_t size_of_file = file_data.file_size;
			void* buffer = malloc(BLOCK_SIZE);
			
			if (blocks_of_file <= 1)
			{
				buffer=readBlock(start_block, HARD_DISK, size_of_file);
				fwrite(buffer, size_of_file, 1, output_file);
			}
			else
			{
				size_t block_count = 0;
				size_t* file_blocks = (size_t*)malloc(blocks_of_file * sizeof(int));
				fseek(fphd, start_block * BLOCK_SIZE, SEEK_SET);
				fread(file_blocks, sizeof(size_t), blocks_of_file, fphd);
				while (block_count < blocks_of_file-1)
				{
					buffer = readBlock(file_blocks[block_count], fphd, BLOCK_SIZE);
					fwrite(buffer, BLOCK_SIZE, 1, output_file);
					block_count++;
				}
				buffer = readBlock(file_blocks[blocks_of_file], fphd, size_of_file % BLOCK_SIZE);
				fwrite(buffer, size_of_file % BLOCK_SIZE, 1,output_file );
				fclose(output_file);
			}
			return;
		}
		file_count++;
	}
	printf("File not found\n");
}
//--------------------------------------------------


//---------------LIST_FUNCTIONS----------------------------
void listFiles(FILE * fphd)
{
	metaData meta_data;
	fread(&meta_data, sizeof(metaData), 1, fphd);
	
	size_t noOfFiles = meta_data.filesInDisk;
	int file_count = 0;
	while (file_count < noOfFiles)
	{
		printf("%s\n",meta_data.files[file_count].filename);
		file_count++;
	}
	fseek(fphd, 0, SEEK_SET);
}

void listTable(FILE * fphd)
{
	metaData meta_data;
	fseek(fphd, 0, SEEK_SET);
	fread(&meta_data, sizeof(metaData), 1, fphd);

	size_t noOfFiles = meta_data.filesInDisk;

	int file_count = 0;
	//tableData filesInDisk[MAX_TABLE_ENTRIES];
	//memcpy(filesInDisk, meta_data.files, sizeof(tableData));
	printf("\tFILE_NAME\tSTART_BLOCK\tNO_OF_BLOCKS\tFILE_SIZE\n");
	printf("\t--------------------------------------------------------\n");
	while (file_count < noOfFiles)
	{
		printf("\t%s\t%d\t\t%d\t\t%d\n",meta_data.files[file_count].filename,
									meta_data.files[file_count].start_block,
									meta_data.files[file_count].blocks,
									meta_data.files[file_count].file_size);
		file_count++;
	}
	
}
//-----------------------------------------------------


//-------------------UTILS-------------------
size_t numberOfFiles(FILE * fphd)
{
	metaData header_data;
	fread(&header_data, sizeof(metaData), 1, fphd);
	size_t noOfFiles = header_data.filesInDisk;
	fseek(fphd, 0, SEEK_SET);
	return noOfFiles;
}

FILE* openHardDisk(char* hardDiskName)
{
	FILE* hardDisk = fopen(hardDiskName, "rb+");
	if (openHardDisk == NULL)
	{
		return;
	}
	return hardDisk;
}

int sizeOfFile(FILE * fp)
{
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

//HAVE TO IMPLEMENT!!!!!!!!!!!!!
/*
void debugInfo();
*/
//!!!!!!!!!!!!!!!!!!!

void format(char* hardDiskName)
{
	metaData* new_metaData = createHeader(BLOCK_SIZE);
	FILE* fphd = fopen(hardDiskName, "rb+");
	writeBlock(fphd, new_metaData, 0, sizeof(metaData));
}

void deleteFile(char* fileName)
{
	FILE* fphd = fopen(HARD_DISK, "rb+");
	metaData* meta_data;
	meta_data=readBlock(0, fphd, sizeof(metaData));
	int file_count = 0;
	while (file_count < meta_data->filesInDisk)
	{
		if (!strcmp(fileName, meta_data->files[file_count]))
		{
			int no_of_blocks = meta_data->files[file_count].blocks;
			int file_start_block = meta_data->files[file_count].start_block;
			int file_size = meta_data->files[file_count].file_size;

			//Updating the free size
			meta_data->free_space += file_size;


			if (no_of_blocks == 1)
			{
				meta_data->block_vector[file_start_block] = 0;	
			}
			else
			{
				char* blocks = (char*)malloc(no_of_blocks);
			}
			memcpy(meta_data->files[file_count], meta_data->files[meta_data->filesInDisk - 1], sizeof(tableData));
			meta_data->filesInDisk--;
		}
	}
}

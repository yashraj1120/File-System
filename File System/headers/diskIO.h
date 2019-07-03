#ifndef __diskIO__H_
#define  __diskIO__H_


void* readBlock(size_t  block_pos, FILE* , size_t block_size);
void writeBlock(FILE*, void* block, size_t block_pos, size_t block_size);
void initNewHardDisk(char*, size_t block_size);




#endif  

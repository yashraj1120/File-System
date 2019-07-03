#include<stdio.h>
#include"hardDisk.h"
#include"diskIO.h"


int main()
{
	char* hardDiskName = "hardDisk.hdd";
	//(char*)malloc(20);
	printf("Welcome to virtual HardDisk Manager : hardDisk.hdd\n");
	//printf("Enter hardDisk name : ");
	//scanf("%s", hardDiskName);*/
	//printf("Enter block size for hdd\n");
	//int bs = 0;
	//scanf("%d", &bs);
	//initNewHardDisk(hardDiskName,bs);
	hardDiskManagement_block(hardDiskName);
}
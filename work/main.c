#define _CRT_SECURE_NO_WARNINGS
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include "1.h"
#define MAXSYMB 256
#include<stdio.h>
#define CHECK_FALL 1
#define CHECK_OK 0
#define WRITE "wb"
#define READ "rb"
#define SIZEPAK 8
#define EMPTY 0
#define GARBAGE 9
typedef unsigned long long int ULL;
typedef unsigned long int BASE_TYPE;
typedef unsigned char UC;
main(int argc, UC *argv[])
{
	
	//argv[1] = "1.txt";//it need for debug
	// argc = 2;
	
	int result;
	char extensionEtalon[] = ".mol";
	UC*extension=0x0;
	extension=findExtension(argv[1]);
	result = 0;

	result = strncmp(extension, extensionEtalon, strnlen(argv[1], MAXSYMB));
	if (result == CHECK_OK)
	{
		result = 0;
		result = unzip(argc, argv, extensionEtalon);
	}
	else
	{
		result = 0;
		result = zip(argc, argv, extensionEtalon);
	}

	remove("file with 101.txt");// delete the file 101

	//fgetc(stdin);
	
	return 0;
}
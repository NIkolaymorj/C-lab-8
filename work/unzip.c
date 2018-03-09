#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include "1.h"
#define MAXSYMB 256
#define successfully 1
#define fall 0
#define SIZEPAK 8
#define EMPTY 0
#define GARBAGE 9
#define ERRORPointer NULL
#define WRONGNEGATIVEVALUE -100
#define CHECK_FALL 0
#define CHECK_OK 1
#define WRITE "wb"
#define WRITE_READ "wb+"
#define READ "rb"
#define INTERRUPT_COUNTER 100


int unzip(int argc, UC*argv[], char extensionEtalon[])
{int timer = 5;
printf("\rremained %.i", timer--);
	if (argc == 1)
	{
		puts("ERROR: not file");
		return 1;
	}

	int number0LastBit;//number added zerros in the last bit
	int maxlengthArray;//the number of unique charactrs.Contains the numberof rows in the table occurences.
	struct SYM *root;
	FILE*fpMOL;// result of our work
	long int lengthFpMOL;
	PSYM *psyms;
	PSYM *psymsCode;
	ULL sizeInputFile;
	int ch = ' ';
	fpMOL = fopen(argv[1], READ);
	if (fpMOL == NULL)
	{
		puts("ERROR opened the first file");
		return 1;
	}
	number0LastBit = -1;// WRONGNEGATIVEVALUE;//initialisatioon wrong value 
	psyms = 0x0;
	maxlengthArray = WRONGNEGATIVEVALUE;
	sizeInputFile = 0;
	char*extension = 0x0;
	psyms = readHeaderInfinalFile(fpMOL, &maxlengthArray, psyms, &number0LastBit, &sizeInputFile, &extension, extensionEtalon);
	if (psyms == NULL)
		return 1;
	else
		;//puts("create header in the final file was OK\n");

printf("\rremained %2.i", timer--); 

	psymsCode = 0x0;
	while (!psymsCode)
		psymsCode = (PSYM*)calloc(maxlengthArray + 2, sizeof(PSYM));

	copyParrToChange(psyms, psymsCode);

printf("\rremained %2.i", timer--); 

	root = 0x0;
	root = buildTree(psymsCode, maxlengthArray);

	//	printArrayForScreen(psyms);
	makeCodes(root);
	//printArrayForScreen(psyms);
	
	//file with changing of data of input file. outcome - Must had the file with '1' and '0'; 
	UC nameFile101[] = "file with 101.txt";
	UC *pathFile101 = 0x0;
	pathFile101 = createPathForFile(argv[1], nameFile101);
	FILE*fp101;
	fp101 = 0x0;
	for (int i = 0;fp101 == NULL;i++)// check auto open
	{
		fp101 = fopen(pathFile101, WRITE_READ);

		if (fp101 != NULL)
			break;
		printf("\rINTERRUPT_COUNTER for  file fp101 =%i", i);
		if (i >= INTERRUPT_COUNTER)
		{
			printf("\nERROR INTERRUPT_COUNTER for  file fp101 = %s\n press any buttom to exit\n", pathFile101);
			fgetc(stdin);
			break;
		}
	}
	//old fersion, bad of work
	/*fp101 = fopen("file with 101.txt", WRITE);
	if (fp101 == NULL)
	{
		puts("ERROR opened the file .101");
		return 1;
	}
	*/

printf("\rremained %2.i", timer--); 

	lengthFpMOL = readPak(fp101, fpMOL, number0LastBit);// final
	if (lengthFpMOL == EMPTY)
	{
		printf("Error create final file. EXIT");
		return 1;
	}

//	fclose(fp101);// close, for open of reading
//fp101 = fopen("file with 101.txt", READ);

printf("\rremained %2.i", timer--); 

	rewind(fp101);

	FILE *fpOut = 0x0;
	UC flagCopy[MAXSYMB] = { 0 };
	UC*newExtension = extension;
	UC* nameOutputFile = 0x0;
	do {
		nameOutputFile = changeExtensionFile(argv[1], newExtension, flagCopy);

		if (nameOutputFile == NULL)
			return 1;
		fpOut = fopen(nameOutputFile, READ);
		if (fpOut != NULL)
		{
			UC*flagCopyNew = "_copy";//// add to the name if you want to notify the user about protecting an existing file
			int lengthCopyNew = strlen(flagCopyNew);
			strncat(flagCopy, flagCopyNew, lengthCopyNew);
			fclose(fpOut);
		}
	} while (fpOut != NULL);

	fpOut = fopen(nameOutputFile, WRITE);
	if (fpOut == NULL)
	{
		printf("ERROR opened the final for record-%s", nameOutputFile);
		return 1;
	}

printf("\rremained %i\n", timer--);

	if (CHECK_FALL == createFp(fp101, root, fpOut))
	{
		printf("ERROR create file Fp");
		return 1;
	}
	else
	{
		ULL result = 0;
		if ((result = (sizeFile(fpOut) - sizeInputFile)) == 0)
			;//printf("The file has been successfully unpacked \n The adress for the file - %s\n", nameOutputFile);// need for check
		else
			printf("The file has been unpacked with corruption =%llu\n The adress for the file - %s\n", result, nameOutputFile);


	}
	if (fpOut != NULL)
	{
		fclose(fpOut);
		fpOut = NULL;
	}

	if (fp101 != NULL)
	{
		fclose(fp101);
		fp101 = NULL;
	}
	if (fpMOL != NULL)
	{
		fclose(fpMOL);
		fpMOL = NULL;
	}


	if (nameOutputFile != NULL)
	{
		free(nameOutputFile);
		nameOutputFile = NULL;
	}

	if (root != NULL)
	{
		free(root);
		root = NULL;
	}


	if (extension != NULL)
	{
		free(extension);
		extension = NULL;
	}
	

printf("\rremained %i\n", timer--);

	return 0;
}
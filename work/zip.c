#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include<string.h>
#include "1.h"
#define MAXSYMB 256
//xtern FILE *fpOut;
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
#define READ "rb"
#define INTERRUPT_COUNTER 100
int zip(int argc, UC*argv[],char extensionNew[])
{


	int number0LastBit;//number added zerros in the last bit
	int maxlengthArray;//the number of unique charactrs.Contains the numberof rows in the table occurences.
	int countGap;
	long int result;// need for checks
	struct SYM *root;
	BASE_TYPE maxCount;
	long int lengthFp101;
	long int lengthFpMOL;
	PSYM *psyms;
	PSYM *psymsCode;
	ULL numberLetter;
	TSYM* treeSym;
	int ch = ' ';
	char checkchar;
	int totalStructInfile;
	int positionEndHeader;

	int lengthInputName;
	ULL sizeInputFile;

	if (argc == 1)
	{
		puts("ERROR: not file");
		return 1;
	}
	lengthInputName = 0;
	lengthInputName = strlen(*(argv + 1));// will be use in the signature before nameFile
	FILE*fp;
	fp = 0x0;
	
	for (int i = 0;fp == NULL;i++)// check auto open
	{	
		fp = fopen(argv[1], READ);
		if (fp != NULL)
			break;
		
			// add condition
		if(i>2)		printf("\rINTERRUPT_COUNTER for  file fp =%i", i);
		
		if (i >= INTERRUPT_COUNTER)
		{
			puts("\nERROR INTERRUPT_COUNTER for  file fp = 100\n");
			break;
		}
	}
	
	//fp = fopen(argv[1], READ);// The input file
	if (fp == NULL)
	{
		puts("error opening file fp");
		return 1;// error opening  files 
	}
	for (treeSym = 0x0, numberLetter = 0;(ch = fgetc(fp)) != EOF;numberLetter++)// are creating a tree from an input file and the total quantity of letters
	{

		checkchar = ch;
		treeSym = makeTree(treeSym, ch, 128);//adress the first node

	}
	if (numberLetter == EMPTY)
	{
		printf("ERROR maketree.numberLenner = %llu", numberLetter);
		return 1;
	}

	totalStructInfile = 0;//The quantity of structs in file
	countTotalStructInTree(treeSym, &totalStructInfile);
	if (treeSym == 0x0)
	{
		puts("ERROR check to create a tree");
		return 1;// error create tree
	}
	/*/ error in auto // use for debug
	if (!check2Tree(treeSym, numberLetter))// check true of created tree
	{
		puts("ERROR; check to create a tree");
		return  1;
	}
	*/
	psyms = 0x0;
	while (!psyms)
		psyms = (PSYM*)calloc(totalStructInfile + 2, sizeof(PSYM));// add 2 cell for 
	copyTree2Parr(treeSym, psyms);////create array of pointers for structures
	maxlengthArray = 0;
	maxlengthArray = totalStructInfile;
	psyms[maxlengthArray] = NULL;
	if (!checkDataParray(psyms, numberLetter, treeSym))
	{
		puts("Error copy Tree to syms");
		return 1;
	}
	makeFrequencyForArray(psyms, numberLetter, maxlengthArray);//filling in the array of frequency of occurence

	if (!checkSumForFrequencyArray(psyms))
	{
		puts("ERROR: made frequency in the array");
		return 1;
	}

	//if necessary for the developer

	//printArrayForCheck(psyms);

	maxCount = 0;
	Tree2max(treeSym, &maxCount);// search the maximum value for a single symbol
	if (!sort(psyms, maxCount))
	{
		puts("Error: data of psyms different from data the treeSym");
		return 1;
	}
	if (!checkDataParray(psyms, numberLetter, treeSym))
	{
		puts("Error: data of psyms different from data the treeSym");
		return 1;
	}

	psymsCode = 0x0;
	while (!psymsCode)
		psymsCode = (PSYM*)calloc(maxlengthArray + 2, sizeof(PSYM));

	copyParrToChange(psyms, psymsCode);
	brushAdresses(psymsCode);//preparation for the Assembly of the tree, do cleaning old addresses
	root = 0x0;
	root = buildTree(psymsCode, maxlengthArray);//
	makeCodes(root);

	//printArrayForScreen(psyms);//if necessary for the developer

	countGap = 0;
	countGap = checkMadeCodesUsually(psyms);
	if (countGap > 0)
	{
		printf("Error makeCodes: structs have field not contained code of '0' or '1'  =%i ", countGap);
		return 1;
	}
	//printArrayForScreen(psyms);//if necessary for the developer
	FILE*fp101;
	fp101 = 0x0;
	for (int i=0;fp101 == NULL ;i++)// check auto open
	{
		fp101 = fopen("file with 101.txt", WRITE);
	
		if (fp101 != NULL)
			break;
	printf("\rINTERRUPT_COUNTER for  file fp101 =%i",i);
		if (i >= INTERRUPT_COUNTER)
		{
			puts("ERROR INTERRUPT_COUNTER for  file fp101 = 100\n");
			break;
		}
	}
	
	if (fp101 == NULL)
	{
		puts("ERROR opened file fp101");
		return 1;
	}
	long int pos = ftell(fp101);
	lengthFp101 = EMPTY;
	lengthFp101 = createFile101(fp, psyms, fp101);// 
	if (lengthFp101 == EMPTY)
	{
		puts("ERROR create File101:length file equalty ZERO ");
		return 1;
	}


	fclose(fp101);// close for open for reading

	fp101 = fopen("file with 101.txt", READ);
	if (fp101 == NULL)
	{
		puts("ERROR the second opened fp101 gor read");
		return 1;
	}
	
	UC*nameOutputFile = 0x0;
	UC*flagChange = 0x0; //"_copy";// add to the name if you want to notify the user about protecting an existing file
	if ((nameOutputFile = createNameFile(argv[1], extensionNew, flagChange)) == NULL)
		return 1;
	FILE*fpMOL;
	fpMOL = 0x0;
	fpMOL = fopen(nameOutputFile, WRITE);
	if (fpMOL == NULL)
	{
		puts("ERROR opened the final file");
		return 1;
	}


	rewind(fp101);
	pos = ftell(fp101);
	number0LastBit = WRONGNEGATIVEVALUE;//initialisatioon wrong value 
	number0LastBit = SIZEPAK - lengthFp101 % SIZEPAK;// calculate how much it will take zeros in the tail

	char*extension = 0x0;
	if ((extension = findExtension(argv[1])) == NULL)
		return 1;

	sizeInputFile = 0;
	sizeInputFile = findSizeInputFile(fp);
	result = creatHeaderInfinalFile(fpMOL, maxlengthArray, psyms, number0LastBit, sizeInputFile, extension, extensionNew);
	if (result == CHECK_FALL)
		return 1;

	positionEndHeader = WRONGNEGATIVEVALUE;
	positionEndHeader = result;
	lengthFpMOL = 0;
	lengthFpMOL = createPak(fpMOL, fp101, number0LastBit);// final
	if (lengthFpMOL == EMPTY)
	{
		printf("Error create final file. EXIT");
		return 1;
	}
	if (number0LastBit == WRONGNEGATIVEVALUE)
	{
		printf("ATTENTIION: number0LastBit: to check the final assignment");
	}

	result = WRONGNEGATIVEVALUE;
	result = lengthFp101 - ((lengthFpMOL * SIZEPAK) - number0LastBit);
	if (result != 0)
	{
		printf("ERROR: wrong creatPak or createFile101, different=%li.   Exit\n", result);
		return 1;
	}
	else
	{
		printf("The file is successfully archived, different=%li\nthe Adress for the file - %s\n", result, nameOutputFile);
	}

	if (fp != NULL)
	{
		fclose(fp);
		free(fp);
	}

	if (fp101 != NULL)
	{
		fclose(fp101);
		free(fp101);
	}
	if (fpMOL != NULL)
	{
		fclose(fpMOL);
		free(fpMOL);
	}


	if (nameOutputFile != NULL)
		free(nameOutputFile);

	if (root != NULL)
		free(root);
	if (treeSym != NULL)
		free(treeSym);
	return 0;
}
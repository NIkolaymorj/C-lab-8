#define _CRT_SECURE_NO_WARNINGS
#include "kompressor.h"
#include <stdlib.h>
#define READ "rb"
#define WRITE "wb"
#define PATH_SOURCE_FILE "kompressor.h"
#define PATH_FINAL_FILE "final.gol"
#define NAME_HEADER "GOL"


int main(int argc, char* argv[])
{
	struct SYM* root = 0;
	struct SYM* syms = 0;
	struct SYM** psyms = 0;
	int numSymb = 0;
	unsigned long long sizeFile = 0;
	char fileExtension[10] = { 0 };
	int rest=0;
	FILE *fpBegin, *fp101, *fpFinal;


	if (argc > 1)
	{
		fpBegin = fopen(argv[1],READ);
		getExtension(argv[1], fileExtension);
	}
	else
	{
		fpBegin = fopen(PATH_SOURCE_FILE, READ);
		getExtension(PATH_SOURCE_FILE, fileExtension);
	}
	if (!fpBegin)
	{
		printf("Can`t open source file\n");
		return 1;
	}
	fp101 = fopen("101.txt", WRITE);
	fpFinal = fopen(PATH_FINAL_FILE, WRITE);
	if (!fp101 || !fpFinal)
	{
		printf("Can`t open 101 and final files\n");
		return 1;
	}
		
	syms = mkSyms(fpBegin, &numSymb,&sizeFile);
	psyms = mkPsyms(syms, numSymb);
	printPsyms(psyms, numSymb);
	makeHeader(fpFinal, NAME_HEADER, numSymb, psyms, rest, sizeFile, fileExtension,1);
	root = buildTree(psyms, numSymb);
	makeCodes(root);
	printSym(syms, SIZE);
	rest = make101File(syms, fpBegin, fp101);
	makeHeader(fpFinal, NAME_HEADER, numSymb, psyms, rest, sizeFile, fileExtension, 0);
	
	if (fclose(fp101) == EOF)
		printf("Error! Can`t close file!!");
	fp101 = fopen("101.txt", READ);
	if (!fp101)
	{
		printf("Can`t open 101 file\n");
		return 1;
	}

	makeOutputFile(fp101, fpFinal);

	if (fclose(fpBegin) == EOF || fclose(fp101) == EOF || fclose(fpFinal)==EOF)
		printf("Error! Can`t close file!!");
	if (remove("101.txt") == -1)
	{
		printf("File cant remove!\n");
		return 1;
	}
	return 0;
}
#define _CRT_SECURE_NO_WARNINGS
#include "decompressor.h"
#define READ "rb"
#define WRITE "wb"
#define PATH_SOURCE_FILE "source."
#define PATH_FINAL_FILE "final.gol"


int main(int argc, char * argv[])
{
	struct SYM* root = 0;
	struct SYM* syms = 0;
	struct SYM** psyms = 0;
	int numSymb = 0;
	unsigned long long sizeFile = 0;
	char fileExtension[10] = { 0 };
	char nameSourceFile[50] = PATH_SOURCE_FILE;
	int rest = 0;
	FILE *fpArchive,*fp101,*fpSource;
	if (argc > 1)
		fpArchive = fopen(argv[1], READ);
	fpArchive = fopen(PATH_FINAL_FILE,READ);
	if (!fpArchive)
	{
		printf("Can`t open file archive!");
		return 1;
	}
	if ((syms=getHeader(fpArchive, &numSymb, syms, &rest, &sizeFile, fileExtension))==0)
	{
		printf("Can`t read header!");
		return 1;
	}
	fp101 = fopen("101.txt", WRITE);
	if (!fp101)
	{
		printf("Can`t open file 101!");
		return 1;
	}
	printSym(syms, numSymb);
	make101File(fpArchive, fp101);
	fclose(fp101);
	fopen("101.txt", READ);
	psyms=mkPsyms(syms, numSymb);
	printPsyms(psyms, numSymb);
	root=buildTree(psyms, numSymb);
	makeNameFile(nameSourceFile, fileExtension);
	fpSource = fopen(nameSourceFile, WRITE);
	if (!fpSource)
	{
		printf("Can`t open file Source!");
		return 1;
	}
	makeSourceFile(fp101, fpSource,root,rest);
	long u = ftell(fpSource);
	if (ftell(fpSource) != sizeFile)
		printf("File is corrupted!");

	if (fclose(fpArchive) == EOF || fclose(fp101) == EOF || fclose(fpSource) == EOF)
	{
		printf("Can`t close files!");
		return 1;
	}
	if (remove("101.txt") == -1)
	{
		printf("File cant remove!\n");
		return 1;
	}
	return 0;
}
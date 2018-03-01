#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "taskFinal.h"

#define SIZE 256
#define READ "rb" 
#define WRITE "wb"

int main(int argc, char* argv[])
{
	int sizeExp = 0;
	int tail = 0;
	int count = 0;
	int countUnique = 0;
	SYM_t *root = NULL;
	SYM_t *syms = (SYM_t*)calloc(SIZE, sizeof(SYM_t));
	SYM_t *psysms[SIZE] = { 0 };
	FILE *fp;
	FILE *wp;
	wp = fopen("zero-one.txt", WRITE);
	if (argc > 1)
		fp = fopen(argv[1], READ);
	else
		fp = fopen("left.bmp", READ);
	if (!fp)
	{
		perror("File:");
		return 1;
	}
	createBuf(syms);
	rewind(fp);
	count = simbolCounter(fp, syms);
	countUnique = UniqueSimbol(syms);
	bubleSort(syms);
	tofraction(syms, count);
	printTable(syms);
	
	for (int i = 0; i < SIZE; i++)
		psysms[i] = &syms[i];

	root = buildTree(psysms, countUnique);
	makeCodes(root);

	rewind(fp);

	codefile(fp, wp, syms, countUnique);

	fclose(fp);
	fclose(wp);

	fp = fopen("zero-one.txt", READ);
	if (!fp)
	{
		perror("File:");
		return 1;
	}

	tail = seekTail(fp);

	rewind(fp);
	wp = fopen("packfile.txt", WRITE);

	char *sign = "per";
	for (int i = 0; i < 3; i++)
		fputc(sign[i], wp);
	fprintf(wp, "%03d", countUnique);
	tableToFile(syms, countUnique, wp);
	fprintf(wp, "%d", tail);
	int i = 0;
	int j = 0;
	if (argc > 1)
	{
		while (1)
		{
			if (argv[1][i] == '.')
			{
				i++;
				while (argv[1][i] != '\0')
				{
					i++;
					sizeExp++;
				}
			}
			if (argv[1][i] == '\0')
				break;
			i++;
		}
	}
	fprintf(wp, "%d", sizeExp);
	fprintf(wp, "%d", count);
	char exp[10] = { 0 };
	i = 0;
	j = 0;
	if (argc > 1)
	{
		while (1)
		{
			if (argv[1][i] == '.')
			{
				i++;
				while (argv[1][i] != '\0')
				{
					exp[j++] = argv[1][i++];
				}
			}
			if (argv[1][i] == '\0')
				break;
			i++;
		}
		for (int i = 0; i < j; i++)
			fputc(exp[i], wp);
	}

	packfun(fp, wp);

	fclose(fp);
	fclose(wp);

	return 0;
}
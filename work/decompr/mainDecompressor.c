#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "taskDecompressor.h"

#define SIZE 256
#define READ "rb" 
#define WRITE "wb"

int main(int argc, char* argv[])
{
	int sizeExp = 0;
	char fileName[SIZE] = { 0 };
	char exp[10] = { 0 };
	char tmp = 0;
	int sizeFile = 0;
	int tail = 0;
	SYM_t *root = NULL;
	SYM_t *psysms[SIZE] = { 0 };
	FILE *fp;
	FILE *wp;
	wp = fopen("zero-one.txt", WRITE);
	if (argc > 1)
		fp = fopen(argv[1], READ);
	else
		fp = fopen("packfile.txt", READ);
	if (!fp)
	{
		perror("File:");
		return 1;
	}
	int countUnique = 0;
	char *sign = "per";
	char buf[SIZE] = { 0 };
	for (int i = 0; i < 3; i++)
		buf[i] = fgetc(fp);
	if (strcmp(buf, sign) == 0)
	{
		for (int i = 0; i < 3; i++)
			buf[i] = fgetc(fp);
		countUnique = atoi(buf);


		SYM_t *syms = (SYM_t*)calloc(countUnique, sizeof(SYM_t));
		createBuf(syms, fp, countUnique);
		for (int i = 0; i < countUnique; i++)
			psysms[i] = &syms[i];
		root = buildTree(psysms, countUnique);
		makeCodes(root);
		int i = 0;
		buf[i++] = fgetc(fp);
		buf[i] = '\0';
		tail = atoi(buf);
		i = 0;
		buf[i++] = fgetc(fp);
		buf[i] = '\0';
		sizeExp = atoi(buf);
		i = 0;
		while (1)
		{
			buf[i] = fgetc(fp);
			if (buf[i] > '9' || buf[i] < '0')
			{
				sizeFile = atoi(buf);
				tmp = buf[i];
				break;
			}
			i++;
		}
		exp[0] = tmp;
		for (int i = 1; i < sizeExp; i++)
			exp[i] = fgetc(fp);

		char *name = "depackFile.";
		for (i = 0; i < 11; i++)
			fileName[i] = name[i];
		for (int j = 0; j < sizeExp; j++)
			fileName[i++] = exp[j];
		fileName[i++] = '\0';



		depackfun(fp, wp);
		fclose(fp);
		fclose(wp);
		fp = fopen("zero-one.txt", READ);
		if (!fp)
		{
			perror("File:");
			return 1;
		}
		wp = fopen(fileName, WRITE);

		fileRecovery(fp, root, wp, tail);

		fclose(fp);
		fclose(wp);
	}
	else
	{
		printf("Unknown file!!!\n");
		return 1;
	}

	return 0;
}
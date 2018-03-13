#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _SYM
{
	unsigned char ch; // ASCII-код
	float freq; // частота встречаемости
	char code[256]; // массив для нового кода
	struct _SYM *left; // левый потомок в дереве
	struct _SYM *right; // правый потомок в дереве
} SYM;

typedef union _CODE {
	unsigned char ch;
	struct {
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;
	} byte;
} CODE;

int compare(const void * x1, const void * x2)
{
	const SYM *sym1 = *(const SYM **)x1;
	const SYM *sym2 = *(const SYM **)x2;

	if (sym2->freq > sym1->freq)
		return 1;
	if (sym2->freq < sym1->freq)
		return -1;
	return 0;
}

SYM* buildTree(SYM *psym[], int N)
{
	// создаём временный узел
	SYM *temp = (SYM *)malloc(sizeof(SYM));

	// в поле частоты записывается сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;

	// связываем созданный узел с двумя последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;

	if (N == 2)
	{
		// мы сформировали корневой элемент с частотой 1.0
		return temp;
	}

	// добавляем temp в нужную позицию psym,
	// сохраняя порядок убывания частоты

	for (int i = 0; i < N; i++)
	{
		if (temp->freq > psym[i]->freq)
		{
			for (int j = N - 1; j > i; j--)
				psym[j] = psym[j - 1];

			psym[i] = temp;

			break;
		}
	}

	return buildTree(psym, N - 1);
}

void makeCodes(SYM *root)
{
	if (root->left)
	{
		strcpy(root->left->code, root->code);
		strcat(root->left->code, "0");
		makeCodes(root->left);
	}

	if (root->right)
	{
		strcpy(root->right->code, root->code);
		strcat(root->right->code, "1");
		makeCodes(root->right);
	}
}

int main(int argc, const char *argv[])
{
	if (argc != 2)
	{
		printf("Missing file name\n");
		return 1;
	}

	const char* inputFileName = argv[1];

	char tempFileName[_MAX_PATH] = { 0 };
	strcat(tempFileName, inputFileName);
	strcat(tempFileName, ".101");

	char outputFileName[_MAX_PATH] = { 0 };
	strcat(outputFileName, inputFileName);
	strcat(outputFileName, ".zz");

	FILE* inFile = fopen(inputFileName, "r");
	if (inFile == NULL)
	{
		printf("Can't open file %s\n", inputFileName);
		return 1;
	}

	int chNum[256] = { 0 };
	int totalNum = 0;
	
	while (1)
	{
		int ch = fgetc(inFile);
		if (ch == EOF)
			break;

		chNum[(unsigned int)ch]++;
		totalNum++;
	}

	int inputFileSize = ftell(inFile);

	if (totalNum == 0)
	{
		printf("File %s is empty\n", argv[1]);
		return 1;
	}

	float chFreq[256];
	for (int i = 0; i < 256; ++i)
	{
		chFreq[i] = (float)chNum[i] / totalNum;
	}
	
	SYM symTbl[256];
	memset(symTbl, 0, sizeof(symTbl));

	SYM *pSymTbl[256] = { NULL };

	int nonNullCount = 0;

	for (int i = 0; i < 256; ++i)
	{
		if (chNum[i] != 0)
		{
			symTbl[nonNullCount].ch = i;
			symTbl[nonNullCount].freq = chFreq[i];
			pSymTbl[nonNullCount] = &symTbl[nonNullCount];
			++nonNullCount;
		}
	}

	qsort(pSymTbl, nonNullCount, sizeof(SYM*), compare);

	SYM *root = buildTree(pSymTbl, nonNullCount);

	makeCodes(root);

	rewind(inFile);
	FILE *tempFile = fopen(tempFileName, "w");

	int tempFileSize = 0;

	while (1)
	{
		int ch = fgetc(inFile);
		if (ch == EOF)
			break;

		for (int i = 0; i < nonNullCount; i++)
		{
			if (ch == symTbl[i].ch)
			{
				fprintf(tempFile, "%s", symTbl[i].code);
				tempFileSize += strlen(symTbl[i].code);
				break;
			}
		}
	}

	int rem = tempFileSize % 8;

	fclose(tempFile);
	tempFile = fopen(tempFileName, "r");

	FILE *outputFile = fopen(outputFileName, "wb");

	fwrite("zz!!!", sizeof(char), 5, outputFile);
	fwrite(&nonNullCount, sizeof(int), 1, outputFile);
	for (int i = 0; i < nonNullCount; i++)
	{
		fwrite(&symTbl[i].ch, sizeof(unsigned char), 1, outputFile);
		fwrite(&symTbl[i].freq, sizeof(float), 1, outputFile);
	}
	fwrite(&inputFileSize, sizeof(int), 1, outputFile);
	fwrite(&tempFileSize, sizeof(int), 1, outputFile);
	fwrite(&rem, sizeof(int), 1, outputFile);

	int symArr[8];

	int j = 0;
	for (int i = 0; i < (tempFileSize / 8) * 8; i++)
	{
		symArr[j] = fgetc(tempFile);

		if (j == 7)
		{
			CODE code;
			code.byte.b1 = symArr[0] - '0';
			code.byte.b2 = symArr[1] - '0';
			code.byte.b3 = symArr[2] - '0';
			code.byte.b4 = symArr[3] - '0';
			code.byte.b5 = symArr[4] - '0';
			code.byte.b6 = symArr[5] - '0';
			code.byte.b7 = symArr[6] - '0';
			code.byte.b8 = symArr[7] - '0';

			fwrite(&code.ch, sizeof(unsigned char), 1, outputFile);

			j = 0;
		}
		else
		{
			j++;
		}
	}

	memset(symArr, 0, sizeof(symArr));

	for (int i = 0; i <= rem; i++)
	{
		symArr[i] = fgetc(tempFile);

		if (i == rem)
		{
			CODE code;
			code.byte.b1 = symArr[0] - '0';
			code.byte.b2 = symArr[1] - '0';
			code.byte.b3 = symArr[2] - '0';
			code.byte.b4 = symArr[3] - '0';
			code.byte.b5 = symArr[4] - '0';
			code.byte.b6 = symArr[5] - '0';
			code.byte.b7 = symArr[6] - '0';
			code.byte.b8 = symArr[7] - '0';

			fwrite(&code.ch, sizeof(unsigned char), 1, outputFile);
		}
	}

	fclose(outputFile);
	fclose(tempFile);
	fclose(inFile);

	return 0;
}

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

void checkCode(unsigned short bit, char curCode[], int* curCodeInd, int nonNullCount, SYM symTbl[], FILE* outFile)
{
	curCode[*curCodeInd] = bit == 0 ? '0' : '1';
	++(*curCodeInd);

	for (int i = 0; i < nonNullCount; i++)
	{
		if (strcmp(curCode, symTbl[i].code) == 0)
		{
			fprintf(outFile, "%c", symTbl[i].ch);

			memset(curCode, 0, 256);
			*curCodeInd = 0;

			break;
		}
	}
}

int main(int argc, const char* argv[])
{
	if (argc != 2)
	{
		printf("Missing file name\n");
		return 1;
	}

	const char* inputFileName = argv[1];

	char* inputExt = strstr(inputFileName, ".zz");
	if (inputExt == NULL)
	{
		printf("Invalid file %s\n", inputFileName);
		return 1;
	}

	char outputFileName[_MAX_PATH] = { 0 };
	memcpy(outputFileName, inputFileName, inputExt - inputFileName);

	FILE* inFile = fopen(inputFileName, "rb");
	if (inFile == NULL)
	{
		printf("Can't open file %s\n", inputFileName);
		return 1;
	}

	char hdr[6] = { 0 };
	fread(hdr, sizeof(char), 5, inFile);

	if (strcmp(hdr, "zz!!!") != 0)
	{
		printf("Invalid file %s\n", inputFileName);
		return 1;
	}

	int nonNullCount = 0;
	fread(&nonNullCount, sizeof(int), 1, inFile);

	SYM symTbl[256];
	memset(symTbl, 0, sizeof(symTbl));

	for (int i = 0; i < nonNullCount; i++)
	{
		fread(&symTbl[i].ch, sizeof(unsigned char), 1, inFile);
		fread(&symTbl[i].freq, sizeof(float), 1, inFile);
	}
	
	int inputFileSize;
	fread(&inputFileSize, sizeof(int), 1, inFile);

	int tempFileSize;
	fread(&tempFileSize, sizeof(int), 1, inFile);

	int rem = 0;
	fread(&rem, sizeof(int), 1, inFile);

	SYM *pSymTbl[256] = { NULL };

	for (int i = 0; i < nonNullCount; ++i)
	{
		pSymTbl[i] = &symTbl[i];
	}

	qsort(pSymTbl, nonNullCount, sizeof(SYM*), compare);

	SYM *root = buildTree(pSymTbl, nonNullCount);

	makeCodes(root);

	FILE* outFile = fopen(outputFileName, "w");

	char curCode[256] = { 0 };
	int curCodeInd = 0;

	for (int i = 0; i < tempFileSize / 8; ++i)
	{
		CODE code;
		if (fread(&code.ch, sizeof(unsigned char), 1, inFile) != 1)
		{
			printf("1 : Invalid file %s\n", inputFileName);
			return 1;
		}

		checkCode(code.byte.b1, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b2, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b3, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b4, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b5, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b6, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b7, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		checkCode(code.byte.b8, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
	}

	if (rem > 0)
	{
		CODE code;
		if (fread(&code.ch, sizeof(unsigned char), 1, inFile) != 1)
		{
			printf("2 : Invalid file %s\n", inputFileName);
			return 1;
		}

		if (rem >= 1)
			checkCode(code.byte.b1, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 2)
			checkCode(code.byte.b2, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 3)
			checkCode(code.byte.b3, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 4)
			checkCode(code.byte.b4, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 5)
			checkCode(code.byte.b5, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 6)
			checkCode(code.byte.b6, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 7)
			checkCode(code.byte.b7, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
		if (rem >= 8)
			checkCode(code.byte.b8, curCode, &curCodeInd, nonNullCount, symTbl, outFile);
	}

	int outputFileSize = ftell(outFile);
	if (outputFileSize != inputFileSize)
	{
		printf("Oooops!!!!!\n");
	}

	fclose(inFile);
	fclose(outFile);

	return 0;
}

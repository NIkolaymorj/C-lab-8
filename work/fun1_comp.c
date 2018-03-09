#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fun_comp.h"

static int compare(const void *a, const void *b)
{
	if (((SSYM*)a)->freq < ((SSYM*)b)->freq)
		return 1;
	else
		return -1;
}

void lineSort(SSYM *arr, int size)
{
	qsort(arr, size, sizeof(SSYM), compare);
}

void mySort(SSYM *syms)
{
	float hold = 0;
	char save = '\0';
	for (int pass = 1; pass < SYM_NUM; pass++)
	{
		for (int i = 0; i < SYM_NUM - 1; i++)
		{
			if (syms[i].freq < syms[i + 1].freq)
			{
				hold = syms[i + 1].freq;
				syms[i + 1].freq = syms[i].freq;
				syms[i].freq = hold;
				save = syms[i + 1].ch;
				syms[i + 1].ch = syms[i].ch;
				syms[i].ch = save;
			}
		}
	}
}

void clearStruct(SSYM *arr)
{
	for (int i = 0; i < SYM_NUM; i++)
	{
		arr[i].ch = i;
		arr[i].freq = 0.0f;
		for (int j = 0; j < SYM_NUM; j++)
			arr[i].code[j] = '\0';
		arr[i].left = NULL;
		arr[i].right = NULL;
	}
}

void fillStruct(FILE *fp, SSYM *arr, unsigned int *countBtInit, unsigned int *countUniqSym)
{
	int ch;
	int tmpArr[SYM_NUM] = { 0 };
	*countBtInit = 0;
	*countUniqSym = 0;
	rewind(fp);

	while ((ch = fgetc(fp)) != EOF)
	{
		arr[(unsigned int)ch].freq++;
		tmpArr[(unsigned int)ch]++;
		(*countBtInit)++;
	}

	int sum = 0;
	for (int i = 0; i < SYM_NUM; i++)
	{
		sum += tmpArr[i];
		arr[i].freq = ((float)tmpArr[i]) / (*countBtInit);
		if (tmpArr[i] != 0)
			(*countUniqSym) += 1;
	}

	//lineSort(arr, SYM_NUM);
	mySort(arr);
}

void linkPtrs(SSYM *arr, SSYM **parr)
{
	for (int i = 0; i < SYM_NUM; i++)
		parr[i] = &(arr[i]);
}

int findFileExt(char *inStr, char *extStr)
{
	int len = strlen(inStr), k = 0;
	for (int i = (len - 1); i >= 0; i--)
	{
		if (inStr[i] == '.')
		{
			k = i;
			break;
		}
	}

	for (int i = (k + 1); i < len; i++)
		extStr[i - (k + 1)] = inStr[i];
	extStr[len - (k + 1)] = '\0';

	return (k + 1);
}

unsigned int writeCodesIn101(FILE *fp_in, FILE *fp_101, SSYM *arr)
{
	rewind(fp_in);
	rewind(fp_101);
	int ch; // код символа из файла
	unsigned int countCodeSumBits = 0;
	while ((ch = fgetc(fp_in)) != -1)
	{
		for (int i = 0; i < SYM_NUM; i++)
		{
			if (arr[i].ch == (unsigned char)ch) 
			{
				countCodeSumBits += strlen(arr[i].code);
				fputs(arr[i].code, fp_101); // выводим строку с кодом
				break; // прерываем поиск
			}
		}
	}
	return countCodeSumBits;
}

void writeHeader(FILE *fp_fin, SSYM *arr, char *extStr, unsigned int countBtInit, unsigned int countCodeSumBits, unsigned int countUniqSym)
{
	rewind(fp_fin);
	// Field #1
	fwrite("CompCLan!", sizeof(char), 9, fp_fin); 
	// Field #2
	unsigned char countUniqSym2 = countUniqSym - 1;
	fwrite(&countUniqSym2, sizeof(unsigned char), 1, fp_fin);
	// Field #3
	for (int i = 0; i < countUniqSym; i++) 
	{
		fwrite(&(arr[i].ch), sizeof(unsigned char), 1, fp_fin);
		fwrite(&(arr[i].freq), sizeof(float), 1, fp_fin);
	}
	// Field #4
	unsigned char tailLen = countCodeSumBits % 8;
	fwrite(&tailLen, sizeof(unsigned char), 1, fp_fin); 
	// Field #5
	fwrite(&countBtInit, sizeof(unsigned int), 1, fp_fin); 
	// Field #6
	fwrite(extStr, sizeof(char), strlen(extStr), fp_fin); 
	char tmp = '\0';
	fwrite(&tmp, sizeof(char), 1, fp_fin);
}

void readHeader(FILE *fp_fin, SSYM *arr, unsigned int *numUniqSym, unsigned char *tailLen, unsigned int *totBtInit, char *extStr)
{
	rewind(fp_fin);
	char signArr[9] = { '\0' };
	// Field #1
	fread(signArr, sizeof(char), 9, fp_fin);
	// Field #2
	unsigned char numUniqSym2 = 0;
	fread(&numUniqSym2, sizeof(unsigned char), 1, fp_fin);
	*numUniqSym = numUniqSym2 + 1;
	// Field #3
	for (int i = 0; i < (*numUniqSym); i++)
	{
		fread(&(arr[i].ch), sizeof(unsigned char), 1, fp_fin);
		fread(&(arr[i].freq), sizeof(float), 1, fp_fin);
	}
	// Field #4
	*tailLen = 0;
	fread(tailLen, sizeof(unsigned char), 1, fp_fin);
	// Field #5
	*totBtInit = 0;
	fread(totBtInit, sizeof(unsigned int), 1, fp_fin);
	// Field #6
	char tmp = '0';
	int k = 0;
	while (tmp != '\0')
	{
		fread(&tmp, sizeof(char), 1, fp_fin);
		extStr[k++] = tmp;
	}
	extStr[k] = '\0';
}

void writeFinBinData(FILE *fp_fin, FILE *fp_101)
{
	rewind(fp_101);
	int ch; // код символа из файла
	int byteCount = 0, count2 = 0;
	unsigned char buf1[8] = { '\0' };
	unsigned char c2 = 0;
	while ((ch = fgetc(fp_101)) != -1)
	{
		count2++;
		buf1[byteCount % 8] = (unsigned char)ch;
		if (((byteCount + 1) % 8) == 0)
		{
			c2 = pack(buf1);
			fwrite(&c2, sizeof(unsigned char), 1, fp_fin);
			for (int m = 0; m < 8; m++)
				buf1[m] = (unsigned char)'0';
			count2 = 0;
		}
		byteCount++;
	}

	// Write tail
	c2 = pack(buf1);
	fwrite(&c2, sizeof(unsigned char), 1, fp_fin);
}

void fromChar2Byte(unsigned char sym, char *strByte)
{
	for (int i = 0; i < 8; i++)
		strByte[i] = '\0';

	unsigned char tmp1 = 0;
	for (int i = 7; i >= 0; i--)
	{
		tmp1 = 1 << i;
		strByte[i] = ((sym & tmp1) != 0) + '0';
	}
}

void decodeBinData(FILE *fp_fin, FILE *fp_fin_dec, SSYM *p, unsigned int countBtInit)
{
	//rewind(fp_fin);
	rewind(fp_fin_dec);

	int ch;
	char strByte[8] = { '\0' };
	SSYM *tmp = NULL, *p2 = p;
	unsigned int countBt = 0;

	while ((ch = fgetc(fp_fin)) != -1)
	{
		fromChar2Byte((unsigned char)ch, strByte);
		for (int i = 0; i < 8; i++)
		{
			if (strByte[i] == '1')
				tmp = p2->right;
			else if (strByte[i] == '0')
				tmp = p2->left;
			p2 = tmp;

			if ((p2->left == NULL) && (p2->right == NULL))
			{
				countBt++;
				fwrite(&(p2->ch), sizeof(unsigned char), 1, fp_fin_dec);
				p2 = p;
				if (countBtInit == countBt)
					return;
			}
		}
	}
}

void createFileNames(char* in, char* f101, char* comp, char *decomp)
{
	char extStr[20] = { '\0' };
	int exIdx = findFileExt(in, extStr);
	
	strcpy(f101, in);
	strcpy(f101 + exIdx, "f101");

	strcpy(comp, in);
	strcpy(comp + exIdx, "comp");

	strcpy(decomp, in);
	strcpy(decomp + (exIdx - 1), "_dec.");
	strcpy(decomp + (exIdx + 4), extStr);
}
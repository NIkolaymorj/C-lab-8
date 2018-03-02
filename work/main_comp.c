#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fun_comp.h"

int main(int argc, char *argv[])
{
	char ext1[NAME_SIZE] = { '\0' };
	int i = 0;
	while (argv[1][i] != '\0' && argv[1][i] != '\n')
	{
		ext1[i] = argv[1][i];
		i++;
	}
	char extStr[NAME_SIZE] = { '\0' };
	findFileExt(ext1, extStr);
	char ext2[NAME_SIZE] = { '\0' };
	char ext3[NAME_SIZE] = { '\0' };
	char ext4[NAME_SIZE] = { '\0' };
	createFileNames(ext1, ext2, ext3, ext4);

	FILE *fp_in = fopen(ext1, "rb"); // исходный файл
	FILE *fp_101 = fopen(ext2, "w+b"); // закодированный файл
	FILE *fp_fin = fopen(ext3, "w+b"); // сжатый файл
	FILE *fp_fin_dec = fopen(ext4, "w+b"); // восстановленный файл
	
	// Compressor
	SSYM arr[SYM_NUM];
	SSYM* parr[SYM_NUM] = { NULL };
	unsigned int numBtInit = 0;
	unsigned int numUniqSym = 0;

	clearStruct(arr);
	fillStruct(fp_in, arr, &numBtInit, &numUniqSym);
	linkPtrs(arr, parr);
	SSYM *p = buildTree(parr, numUniqSym);
	makeCodes(p);
	unsigned int numCodeSumBits = writeCodesIn101(fp_in, fp_101, arr);
	writeHeader(fp_fin, arr, extStr, numBtInit, numCodeSumBits, numUniqSym);
	writeFinBinData(fp_fin, fp_101);

	// Decompressor
	SSYM arr2[SYM_NUM];
	SSYM* parr2[SYM_NUM] = { NULL };
	char extStr2[NAME_SIZE] = { '\0' };
	unsigned int numUniqSym2;
	unsigned char tailLen2;
	unsigned int numBtInit2;

	clearStruct(arr2);
	readHeader(fp_fin, arr2, &numUniqSym2, &tailLen2, &numBtInit2, extStr2);
	linkPtrs(arr2, parr2);
	SSYM *p2 = buildTree(parr2, numUniqSym2);
	makeCodes(p2);
	decodeBinData(fp_fin, fp_fin_dec, p2, numBtInit2);

	fclose(fp_in);
	fclose(fp_101);
	fclose(fp_fin);
	fclose(fp_fin_dec);
	return 0;
}

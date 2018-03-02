#include "Header.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

struct SYM * fileAnalysis(unsigned char *fileByte, long int countByte, int *sizeArrSyms)
{
	int countNaimChar = 0;	//Количество уникальных байтов
	unsigned long int countChar[256] = { 0 };

	for (int i = 0; i < countByte; i++)
	{
		countChar[fileByte[i]]++;
	}
	//Вычисляем количество уникальных байтов
	for (int i = 0; i < 256; i++)
	{
		if (countChar[i] != 0)
		{
			countNaimChar++;
		}
	}
	struct SYM * syms = (struct SYM *)calloc(countNaimChar, sizeof(struct SYM));	//Выделяем память под массив структур SYM
	for (int i = 0; i < 256; i++)
	{
		if (countChar[i] != 0)
		{
			syms[*sizeArrSyms].ch = i;
			syms[*sizeArrSyms].freq = (float)(countChar[i]) / countByte;
			(*sizeArrSyms)++;			
		}
	}
	return syms;
}
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "taskFinal.h"

#define SIZE 256

void createBuf(SYM_t *syms)
{
	for (int i = 0; i < SIZE; i++)
	{
		(syms + i)->ch = i;
		for (int j = 0; j < MAXSTRING; j++)
			(syms + i)->code[j] = 0;
		(syms + i)->freq = 0;
		(syms + i)->left = NULL;
		(syms + i)->right = NULL;
	}
}


int simbolCounter(FILE *fp, SYM_t *syms)
{
	unsigned char s = 0;
	int count = 0;
	while (1)
	{
		if (fscanf(fp, "%c", &s) == -1)
			break;
		count++;
		for (int i = 0; i < SIZE; i++)
			if ((syms + i)->ch == s)
			{
				(syms + i)->freq++;
				break;
			}
	}
	return count;
}

int UniqueSimbol(SYM_t *syms)
{
	int count = 0;
	for (int i = 0; i < SIZE; i++)
	{
		if ((syms + i)->freq > 0)
			count++;
	}
	return count;
}

void bubleSort(SYM_t *syms)
{
	SYM_t tmp = { 0 };
	int j = SIZE - 1;
	for (int i = 0; i < j; i++)
	{
		for (; j > i; j--)
		{
			if ((syms + i)->freq < (syms + j)->freq)
			{
				tmp = *(syms + i);
				*(syms + i) = *(syms + j);
				*(syms + j) = tmp;
			}
		}
		j = SIZE - 1;
	}
}

void tofraction(SYM_t *syms, int count)
{
	for (int i = 0; i < SIZE; i++)
		(syms + i)->freq = (syms + i)->freq / count;
	for (int i = 0; i < SIZE; i++)
		(syms + i)->freq = ((int)((syms + i)->freq * 10000)) / 10000.0;
}

void printTable(SYM_t *syms)
{
	for (int i = 0; i < SIZE; i++)
		printf("%c - %f\n", (syms + i)->ch, (syms + i)->freq);
}
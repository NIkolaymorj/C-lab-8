#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "taskFinal.h"

#define SIZE 256


struct SYM* buildTree(SYM_t *psym[], int N)
{
	// создаём временный узел
	struct SYM *temp = 0x0;
	while (temp == NULL)
		temp = (struct SYM*)malloc(sizeof(struct SYM));
	// в поле частоты записывается сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// связываем созданный узел с двумя последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2) // мы сформировали корневой элемент с частотой 1.0
		return temp;

	int j = N - 3;
	psym[N - 1] = 0;
	psym[N - 2] = 0;
	for (; j >= 0; j--)
	{
		if (psym[j]->freq < temp->freq)
		{
			psym[j + 1] = psym[j];
		}
		else
		{
			psym[j + 1] = temp;
			break;
		}
		if (j == 0 && temp->freq > psym[j]->freq)
			psym[j] = temp;
	}

	// добавляем temp в нужную позицию psym,
	// сохраняя порядок убывания частоты
	return buildTree(psym, N - 1);
}

void makeCodes(SYM_t *root)
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

void codefile(FILE *fp, FILE * wp, SYM_t* syms, int count)
{
	int ch = 0;
	while ((ch = fgetc(fp)) != -1)
	{
		for (int i = 0; i <= count; i++)
			if (syms[i].ch == (unsigned char)ch)
			{
				fputs(syms[i].code, wp); // выводим строку с кодом
				break; // прерываем поиск
			}
	}
}

int seekTail(FILE *fp)
{
	int tail = 0;
	while ((fgetc(fp)) != -1)
	{
		tail++;
	}
	tail = tail % 8;
	return tail;
}

void packfun(FILE *fp, FILE*wp)
{
	unsigned char ch = 0;
	int i = 0;
	unsigned char buf[8] = { 0 };
	while ((ch = fgetc(fp)) != -1)
	{
		if (ch != '0' && ch != '1')
			break;
		buf[i++] = ch;
		if (i == 8)
		{
			ch = pack(buf);
			fputc(ch, wp);
			i = 0;
		}
	}
	if (i > 0)
	{
		while (i < 8)
		{
			buf[i++] = '1';
		}
		if (i == 8)
		{
			ch = pack(buf);
			fputc(ch, wp);
		}
	}
}

unsigned char pack(unsigned char buf[])
{
	union CODE code;
	code.byte.b1 = buf[0] - '0';
	code.byte.b2 = buf[1] - '0';
	code.byte.b3 = buf[2] - '0';
	code.byte.b4 = buf[3] - '0';
	code.byte.b5 = buf[4] - '0';
	code.byte.b6 = buf[5] - '0';
	code.byte.b7 = buf[6] - '0';
	code.byte.b8 = buf[7] - '0';
	return code.ch;
}


void tableToFile(SYM_t *syms, int countUnique, FILE * wp)
{
	for (int i = 0; i < countUnique; i++)
	{
		fputc((syms + i)->ch, wp);
		fprintf(wp, "%f", (syms + i)->freq);
	}
}
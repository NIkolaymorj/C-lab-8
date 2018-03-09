#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "taskDecompressor.h"

#define SIZE 256

void depackfun(FILE *fp, FILE*wp)
{
	unsigned char ch = 0;
	int i = 0;
	int j = 1;
	unsigned char buf[8] = { 0 };
	while (1)
	{
		ch = fgetc(fp);
		if (feof(fp) != 0)
			break;
		depack(buf, ch);
		for (i = 0; i < 8; i++)
			fputc(buf[i], wp);
	}
}

void depack(unsigned char buf[],char s)
{
	union CODE code;
	code.ch = s;
	buf[0] = code.byte.b1 + '0';
	buf[1] = code.byte.b2 + '0';
	buf[2] = code.byte.b3 + '0';
	buf[3] = code.byte.b4 + '0';
	buf[4] = code.byte.b5 + '0';
	buf[5] = code.byte.b6 + '0';
	buf[6] = code.byte.b7 + '0';
	buf[7] = code.byte.b8 + '0';
}

void createBuf(SYM_t *syms, FILE * fp, int countUnique)
{
	char buf[8] = { 0 };
	for (int i = 0; i < countUnique; i++)
	{
		(syms + i)->ch = fgetc(fp);
		for (int j = 0; j < 8; j++)
			buf[j] = fgetc(fp);
		(syms + i)->freq = atof(buf);
	}
}

struct SYM* buildTree(SYM_t *psym[], int N)
{
	// создаЄм временный узел
	struct SYM *temp = 0x0;
	while (temp == NULL)
		temp = (struct SYM*)malloc(sizeof(struct SYM));
	// в поле частоты записываетс€ сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// св€зываем созданный узел с двум€ последними узлами
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

	// добавл€ем temp в нужную позицию psym,
	// сохран€€ пор€док убывани€ частоты
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

void fileRecovery(FILE*fp, SYM_t*root, FILE* wp, int tail)
{
	int sizeFile = 0;
	SYM_t *branch = root;
	char s = 0;
	while (1)
	{
		s = fgetc(fp);
		if (feof(fp) != 0)
			break;
		sizeFile++;
	}
	rewind(fp);
	if (tail != 0)
		sizeFile = sizeFile - (8 - tail);
	for (int i = 0; i <= sizeFile; i++)
	{
		s = fgetc(fp);
		branch = seekCode(branch, s, wp, root);
	}
}


SYM_t * seekCode(SYM_t *branch, char s, FILE * wp, SYM_t *root)
{
	if (s == -1)
		fputc(branch->ch, wp);
	if (s == '1')
	{
		if (branch->right)
			return branch->right;
		else
		{
			fputc(branch->ch, wp);
			return root->right;
		}
	}
	else if (s == '0')
	{
		if (branch->left)
			return branch->left;
		else
		{
			fputc(branch->ch, wp);
			return root->left;
		}
	}

}
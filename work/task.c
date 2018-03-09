#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#define CH 256
#include "task.h"

int compare(const void *a, const void *b)
{
	if ( ((SYM*)a)->freq < ((SYM*)b)->freq )
		return 1;
	else
		return -1;
}

void sortCh(SYM syms[], int size)
{
	qsort(syms, size, sizeof(SYM), compare);	
}


SYM* buildTree(SYM **psym, int N)
{
	SYM *temp = (SYM*)malloc(sizeof(SYM));
	// в поле частоты записывается сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// связываем созданный узел с двумя последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2) // мы сформировали корневой элемент с частотой 1.0
		return temp;
	else
	{
		for (int i = 0; i<N; i++)
		{
			if (temp->freq>psym[i]->freq)
			{
				for (int j = N - 1; j>i; j--)
				{
					psym[j] = psym[j - 1];
				}
				psym[i] = temp;
				break;
			}
		}
	}
	return buildTree(psym, N - 1);
}

void makeCodes(SYM *root)
{
	//printf("root1\n");
	if (root->left)
	{
		//printf("Left\n");
		strcpy(root->left->code, root->code);
		strcat(root->left->code, "0");
		makeCodes(root->left);
	}
	if (root->right)
	{
		//printf("Right\n");
		strcpy(root->right->code, root->code);
		strcat(root->right->code, "1");
		makeCodes(root->right);
	}
}

unsigned char pack(unsigned char buf[])
{
	union CODE code;
	code.byte.b1 = buf[0];
	code.byte.b2 = buf[1];
	code.byte.b3 = buf[2];
	code.byte.b4 = buf[3];
	code.byte.b5 = buf[4];
	code.byte.b6 = buf[5];
	code.byte.b7 = buf[6];
	code.byte.b8 = buf[7];

	/*for (int i = 0; i < 8; i++)
		printf("%c", buf[i]);
	printf("\n");
	printf("%c\n", code.ch);*/
	return code.ch;
}

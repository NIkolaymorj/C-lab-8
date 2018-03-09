#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fun_comp.h"

SSYM* buildTree(SSYM **psym, int N)
{
	// создаём временный узел
	SSYM *temp = (SSYM*)malloc(sizeof(SSYM));
	// в поле частоты записывается сумма частот
	// последнего и предпоследнего элементов массива psym
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	// связываем созданный узел с двумя последними узлами
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;

	if (N == 2)
	{
		return temp;
	}
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

void makeCodes(SSYM *root)
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
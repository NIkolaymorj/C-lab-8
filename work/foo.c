#define _CRT_SECURE_NO_WARNINGS
#include "Header.h"
#include <string.h>
#include <malloc.h>
#include <stdlib.h>

int testFormat(unsigned char *str1, unsigned char *str2, int len)
{
	for (int i = 0; i < len; i++)
	{
		if (str1[i] != str2[i])
		{
			return 1;
		}
	}
	return 0;
}

void makeCodes(struct SYM *root)
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


struct SYM* buildTree(struct SYM *psym[], int N)
{
	//������ ��������� ����
	struct SYM *temp = (struct SYM*)calloc(1, sizeof(struct SYM));
	//� ���� ������� ������������ ����� ������
	//���������� � ��������������
	temp->freq = psym[N - 2]->freq + psym[N - 1]->freq;
	//��������� ��������� ���� � ����� ���������� ������
	temp->left = psym[N - 1];
	temp->right = psym[N - 2];
	temp->code[0] = 0;
	if (N == 2)
	{
		return temp;
	}
	else
	{		
		for (int i = N-1; 1; i++)
		{
			if (psym[i] == NULL)
			{
				for (; i >= N - 1; i--)
				{
					psym[i] = psym[i - 1];
				}
				break;
			}
		}
		psym[N - 2] = temp;
	}
	return buildTree(psym, N - 1);
}


//������� ���������� ������� �������� SYM
void ShellSort(int size, struct SYM syms[])	//��� size - ���������� ��������� � �������
{
	int i, j, step;
	struct SYM tmp;
	for (step = size / 2; step > 0; step /= 2)
	{
		for (i = step; i < size; i++)
		{
			tmp = syms[i];
			for (j = i; j >= step; j -= step)
			{
				if (tmp.freq > syms[j - step].freq)	//���� ����� ���� ">" �� ���������� �� �������� � ��������, ���� "<" �� �������� � ��������
				{
					syms[j] = syms[j - step];
				}
				else
				{
					break;
				}
			}
			syms[j] = tmp;
		}
	}
}
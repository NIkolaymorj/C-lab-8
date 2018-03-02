#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Header.h"


/*������� ������������� ��� ������� ������� ����� � ������������ ������ ����� ����������� �����������
����� � �������������� ����. ������� ��������� ���������� ���������� ��������*/
unsigned int genFile101(char * fname, unsigned char *fileByte, long int countByte, struct SYM syms[], int sizeArrSyms)
{
	char file[256];
	strcpy(file, fname);
	strcat(file, ".101");	//��������� ���������� � ������������ ����� ��� �����������
	FILE *fp_101;
	fp_101 = fopen(file, "wt");
	unsigned int countChar = 0;	//���������� ���������� ��������
	for (long int i = 0; i < countByte; i++)
	{
		for (int k = 0; k < sizeArrSyms; k++)
		{
			if (fileByte[i] == syms[k].ch)
			{
				fputs(syms[k].code, fp_101);	//���������� ���� �������� � ��������� ������������� � ���� .101
				countChar += strlen(syms[k].code);	//��������� ���������� �������� ���� � ����������� countChar
				break;
			}
		}

	}
	if (fclose(fp_101) == EOF)
	{
		printf("Error close file .101\n");
	}
	return countChar;
}
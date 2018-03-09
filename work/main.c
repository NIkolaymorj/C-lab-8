#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "Header.h"


int packFile(char *fname);
int unPackFile(char *fname);

char SIGNATURE[] = ".pack";


//������� --p ��� ������ ����� ��� --u ��� �������������� ������� �����
int main(int argc, char *argv[])
{
	
	char fname[256] = { 0 };

	int flag = 0;	//���������� ��������: 1 - ������, 2 - �������������� ������� �����
	if (argc != 3)
	{
		printf("Enter file name: ");	//������� ��� �����
		scanf("%s", fname);	//������ ����� �����
		//����� �������� 1 - ������, 2 - ����������
		printf("Select the required action:\nEnter 1 to compress the file;\nEnter 2 to restore the compressed file.\n");	
		scanf("%d", &flag);	//����������� ������������ ��������
	}
	else if (argc == 3)
	{
		strcpy(fname, argv[1]);
		if (strcmp(argv[2], "--p") == 0)
		{
			flag = 1;
		}
		else if (strcmp(argv[2], "--u") == 0)
		{
			flag = 2;
		}
		else
		{
			printf("Select the required action:\nEnter 1 to compress the file;\nEnter 2 to restore the compressed file.\n");
			scanf("%d", &flag);
		}
	}
	if (flag == 1)
	{
		packFile(fname);
	}
	else if (flag == 2)
	{
		unPackFile(fname);
	}
	else
	{
		printf("Error");
		return 1;
	}
	return 0;
}




int packFile(char *fname)
{
	unsigned char *fileByte;	//������ ���� ����������� �� �����
	long int countByte = 0;	//���������� ����������� ����
	fileByte = fileOpenAndRead(fname, &countByte);	//������ ����� � ������ � text
	int sizeArrSyms = 0;	//���������� ��������� ������� �������� syms
	struct SYM * syms = fileAnalysis(fileByte, countByte, &sizeArrSyms);	//�������� ������� �������������	
	ShellSort(sizeArrSyms, syms);	//���������� ������� �������������
	//������ � ��������� ������ ���������� �� ���������
	struct SYM **psyms = (struct SYM **)calloc((sizeArrSyms + sizeArrSyms - 2), sizeof(struct SYM *));	//�������� ������ ��� ������ ���������� �� ��������� SYM
	for (int i = 0; i < sizeArrSyms; i++)
	{
		psyms[i] = &syms[i];
	}
	struct SYM * root = buildTree(psyms, sizeArrSyms);	//��������� �� ������ ������
	makeCodes(root);	//����������� ���������� ����
	genFile101(fname, fileByte, countByte, syms, sizeArrSyms);
	genFilePack(fname, fileByte, countByte, syms, sizeArrSyms);

	free(psyms);
	free(syms);
	/*
	//------------����---------------------//
	printf("test syms\n");
	for (int i = 0; i < sizeArrSyms; i++)
	{
		printf("char: \'%c\'\tcode:%s\n", syms[i].ch, syms[i].code);
	}
	printf("test psyms\n");
	for (int i = 0; i < (sizeArrSyms + sizeArrSyms - 2); i++)
	{
		if (psyms[i]->ch != NULL)
		{
			printf("char: \'%c\'\tcode:%s\n", psyms[i]->ch, psyms[i]->code);
		}
	}
	*/
	return 0;
}

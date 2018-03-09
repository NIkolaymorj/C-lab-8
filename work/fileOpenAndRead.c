#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "Header.h"

extern char SIGNATURE[];


unsigned char * fileOpenAndRead(char *fname, long int *countByte)
{

	FILE *fp;
	fp = fopen(fname, "rb");
	if (fp == NULL) //�������� ������� ����� � ��� ��������
	{
		printf("Error open file in\n");
		return -1;
	}
	//------------�������� �� ������ �����---------------------------//
	unsigned char * buf = (char*)calloc(strlen(SIGNATURE), sizeof(char));
	fread(buf, sizeof(unsigned char), strlen(SIGNATURE), fp);
	if (testFormat(buf, &(SIGNATURE), strlen(SIGNATURE)) == 0)
	{	//���� ���������� ������ (���������) ��������� � �����, �� ��� ������ �� �������� � ������� � ������ ������
		printf("This file is already compressed, re-compression is not possible and will result in loss of data!!!");
		return -1;
	}
	free(buf);	//����������� ������ �� ��� �������� ����������
	fseek(fp, 0, SEEK_END);	//���������� ������� ��������� � ������ �� ����� �����
	long int sizeFile = ftell(fp);	// �������� ������ � ������
	long int RAM = sizeFile + (int)(sizeFile / 100 * 10);	//����������� ���������� ���������� ������ �� ������
	fseek(fp, 0, SEEK_SET);	//���������� ������� ��������� � ������ �� ������ �����
	unsigned char *fileByte = (unsigned char*)malloc(RAM * sizeof(unsigned char));
	if (fileByte == NULL)
	{
		fputs("Error RAM\n", stderr);
		return -1;
	}

	long int countByteRead = 0;	//���������� ������� ��������� ����

	countByteRead = fread(fileByte, sizeof(unsigned char), RAM, fp);	// ��������� ���� � �����

	if (sizeFile == countByteRead)
	{
		printf("Read file end\n");
	}
	
	if (countByteRead != sizeFile)
	{
		fputs("Error read file\n", stderr);
		exit(3);
	}
	if (fclose(fp) == EOF)
	{
		printf("Error close file in\n");
		return -1;
	}
	(*countByte) = countByteRead;	//���������� ���������� ����������� � ���������� ����
	return fileByte;
}
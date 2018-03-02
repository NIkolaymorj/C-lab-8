#define _CRT_SECURE_NO_WARNINGS
#include "Header.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Функция упаковки
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
extern char SIGNATURE[];


int genFilePack(char *fname, unsigned char *fileByte, long int countByte, struct SYM syms[], int sizeArrSyms)
{
	char file[256];
	strcpy(file, fname);
	strcat(file, SIGNATURE);	//Добавляем расширение к упакованному файлу для распознания
	unsigned long int sizeStr101 = (unsigned long int)(countByte * (sizeArrSyms / 2));	//Размер динамического массива char содержащего закодированные данные в строковом представлении
	unsigned char * str_101 = (unsigned char*)calloc(sizeStr101, sizeof(unsigned char));	//Создаём буферную строку которая будет содержать нули и едницы
	if (str_101 == NULL)
	{
		fputs("Error RAM\n", stderr);
		exit(2);
	}
	FILE *fp_pack;
	unsigned long int countChar = 0;	//Количество записанных символов
	for (long int i = 0; i < countByte; i++)
	{
		for (int k = 0, indLoc = 0; k < sizeArrSyms; k++)
		{
			if (fileByte[i] == syms[k].ch)
			{				
				//Переписываем коды символов в буферную строку
				for (int indLoc = 0; syms[k].code[indLoc] != '\0'; indLoc++, countChar++)
				{
					str_101[countChar] = syms[k].code[indLoc];
				}				
				if (countChar > (sizeStr101 - sizeArrSyms))	//В случае необходимости увеличиваем размер выделенной памяти
				{
					sizeStr101 += 10240;
					str_101 = (unsigned char*)realloc(str_101, sizeStr101 * sizeof(char));
					if (str_101 == NULL)
					{
						fputs("Error RAM\n", stderr);
						exit(2);
					}
				}
				break;
			}

		}
	}
	free(fileByte);
	fp_pack = fopen(file, "wb");	//Открываем файл который будет содержать зашифрованные данные
	fwrite(&(SIGNATURE), sizeof(char), strlen(SIGNATURE), fp_pack);	//Записываем сигнатуру (подпись формата) в файл .pack
	fwrite(&(sizeArrSyms), sizeof(int), 1, fp_pack);	//Записываем количество уникальных символов в файл .pack
	for (int i = 0; i < sizeArrSyms; i++)
	{
		fwrite(&(syms[i].ch), sizeof(char), 1, fp_pack);	//Записываем код символа в файл .pack
		fwrite(&(syms[i].freq), sizeof(float), 1, fp_pack);	//Записываем частоту встречаемости символа в файл .pack
	}
	int extraBits = countChar % 8;
	fwrite(&(extraBits), sizeof(int), 1, fp_pack);	//Записываем длинну хвоста в файл .pack
	fwrite(&(countByte), sizeof(long int), 1, fp_pack);	//Записываем количество символов в исходном файле в файл .pack
														
	//Производим сжатие и запись файла
	for (unsigned long int i = 0; i < countChar; i += 8)
	{
		unsigned char bufCharCode;	//Переменная содержащая буферный код символа
		bufCharCode = pack(&(str_101[i]));
		fwrite(&(bufCharCode), sizeof(unsigned char), 1, fp_pack);	//Записываем символы в файл .pack
	}
	free(str_101);	//Возращаем память выделенную под str_101
	if (fclose(fp_pack) == EOF)
	{
		printf("Error close file in\n");
	}
	return countChar;	

}
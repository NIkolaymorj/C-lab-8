#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


#define N_ASCII 256




struct SYM {
	unsigned char ch;// код символа
//	unsigned int c;//
	float freq;//частота встречаемости
//	char syms[256];//массив для хранения структур//????
	char code[255];//массив для нового кода
	struct SYM *left;
	struct SYM *right;

};


struct SYM syms[256] = {0};// массив для хранения записей
struct SYM *psyms[256] = { 0 };
struct SYM code[255] = {0};
int buf[8] = { 0 };
//struct SYM freq = 0;
//unsigned char ch = 0;
int count = 0;//счетчик всех символов
int count_used = 0;//счетчик используемых символов
int count_bin = 0;//cчетчик символов бинарного файла
int num_bytes = 0;//число целых байт в бинарном файле
int tail_size = 0;//счетчик символов "хвоста"
struct SYM *left = NULL;
struct SYM *right = NULL;
FILE *fp;
FILE *fp_101;
FILE *fp_out;



struct SYM* buildTree(struct SYM*psyms[], int n);
void makeStruct(struct SYM *syms);
//void freqToStruct(FILE *fp, struct SYM *syms);
void makeCodes(struct SYM *root);
void sortByFreq(struct SYM *syms);
int printSort(struct SYM *syms, int count);
//int countBinary();
void packInBytes(FILE *fp, FILE *fp_out, int num_bytes, int tail_size);
unsigned char pack(unsigned char buf[]);

union CODE {
	unsigned char ch;//ch - будет содержать код для записи в сжатый файл fp_out

	struct
	{
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;

	}byte;
};










struct SYM* buildTree(struct SYM*psyms[], int N)
{

	struct SYM *temp = (struct SYM*)malloc(sizeof(struct SYM));
	temp->freq = psyms[N - 2]->freq + psyms[N - 1]->freq;
	temp->left = psyms[N - 2];
	temp->right = psyms[N - 1];
	temp->code[0] = 0;
	if (N == 2)
		return temp;

	else//добавляем в массив новый элемент psym дерева Хаффмана, копируем в него из temp частоту двух минимальных элементов
	{
		int i=0;


		while (i < N)
		{
			++i;
			if (temp->freq > psyms[i]->freq)
			{
				for (int j = N - 1; j > 1; j--)
				{
					psyms[j] = psyms[j - 1];
				}
				psyms[i] = temp;
				break;
			}

		}
	}
	//	free();
	return buildTree(psyms, N - 1);
}

void makeCodes(struct SYM *root)
{
	if (root->left)
	{
		strcpy(root->left->code, root->code);//копирует родительский код
		strcat(root->left->code, "0");//дописывает свой
		makeCodes(root->left);

	}
	if (root->right)
	{
		strcpy(root->right->code, "1");
		makeCodes(root->right);
		makeCodes(root->left);
	}


}


void makeStruct(struct SYM *syms)
{

	for (int i = 0; i < N_ASCII; i++)
	{
		syms[i].ch = i;
		syms[i].freq = 0;
		syms[i].code[0] = 0;
		syms[i].right = NULL;
		syms[i].left = NULL;
	}
}
/*
void freqToStruct(FILE *fp, struct SYM *syms)//заполняем структуру
{
	char c=0;//
	int c_num=0;


	while ((c_num = fgetc(fp)) != EOF)
	{
		//fscanf(fp, "%c", &c);
			c_num = (unsigned char)c;
			//++syms[(unsigned char)c].freq;
			++syms[c_num].freq;
			++count;
		
	}
	printf("count:%d\n", count);

	//for (int i = 0; i < N_ASCII; i++)
	//	syms[i].freq = syms[i].freq / count;
		

	for (int j = 0; j < N_ASCII; j++)
	{
		syms[j].freq = syms[j].freq / count;
	}


	
	
}
*/
void sortByFreq(struct SYM *syms)
{
	int i, j;
	struct SYM temp;

	for (i = 0; i < N_ASCII - 1; i++)
	{
		for (j = 0; j < (N_ASCII - 1 - i); j++)
		{
			if (syms[j].freq < syms[j + 1].freq)
			{
				temp = syms[j];
				syms[j] = syms[j + 1];
				syms[j + 1] = temp;
			}
		}
	}
}

int printSort(struct SYM *syms, int count)
{
	int i = 0;
	printf("Sorted:\n");
	for (i = 0; i < N_ASCII; i++)
	{
		if (syms[i].freq > 0)
		{
			printf("%0.f  %0.2f%%     '%c'  \n", syms[i].freq, (100 * syms[i].freq) / count, syms[i].ch);
			count_used++;
		}
	}
	printf("count_used:%d\n", count_used);
	return count_used;
}





unsigned char pack(unsigned char buf[])
{
	union CODE code;
	code.byte.b1 = buf[0] - '0';
	code.byte.b1 = buf[1] - '0';
	code.byte.b1 = buf[2] - '0';
	code.byte.b1 = buf[3] - '0';
	code.byte.b1 = buf[4] - '0';
	code.byte.b1 = buf[5] - '0';
	code.byte.b1 = buf[6] - '0';
	code.byte.b1 = buf[7] - '0';
	return code.ch;
};

void packInBytes(FILE *fp_101, FILE *fp_out, int num_bytes, int tail_size)
{

	union CODE code;

	int j = 0;

	for (int i = 0; i < count_bin - tail_size; i++)
	{
		buf[j] = fgetc(fp_101);
		if (j == 7)
		{
			code.byte.b1 = buf[0] - '0';
			code.byte.b2 = buf[1] - '0';
			code.byte.b3 = buf[2] - '0';
			code.byte.b4 = buf[3] - '0';
			code.byte.b5 = buf[4] - '0';
			code.byte.b6 = buf[5] - '0';
			code.byte.b7 = buf[6] - '0';
			code.byte.b8 = buf[7] - '0';
			fputc(code.ch, fp_out);
			j = 0;
		}
	    j++;
    }

	j = 0;
	for (int i = 0; i <= tail_size; i++)
	{
		buf[j] = fgetc(fp_101);
		if (j == tail_size)
		{
			code.byte.b1 = buf[0] - '0';
			code.byte.b2 = buf[1] - '0';
			code.byte.b3 = buf[2] - '0';
			code.byte.b4 = buf[3] - '0';
			code.byte.b5 = buf[4] - '0';
			code.byte.b6 = buf[5] - '0';
			code.byte.b7 = buf[6] - '0';
			code.byte.b8 = buf[7] - '0';
			fputc(code.ch, fp_out);
		}
		j++;
	}
}
	


int main()
{	
	FILE *fp = fopen("tale.txt", "rb");//открываем исходный файл
	FILE *fp_101 = fopen("bin.txt", "wb");//файл для записи бинарного кода
	FILE *fp_out = fopen("out.txt", "wb");// - для записи сжатого файла

	if (fp == NULL|| fp_101==NULL || fp_out==NULL)
	{
		perror("fopen");
		return 1;
	}

	int c = 0;//в эту переменную считываем информацию из файла

	makeStruct(syms);

	while ((c = fgetc(fp)) != EOF)
	{
		++syms[(unsigned char)c].freq;
		++count;
	}
	printf("count:%d\n", count);

	

	for (int i = 0; i < 256; i++)
	{
		psyms[i] = &syms[i];
	}
	
	for (int v = 0; v < N_ASCII; v++)
		printf("freq:%0.2f%% - '%c'\n", 100*syms[v].freq/count,syms[v].ch);
	

	sortByFreq(syms);
	printSort(syms, count);

	for (int j = 0;j < N_ASCII;j++)//инициализация массива указателей адресами записей
	{
		psyms[j] = &syms[j];
	}

	struct SYM *root = buildTree(psyms, count_used );//создаем дерево Хаффмана
	makeCodes(root);//получаем код
	rewind(fp);//возвращаем указатель в файле в начало файла
	//далее читаем исходный файл и записываем полученные коды в бинарный файл
	while ((c=fgetc(fp))!=EOF)
	{
		for (int i = 0; i < count_used; i++)
		{
			if (c==syms[i].ch)
			{
				fputs(syms[i].code, fp_101);
				break;
			}
		}
	}

	fclose(fp_101);//закрываем файл
	fp_101 = fopen("bin.txt", "rb");//снова открываем бинарный файл для чтения

	//rewind(fp_101);
	

	while ((c=fgetc(fp_101))!= EOF)
	{
		count_bin++;
	}
	printf("count_bin: %d\n", count_bin);

	num_bytes = count_bin / 8;//число целых байт
	tail_size = count_bin % 8;//остаток символов (хвост)

	printf("num_bytes:%d\n", num_bytes);
	printf("tail_size:%d\n", tail_size);
	//записываем в заголовок сжатого файла:
	fwrite("Compression done!", sizeof(char), 17, fp_out);
	fwrite(&count_used, sizeof(int), 1, fp_out);//кол-во используемых символов
	fwrite(&tail_size,sizeof(int),1, fp_out);//кол-во символов в "хвосте"
	//записываем в файл таблицу встречаемости
	for (int j = 0; j < count_used; j++)
	{
		fwrite(&syms[j].ch,sizeof(struct SYM),1,fp_out);
		fwrite(&syms[j].freq, sizeof(struct SYM), 1, fp_out);
		
	}
	rewind(fp_101);//указатель возвращаем в начало бинарного файла
    
	//читаем бинарный файл, занося последовательно по 8 элементов в массив для дальнейшей упаковки
	packInBytes(fp_101, fp_out, num_bytes, tail_size);
	
	//закрываем все файлы
	fclose(fp);
	fclose(fp_101);
	fclose(fp_out);
	return 0;
}
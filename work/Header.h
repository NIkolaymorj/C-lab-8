#define _CRT_SECURE_NO_WARNINGS




//��������� ��� �������� ������ � ������� � �����
struct SYM	//������������� �������
{
	unsigned char ch; // ASCII - ���
	float freq;	//������� ������������� �� 0 �� 1
	unsigned char code[256];	//������ ��� ������ ����
	struct SYM *left;	//����� ������� � ������
	struct SYM *right;	//������ ������� � ������
};

//����������� ��� ����������� � �������������
union CODE {
	unsigned char ch;
	struct {
		unsigned short b1 : 1;
		unsigned short b2 : 1;
		unsigned short b3 : 1;
		unsigned short b4 : 1;
		unsigned short b5 : 1;
		unsigned short b6 : 1;
		unsigned short b7 : 1;
		unsigned short b8 : 1;
	} byte;
};

void ShellSort(int size, struct SYM syms[]);
struct SYM* buildTree(struct SYM *psym[], int N);
void makeCodes(struct SYM *root);
unsigned char * fileOpenAndRead(char *fname, long int *countByte);
struct SYM * fileAnalysis(unsigned char *fileByte, long int countByte, int *sizeArrSyms);
unsigned int genFile101(char * fname, unsigned char *fileByte, long int countByte, struct SYM syms[], int sizeArrSyms);
int genFilePack(char *fname, unsigned char *fileByte, long int countByte, struct SYM syms[], int sizeArrSyms);
int unPackFile(char * fname);





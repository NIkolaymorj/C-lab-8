
#define SYM_NUM 256
#define BT_SIZE 8
#define NAME_SIZE 100

struct SYM // представление символа
{
	unsigned char ch;   // ASCII-код
	float freq;         // частота
	char code[SYM_NUM]; // новый код
	struct SYM *left;   // левый потомок
	struct SYM *right;  // правый потомок
};

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

typedef struct SYM SSYM;

void lineSort(SSYM *arr, int size);
void clearStruct(SSYM *arr);
void fillStruct(FILE *fp, SSYM *arr, unsigned int *countBtInit, unsigned int *countUniqSym);
void linkPtrs(SSYM *arr, SSYM **parr);
int findFileExt(char *inStr, char *extStr);

unsigned int writeCodesIn101(FILE *fp_in, FILE *fp_101, SSYM *arr);
void writeHeader(FILE *fp_fin, SSYM *arr, char *extStr, unsigned int countBtInit, unsigned int countCodeSumBits, unsigned int countUniqSym);
void readHeader(FILE *fp_fin, SSYM *arr, unsigned int *numUniqSym, unsigned char *tailLen, unsigned int *totBtInit, char *extStr);
void writeFinBinData(FILE *fp_fin, FILE *fp_101);

SSYM* buildTree(SSYM **psym, int N);
void makeCodes(SSYM *root);
unsigned char pack(unsigned char buf[]);

void fromChar2Byte(unsigned char sym, char *strByte);
void decodeBinData(FILE *fp_fin, FILE *fp_fin_dec, SSYM *p, unsigned int countBtInit);
void createFileNames(char* in, char* f101, char* comp, char *decomp);
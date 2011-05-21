typedef struct {
	unsigned char c;
} MyStruct1;

typedef struct MyStruct2 {
	unsigned char c;
	unsigned char e;
} *pMyStruct2;

int main(int argc, char **argv)
{
	MyStruct1 a;
	struct MyStruct2 bbbbbbbbbbbbb;
	pMyStruct2 pggggg;
	
	a.c = 0;
	return 0;
}

#include"FileCompress.h"
#include<windows.h>

int main()
{
	char* pstr = "���";
	FileCompress fc;
	fc.CompressFile("add.c");
	fc.UNCompressFile("add.hzp");
	system("pause");
	return 0;
}
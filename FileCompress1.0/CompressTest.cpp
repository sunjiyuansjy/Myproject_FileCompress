#include"FileCompress.h"
#include<windows.h>

int main()
{
	FileCompress fc;
	fc.CompressFile("1.txt");
	system("pause");
	return 0;
}
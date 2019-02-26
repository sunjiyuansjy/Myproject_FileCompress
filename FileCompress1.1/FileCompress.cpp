#include "FileCompress.h"
#include<iostream>
#include<assert.h>

using namespace std;

FileCompress::FileCompress()
{
	_fileInfo.resize(256);
	for (size_t i = 0; i < 256; ++i)
	{
		_fileInfo[i]._ch = i;
		_fileInfo[i]._count = 0;
	}
}
void FileCompress::CompressFile(const std::string& strFilePath)
{
//	1.ͳ��Դ�ļ���ÿ���ַ����ֵĴ���
	FILE* fIn = fopen(strFilePath.c_str(), "r");
	if (nullptr == fIn)
	{
		cout << "���ļ�ʧ��" << endl;
		return;
	}
	cout << "���ļ��ɹ�" << endl;
//	unsigned long long  count[255] = { 0 };
	UCH* pReadBuff = new UCH[1024];

	while (true)
	{
		size_t rdSize = fread(pReadBuff, 1, 1024, fIn);
		if (0 == rdSize)
			break;

		for (size_t i = 0; i < rdSize; ++i)
		
			_fileInfo[pReadBuff[i]]._count++;
		
	}

//	2.����ÿ���ַ����ֵĴ���ΪȨֵ������������
	HuffmanTree<CharInfo> ht;
	ht.CreatHuffmanTree(_fileInfo, CharInfo(0));
	//3.ͨ������������ȡÿ���ַ��ı���
	GetHuffmanCode(ht.GetRoot());
	//4.�û�ȡ���Ĺ������������¸�д�ļ�
	char ch = 0;//�������ڱ���ѹ�����
	char bitCount = 0;
	FILE* fOut = fopen("add.hzp", "w");
	assert(fOut);

	WriteHeadInfo(fOut, strFilePath);
	fseek(fIn, 0, SEEK_SET);
	while (true)
	{
		size_t rdSize = fread(pReadBuff, 1, 1024, fIn);
		if (0 == rdSize)
			break;
		//�ñ����дԴ�ļ�
		for (size_t i = 0; i < rdSize; ++i)
		{
			string& strCode = _fileInfo[pReadBuff[i]]._strCode;
			for (size_t j = 0; j < strCode.size(); ++j)
			{
				//A ---> 100
				//B--->101
				//ch = 0000 0000
				ch <<= 1;
				if ('1' == strCode[j])
					ch |= 1;
				bitCount++;
				if (8 == bitCount)
				{
					//����һ��ch��8������λ�Ѿ��õ�����chд��ѹ���ļ�
					fputc(ch, fOut);
					ch = 0;//�������������һ���ַ�
					bitCount = 0;
				}
			}
		}
	}
	//˵�����һ���ַ��ı���λ��û�����
	if(bitCount>0 && bitCount< 8)
	{ 
		ch <<= (8 - bitCount);
		fputc(ch, fOut);
	}
	delete[] pReadBuff;
	fclose(fIn);
	fclose(fOut);
}
void FileCompress::UNCompressFile(const std::string& strFilePath)
{
	//��ѹ��֮ǰ����Ҫ����ļ��Ƿ��ǵ�ǰ�㷨����ʶ���
	string postFix = strFilePath.substr(strFilePath.rfind('.') + 1);
	if (postFix != "hzp")
	{

		cout << "ѹ���ļ���ʽ��֧��" << endl;
		return;
	}

	FILE* fIn = fopen(strFilePath.c_str(), "r");
	if (nullptr == fIn)
	{
		cout << "ѹ���ļ���ʧ��" << endl;
		return;
	}

	//��ѹ���ļ��л�ȡԴ�ļ��ĺ�׺
	postFix = "";
	GetLine(fIn, postFix);

	//��ѹ���ļ��л�ȡ�ַ��������Ϣ
	string strContent;
	GetLine(fIn, strContent);
	size_t lineContent = atoi(strContent.c_str());
	size_t charCount = 0;
	for (size_t i = 0; i < lineContent; ++i)
	{
		strContent = "";
		GetLine(fIn, strContent);

	    charCount = atoi(strContent.c_str() + 2);
		_fileInfo[strContent[0]]._count = charCount;
	}

	//��ԭhuffman��
	HuffmanTree<CharInfo> ht;
	ht.CreatHuffmanTree(_fileInfo, CharInfo(0));

	//��ѹ��
	string strUNFileName("2");
	strUNFileName += postFix;
	FILE* fOut = fopen(strUNFileName.c_str(), "w");
    UCH* pReadBuff = new UCH[1024];
	int pos = 7;
	HTNode<CharInfo>* pCur = ht.GetRoot();
	long long fileSize = pCur->_weight._count;
	while (true)
	{
		size_t rdSize = fread(pReadBuff, 1, 1024, fIn);
		if (0 == rdSize)
			break;
		for (size_t i = 0; i < rdSize; ++i)
		{
			pos = 7;
			//��ѹ����ǰѹ���ļ�������
			for (size_t j = 0; j < 8; ++j)
			{
				if (pReadBuff[i] & (1 << pos))
					pCur = pCur->_pRight;
				else
					pCur = pCur->_pLeft;
				pos--;

				
				//	�ߵ�Ҷ�ӽڵ�
				if (nullptr == pCur->_pLeft && nullptr == pCur->_pRight)
				{
					fputc(pCur->_weight._ch, fOut);
					pCur = ht.GetRoot();
					--fileSize;
					if (fileSize == 0)
						break;
				}
				if (pos < 0) {
					break;
				}
			}

		}
	}
	delete[] pReadBuff;
	fclose(fIn);
	fclose(fOut);
}
void FileCompress::GetHuffmanCode(HTNode<CharInfo>* pRoot)
{
	if (nullptr == pRoot)
		return;
		GetHuffmanCode(pRoot->_pLeft);
		GetHuffmanCode(pRoot->_pRight);

	if (nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight)
	{
		HTNode<CharInfo>* pCur = pRoot;
		HTNode<CharInfo>* pParent = pCur->_pParent;
		string& strCode = _fileInfo[pRoot->_weight._ch]._strCode;
		while (pParent)
		{
			if (pCur == pParent->_pLeft)
				strCode += '0';
			else
				strCode += '1';

			pCur = pCur->_pParent;
			pParent = pCur->_pParent;

		}
		reverse(strCode.begin(), strCode.end());
	}
}

void FileCompress::WriteHeadInfo(FILE* pf, const string& strFileName)
{
	//1.��ȡԴ�ļ��ĺ�׺��
	string postFix = strFileName.substr(strFileName.rfind('.'));
	//2.��ȡ��Ч����ı�������

	//3.��Ч�ַ�������ִ���
	string strCharCount ;
	size_t lineCount = 0;
	char szCount[32] = { 0 };
	for (size_t i = 0; i < 256; ++i)
	{
		if (0 != _fileInfo[i]._count)
		{
			strCharCount += _fileInfo[i]._ch;
			strCharCount += ',';
			memset(szCount, 0,32);
			_itoa(_fileInfo[i]._count, szCount, 10);
			strCharCount += szCount;
			strCharCount += "\n";
			lineCount++;
		}
	}
	//д��ͷ����Ϣ
	string strHeadInfo;
	strHeadInfo += postFix;
	strHeadInfo += "\n";

	memset(szCount, 0, 32);
	_itoa(lineCount, szCount, 10);
	strHeadInfo += szCount;
	strHeadInfo += "\n";

	strHeadInfo += strCharCount;

	fwrite(strHeadInfo.c_str(), 1, strHeadInfo.size(), pf);
}
void FileCompress::GetLine(FILE* pf, string& strCotent)
{
	while (!feof(pf))
	{
		char ch = fgetc(pf);   
		if (('\n' == ch))
			return;
		strCotent += ch; 
	}
}

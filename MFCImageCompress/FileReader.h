#pragma once
#include <fstream>
using namespace std;

typedef unsigned char Byte;

/*
	�ļ�������
*/
class FileReader
{
public:
	FileReader(ifstream* filein);
	void Prepare();
	bool Read(Byte& byte, int bit);
	void Stop();

private:
	int read;
	unsigned int buffer;
	ifstream* filein;
};

FileReader::FileReader(ifstream* filein)
{
	this->filein = filein;
	buffer = 0;
	read = 0;
}

/*
	��ʼ���ļ�ǰִ���������
*/
void FileReader::Prepare()
{
	filein->read((char*)&buffer, sizeof(buffer));
	read = 0;
}

/*
	���ļ�����bitλ���ݴ��뵽byte��
	����ֵ:trueΪ�����˻���,falseΪδ���»���
*/
bool FileReader::Read(Byte& byte, int bit)
{
	if (read + bit <= 32)
	{
		read += bit;
		byte = buffer >> (32 - read);
		if (read < 32)
		{
			buffer = (buffer << read) >> read;
		}
		else
		{
			buffer = 0;
		}

		return false;
	}
	else
	{
		int bit1 = 32 - read;
		int bit2 = bit - bit1;

		byte = (Byte)buffer;

		filein->read((char*)&buffer, sizeof(buffer));
		read = bit2;

		byte = byte << bit2;
		byte = byte | (buffer >> (32 - read));
		buffer = (buffer << read) >> read;

		return true;
	}
}

void FileReader::Stop()
{
	filein = NULL;
}


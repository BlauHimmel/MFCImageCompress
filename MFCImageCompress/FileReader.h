#pragma once
#include <fstream>
using namespace std;

typedef unsigned char Byte;

/*
	文件读入器
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
	开始读文件前执行这个函数
*/
void FileReader::Prepare()
{
	filein->read((char*)&buffer, sizeof(buffer));
	read = 0;
}

/*
	读文件，将bit位数据存入到byte中
	返回值:true为更新了缓冲,false为未更新缓冲
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


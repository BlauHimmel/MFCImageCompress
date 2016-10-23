#pragma once
#include <fstream>
using namespace std;

typedef unsigned char Byte;
/*
	文件写入器
*/
class FileWriter
{
public:
	FileWriter(ofstream* fileout);
	void Start();
	bool Write(Byte byte, int bit);
	void Stop();
private:
	int written;
	unsigned int buffer;
	ofstream* fileout;
};

/*
	构造时传入用于写文件的流的指针
*/
FileWriter::FileWriter(ofstream* fileout)
{
	this->fileout = fileout;
	written = 0;
	buffer = 0;
}

/*
	开始写文件前执行这个函数
*/
void FileWriter::Start()
{
	buffer = 0;
	written = 0;
}

/*
	写文件,byte参数为写入数据,bit参数为以多少位写入该数据
	返回值:true缓冲区已满导致写入文件操作，false缓冲区未满
*/
bool FileWriter::Write(Byte byte, int bit)
{
	if (written + bit <= 32)
	{
		buffer = buffer << bit;
		buffer = buffer | byte;
		written += bit;
		return false;
	}
	else 
	{
		int bit1 = 32 - written;
		int bit2 = bit - bit1;

		buffer = buffer << bit1;
		buffer = buffer | (byte >> bit2);
		fileout->write((char*)&buffer, sizeof(buffer));
		buffer = 0;
		written = 0;

		buffer = buffer << bit2;
		buffer = buffer | ((byte << (8 - bit2)) >> (8 - bit2));
		written += bit2;
		return true;
	}
}

/*
	写入文件操作结束后执行这个函数,会刷新缓冲区
*/
void FileWriter::Stop()
{
	int leftBit = 32 - written;
	if (leftBit > 0)
	{
		buffer = buffer << leftBit;
	}
	fileout->write((char*)&buffer, sizeof(buffer));
	fileout = NULL;
}


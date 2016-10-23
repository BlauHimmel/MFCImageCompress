#pragma once
#include <fstream>
using namespace std;

typedef unsigned char Byte;
/*
	�ļ�д����
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
	����ʱ��������д�ļ�������ָ��
*/
FileWriter::FileWriter(ofstream* fileout)
{
	this->fileout = fileout;
	written = 0;
	buffer = 0;
}

/*
	��ʼд�ļ�ǰִ���������
*/
void FileWriter::Start()
{
	buffer = 0;
	written = 0;
}

/*
	д�ļ�,byte����Ϊд������,bit����Ϊ�Զ���λд�������
	����ֵ:true��������������д���ļ�������false������δ��
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
	д���ļ�����������ִ���������,��ˢ�»�����
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


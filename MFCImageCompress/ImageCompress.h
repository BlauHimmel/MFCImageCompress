#pragma once

#include <fstream>
#include <Windows.h>
#include "FileWriter.h"
#include "FileReader.h"
#include "SubsectionInfo.h"

using namespace std;

typedef unsigned char Byte;

class ImageCompress
{
public:
	ImageCompress();
	~ImageCompress();
	bool ReadBitmap(CString& pathname);
	bool WriteBitmap(CString& pathname);
	void Compress(CString& filename);
	void UnCompress(CString& filename);

private:
	BITMAPFILEHEADER* ptFileHeader;
	BITMAPINFOHEADER* ptInfoHeader;
	RGBQUAD* ptRGBQuad;
	Byte** ppImageMatrix;
	Byte* pImageArray;
	bool tag;
	int numRGB;

private:
	void ArrayGenerate();
	void MatrixGenerate();
	int GetBinaryDigit(Byte num);		//返回二进制位数
	void CompressPrepare(int* length, int* bits, int n, int* totalBits, int* cut);
	int GetCompressedSubsectionLength(int* lengthCompressed, int n, int* cut);
	SubsectionInfo* GetCompressedSubsection(int* lengthCompressed, int n, SubsectionInfo* info);
	void Clear();
	void Prepare();
	void F();
};

ImageCompress::ImageCompress()
{
	numRGB = 0;
	ptRGBQuad = NULL;
	ptFileHeader = NULL;
	ptInfoHeader = NULL;
	ppImageMatrix = NULL;
	pImageArray = NULL;
	tag = false;
}

ImageCompress::~ImageCompress()
{
	Clear();
}

void ImageCompress::Clear()
{
	if (tag)
	{
		for (int i = 0; i < ptInfoHeader->biHeight; i++)
		{
			delete[] ppImageMatrix[i];
		}
		delete[] ppImageMatrix;
		delete[] pImageArray;
		delete[] ptRGBQuad;
		delete ptFileHeader;
		delete ptInfoHeader;
		tag = false;
	}
}

void ImageCompress::Prepare()
{
	Clear();
	tag = true;
	ptRGBQuad = new RGBQUAD[256];
	ptFileHeader = new BITMAPFILEHEADER();
	ptInfoHeader = new BITMAPINFOHEADER();
	ppImageMatrix = NULL;
	pImageArray = NULL;
	numRGB = 0;
}

bool ImageCompress::ReadBitmap(CString& pathname)
{
	Prepare();
	ifstream filein(pathname, ios::binary);
	filein.read((char*)ptFileHeader, sizeof(BITMAPFILEHEADER));
	filein.read((char*)ptInfoHeader, sizeof(BITMAPINFOHEADER));
	numRGB = (ptFileHeader->bfOffBits - (int)filein.tellg()) / sizeof(RGBQUAD);
	filein.read((char*)ptRGBQuad, numRGB * sizeof(RGBQUAD));

	ppImageMatrix = new Byte*[ptInfoHeader->biHeight];
	for (int i = 0; i < ptInfoHeader->biHeight; i++)
	{
		ppImageMatrix[i] = new Byte[ptInfoHeader->biWidth];
	}
	pImageArray = new Byte[ptInfoHeader->biHeight*ptInfoHeader->biWidth];

	if ((int)filein.tellg() != ptFileHeader->bfOffBits)
	{
		filein.close();
		return false;
	}

	//每行实际的字节数
	int lenOfEachLine = (((ptInfoHeader->biWidth * 8 * sizeof(Byte)) + 31) >> 5) << 2;

	//每行所补0的个数
	int skipLen = 4 - ((ptInfoHeader->biWidth * 8 * sizeof(Byte)) >> 3) & 3;

	for (int i = 0; i < ptInfoHeader->biHeight; i++)
	{
		filein.read((char*)ppImageMatrix[i], ptInfoHeader->biWidth * sizeof(Byte));
		filein.seekg(skipLen, ios::cur);
	}

	filein.close();
	return true;
}

bool ImageCompress::WriteBitmap(CString& pathname)
{
	ofstream fileout(pathname, ios::binary);

	if (ptFileHeader->bfOffBits != sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + numRGB * sizeof(RGBQUAD))
	{
		fileout.close();
		return false;
	}

	//每行实际的字节数
	int lenOfEachLine = (((ptInfoHeader->biWidth * 8 * sizeof(Byte)) + 31) >> 5) << 2;

	//每行所补0的个数
	int skipLen = 4 - ((ptInfoHeader->biWidth * 8 * sizeof(Byte)) >> 3) & 3;

	fileout.write((char*)ptFileHeader, sizeof(BITMAPFILEHEADER));
	fileout.write((char*)ptInfoHeader, sizeof(BITMAPINFOHEADER));
	fileout.write((char*)ptRGBQuad, numRGB * sizeof(RGBQUAD));

	for (int i = 0; i < ptInfoHeader->biHeight; i++)
	{
		fileout.write((char*)ppImageMatrix[i], ptInfoHeader->biWidth * sizeof(Byte));
		for (int j = 0; j < skipLen; j++)
		{
			Byte temp = 0;
			fileout.write((char*)&temp, sizeof(Byte));
		}
	}

	fileout.close();
	return true;
}

/*
	文件结构:
	BITMAPFILEHEADER+BITMAPINFOHEADER+256个RGBQUAD+((段长度-1)+(最大位数-1)+压缩数据)*n
*/
void ImageCompress::Compress(CString& filename)
{
	ofstream fileout(filename, ios::binary);
	FileWriter fileWriter(&fileout);
	ArrayGenerate();

	SubsectionInfo* info = new SubsectionInfo();
	info->bits = new int[ptInfoHeader->biHeight*ptInfoHeader->biWidth + 1];
	info->length = new int[ptInfoHeader->biHeight*ptInfoHeader->biWidth + 1];
	for (int i = 1; i <= ptInfoHeader->biHeight*ptInfoHeader->biWidth; i++)
	{
		info->bits[i] = GetBinaryDigit(pImageArray[i - 1]);
		info->length[i] = 1;
	}
	info->sum = ptInfoHeader->biHeight*ptInfoHeader->biWidth;

	int* totalBits = new int[info->sum + 1];
	int* cut = new int[info->sum + 1];
	int* lengthCompressed = new int[info->sum + 1];

	CompressPrepare(info->length, info->bits, info->sum, totalBits, cut);
	int sum = GetCompressedSubsectionLength(lengthCompressed, info->sum, cut);

	fileout.write((char*)ptFileHeader, sizeof(BITMAPFILEHEADER));
	fileout.write((char*)ptInfoHeader, sizeof(BITMAPINFOHEADER));
	fileout.write((char*)ptRGBQuad, numRGB * sizeof(RGBQUAD));

	int index = 0;

	fileWriter.Start();

	SubsectionInfo* compressedInfo = GetCompressedSubsection(lengthCompressed, sum, info);

	int maxProgress = 0;
	g_progressCtrl->SetPos(0);
	for (int i = 1; i <= compressedInfo->sum; i++)
	{
		maxProgress += 11;
		for (int j = 0; j < compressedInfo->length[i]; j++)
		{
			maxProgress += compressedInfo->bits[i];
		}
	}

	int progress = 0;
	for (int i = 1; i <= compressedInfo->sum; i++)
	{
		fileWriter.Write(compressedInfo->length[i] - 1, 8);
		fileWriter.Write(compressedInfo->bits[i] - 1, 3);
		progress += 11;
		for (int j = 0; j < compressedInfo->length[i]; j++)
		{
			fileWriter.Write(pImageArray[index++], compressedInfo->bits[i]);
			progress += compressedInfo->bits[i];
			if (progress >= maxProgress / 100)
			{
				if (g_progressCtrl->GetPos() < 100)
				{
					g_progressCtrl->OffsetPos(1);
				}
				progress = 0;
			}
		}
	}

	g_progressCtrl->SetPos(100);

	fileWriter.Stop();
	fileout.close();
	delete[] cut;
	delete[] totalBits;
	delete[] lengthCompressed;
	delete[] info->bits;
	delete[] info->length;
	delete info;
	delete[] compressedInfo->bits;
	delete[] compressedInfo->length;
	delete compressedInfo;
}

void ImageCompress::UnCompress(CString& filename)
{
	Prepare();
	ifstream filein(filename, ios::binary);

	filein.read((char*)ptFileHeader, sizeof(BITMAPFILEHEADER));
	filein.read((char*)ptInfoHeader, sizeof(BITMAPINFOHEADER));
	numRGB = (ptFileHeader->bfOffBits - (int)filein.tellg()) / sizeof(RGBQUAD);
	filein.read((char*)ptRGBQuad, numRGB * sizeof(RGBQUAD));

	ppImageMatrix = new Byte*[ptInfoHeader->biHeight];
	for (int i = 0; i < ptInfoHeader->biHeight; i++)
	{
		ppImageMatrix[i] = new Byte[ptInfoHeader->biWidth];
	}

	pImageArray = new Byte[ptInfoHeader->biHeight*ptInfoHeader->biWidth];

	FileReader fileReader(&filein);
	fileReader.Prepare();

	int index = 0;

	int maxProgress = ptInfoHeader->biHeight*ptInfoHeader->biWidth;
	int progress = 0;
	g_progressCtrl->SetPos(0);

	while (index < ptInfoHeader->biHeight*ptInfoHeader->biWidth)
	{
		Byte len, bit;
		fileReader.Read(len, 8);
		fileReader.Read(bit, 3);
		int length = len + 1;
		int bits = bit + 1;

		for (int i = 0; i < length; i++)
		{
			Byte byte = 0;;
			fileReader.Read(byte, bits);
			pImageArray[index++] = byte;
			progress++;
			if (progress >= maxProgress / 100)
			{
				if (g_progressCtrl->GetPos() <= 99)
				{
					g_progressCtrl->OffsetPos(1);
				}
				progress = 0;
			}
		}
	}
	g_progressCtrl->SetPos(100);

	fileReader.Stop();
	MatrixGenerate();
	filein.close();
}

void ImageCompress::ArrayGenerate()
{
	int index = 0;
	for (int i = 0; i < ptInfoHeader->biHeight; i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < ptInfoHeader->biWidth; j++)
			{
				pImageArray[index++] = ppImageMatrix[i][j];
			}
		}
		if (i % 2 == 1)
		{
			for (int j = ptInfoHeader->biWidth - 1; j >= 0; j--)
			{
				pImageArray[index++] = ppImageMatrix[i][j];
			}
		}
	}
}

void ImageCompress::MatrixGenerate()
{
	int index = 0;
	for (int i = 0; i < ptInfoHeader->biHeight; i++)
	{
		if (i % 2 == 0)
		{
			for (int j = 0; j < ptInfoHeader->biWidth; j++)
			{
				ppImageMatrix[i][j] = pImageArray[index++];
			}
		}
		if (i % 2 == 1)
		{
			for (int j = ptInfoHeader->biWidth - 1; j >= 0; j--)
			{
				ppImageMatrix[i][j] = pImageArray[index++];
			}
		}
	}
}

int ImageCompress::GetBinaryDigit(Byte num)
{
	int n = (int)num;
	int digit = 0;

	if (n == 0)
	{
		return 1;
	}

	while (n != 0)
	{
		digit++;
		n = n / 2;
	}
	return digit;
}

void ImageCompress::CompressPrepare(int* length, int* bits, int n, int* totalBits, int* cut)
{
	totalBits[0] = 0;
	for (int i = 1; i <= n; i++)
	{
		int lengthSum = length[i];
		int bitMax = bits[i];
		totalBits[i] = totalBits[i - 1] + lengthSum * bitMax;
		cut[i] = 1;

		for (int k = 2; k <= i && lengthSum + length[i - k] <= 256; k++)
		{
			lengthSum += length[i - k + 1];
			if (bitMax < bits[i - k + 1])
			{
				bitMax = bits[i - k + 1];
			}
			if (totalBits[i] > totalBits[i - k] + lengthSum * bitMax)
			{
				totalBits[i] = totalBits[i - k] + lengthSum * bitMax;
				cut[i] = k;
			}
		}
		totalBits[i] += 11;
	}
}

/*
	返回分段段数
*/
int ImageCompress::GetCompressedSubsectionLength(int* lengthCompressed, int n, int* cut)
{
	int index = 1;
	for (int i = 0; i <= n; i++)
	{
		lengthCompressed[i] = 0;
	}
	while (n != 0)
	{
		lengthCompressed[index++] = cut[n];
		n = n - cut[n];
	}
	/*逆置*/
	int left = 1;
	int right = index - 1;
	while (left < right)
	{
		int temp = lengthCompressed[left];
		lengthCompressed[left] = lengthCompressed[right];
		lengthCompressed[right] = temp;
		left++;
		right--;
	}
	return index - 1;
}

SubsectionInfo* ImageCompress::GetCompressedSubsection(int* lengthCompressed, int n, SubsectionInfo* info)
{
	SubsectionInfo* compressedInfo = new SubsectionInfo();
	compressedInfo->bits = new int[n + 1];
	compressedInfo->length = new int[n + 1];
	compressedInfo->sum = n;

	int index = 1;
	for (int i = 1; i <= n; i++)
	{
		int maxBit = 0;
		int len = 0;

		for (int j = 0; j < lengthCompressed[i]; j++)
		{
			if (info->bits[index] > maxBit)
			{
				maxBit = info->bits[index];
			}
			len = len + info->length[index];
			index++;
		}
		compressedInfo->bits[i] = maxBit;
		compressedInfo->length[i] = len;
	}
	return compressedInfo;
}

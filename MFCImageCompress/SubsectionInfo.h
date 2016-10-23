#pragma once

/*
	分段信息
*/
struct SubsectionInfo
{
	int* length;	//每一段的长度
	int* bits;		//每一段的像素的位数
	int sum;		//分的段数
};
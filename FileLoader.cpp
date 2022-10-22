#include <iostream>
#include <fstream>
#include <direct.h>
#include "FileLoader.h"

using namespace std;

void FileLoad(const char* filePath, void* readedData, int readSize)
{
	FILE* v1; // eax@1
	v1 = fopen(filePath, "rb");
	if (v1)
	{
		fread(readedData, 1, readSize, v1);
		fclose(v1);
	}
}

__int32 FileSize(const char* filePath)
{
	FILE* v1; // eax@1
	__int32 v3; // edi@2
	__int32 result; // eax@2

	v1 = fopen(filePath, "rb");
	if (v1)
	{
		fseek(v1, 0, 2);
		v3 = ftell(v1);
		fclose(v1);
		result = v3;
	}
	else
	{
		result = 0;
	}
	return result;
}

void FileWrite(const char* filePath, void* pointData, int writeSize) {
	char pathTemp[255];
	char* pathPoint = (char*)filePath;
	while (*pathPoint != '\0')
	{
		if (*pathPoint == '\\' || *pathPoint == '/') {
			unsigned int size = (pathPoint - filePath);
			strncpy(pathTemp, filePath, size);
			pathTemp[size] = '\0';

			if (!_mkdir(pathTemp)) {
				//printf("folderPath %s\n", pathTemp);
			}
		}
		pathPoint++;
	}

	FILE* v1; // eax@1
	v1 = fopen(filePath, "wb");
	if (v1) {
		fwrite(pointData, 1, writeSize, v1);
		fclose(v1);
		printf("file %s\n", filePath);
	}
}
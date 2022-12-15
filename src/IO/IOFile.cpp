#include <IO/IOFile.hpp>
#include <iostream>

void* LoadGameFile(const char* filePath){
	unsigned int fileSize = FileSize(filePath);
	if (fileSize>0){
		void* file = malloc(fileSize);
		FileLoadToMemory(filePath, file, fileSize);
		return file;
	}else{
		return 0;
	}
}

unsigned int FileSize(const char* filePath)
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

void FileLoadToMemory(const char* filePath, void* readedData, unsigned int readSize)
{
	FILE* v1; // eax@1
	v1 = fopen(filePath, "rb");
	if (v1)
	{
		fread(readedData, 1, readSize, v1);
		fclose(v1);
	}
}
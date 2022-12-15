#include <Wad.h>
#include <iostream>

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

void ReadData(char* data, unsigned int offsetHash, unsigned int point) {
	unsigned int* pointNext = 0;
	do
	{
		pointNext = (unsigned int*)((unsigned char*)data + offsetHash + point);
		point = *pointNext++;
		unsigned int pointer = *(unsigned int*)pointNext++;
		unsigned int size = *pointNext++;
		FileWrite((char*)pointNext, data + pointer, size);//Save to file
	} while (point);
}

bool WADReader(void* data) {
	unsigned int magic = *(unsigned int*)data;
	unsigned int size = *((unsigned int*)data + 1);
	unsigned int offsetHash = *((unsigned int*)data + 2);
	if (magic == 441600324) {
		unsigned int* pointHash = (unsigned int*)((unsigned char*)data + offsetHash);
		unsigned int hashMagic = *pointHash++;
		if (hashMagic == 10) {
			for (size_t i = 0; i < 1024; i++)
			{
				if (*pointHash) {
					ReadData((char*)data, offsetHash, *pointHash);
				}
				pointHash++;
			}
		}
		return true;
	}
	else {
		return false;
	}
}
/*
void SampleWad() {
	const char* file = "sfx.wad";
	int readedBytes = FileSize(file);
	char* data = NULL;
	if (readedBytes > 0) {
		data = (char*)malloc(readedBytes);
		FileLoad(file, data, readedBytes);
		if (WADReader(data)) {
			printf("Correct\n");
		}
	}
	if (data)
		free(data);
}
*/
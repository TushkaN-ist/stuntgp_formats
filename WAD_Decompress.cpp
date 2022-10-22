#include <iostream>
#include <fstream>
#include "FileLoader.h"
#include "TexturePC.h"
#include "Wad.h"
#include "ModelPMD.h"

void SamplePC(const char* file) {
	int readedBytes = FileSize(file);
	char* data = NULL;
	if (readedBytes > 0) {
		data = (char*)malloc(readedBytes);
		FileLoad(file, data, readedBytes);
		if (ReadPC(data)) {
			printf("Correct\n");
		}
	}
	if (data)
		free(data);
}
void SampleWad(const char *file) {
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
void SamplePMD(const char* file) {
	int readedBytes = FileSize(file);
	char* data = NULL;
	if (readedBytes > 0) {
		data = (char*)malloc(readedBytes);
		FileLoad(file, data, readedBytes);
		if (PMDRead(data)) {
			printf("Correct\n");
		}
	}
	if (data)
		free(data);
}

int main()
{
	SamplePC("cars/car04.pc");
	//SamplePC();
}
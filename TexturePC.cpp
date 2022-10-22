#include <iostream>
#include "TexturePC.h"
#include "FileLoader.h"
#include <math.h>

struct PCHeader {
	unsigned short version, width, height;
};

bool GenerateBmp(unsigned short width, unsigned short height, char*& dataBmp, unsigned int& totalSize) {
	const char* bmpDataHeader = "\x42\x4D\x3A\x00\x00\x00\x00\x00\x00\x00\x36\x00\x00\x00\x28\x00\x00\x00\x01\x00\x00\x00\x01\x00\x00\x00\x01\x00\x20\x00\x00\x00\x00\x00\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	unsigned int size = 54;
	unsigned int sizeData = width * height;
	totalSize = size + sizeData * 4;
	dataBmp = (char*)malloc(totalSize);
	if (dataBmp) {
		memcpy(dataBmp, bmpDataHeader, size);
		*(unsigned int*)(dataBmp + 2) = size + sizeData;
		unsigned int* wh = (unsigned int*)(dataBmp + 0x12);
		*wh++ = width;
		*wh = height;
		return true;
	}
	totalSize = 0;
	return false;
}

void setPixel(unsigned char* p, unsigned short c) {

	p[0] = round((c >> 10 & 31) * (255.0 / 31.0));
	p[1] = round((c >> 5 & 31) * (255.0 / 31.0));
	p[2] = round((c & 31) * (255.0 / 31.0));
	p[3] = 0xFF;
}

bool ReadPC(char* data) {
	unsigned int magic = *(unsigned int*)data;
	if (magic == 438390100) {
		PCHeader* header = (PCHeader*)(data + 4);
		printf("version %i\n", header->version);
		printf("width %i\n", header->width);
		printf("height %i\n", header->height);

		char* Bmpdata = 0;
		unsigned int size;
		if (GenerateBmp(header->width, header->height, Bmpdata, size)) {

			unsigned int* dataRGBA = (unsigned int*)(Bmpdata + 54);
			unsigned short* pixelData = (unsigned short*)(data + 10);
			setPixel((unsigned char*)dataRGBA, *pixelData++);
			unsigned int unpacked = 1;
			dataRGBA++;
			while ((char*)dataRGBA < Bmpdata + size) {
				unsigned short read = *pixelData;
				if (read >= 0x8000){
					setPixel((unsigned char*)dataRGBA, read);
					dataRGBA++;
					pixelData++;
				}
				else {
					if (read >= 0x4000) {
						unsigned int repeats = int(((read >> 11) & 7) + 2);
						unsigned int location = int(read & 0x7FF)+1;
						while (repeats-->0) {
							*(dataRGBA) = *(dataRGBA++ - location);
						}
						pixelData++;
					}else{
						if (read > 0) {
							memset(dataRGBA, 0, read*4);
							dataRGBA += read;
							pixelData++;
						}
					}
				}
			}
			FileWrite("OutTexture.bmp", Bmpdata, size);
			free(Bmpdata);
		}

		return true;
	}
	return false;
}
/*
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
*/
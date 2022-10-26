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

void setPixel(unsigned char* p, unsigned short c, bool swapRB) {

	p[0] = round((c >> 10 & 31) * (255.0 / 31.0));
	p[1] = round((c >> 5 & 31) * (255.0 / 31.0));
	p[2] = round((c & 31) * (255.0 / 31.0));
	p[3] = 0xFF;
	if (swapRB) {
		unsigned char b = p[2];
		p[2] = p[0];
		p[0] = b;
	}
}

void DecodeToBMP_V2(unsigned int* dataRGBA, char* pcData, char* finalPoint) {
	unsigned short paletteSize = *(unsigned short*)pcData;
	unsigned int* palette = (unsigned int*)(pcData+2);
	unsigned char* pixelPoint = (unsigned char*)(palette + paletteSize);

	while ((char*)dataRGBA < finalPoint) {
		*dataRGBA = palette[*pixelPoint];
		dataRGBA++;
		pixelPoint++;
	}
}

void DecodeToBMP_V345(unsigned short version,unsigned int* dataRGBA,char* pcData,char* finalPoint) {
	unsigned short* pixelData = (unsigned short*)pcData;
	bool swapRB = version == 5;
	setPixel((unsigned char*)dataRGBA, *pixelData++, swapRB);
	unsigned int unpacked = 1;
	dataRGBA++;
	while ((char*)dataRGBA < finalPoint) {
		unsigned short read = *pixelData;
		if (read >= 0x8000) {
			setPixel((unsigned char*)dataRGBA, read, swapRB);
			dataRGBA++;
			pixelData++;
		}
		else {
			if (read >= 0x4000) {
				unsigned int repeats = int(((read >> 11) & 7) + 2);
				unsigned int location = int(read & 0x7FF) + 1;
				while (repeats-- > 0) {
					*(dataRGBA) = *(dataRGBA++ - location);
				}
				pixelData++;
			}
			else {
				if (read > 0) {
					memset(dataRGBA, 0, read * 4);
					dataRGBA += read;
					pixelData++;
				}
			}
		}
	}
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
			switch (header->version)
			{
			case 2://PS2 version index color ✖
				DecodeToBMP_V2(dataRGBA, data+10, Bmpdata + size);
				break;
			case 3://PC version ✓
			case 4://DC version needed unwiddle ✖
			case 5://PS2 version need swap RB channel ✓
				DecodeToBMP_V345(header->version, dataRGBA, data+10, Bmpdata + size);
				break;
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

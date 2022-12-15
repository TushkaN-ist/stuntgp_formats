#include <TexturePC.h>
#include <GL/Extensions.h>
#include <iostream>

void UnpackIndex(unsigned short* pcdata, void* data, unsigned int size){
	unsigned short paletteSize = *pcdata;
	unsigned int* palette = (unsigned int*)(pcdata+1);
	unsigned int *imageData = (unsigned int*)data;
	unsigned char* pixelPoint = (unsigned char*)(palette + paletteSize);

	while (imageData < data + size) {
		*imageData = palette[*pixelPoint++];
		imageData++;
	}
}

void Upnack(unsigned short* pcData, void* pointData, unsigned int size){
	unsigned short *imageData = (unsigned short*)pointData;
	do{
		unsigned short read = *pcData++;
		if (read >= 0x8000){
			*imageData = read;
			imageData++;
		}else{
			if (read >= 0x4000) {
				unsigned int repeats = int(((read >> 11) & 7) + 2);
				unsigned int location = int(read & 0x7FF) + 1;
				while (repeats-- > 0) {
					*imageData = *(imageData - location);
					imageData++;
				}
			}
			else {
				if (read > 0) {
					memset(imageData, 0, read * 2);
					imageData += read;
				}
			}
		}
	}while(imageData < pointData + size);
}

void FillNoise(unsigned short* pcData, void* pointData, unsigned int size){
	unsigned short *imageData = (unsigned short*)pointData;
	while(imageData < pointData + size){
		*imageData = (unsigned short)rand()%65535;
		imageData++;
	}
}

void FillTexture(void* pcdata){
	unsigned short* pointData = (unsigned short*)pcdata;
	pointData += 2;
	unsigned short version = *pointData++;
	unsigned short w = *pointData++;
	unsigned short h = *pointData++;
	printf("version %i\twidth %i\theight %i\n", version, w, h);
	void* dataPoint = 0;
	switch (version)
	{
	case 2:
		dataPoint = malloc(w * h * 4);
		UnpackIndex(pointData, dataPoint, w * h * 4);
		glTexImage2D( GL_TEXTURE_2D, 0,  GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_INT, dataPoint );
		break;
	case 3:
	case 4:
		dataPoint = malloc(w * h * 2);
		Upnack(pointData, dataPoint, w * h * 2);
		glTexImage2D( GL_TEXTURE_2D, 0,  GL_RGB5_A1, w, h, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, dataPoint );
		break;
	case 5:
		dataPoint = malloc(w * h * 2);
		Upnack(pointData, dataPoint, w * h * 2);
		glTexImage2D( GL_TEXTURE_2D, 0,  GL_RGB5_A1, w, h, 0, GL_RGBA, GL_UNSIGNED_SHORT_1_5_5_5_REV, dataPoint );
		break;
	default:
		dataPoint = malloc(w * h * 2);
		FillNoise(pointData, dataPoint, w * h);
		glTexImage2D( GL_TEXTURE_2D, 0,  GL_RGB5_A1, w, h, 0, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV, dataPoint );
		break;
	}
	free(dataPoint);
}
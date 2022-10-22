#include <iostream>
#include <mem.h>
#include "ModelPMD.h"

using namespace std;
PMD::PMD(void* point) {
	header = (Header*)point;
	if (IsValid()) {
		char* startPoint = (char*)point + 0x148;
		
		SetOffsetPoint(0, vertexs2);
		SetOffsetPoint(1, polygons);
		SetOffsetPoint(2, mapUVs);
		SetOffsetPoint(3, vertexs);
		SetOffsetPoint(4, faces);
		SetOffsetPoint(5, names);
		SetOffsetPoint(6, offsetNames);
		SetOffsetPoint(7, boundsVertex);
		SetOffsetPoint(8, holdsIDs);
		SetOffsetPoint(9, transforms);
		SetOffsetPoint(10, meshs);
		/*
		PolyData polygons = [header->offsetsTableSize[1] / 8];//0x148 +
		UVMap uvsMap[offsetsTableSize[2] / 16];//0x148 +
		Vertex vertex[offsetsTableSize[3] / 8];//0x148 +
		short faces[offsetsTableSize[4] / 2];//0x148 +
		char textureNames[offsetsTableSize[5]];//0x148 +
		int offsetNames[offsetsTableSize[6] / 4];//0x148 +
		Vertex verticesData[offsetsTableSize[7] / 16];//0x148 +
		char verticesData[offsetsTableSize[8]];//0x148 +
		Transform transform[offsetsTableSize[9] / 80];//0x148 +
		Mesh meshs[offsetsTableSize[10] / 64];//0x148 +
		char lods[offsetsTableSize[11]];//0x148 +
		*/
	}
}

bool PMD::IsValid() {
	return strcmp("\x50\x4D\x44\x20\x56\x31\x2E\x38\x33\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", header->magic) == 0;
}

template<typename T>
void PMD::SetOffsetPoint(unsigned short offsetId, T*& point) {
	point = (T*)((char*)header + 0x148 + header->offsetsTable[offsetId]);
}

bool PMDRead(void* data) {
	PMD* pmd = new PMD(data);
	bool valide = pmd->IsValid();
	if (!valide)
		delete pmd;
	return valide;
}
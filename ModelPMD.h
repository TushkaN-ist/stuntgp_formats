#pragma once

struct PolyData {
	unsigned char vertexCount, materialId, unk1, unk2;
	short verticesId, faceStartIndex;
};

struct Transform {
	float posX,posY,posZ,posW,sclX, sclY, sclZ, sclW;
	float rot[8];
	int unknown[4];
};

struct UVMap {
	float x, y;
	short vertexID, unk1;
	int unk2;
};

struct Vertex {
	float x, y, z, w;
};

struct Lod {
	short unk0;
	short unk1;
	char unk[0x18];
	float always;
	int unkdata;
	int sufraceEach;
	int lod0, lod1, lod2, lod3;
	int numLods;
};

struct Mesh {
	short unk[4];
	short transformIndex;
	unsigned char zeros[18];
	float weight;
	short unk0_Index, unk1_Index, polysCount, UVsCount;
	int vertexCount, UVsPoint, vertPoint, polysPoint;
	int zeros3[2];
};
struct Header {
	char magic[0x18];
	int type;
	int zero;
	int offsetsTable[37];
	int offsetsTableSize[37];
};

class PMD {
public:
	Header *header;
	Transform *transforms;
	UVMap *mapUVs;
	Vertex *vertexs2;//Шар вертексов, зачем хз, возможно коллайдер или subvertex
	Vertex *vertexs;
	Vertex *boundsVertex;//Не уверен что так
	PolyData *polygons;
	Lod *lods;
	Mesh *meshs;
	unsigned short *faces;
	char *names;
	unsigned int *offsetNames;
	unsigned int *holdsIDs;
	
	template<typename T>
	void SetOffsetPoint(unsigned short offsetId, T*& point);
	PMD(void* point);
	bool IsValid();
};
bool PMDRead(void* data);
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
	unsigned short vertexID, unk1;
	int unk2;
};

struct Vertex {
	float x, y, z, w;
};


struct Mesh {
	short typeFile,typeData;
	short unk[2];
	short transformIndex;
	unsigned char zeros[18];
	float weight;
	short unk0_Index, unk1_Index, polysCount, UVsCount;
	int vertexCount;
	UVMap *UVsPoint;
	Vertex *vertPoint;
	PolyData *polysPoint;
	int zeros3[2];
};

struct Lod{
	short typeFile,typeData;
	char unk1[28];
	int unk2,size;
	unsigned int pointsData[8];//[0,1,2,3,4,9,8,10] << ids offset array
	Mesh *meshLOD[4];//size 4
	int sizeReconstructMesh,unk3;
	int point_6,unk4;//[6] << ids offset array
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
	Vertex *normals;//Шар вертексов, зачем хз, возможно коллайдер
	Vertex *vertexs;
	Vertex *boundsVertex;//Не уверен что так
	PolyData *polygons;
	Lod *lod;
	Mesh *meshs;
	unsigned short *faces;
	char *names;
	unsigned int *offsetNames;
	unsigned int *holdsIDs;
	
	template<typename T>
	void SetOffsetPoint(unsigned short offsetId, T*& point);
	PMD(void* point);
	bool IsValid();
	void LODFill();
	void ReconstructMesh();
	void ReconstructNames();
	void ReconstructHoldenIDS();
};
bool PMDRead(void* data);
void DrawMesh(Lod &pointLOD, Mesh &pointMesh);

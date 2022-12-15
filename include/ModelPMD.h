#pragma once

enum TypePMD : int{
	Shadow_Sky = 0,
	Track = 1,
	Car = 2
};

struct UVMap {
	float x, y;
	unsigned short vertexID, unk1;
	int unk2;
};

struct Vertex {
	float x, y, z, w;
};

struct Transform {
	float matrix[16];
	int unknown[4];
};

struct PolygonData {
	unsigned char vertexCount, textureID, materialPower, materialMask;
	short normalID, faceStartIndex;
	void Draw(Vertex* vertPoint, Vertex* normalsPoint, UVMap* UVsPoint, unsigned short* facesPoint);
};

struct OCTree{
	unsigned short nextTree, idB25, idB24, idBounce;
};

struct Mesh {
	short isVisible, specularMode;
	short unk[2];
	Transform *transformPoint;
	unsigned int zeros[4];
	float weight;
	short unk0_Index, unk1_Index, polysCount, UVsCount;
	int vertexCount;
	UVMap *UVsPoint;
	Vertex *vertPoint;
	PolygonData *polysPoint;
	Vertex *normalsPoint;//in original not used but i'm use
	unsigned short *facesPoint;//in original not used but i'm use
	void DrawPolygon(unsigned int id, unsigned int* textures);
	void Draw(unsigned int* textures);
};

struct PMDHeader;

class Lod{
	unsigned short type, materialMask, brighting, texturesCount;// materialMask - if 0C we have transparent and blend mesh, if 04 has default material
	unsigned int *textures,repointOnSelf;// textures - in original not used but i'm use ; repointOnSelf - unknown but is point on self by through pointer
	Vertex xyzwOffset;//global offset position
	unsigned short holdsOffsetUnk, holdsOffsetBounds;//in 8
	int countMeshLod;
	Vertex *normals;//0
	PolygonData *polygons;//1
	UVMap *uvMaps;//2
	Vertex *vertexMesh;//3
	unsigned short *faces;//4
	Transform *transforms;//9
	Vertex **holdsId;//8
	Mesh *meshs;//10
	Mesh *meshLOD[4];//size 4
	int sizeLods,currentLod;
	char **names;//6
	int prevLod;
public:
	void LoadTextures(const char* mainPath, char* alterFolders);
	void FreeTextures();
	char* GetNameTexture(unsigned int id);
	void RePoints(PMDHeader* header);
	void Draw();
	void Draw(unsigned char id);
	void Draw(unsigned int id, OCTree* octree, Vertex* bounds, unsigned short *block25, unsigned short *block24);
	void SetLod(unsigned char lod);
	Vertex *GetBounds();
};

struct AIPath{
	unsigned int id;
	Vertex *pointVertex, *endPoints;
	unsigned int unk1, unk2, unk3;
	float unk4;
	void Draw();
};

struct Block30{
	unsigned short v1, vertexFromB27, normal, v4;
};

struct Block31{
	Vertex position;
	unsigned short nextB31[8];
	unsigned short unk[8];
};

struct LB31{
	float lenght;
	Block31 *b31;
};

struct CollisionBlock{
	unsigned short *block28, *block27;
	Vertex *vertex, *normal;
	Block30 *block30;
	unsigned short *block29;
	LB31 data[2];
	float NU;

	unsigned short GetB28FromB31(Vertex &pos, Block31* current);
	unsigned short* GetB28Point(Vertex &pos, float lenght);
	void GetDataFromB30(Block30 &block);

	void Test(Vertex &pos, float lenght);
};

struct PMDHeader {
	char magic[9];
	char zeros[15];
	TypePMD type;
	unsigned int zero;
	int offsetsTable[37];
	int offsetsTableSize[37];
	template<typename T>
	void SetOffsetPoint(unsigned short offsetId, T*& point);
	void SetOffsetPoint(void* point);
	Lod* GetLodPoint();
	OCTree* TrackGetOCTree();
	Vertex* TrackGetBounds();
	AIPath* TrackGetAIPaths();
	CollisionBlock* TrackGetCollision();
	bool IsValid();
};
void DrawBounds(Vertex** point, unsigned short start);
void DrawBounds(Vertex* point);
void DrawLine(Vertex* pointA, Vertex* pointB);
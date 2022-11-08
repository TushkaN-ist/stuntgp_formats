#include <PMD/ModelPMD.h>
#include <GL/gl.h>

using namespace std;

PMD::PMD(void* point) {
	header = (Header*)point;
	if (IsValid()) {
		char* startPoint = (char*)point + 0x148;
		
		SetOffsetPoint(0, normals);
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
		SetOffsetPoint(11, lod);

		LODFill();
		ReconstructMesh();
		ReconstructNames();
		ReconstructHoldenIDS();
		if (lod->size==1){

		}
	}
}

void PMD::LODFill(){
	Lod &_lod = *lod;
	unsigned offsetPoint = (unsigned)header + 328;
	_lod.pointsData[0] = offsetPoint + header->offsetsTable[0];
	_lod.pointsData[1] = offsetPoint + header->offsetsTable[1];
	_lod.pointsData[2] = offsetPoint + header->offsetsTable[2];
	_lod.pointsData[3] = offsetPoint + header->offsetsTable[3];
	_lod.pointsData[4] = offsetPoint + header->offsetsTable[4];
	_lod.pointsData[5] = offsetPoint + header->offsetsTable[9];
	_lod.pointsData[6] = offsetPoint + header->offsetsTable[8];
	_lod.pointsData[7] = offsetPoint + header->offsetsTable[10];
	unsigned char v3 = 4;
	_lod.point_6 = offsetPoint + header->offsetsTable[6];
	unsigned int *result = (unsigned int*)_lod.meshLOD;
	do
	{
		*result = _lod.pointsData[7] + (*result << 6);
		++result;
		--v3;
	}
	while ( v3 );
}

void PMD::ReconstructMesh(){
	int result = lod->sizeReconstructMesh + 1;
	if (result != 0){
		unsigned int *pointOffsetMesh = (unsigned int*)lod->meshLOD;
		int v12 = result;
		bool v11 = true;
		do{
			unsigned char *pointMesh = (unsigned char*)*pointOffsetMesh;
			int v2 = lod->size;
			if (v2){
				int v3 = lod->pointsData[5];
				unsigned char *v4 = pointMesh + 44;
				do{
					unsigned int v5 = *(unsigned int *)(v4 - 36);
					unsigned int v6 = lod->pointsData[2];
					v4 += 64;
					*(unsigned int*)(v4 - 100) = v3 + 80 * v5;
					unsigned int v7 = v6 + 16 * *(unsigned int*)(v4 - 64);
					unsigned int v8 = lod->pointsData[3];
					*(unsigned int *)(v4 - 64) = v7;
					unsigned int v9 = v8 + 16 * *(unsigned int *)(v4 - 60);
					unsigned int v10 = *(unsigned int *)(v4 - 56);
					*(unsigned int *)(v4 - 60) = v9;
					--v2;
					*(unsigned int *)(v4 - 56) = lod->pointsData[1] + 8 * v10;
				}while(v2);
			}
			result = v12 - 1;
			v11 = v12 == 1;
			pointOffsetMesh++;
			--v12;
		}
		while ( !v11 );
	}
}
void PMD::ReconstructNames(){

	unsigned offsetPoint = (unsigned)header + 328;
	unsigned int *index =  (unsigned int*)(offsetPoint + header->offsetsTable[6]);
	if ( *index )
	{
		unsigned int* v4 = index;
		do
		{
			*index = offsetPoint + *index;
			++index;
		}
		while ( *index );
	}
}
void PMD::ReconstructHoldenIDS(){
	
	unsigned offsetPoint = (unsigned)header + 328 + header->offsetsTable[7];
	unsigned int *result = (unsigned int *)lod->pointsData[6];
	unsigned int v3=0;
	for (int i = header->offsetsTableSize[8] >> 2; i; *(result - 1) = offsetPoint + 16 * v3 )
	{
		v3 = *result;
		++result;
		--i;
	}
}

bool PMD::IsValid() {
	return strcmp("\x50\x4D\x44\x20\x56\x31\x2E\x38\x33\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", header->magic) == 0;
}

template<typename T>
void PMD::SetOffsetPoint(unsigned short offsetId, T*& point) {
	point = (T*)((char*)header + 0x148 + header->offsetsTable[offsetId]);
}

/*
car have offset for wheel
SAMPLE:
DrawMesh(*pmd->lod,*pmd->lod->meshLOD[0]);<< draw mainBody
DrawMesh(*pmd->lod,*(pmd->lod->meshLOD[0]+1));<< draw wheel
DrawMesh(*pmd->lod,*(pmd->lod->meshLOD[0]+2));<< draw wheel
DrawMesh(*pmd->lod,*(pmd->lod->meshLOD[0]+3));<< draw wheel
DrawMesh(*pmd->lod,*(pmd->lod->meshLOD[0]+4));<< draw wheel
*/
void DrawMesh(Lod &pointLOD, Mesh &pointMesh)
{
	unsigned int v17 = 0;
	if (pointLOD.typeData & 8 && pointMesh.typeData & 0x80)
		v17 = pointMesh.typeData >> 8; 
	Vertex* pointNormal = (Vertex *)pointLOD.pointsData[0];
	Vertex* pointVertex = pointMesh.vertPoint;
	UVMap* pointUV = pointMesh.UVsPoint;
	unsigned int polyCount = pointMesh.polysCount;
	unsigned short *pointFaces = (unsigned short*)pointLOD.pointsData[4];
	
	PolyData *pointPolygons = pointMesh.polysPoint;
	if ( polyCount )
	{
		unsigned int polyCount_t = pointMesh.polysCount;
		do
		{
			unsigned short* faceID = pointFaces + pointPolygons->faceStartIndex;
			
			Vertex *currentNormal = pointNormal + (pointPolygons->verticesId);
			unsigned int vertexID = (pointUV+*faceID)->vertexID;
			if (pointPolygons || (currentNormal, vertexID, 
				((pointVertex+vertexID)->z - 0) * currentNormal->z,
				((pointVertex+vertexID)->y - 0) * currentNormal->y,
				((pointVertex+vertexID)->x - 0) * currentNormal->x<0.0)
				){
					UVMap *UV;
					if (v17 || pointPolygons->vertexCount & 0x40000000){
						glBegin(GL_POLYGON);
						for(int i=0;i<pointPolygons->vertexCount;i++){
							UV = pointUV+*(faceID+i);
							glTexCoord2f(UV->x,UV->y);
							//glNormal3fv((float*)(currentNormal+i));
							glVertex4fv((float*)(pointVertex+UV->vertexID));
						}
						glEnd();
						/*pointPreDrawData2->faceID = faceID;
						pointPreDrawData2->poly = pointPolygons;
						pointPreDrawData2->uv = pointUV;
						pointPreDrawData2++;*/
					}else{
						glBegin(GL_POLYGON);
						for(int i=0;i<pointPolygons->vertexCount;i++){
							UV = pointUV+*(faceID+i);
							glTexCoord2f(UV->x,UV->y);
							//glNormal3fv((float*)(currentNormal+i));
							glVertex4fv((float*)(pointVertex+UV->vertexID));
						}
						glEnd();
						/*pointPreDrawData->faceID = faceID;
						pointPreDrawData->poly = pointPolygons;
						pointPreDrawData->uv = pointUV;
						pointPreDrawData++;*/
					}
			}
			pointPolygons++;
		} while (--polyCount_t);
	}
}

PMD* PMDRead(void* data) {
	PMD *outPMD = new PMD(data);
	bool valide = outPMD->IsValid();
	if (!valide){
		delete outPMD;
		outPMD = 0;
	}
	return outPMD;
}

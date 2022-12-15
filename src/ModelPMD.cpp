#include <ModelPMD.h>
#include <GL/gl.h>
#include <iostream>
#include <vector>
#include <TexturePC.h>
#include <IO/IOFile.hpp>

using namespace std;

template<typename T>
void PMDHeader::SetOffsetPoint(unsigned short offsetId, T*& point){
	point = (T*)((char*)this + 0x148 + offsetsTable[offsetId]);
}
void PMDHeader::SetOffsetPoint(void* point){
	unsigned int *index = (unsigned int*)point;
	*index = (unsigned)this + 0x148 + *index;
}

Lod* PMDHeader::GetLodPoint(){
	if (IsValid()) {
		Lod* lod;
		SetOffsetPoint(11, lod);
		lod->RePoints(this);
		return lod;
	}
	return nullptr;
}
bool PMDHeader::IsValid() {
	return strcmp("\x50\x4D\x44\x20\x56\x31\x2E\x38\x33", magic) == 0;
}

void AIPath::Draw(){
	glBegin(GL_LINES);
	Vertex* point = pointVertex;
	do
	{
		glVertex3fv((float*)point);
		point++;
		glVertex3fv((float*)point);
	}while (point<endPoints-1);
	glEnd();
}

AIPath* PMDHeader::TrackGetAIPaths(){
	Vertex* trackAIPoints;
	AIPath* point;
	SetOffsetPoint(21, trackAIPoints);
	SetOffsetPoint(22, point);
	for(AIPath* ai = point;ai<point+offsetsTableSize[22]/0x1c;ai++){
		ai->pointVertex = trackAIPoints + ((unsigned)ai->pointVertex);
		ai->endPoints = ai->pointVertex + ((unsigned)ai->endPoints);
	}
	return point;
}

OCTree* PMDHeader::TrackGetOCTree(){
	OCTree* point;
	SetOffsetPoint(26, point);
	return point;
}
Vertex* PMDHeader::TrackGetBounds(){
	Vertex* point;
	SetOffsetPoint(23, point);
	return point;
}
CollisionBlock* PMDHeader::TrackGetCollision(){
	CollisionBlock* point;
	SetOffsetPoint(32, point);

	SetOffsetPoint(3, point->vertex);
	SetOffsetPoint(0, point->normal);
	SetOffsetPoint(30, point->block30);
	SetOffsetPoint(29, point->block29);
	SetOffsetPoint(28, point->block28);
	SetOffsetPoint(27, point->block27);

	unsigned int i = 0, c=0;
	while(point->data[c].lenght>0){
		i = (unsigned int)point->data[c].b31;
		SetOffsetPoint(31, point->data[c].b31);
		point->data[c].b31+=i;
		c++;
	}

	return point;
}

void Lod::LoadTextures(const char* mainPath,char* alterFolders){
	char pathBuffer[255];
	char *pathPoint = pathBuffer;
	char *texName;
	bool alterFolder = false;
	unsigned int sizePre = strlen(mainPath);
	textures = new unsigned int[texturesCount];
	glGenTextures(texturesCount, textures);
	for(int i=0; i<texturesCount; i+=1){
		pathPoint = pathBuffer;
		texName = GetNameTexture(i);
		unsigned int size = strlen(texName);
		memcpy(pathPoint, mainPath, sizePre);
		pathPoint += sizePre;

		if (alterFolder){
			while(size--){
				texName++;
				if (*texName == '\\' || *texName == '/')
					break;
			}
			int s = strlen(alterFolders);
			memcpy(pathPoint, alterFolders, s);
			pathPoint += s;
		}

		memcpy(pathPoint, texName, size);
		pathPoint += size;
		memcpy(pathPoint, ".pc\0", 5);
		void* filePC = LoadGameFile(pathBuffer);
		if (filePC){
			printf("Load:%s\n",pathBuffer);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			FillTexture(filePC);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			free(filePC);
		}else{
			alterFolder = !alterFolder;
			if (alterFolder)
				i--;
			else
				printf("File not found in:%s\n",pathBuffer);
			//return;
		}
	}
}

void Lod::FreeTextures(){
	glDeleteTextures(texturesCount,textures);
}

void ConvertMatrix(float matrix[16],bool inverseX){
	float posXYZW[3] = {matrix[0],matrix[1],matrix[2]};
	float sclXYZW[3] = {matrix[4],matrix[5],matrix[6]};
	float rotUp[3] = {matrix[8],matrix[9],matrix[10]};
	float rotForward[3] = {matrix[12],matrix[13],matrix[14]};

	float cross[4] = {
		rotUp[1]*rotForward[2] - rotUp[2]*rotForward[1],
		rotUp[2]*rotForward[0] - rotUp[0]*rotForward[2],
		rotUp[0]*rotForward[1] - rotUp[1]*rotForward[0]};
	matrix[0] = -cross[0]*sclXYZW[0];
	matrix[1] = cross[1]*sclXYZW[0];
	matrix[2] = cross[2]*sclXYZW[0];
	matrix[3] = 0;

	matrix[4] = rotForward[0]*sclXYZW[1];
	matrix[5] = rotForward[1]*sclXYZW[1];
	matrix[6] = rotForward[2]*sclXYZW[1];
	matrix[7] = 0;

	matrix[8] = rotUp[0]*sclXYZW[2];
	matrix[9] = rotUp[1]*sclXYZW[2];
	matrix[10] = rotUp[2]*sclXYZW[2];
	matrix[11] = 0;

	matrix[12] = inverseX?-posXYZW[0]:posXYZW[0];
	matrix[13] = posXYZW[1];
	matrix[14] = posXYZW[2];
	matrix[15] = 1;
}

void Lod::RePoints(PMDHeader *header){
		header->SetOffsetPoint(0,normals);
		header->SetOffsetPoint(1,polygons);
		header->SetOffsetPoint(2,uvMaps);
		header->SetOffsetPoint(3,vertexMesh);
		header->SetOffsetPoint(4,faces);
		header->SetOffsetPoint(9,transforms);
		header->SetOffsetPoint(8,holdsId);
		header->SetOffsetPoint(10,meshs);
		header->SetOffsetPoint(6,names);
		unsigned int *index = (unsigned int*)names;
		while ( *index ){
			texturesCount++;
			header->SetOffsetPoint(index);
			++index;
		}
		index = (unsigned int*)meshLOD;
		unsigned char c = 4;
		do
		{
			*index = (unsigned)meshs + (*index << 6);
			++index;
			--c;
		}while ( c );
		Mesh *meshPoint = meshs;
		c = countMeshLod*(sizeLods+1);
		index = &header->zero;
		while(c--){
			*index = (unsigned int)meshPoint->transformPoint;
			meshPoint->transformPoint = transforms+*index;
			*index = (unsigned int)meshPoint->UVsPoint;
			meshPoint->UVsPoint = uvMaps+*index;
			*index = (unsigned int)meshPoint->polysPoint;
			meshPoint->polysPoint = polygons+*index;
			*index = (unsigned int)meshPoint->vertPoint;
			meshPoint->vertPoint = vertexMesh+*index;
			meshPoint->normalsPoint = normals;
			meshPoint->facesPoint = faces;
			meshPoint++;
		}
		*index = 0;
		c = header->offsetsTableSize[8] >> 2;
		Vertex *holdsVertex;
		header->SetOffsetPoint(7,holdsVertex);
		index = (unsigned int*)holdsId;
		while(c--){
			*index = (unsigned)holdsVertex + *index;
			index++;
		}
		c=0;
		while(c<countMeshLod){
			ConvertMatrix((transforms+c)->matrix, header->type == TypePMD::Car && (c%countMeshLod)>10 && (c%countMeshLod)<21);
			c++;
		}
}

//DrawCar
void Lod::Draw(){
	int count = 0;
	while(count < countMeshLod){
		Draw(count);
		count++;
	}
}

//DrawMesh
void Lod::Draw(unsigned char id){
	if (!(meshLOD[currentLod]+id)->isVisible)
		return;
	glPushMatrix();
	glMultMatrixf((meshLOD[currentLod]+id)->transformPoint->matrix);
	(meshLOD[currentLod]+id)->Draw(textures);
	glPopMatrix();
}

//Draw Track
void Lod::Draw(unsigned int id, OCTree* octree, Vertex *bounds, unsigned short *block25Polygons,unsigned short *block24Vertex){
	int count = 8;
	OCTree* point = octree+id;
	do{
		if (point->nextTree!=0xFFFF){
			if (point->nextTree){
				Draw((point->nextTree<<6)/8, octree, bounds, block25Polygons, block24Vertex);
			}else{
				//DrawBounds(bounds+point->idBounce);
				unsigned short *c = (block25Polygons+point->idB25);
				while(*c!=0xFFFF)
				{
					meshLOD[currentLod]->DrawPolygon(*c,textures);
					c++;
				}
				//used in old technologies cpu multiply vertex by matrix before render
				if (block24Vertex){
					c = (block24Vertex+point->idB24);
					glBegin(GL_POINTS);
					while(*c!=0xFFFF)
					{
						glVertex4fv((float*)(vertexMesh+*c));
						c++;
					}
					glEnd();
				}
			}
		}
		point++;
	}while(--count);
	count = 0;
	while(++count < countMeshLod){
		Draw(count);
	}
}

char* Lod::GetNameTexture(unsigned int id){
	return names[id];
}
Vertex* Lod::GetBounds(){
	return holdsId[holdsOffsetBounds];
}

void PolygonData::Draw(Vertex *vertPoint, Vertex *normalsPoint, UVMap *UVsPoint, unsigned short* facesPoint){
	facesPoint = facesPoint + faceStartIndex;
	UVMap *UV;
	normalsPoint+=normalID;
	glBegin(GL_POLYGON);
	for(int i=0; i<vertexCount; i++){
		UV = UVsPoint + *(facesPoint+i);
		glTexCoord2fv((float*)UV);
		glNormal3fv((float*)normalsPoint);
		glVertex4fv((float*)(vertPoint+UV->vertexID));
	}
	glEnd();
}

///
// if lod::typeData & 0x8 && mesh::specularMode & 0x80 || PolygonData::vertexCount & 0x40000000
// some mode On
///
void Mesh::DrawPolygon(unsigned int id,unsigned int* textures){
	glBindTexture(GL_TEXTURE_2D, textures[polysPoint[id].textureID]);
	polysPoint[id].Draw(vertPoint, normalsPoint, UVsPoint, facesPoint);
}
void Mesh::Draw(unsigned int* textures){
	for(int i=0; i<polysCount; i++){
		DrawPolygon(i,textures);
	}
}

unsigned short CollisionBlock::GetB28FromB31(Vertex &pos, Block31* current){
	
	signed int v2; // edx@2
		while ( 1 )
		{
		v2 = 0;
		if ( pos.x >= current->position.x )// X
			v2 = 1;
		if ( pos.y <= current->position.y )// Y
			v2 |= 4u;
		if ( pos.z <= current->position.z )// Z
			v2 |= 2u;
		if ( !current->nextB31[v2] )
			break;
			current = data[0].b31 + current->nextB31[v2];
		}
	return current->unk[v2];
}

unsigned short* CollisionBlock::GetB28Point(Vertex &pos, float lenght){
	unsigned int c=0;
	while(data[c].lenght>0){
		if (lenght <= data[c].lenght)
			return block28+GetB28FromB31(pos, data[c].b31);
		c++;
	}
}

void CollisionBlock::GetDataFromB30(Block30 &block){
	Vertex *normalP = normal + block.normal;
	Vertex *vertexP = vertex + *(block27+block.vertexFromB27);

}

void CollisionBlock::Test(Vertex &pos, float lenght){
	unsigned short* b28Point = GetB28Point(pos,lenght);
	while(*b28Point!=0xFFFF){
		GetDataFromB30(*(block30+*b28Point));
		b28Point++;
	}
}

void DrawBounds(Vertex* point){
	//printf("%X\n",point[start]);
	glBegin(GL_LINE_LOOP);
	glVertex4fv((float*)(point));
	glVertex4fv((float*)(point+1));
	glVertex4fv((float*)(point+2));
	glVertex4fv((float*)(point+3));
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex4fv((float*)(point+4));
	glVertex4fv((float*)(point+5));
	glVertex4fv((float*)(point+6));
	glVertex4fv((float*)(point+7));
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex4fv((float*)(point));
	glVertex4fv((float*)(point+1));
	glVertex4fv((float*)(point+5));
	glVertex4fv((float*)(point+4));
	glEnd();
	glBegin(GL_LINE_LOOP);
	glVertex4fv((float*)(point+3));
	glVertex4fv((float*)(point+2));
	glVertex4fv((float*)(point+6));
	glVertex4fv((float*)(point+7));
	glEnd();
}
void DrawLine(Vertex* pointA,Vertex* pointB){
	glBegin(GL_LINES);
	glVertex4fv((float*)(pointA));
	glVertex4fv((float*)(pointB));
	glEnd();
}

.PMD is model format 95% in progress
	- can load all 1.83v meshes
	- track collision not complete
	- alpha,blend,specular and other material effect not complete
.PC is texture format. unpacked 98%
	- version 4 (DC) need fix
.WAD is files directory packed. unpacked 100% 


work on clear old openGL, if use shaders use not great 130 version
Sample 130
vertex:
```shader
#version 130
in vec4 position;
out vec2 UV;
void main()
{
	gl_Position = gl_ModelViewProjectionMatrix * position;
	UV = gl_MultiTexCoord0.st;
}
```
fragment:
```shader
#version 130
uniform sampler2D tex;
in vec2 UV;
out vec4 finalColor;
void main()
{
	finalColor = texture(tex, UV);
}
```

load PMD
```cpp
	PMDHeader* pmd = (PMDHeader*)LoadGameFile("meshdata\\track4.PMD");
    Lod &lod = *pmd->GetLodPoint();
	OCTree *tree;
	Vertex *bounds;
	CollisionBlock *collision;
	AIPath *paths;
	unsigned short *block25, *block24;
	unsigned int size31 = pmd->offsetsTableSize[31]/48;
	if (pmd->type == TypePMD::Track){
		tree = pmd->TrackGetOCTree();
		bounds = pmd->TrackGetBounds();
		collision = pmd->TrackGetCollision();//work in progress
		pmd->SetOffsetPoint(25,block25);
		pmd->SetOffsetPoint(24,block24);
		paths = pmd->TrackGetAIPaths();
	}
	
	lod.LoadTextures("Unpacked\\graphics24\\","trackset00");
```
draw PMD
```cpp
	glLoadMatrixf(matrixCameraResult.m);
	switch(pmd->type){
		case TypePMD::Track:{
			lod.Draw(0, tree, bounds, block25, nullptr);
			paths->Draw();
		}break;
		case TypePMD::Car:{
			lod.Draw();
		}break;
	}
```

all information about file formats got from https://sgp.halamix2.pl/wiki/Main_Page

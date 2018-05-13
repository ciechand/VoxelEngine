#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoUtil.hpp"

FastNoise noiseSampler;

//Start of definitions for BaseMesh Class!
BaseMesh::BaseMesh(){
}

BaseMesh::BaseMesh(std::string filename){
	modelFile = filename;
}

BaseMesh::~BaseMesh(){

}

bool BaseMesh::loadModel(){
	if(modelFile.compare("") == 0)
		return false;
	vertices.clear();
	normals.clear();
	texCoords.clear();
	indices.clear();
	//Make sure the object file has a name already assigned
	if (modelFile.empty()){
		std::cerr << "Have not set a modelFile, Please do so before trying to load an obj file again." << std::endl;
		return false;
	}
//std::cerr << "Loading in Asset as : " << modelFile << std::endl;
	//opening the OBJ file for reading
	FILE * objFile = fopen(modelFile.c_str(), "r");
	if (objFile == nullptr){
		perror("THAT FILE DOES NOT EXIST");
		return false;
	}
	//creating a buffer to read the file into
	char fileBuffer[128];
	std::vector<GLuint> vertexIndices;
	std::vector<GLuint> normalIndices;
	std::vector<GLuint> textureIndices;
	while (fgets(fileBuffer, 128, objFile) != nullptr){
		//checking which identifier you are currently reading and assigning appropriately.
		char identifier[3];
		sscanf(fileBuffer, "%s", identifier);
		if (strcmp(identifier, "#") == 0){
			continue;
		} else if (strcmp(identifier, "v") == 0){
			glm::vec4 verts;
			sscanf(fileBuffer, "%*s %f %f %f", &verts.x, &verts.y, &verts.z);
			verts.w = 1.0;
			vertices.emplace_back(verts);
		} else if (strcmp(identifier, "vn") == 0){
			glm::vec4 norm;
			sscanf(fileBuffer, "%*s %f %f %f", &norm.x, &norm.y, &norm.z);
			norm.w = 0.0;
			normals.emplace_back(norm);
		} else if (strcmp(identifier, "vt") == 0){
			glm::vec2 tex;
			sscanf(fileBuffer, "%*s %f %f", &tex.x, &tex.y);
			texCoords.emplace_back(tex);
		} else if (strcmp(identifier, "f") == 0){
			GLuint indices[9];
			sscanf(fileBuffer, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5], &indices[6], &indices[7], &indices[8]);
			for (int j = 0; j < 3; j++){
				vertexIndices.push_back(indices[(3 * j)] - 1);
				normalIndices.push_back(indices[(3 * j) + 2] - 1);
				textureIndices.push_back(indices[(3 * j) + 1] - 1);
			}
		}else{
			perror("Incorrect Identifier!"); //WTF KINDA OBJ FILE ARE YOU READING?!
		}
	}

	fclose(objFile);
	std::map<vertexInfo, GLuint> indexMap;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> tex;

	//Fix all the indices so that it doesnt have to be done later.
	//This needs to be done since open gl only allows one index array for each object so, you need to reorganize the indices that are read in from the OBJ file.

	for (size_t i = 0; i < vertexIndices.size(); i++){
		struct vertexInfo v = {vertices[vertexIndices[i]], normals[normalIndices[i]], texCoords[textureIndices[i]]};
		bool exists = false;
		GLuint index = 0;
		//Checking for unique (vertex,normal,texture) triplets.
		std::map<vertexInfo, GLuint>::iterator it = indexMap.find(v);
		 
		if (it == indexMap.end()){
			exists = false;
		} else{
			index = it->second;
			exists = true;
		}
		if (exists){
			indices.emplace_back(index);
		} else{
			verts.push_back(vertices[vertexIndices[i]]);
			norms.push_back(normals[normalIndices[i]]);
			tex.push_back(texCoords[textureIndices[i]]);
			index = (GLuint)verts.size() - 1;
			indices.emplace_back(index);
			indexMap[v] = index;
		}
	};

	vertices.clear();
	normals.clear();
	texCoords.clear();
	//The vectors must be re-applied so that the order is correct in correspondence with the new index array.
	vertices = verts;
	normals = norms;
	texCoords = tex;
	size = indices.size();
	//std::cerr << "Finnished Loading in Asset of : " << modelFile << std::endl;
	return true;
}

void BaseMesh::addToController(){
	ShaderController.addBaseMesh(this);
}

void BaseMesh::clearMesh(){
	vertices.clear();
	texCoords.clear();
	normals.clear(); 
	indices.clear();
	colors.clear();
	modelMatrices.clear();
	size = 0;
}

unsigned int BaseMesh::getID(){
	return ID;
}

void BaseMesh::setID(unsigned int id){
	ID = id;
}

unsigned int BaseMesh::getVertStart(){
	return vertexStart;
}

void BaseMesh::setVertStart(unsigned int index){
	vertexStart = index;
}

unsigned int BaseMesh::getTexStart(){
	return texStart;
}

void BaseMesh::setTexStart(unsigned int index){
	texStart = index;
}

unsigned int BaseMesh::getNormalStart(){
	return normalStart;
}

void BaseMesh::setNormalStart(unsigned int index){
	normalStart = index;
}


std::vector<glm::vec4> BaseMesh::getVertices(){
	return vertices;
}

std::vector<glm::vec2> BaseMesh::getTexCoords(){
	return texCoords;
}

std::vector<glm::vec4> BaseMesh::getNormals(){
	return normals;
}

std::vector<unsigned int> BaseMesh::getIndices(){
	return indices;
}

void BaseMesh::addVertex(glm::vec3 vert, glm::vec3 norm, glm::vec3 color, glm::vec2 inputCoords){
	for(int i=0; i<vertices.size(); i++){
		if(glm::vec4(vert,1.0f) == vertices[i] && glm::vec4(norm,0.0f) == normals[i] && color == colors[i]){
			indices.push_back(i);
			return;
		}
	}
	indices.push_back(vertices.size());
	vertices.push_back(glm::vec4(vert,1.0f));
	normals.push_back(glm::vec4(glm::normalize(norm),0.0f));
	texCoords.push_back(inputCoords);
	colors.push_back(color);
}

void BaseMesh::addToColors(std::vector<glm::vec3>::iterator begining, std::vector<glm::vec3>::iterator ending){
	colors.insert(colors.end(), begining, ending);
}

void BaseMesh::addToColors(glm::vec3 color){
	colors.emplace_back(color);
}

std::vector<glm::vec3> BaseMesh::getColors(){
	return colors;
}

void BaseMesh::addToModels(std::vector<glm::mat4>::iterator begining, std::vector<glm::mat4>::iterator ending){
	modelMatrices.insert(modelMatrices.end(), begining, ending);
}

void BaseMesh::addToModels(glm::mat4 model){
	modelMatrices.emplace_back(model);
}

std::vector<glm::mat4> BaseMesh::getModels(){
	return modelMatrices;
}

//Start of MESH Class
Mesh::Mesh(){
}

Mesh::~Mesh(){

}

void Mesh::GenerateMesh(Block * parentBlock){
	if(meshIDs.size() != 0 || textureIDs.size() != 0 || colors.size()  != 0 || modelMatrix.size() != 0)
			std::cerr << "DONT TRY AND GENERATE A MESH WHEN THE PREVIOUS ONE HAS NOT BEEN CLEARED!!!" << std::endl;
	std::array<bool,6> tempActiveArr = parentBlock->getActiveSide();
	for(int i=0; i<6; i++){
		if(tempActiveArr[i] == true){
			GenerateCubeSide((CubeFace)i, parentBlock->getSideColor(i), parentBlock->getPosition());
		}
	}
}

void Mesh::GenerateCubeSide(CubeFace face, glm::vec3 c, glm::vec3 offset){
	//std::cerr << "Offset: \n\tX: " << offset.x << "\n\tY: " << offset.y << "\n\tZ: " << offset.z << std::endl; 
	glm::mat4 translated = glm::translate(glm::mat4(), (DirectionVectors[face]*0.5f));
	glm::mat4 rotated;
	switch(face){
		case RightFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			for(int i=0; i<4; i++){
				addColorToMesh(c);
			}
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(-90.0f),glm::vec3(0,1,0));
			addMatrixToMesh(translated*rotated);
			break;
		case LeftFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			for(int i=0; i<4; i++){
				addColorToMesh(c);
			}
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(90.0f),glm::vec3(0,1,0));
			addMatrixToMesh(translated*rotated);
			break;
		case TopFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			for(int i=0; i<4; i++){
				addColorToMesh(c);
			}
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(-90.0f),glm::vec3(1,0,0));
			addMatrixToMesh(translated*rotated);
			break;
		case BottomFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			for(int i=0; i<4; i++){
				addColorToMesh(c);
			}
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(90.0f),glm::vec3(1,0,0));
			addMatrixToMesh(translated*rotated);
			break;
		case FrontFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			//No rotation needed
			for(int i=0; i<4; i++){
				addColorToMesh(c);
			}
			rotated = glm::mat4();
			addMatrixToMesh(translated*rotated);
			break;
		case BackFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			for(int i=0; i<4; i++){
				addColorToMesh(c);
			}
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(180.0f),glm::vec3(0,1,0));
			addMatrixToMesh(translated*rotated);
			break;
		default:
			std::cerr << "INVALID SIDE GENERATION ATTEMPT." << std::endl;
			break;
	}
}

void Mesh::addBaseToMesh(int baseID){
	meshIDs.emplace_back(baseID);
}

std::vector<int> Mesh::getBase(){
	return meshIDs;
}

int Mesh::getBase(unsigned int index){
	return meshIDs[index];
}

void Mesh::addTexIDToMesh(int texID){
	textureIDs.emplace_back(texID);
}

std::vector<int> Mesh::getTexID(){
	return textureIDs;
}

int Mesh::getTexID(unsigned int index){
	return textureIDs[index];
}

void Mesh::addColorToMesh(glm::vec3 c){
	colors.emplace_back(c);
}

std::vector<glm::vec3> Mesh::getColors(){
	return colors;
}

glm::vec3 Mesh::getColors(unsigned int index){
	return colors[index];
}

void Mesh::addMatrixToMesh(glm::mat4 mm){
	modelMatrix.emplace_back(mm);
}

std::vector<glm::mat4> Mesh::getMatrix(){
	return modelMatrix;
}

glm::mat4 Mesh::getMatrix(unsigned int index){
	return modelMatrix[index];
}

void Mesh::addMeshToRenderer(glm::mat4 objectMatrix){
	if(colors.size() == 0)
		std::cerr << "Trying to add a mesh with nothing in it." << std::endl;

	std::vector<BaseMesh *> controllerBases = ShaderController.getBaseMeshes();
	if(controllerBases.size() == 0)
		std::cerr << "attempting to add a mesh without any base meshes." << std::endl;

	for(int i=0; i<meshIDs.size(); i++){
		controllerBases[meshIDs[i]]->addToColors(colors[i]);
		controllerBases[meshIDs[i]]->addToModels(objectMatrix*modelMatrix[i]);
	}
}


//Constructors for the Block Class
Block::Block(){
	sideColors.fill(BlockColors[None]);
}

Block::~Block(){

}

bool Block::isSame(Block* b){
	for(int i=0; i<6; i++){
		if(activeSides[i] != b->getActiveSide(i))
			return false;
		if(sideColors[i] != b->getSideColor(i))
			return false;
		if(sideTextures[i] != b->getSideTexture(i))
			return false;
	}
	return true;
}

bool Block::isSideSame(Block * b, unsigned int side){
	if(activeSides[side] != b->getActiveSide(side))
		return false;
	if(sideColors[side] != b->getSideColor(side))
		return false;
	if(sideTextures[side] != b->getSideTexture(side))
		return false;
	return true;
}

bool Block::getActive(){
	return blockActive;
}

void Block::setActive(bool ba){
	blockActive = ba;
}

glm::vec3 Block::getPosition(){
	return position;
}

void Block::setPosition(glm::vec3 pos){
	position = pos;
}

bool Block::getActiveSide(unsigned int n){
	return activeSides[n];
}

std::array<bool,6> Block::getActiveSide(){
	return activeSides;
}

void Block::setActiveSide(unsigned int n, bool s){
 	activeSides[n] = s;
}

void Block::setActiveSide(std::array<bool,6> s){
	activeSides = s;
}

glm::vec3 Block::getSideColor(unsigned int n){
	return sideColors[n];
}

std::array<glm::vec3,6> Block::getSideColor(){
	return sideColors;
}

void Block::setSideColor(unsigned int n, VoxelColor s){
	sideColors[n] = BlockColors[s];
}

void Block::setSideColor(std::array<glm::vec3,6> s){
	sideColors = s;
}

void Block::setColor(VoxelColor c){
	sideColors.fill(BlockColors[c]);
}

int Block::getSideTexture(unsigned int n){
	return sideTextures[n];
}

std::array<int,6> Block::getSideTexture(){
	return sideTextures;
}

void Block::setSideTexture(unsigned int n, int s){
	sideTextures[n] = s;
}

void Block::setSideTexture(std::array<int,6> s){
	sideTextures = s;
}

//Constructors for Chunk Class

Chunk::Chunk():Chunk(glm::vec3(0,0,0)){

}

Chunk::Chunk(glm::vec3 pos){
	setPos(pos);
	Grid.fill(nullptr);
	//GridMask.fill(false);
	adjacentChunks.fill(-1);
	for(int i=0; i<CHUNKSIZE; i++){
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		glm::vec3 worldBlockPos = ((blockPos)+((float)CHUNKSIDE*chunkPos))-(glm::vec3(CHUNKSIDE)/2.0f);
		//if((blockPos.x >= CHUNKSIDE/2 && blockPos.x <= CHUNKSIDE-5.0f) || (blockPos.y >= 4 && blockPos.y <= CHUNKSIDE-5.0f) || (blockPos.z >= 4 && blockPos.z <= CHUNKSIDE-5.0f))
		//if((blockPos.x == CHUNKSIDE/2 && blockPos.z == CHUNKSIDE/2 && blockPos.y >= CHUNKSIDE/2) || blockPos.y < CHUNKSIDE/2 )
		//	Grid[i] = new Block();
		float height = noiseSampler.GetNoise(worldBlockPos.x,worldBlockPos.z);
		height *= CHUNKSIDE;
		if(worldBlockPos.y <= height)
			Grid[i] = new Block();

	}
	if(DEBUGMODE == true) std::cerr << "Finnished Loading Blocks for Chunk:  " << chunkPos.x << ", " <<chunkPos.y  << ", " << chunkPos.z << ";" << std::endl;

}

Chunk::~Chunk(){
	//DELETE THIS CHUNK FROM ALL THE ADJACENT CHUNKS SO THAT THEY DONT TRY AND ACCESS IT

	for(int i=0; i<CHUNKSIZE; i++){
		if(Grid[i] != nullptr)
			delete Grid[i];
	}
}
void Chunk::initializeMesh(){
	for(int i=0; i<6; i++){
		glm::vec3 checkChunkPos = getPos()+DirectionVectors[i];
		for(int j=0; j<ChunkContainer.size(); j++){
			if(ChunkContainer[j]->getPos() == checkChunkPos)
				adjacentChunks[i] = j;
		}
	}

	// std::cerr << "The adjacent Chunks are: " << std::endl;
	// for(int i=0; i<6; i++){
	// 	std::cerr << "\t"<< i << ":" << adjacentChunks[i] << std::endl;
	// }
	for(int i=0; i<CHUNKSIZE; i++){
		if(Grid[i] == nullptr)
			continue;
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		Grid[i]->setPosition(((blockPos)+((float)CHUNKSIDE*chunkPos))-(glm::vec3(CHUNKSIDE)/2.0f));
		Grid[i]->setColor(None);
		//Grid[i]->setColor((VoxelColor)((i/CHUNKSIDE)%CHUNKSIDE));
		/*if(DEBUGMODE == true){
			glm::vec3 tempBPos = Grid[i]->getPosition();
			std::cerr << "Current Block Pos: \n\tX: " << tempBPos.x << "\n\tY: " << tempBPos.y << "\n\tZ: " << tempBPos.z << std::endl;
		}*/
		for(int s=0; s<3; s++){
			Block* checkBlock = nullptr;
			if(blockPos[s] == 0.0f){
				glm::vec3 transBlockPos = blockPos;
				transBlockPos[s] = CHUNKSIDE-1;
				if(adjacentChunks[(s*2)+1] != -1)
					checkBlock = ChunkContainer[adjacentChunks[(s*2)+1]]->getBlock(transBlockPos);
				if(checkBlock != nullptr && checkBlock->getActive() == true) {
					Grid[i]->setActiveSide((s*2)+1, false);
					checkBlock->setActiveSide((s*2), false);
				}
				checkBlock = nullptr;
				transBlockPos = blockPos+DirectionVectors[s*2];
				checkBlock = this->getBlock(transBlockPos);
				if(checkBlock != nullptr && checkBlock->getActive() == true){
					Grid[i]->setActiveSide((s*2), false);
		 			checkBlock->setActiveSide((s*2)+1, false);
				}
				continue;
			}else if(blockPos[s] == (CHUNKSIDE-1)){
				glm::vec3 transBlockPos = blockPos;
				transBlockPos[s] = 0.0f;
				if(adjacentChunks[(s*2)] != -1)
					checkBlock = ChunkContainer[adjacentChunks[(s*2)]]->getBlock(transBlockPos);
				if(checkBlock != nullptr && checkBlock->getActive() == true) {
					Grid[i]->setActiveSide((s*2), false);
		 			checkBlock->setActiveSide((s*2)+1, false);

				}
				checkBlock = nullptr;
				transBlockPos = blockPos+DirectionVectors[(s*2)+1];
				checkBlock = this->getBlock(transBlockPos);
				if(checkBlock != nullptr && checkBlock->getActive() == true){
					Grid[i]->setActiveSide((s*2)+1, false);
		 			checkBlock->setActiveSide((s*2), false);

				}
				continue;
			}
			
			glm::vec3 transBlockPos;
			checkBlock = nullptr;

			transBlockPos = blockPos+DirectionVectors[(s*2)];
			checkBlock = this->getBlock(transBlockPos);
			if(checkBlock != nullptr && checkBlock->getActive() == true){
				Grid[i]->setActiveSide((s*2), false);
	 			checkBlock->setActiveSide((s*2)+1, false);
			}

			checkBlock = nullptr;
			transBlockPos = blockPos+DirectionVectors[(s*2)+1];
			checkBlock = this->getBlock(transBlockPos);
			if(checkBlock != nullptr && checkBlock->getActive() == true){
				Grid[i]->setActiveSide((s*2)+1, false);
	 			checkBlock->setActiveSide((s*2), false);
			}
		}
	}
}

void Chunk::updateMesh(){
	for(int i=0; i<Grid.size(); i++){
		//Somewhere in this loop I need to setup/update the greedy mesh.
		if(Grid[i] == nullptr)
			continue;
		bool empty = true;
		for(int s=0;s<6;s++){
			if(Grid[i]->getActiveSide(s)){
				empty = false;
				break;
			}
		}
		if(empty == true)
			Grid[i]->setActive(false);
	}
}


//Functions for chunk class 
void Chunk::GenerateMesh(){
	initializeMesh();
	if(DEBUGMODE == true) std::cerr << "initialized Mesh" << std::endl;
	updateMesh();
	if(DEBUGMODE == true) std::cerr << "updated Mesh" << std::endl;

	//std::array<unsigned int, CHUNKSIDE*CHUNKSIDE> ChunkMask = {0};
	chunkMesh.clearMesh();
	chunkMesh.addToModels(glm::translate(glm::mat4(1.0f),chunkPos*(float)CHUNKSIDE));
	glm::vec3 currentQueryPos = glm::vec3(0.0f,0.0f,0.0f);

	unsigned int currentAxis = 0;
	unsigned int direction1 = 0;
	unsigned int direction2 = 0;

	unsigned int difTypeNum = 0;

	for(int sign=0; sign<2; sign++){
		for(int i=0; i<3; i++){
			currentAxis = ((i*2)+((sign+1)%2))%6;
			//int px = ((i+2+(2*((i+1)%2)))%6);
			//int py = ((i+2+(2*(i%2)))%6);

			direction1 = (i+1)%3;
			direction2 = (i+2)%3;
			int px = ((direction1*2)+(sign))%6;
			int py = ((direction2*2)+(sign))%6;

			//std::cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			//std::cerr << "dir: " << currentAxis << std::endl;
			//std::cerr << "px: " << px << std::endl;
			//std::cerr << "py: " << py << std::endl;
			//std::cerr << "dir1: " << direction1 << std::endl;
			//std::cerr << "dir2: " << direction2 << std::endl;
			currentQueryPos = glm::vec3(0.0f,0.0f,0.0f);

			for(currentQueryPos[i]=0; currentQueryPos[i]<CHUNKSIDE;currentQueryPos[i]++){
				GridMask.fill(false);

				for(currentQueryPos[direction2] = 0; currentQueryPos[direction2] < CHUNKSIDE; currentQueryPos[direction2]++){
					for(currentQueryPos[direction1] = 0; currentQueryPos[direction1] < CHUNKSIDE;){
						unsigned int curIndex = translate3DPos(currentQueryPos, CHUNKSIDE);
						glm::vec3 xmod = DirectionVectors[(direction1*2)%6];
						glm::vec3 ymod = DirectionVectors[(direction2*2)%6];
						if(Grid[curIndex]!=nullptr && GridMask[(currentQueryPos[direction2]*CHUNKSIDE)+currentQueryPos[direction1]] != true){
							if(!Grid[curIndex]->getActiveSide(currentAxis)){
								GridMask[(currentQueryPos[direction2]*CHUNKSIDE)+currentQueryPos[direction1]] = true;
								currentQueryPos[direction1]++;
								continue;
							}
							int w, h;
							for(w = 1; (currentQueryPos+(xmod*(float)w))[direction1]>=0 && 
										(currentQueryPos+(xmod*(float)w))[direction1] < CHUNKSIDE && 
										Grid[translate3DPos(currentQueryPos+(xmod*(float)w), CHUNKSIDE)] != nullptr && 
										Grid[translate3DPos(currentQueryPos+(xmod*(float)w), CHUNKSIDE)]->getActiveSide(currentAxis) && 
										Grid[translate3DPos(currentQueryPos+(xmod*(float)w), CHUNKSIDE)]->isSideSame(Grid[curIndex], currentAxis) && 
										!GridMask[(currentQueryPos[direction2]*CHUNKSIDE)+(currentQueryPos[direction1]+w)]; w++){}

							bool done =  false;
							for(h = 1; (currentQueryPos+(ymod*(float)h))[direction2] >= 0 && (currentQueryPos+(ymod*(float)h))[direction2] < CHUNKSIDE; h++){
								for(int k=0; k < w; k++){
									if(Grid[translate3DPos(currentQueryPos+(ymod*(float)h)+(xmod*(float)k), CHUNKSIDE)]== nullptr || 
										!(Grid[translate3DPos(currentQueryPos+(ymod*(float)h)+(xmod*(float)k), CHUNKSIDE)]->isSideSame(Grid[curIndex], currentAxis)) || 
										!Grid[translate3DPos(currentQueryPos+(ymod*(float)h)+(xmod*(float)k), CHUNKSIDE)]->getActiveSide(currentAxis) ||
										GridMask[((currentQueryPos[direction2]+h)*CHUNKSIDE)+(currentQueryPos[direction1]+k)]){done = true; break;}
								}
								if(done){break;}
							}
							//std::cerr << "Current Pos:\n\tX: " <<currentQueryPos.x << "\n\tY: " <<currentQueryPos.y << "\n\tZ: " <<currentQueryPos.z << std::endl;
							//std::cerr << "w: " << w << std::endl;
							//std::cerr << "h: " << h << std::endl;
							glm::vec3 displacement = glm::vec3(0,0,0);
							displacement += xmod*(float)(w-1);
							displacement += ymod*(float)(h-1);
							if(Grid[curIndex]->getActiveSide(currentAxis)){
								GenerateFace(currentQueryPos, currentQueryPos+displacement, currentAxis, Grid[curIndex]->getSideColor(currentAxis));
							}

							for(int height=currentQueryPos[direction2]; height<currentQueryPos[direction2]+(h); height++){
								for(int width=currentQueryPos[direction1];width<currentQueryPos[direction1]+(w); width++){
									GridMask[(height*CHUNKSIDE)+width] = true;
								}
							}
							currentQueryPos[direction1]+=w; // need to add the relative x coordinate. should always be w. 
						}else{
							GridMask[(currentQueryPos[direction2]*CHUNKSIDE)+currentQueryPos[direction1]] = true;
							currentQueryPos[direction1]++;
						}
					}
				}
			}
		}
	}
	if(DEBUGMODE == true) std::cerr << "Generated Mesh" << std::endl;
	ShaderController.addBaseMesh(&chunkMesh);
}

void Chunk::GenerateFace(glm::vec3 start, glm::vec3 end, unsigned int dir, glm::vec3 color){

	//std::cerr << "dirinside: " << dir  << std::endl;
	//std::cerr << "DirectionVectors[Dir]:\n\tX: " <<DirectionVectors[dir].x << "\n\tY: " <<DirectionVectors[dir].y << "\n\tZ: " <<DirectionVectors[dir].z << std::endl;
	//std::cerr << "Start Point:\n\tX: " <<start.x << "\n\tY: " <<start.y << "\n\tZ: " <<start.z << std::endl;
	//std::cerr << "End Point:\n\tX: " <<end.x << "\n\tY: " <<end.y << "\n\tZ: " <<end.z << std::endl;
	//int px=(dir+2+(2*((dir+1)%2)))%6;
	//int py=(dir+2+(2*(dir%2)))%6;
	//int nx=(dir+(((dir%2)==0)?5:1))%6;
	//int ny=(dir+3)%6; 
	int px = ((((dir/2)+1)*2)+(((dir%2)==1)?1:0))%6;
	int py = ((((dir/2)+2)*2)+(((dir%2)==1)?1:0))%6;
	int nx = ((((dir/2)+1)*2)+(((dir%2)==0)?1:0))%6;
	int ny = ((((dir/2)+2)*2)+(((dir%2)==0)?1:0))%6;

	int xmod = px/2;
	int ymod = py/2;
	//std::cerr << "px: " << px << std::endl;
	//std::cerr << "py: " << py << std::endl;
	//std::cerr << "nx: " << nx << std::endl;
	//std::cerr << "ny: " << ny << std::endl;
	//std::cerr << "xmod: " << xmod << std::endl;
	//std::cerr << "ymod: " << ymod << std::endl;

	//glm::vec3 alter = glm::vec3(0.1f);

	glm::vec3 Point1 = glm::vec3(0.0f);
	glm::vec3 Point2 = glm::vec3(0.0f);
	glm::vec3 Point3 = glm::vec3(0.0f); 
	glm::vec3 Point4 = glm::vec3(0.0f);

	glm::vec2 texPoint1 = glm::vec2(0.0f,0.0f);
	glm::vec2 texPoint2 = glm::vec2(0.0f,1.0f);
	glm::vec2 texPoint3 = glm::vec2(1.0f,0.0f);
	glm::vec2 texPoint4 = glm::vec2(1.0f,1.0f);

	Point1 = start;

	Point2[xmod] = start[xmod];
	Point2[ymod] = end[ymod];
	Point2[dir/2] = start[dir/2];

	Point3[xmod] = end[xmod];
	Point3[ymod] = start[ymod];
	Point3[dir/2] = start[dir/2];

	Point4 = end;

/*	Point1 += (alter*glm::vec3(((px%2)*2)-1,((py%2)*2)-1,((dir%2)*2)-1));
	Point2 += (alter*glm::vec3(((px%2)*2)-1,((py%2)*2)-1,((dir%2)*2)-1));
	Point3 += (alter*glm::vec3(((px%2)*2)-1,((py%2)*2)-1,((dir%2)*2)-1));
	Point4 += (alter*glm::vec3(((px%2)*2)-1,((py%2)*2)-1,((dir%2)*2)-1));*/

	if(dir%2 == 0){
		Point1 += ((DirectionVectors[nx]+DirectionVectors[ny]+DirectionVectors[dir])*(CUBESIZE/2)) + chunkPos*(float)CHUNKSIDE;
		Point2 += ((DirectionVectors[nx]+DirectionVectors[py]+DirectionVectors[dir])*(CUBESIZE/2)) + chunkPos*(float)CHUNKSIDE;
		Point3 += ((DirectionVectors[px]+DirectionVectors[ny]+DirectionVectors[dir])*(CUBESIZE/2)) + chunkPos*(float)CHUNKSIDE;
		Point4 += ((DirectionVectors[px]+DirectionVectors[py]+DirectionVectors[dir])*(CUBESIZE/2)) + chunkPos*(float)CHUNKSIDE;

		chunkMesh.addVertex(Point1,DirectionVectors[dir],color,texPoint1);
		chunkMesh.addVertex(Point3,DirectionVectors[dir],color,texPoint3);
		chunkMesh.addVertex(Point2,DirectionVectors[dir],color,texPoint2);

		chunkMesh.addVertex(Point4,DirectionVectors[dir],color,texPoint4);
		chunkMesh.addVertex(Point2,DirectionVectors[dir],color,texPoint2);
		chunkMesh.addVertex(Point3,DirectionVectors[dir],color,texPoint3);


	}else{
		Point1 += ((DirectionVectors[px]+DirectionVectors[py]+DirectionVectors[dir])*(CUBESIZE/2))  + chunkPos*(float)CHUNKSIDE;
		Point2 += ((DirectionVectors[px]+DirectionVectors[ny]+DirectionVectors[dir])*(CUBESIZE/2))  + chunkPos*(float)CHUNKSIDE;
		Point3 += ((DirectionVectors[nx]+DirectionVectors[py]+DirectionVectors[dir])*(CUBESIZE/2))  + chunkPos*(float)CHUNKSIDE;
		Point4 += ((DirectionVectors[nx]+DirectionVectors[ny]+DirectionVectors[dir])*(CUBESIZE/2))  + chunkPos*(float)CHUNKSIDE;

		chunkMesh.addVertex(Point1,DirectionVectors[dir],color,texPoint1);
		chunkMesh.addVertex(Point2,DirectionVectors[dir],color,texPoint2);
		chunkMesh.addVertex(Point3,DirectionVectors[dir],color,texPoint3);

		chunkMesh.addVertex(Point4,DirectionVectors[dir],color,texPoint4);
		chunkMesh.addVertex(Point3,DirectionVectors[dir],color,texPoint3);
		chunkMesh.addVertex(Point2,DirectionVectors[dir],color,texPoint2);

	}
}

glm::vec3 Chunk::getPos(){
	return chunkPos;
}

void Chunk::setPos(glm::vec3 pos){
	chunkPos = pos;
}

std::array<Block*, CHUNKSIZE> Chunk::getBlock(){
	return Grid;
}

Block* Chunk::getBlock(unsigned int index){
	return Grid[index];
}

Block* Chunk::getBlock(glm::vec3 pos){
	unsigned int index = translate3DPos(pos, CHUNKSIDE);
	if(index >= CHUNKSIZE)
		return nullptr;
	return Grid[index];
}
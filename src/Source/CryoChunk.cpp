#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoUtil.hpp"


//Start of definitions for BaseMesh Class!
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
	//Todo, pretty simple, call a function in render controller passing in this mesh.
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

void BaseMesh::addVertex(glm::vec4 vert, glm::vec4 norm, glm::vec3 color){
	for(int i=0; i<vertices.size(); i++){
		if(vert == vertices[i]){
			indices.push_back(i);
			addToColors(color);
			return;
		}
	}
	indices.push_back(vertices.size());
	vertices.push_back(vert);
	normals.push_back(norm);
	texCoords.push_back(glm::vec2(0.0f,1.0f));
	colors.push_back();
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
	setScale(glm::vec3(CUBESIZE,CUBESIZE,CUBESIZE));
	sideColors.fill(BlockColors[None]);
	objectMatrix.fill(glm::mat4());
}

Block::~Block(){
	if (blockMesh != nullptr)
		delete blockMesh;
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

bool isSideSame(Block * b, unsigned int side){
	if(activeSides[side] != b->getActiveSide(side))
		return false;
	if(sideColors[side] != b->getSideColor(side))
		return false;
	if(sideTextures[side] != b->getSideTexture(side))
		return false;
	return true;
}

Mesh * Block::getMesh(){
	return blockMesh;
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
	objectMatrix[TranslateMatrix] = glm::translate(glm::mat4(), pos);
	objectMatrix[CombinedMatrix] = objectMatrix[TranslateMatrix]*objectMatrix[RotationMatrix]*objectMatrix[ScaleMatrix];
	position = pos;
}
	
void Block::setRotation(float angle, glm::vec3 axis){
	objectMatrix[RotationMatrix] = glm::rotate(glm::mat4(), angle, axis);
	objectMatrix[CombinedMatrix] = objectMatrix[TranslateMatrix]*objectMatrix[RotationMatrix]*objectMatrix[ScaleMatrix];
	
}

void Block::setScale(glm::vec3 scale){
	objectMatrix[ScaleMatrix] = glm::scale(glm::mat4(), scale);
	objectMatrix[CombinedMatrix] = objectMatrix[TranslateMatrix]*objectMatrix[RotationMatrix]*objectMatrix[ScaleMatrix];
	
}

glm::mat4 Block::getObjectMatrix(){
	return objectMatrix[CombinedMatrix];
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
	adjacentChunks.fill(-1);
	for(int i=0; i<CHUNKSIZE; i++){
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		//if((blockPos.x >= CHUNKSIDE/2 && blockPos.x <= CHUNKSIDE-5.0f) || (blockPos.y >= 4 && blockPos.y <= CHUNKSIDE-5.0f) || (blockPos.z >= 4 && blockPos.z <= CHUNKSIDE-5.0f))
		if((blockPos.y > CHUNKSIDE/2) && ((blockPos.x != CHUNKSIDE/2) || (blockPos.z != CHUNKSIDE/2)))
			continue;
		Grid[i] = new Block();
	}
	if(DEBUGMODE == true) std::cerr << "Finnished Loading Blocks" << std::endl;
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

	for(int i=0; i<CHUNKSIZE; i++){
		if(Grid[i] == nullptr)
			continue;
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		Grid[i]->setPosition(((blockPos)+((float)CHUNKSIDE*chunkPos))-(glm::vec3(CHUNKSIDE,CHUNKSIDE,CHUNKSIDE)/2.0f));
		Grid[i]->setScale(glm::vec3(CUBESIZE,CUBESIZE,CUBESIZE));
		Grid[i]->setColor(Green);
		Grid[i]->setColor((VoxelColor)(i%CHUNKSIDE));
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

	glm::vec3 startPos = glm::vec3(0,0,0);
	unsigned int startIndex = translate3DPos(startPos);
	glm::vec3 endPos = glm::vec3(0,0,0);
	unsigned int curIndex = 0;
	for(int dir=0; dir<6; dir++){
		//For loops are not the solution. at least not auto advancing
			for(int y=0; y<CHUNKSIDE; y++){
				for(int x=0; x<CHUNKSIDE; x++){
					glm::vec3 curPos = glm::vec3(x,y,z);
					curIndex = translate3DPos(curPos,CHUNKSIDE);
					if(Grid[curIndex] == nullptr || Grid[curIndex]->getActive() == false)
						continue;
					if(Grid[startIndex].isSideSame(Grid[curIndex]))
						endPos = curPos;
						if((x!=(CHUNKSIDE-1)||y!=(CHUNKSIDE-1)||z!=(CHUNKSIDE-1))){
							GeneratePlane(startPos, endPos, dir);
							startPos = curPos;
							startIndex = translate3DPos(startPos);
						}
					else{
						GeneratePlane(startPos, endPos, dir);
						startPos = curPos;
						startIndex = translate3DPos(startPos);
					}	
			}
		}
	}	
	if(DEBUGMODE == true) std::cerr << "Generated Mesh" << std::endl;
}


void Chunk::GenerateFace(glm::vec3 start, glm::vec3 end, unsgined int dir){

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
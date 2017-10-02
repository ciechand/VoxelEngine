#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoUtil.hpp"


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
//Functions for the Block Class
void Block::GenerateMesh(){
	if(blockMesh != nullptr)
			delete blockMesh;
	blockMesh = new Mesh();
	blockMesh->GenerateMesh(this);
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
		//if(blockPos.y > CHUNKSIDE/2 || blockPos.x == 6 || blockPos.z == 11 || blockPos.y == 20)
		//	continue;
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
		Grid[i]->setScale(glm::vec3(HALFSIZE,HALFSIZE,HALFSIZE));
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
					Grid[i]->setColor(Blue);
					checkBlock->setColor(Red);
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
					Grid[i]->setColor(Blue);
					checkBlock->setColor(Red);
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
		if(Grid[i]->getActive() == true)
			Grid[i]->GenerateMesh();
	}
}


//Functions for chunk class 
void Chunk::GenerateMesh(){
	initializeMesh();
	if(DEBUGMODE == true) std::cerr << "initialized Mesh" << std::endl;
	updateMesh();
	if(DEBUGMODE == true) std::cerr << "updated Mesh" << std::endl;
	Mesh * blockMesh = nullptr;
	for(int i=0; i<CHUNKSIZE; i++){
		if(Grid[i] == nullptr)
			continue;
		blockMesh = Grid[i]->getMesh();
		if(blockMesh == nullptr || Grid[i]->getActive() == false)
			continue;
		blockMesh->addMeshToRenderer(Grid[i]->getObjectMatrix());
	}
	if(DEBUGMODE == true) std::cerr << "Generated Mesh" << std::endl;
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
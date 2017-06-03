#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoUtil.hpp"


//Constructors for the Block Class
Block::Block():Voxel(){
}

Block::~Block(){
	if (blockMesh != nullptr)
		delete blockMesh;
}
//Functions for the Block Class
void Block::GenerateMesh(){
	blockMesh = new Mesh();
	if(voxels.size() > 0)
		wholeBlock = false;
	if(wholeBlock == true){
		blockMesh->GenerateMesh(*this);
		blockMesh->UpdateMesh();
	}else{
		for(int i=0; i<voxels.size(); i++){
			blockMesh->GenerateMesh(*voxels[i]);
			blockMesh->UpdateMesh();
		}
	}
}

Mesh * Block::getMesh(){
	return blockMesh;
}

void Block::drawBlock(){
	blockMesh->drawMesh();		
}

bool Block::getWhole(){
	return wholeBlock;
}

void Block::setWhole(bool w){
	wholeBlock = w;
}

Voxel * Block::getVoxel(unsigned int index){
	return voxels[index];
}

std::vector<Voxel *> Block::getVoxel(){
	return voxels;
}

glm::vec3 Block::getPosition(){
	return Voxel::getPosition();
}

void Block::setPosition(glm::vec3 pos){
	Voxel::setPosition(pos);
}

//Constructors for Chunk Class
Chunk::Chunk():Chunk(glm::vec3(0,0,0)){

}

Chunk::Chunk(glm::vec3 pos){
	setID(pos);
	std::vector<Chunk*> ChunksPresent;
	ChunksPresent.assign(6, nullptr);
	for(int i=0; i<6; i++){
		glm::vec3 checkChunkPos = pos+DirectionVectors[i];
		for(int j=0; j<ChunkContainer.size(); j++){
			if(ChunkContainer[j]->getID() == checkChunkPos && ChunkContainer[j]->getID() != pos){
				ChunksPresent[i] = ChunkContainer[j];
				break;
			}
		}
	}

	Grid.assign(CHUNKSIZE, nullptr);
	for(int i=0; i<CHUNKSIZE; i++){
			Grid[i] = new Block();
	}
	unsigned int blockIndex = 0;
	for(int i=0; i<CHUNKSIZE; i++){
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		Grid[i]->setPosition(blockPos);
		Grid[i]->setScale(CUBESIZE);
		Grid[i]->setColor(Green);
		//Grid[i]->setColor((VoxelColor)(i%CHUNKSIDE));
		// for(int b=0; b<6; b++){
		// 	float bright = ((CHUNKSIDE*i)/(CHUNKSIDE*CHUNKSIDE));
		// 	Grid[i]->setBrightness(b, bright/255.0f);
		// }
		if(DEBUGMODE == true){
			glm::vec3 tempBPos = Grid[i]->getPosition();
			std::cerr << "Current Block Pos: \n\tX: " << tempBPos.x << "\n\tY: " << tempBPos.y << "\n\tZ: " << tempBPos.z << std::endl;
		}
		for(int s=0; s<3; s++){
			Block* checkBlock = nullptr;
			if(blockPos[s] == 0.0f){
				glm::vec3 transBlockPos = blockPos;
				transBlockPos[s] = CHUNKSIDE-1;
				if(ChunksPresent[(s*2)+1] != nullptr)
					checkBlock = ChunksPresent[(s*2)+1]->getBlock(transBlockPos);
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
				if(ChunksPresent[(s*2)] != nullptr)
					checkBlock = ChunksPresent[(s*2)]->getBlock(transBlockPos);
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
	if(DEBUGMODE == true) std::cerr << "Finnished Loading Blocks" << std::endl;
}

Chunk::~Chunk(){
	for(int i=0; i<CHUNKSIZE; i++){
		if(Grid[i] != nullptr)
			delete Grid[i];
	}
	if(chunkMesh != nullptr)
		delete chunkMesh;
}
//Functions for chunk class 
void Chunk::GenerateMesh(){
	if(chunkMesh == nullptr)
		chunkMesh = new Mesh;
	else{
		delete chunkMesh;
		chunkMesh = nullptr;
	}
	Mesh * blockMesh = nullptr;
	for(int i=0; i<CHUNKSIZE; i++){
		if(Grid[i]->getActive() == true)
			Grid[i]->GenerateMesh();
		blockMesh = Grid[i]->getMesh();
		if(blockMesh == nullptr || Grid[i]->getActive() == false)
			continue;
		chunkMesh->mergeWithMesh(blockMesh);
	}
	chunkMesh->addMMToMesh(TMatrix);

	chunkMesh->UpdateMesh();
}

void Chunk::drawChunk(){
	if(chunkMesh == nullptr){
		std::cerr << "Trying to Draw uninitialized Chunk" << std::endl;
		return;
	}
	chunkMesh->drawMesh();
}

glm::mat4 Chunk::getTMatrix(){
	return TMatrix;
}

void Chunk::setTMatrix(glm::mat4 mm){
	TMatrix = mm;
}

glm::vec3 Chunk::getID(){
	return ChunkID;
}

void Chunk::setID(glm::vec3 pos){
	ChunkID = pos;
	TMatrix = glm::translate(glm::mat4(), ((float)CHUNKSIDE*CUBESIZE)*pos);
}

std::vector<Block*> Chunk::getBlock(){
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
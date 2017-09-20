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
	blockMesh->GenerateMesh(*this);
}

Mesh * Block::getMesh(){
	return blockMesh;
}

glm::vec3 Block::getPosition(){
	return Voxel::getPosition();
}

void Block::setPosition(glm::vec3 pos){ 
	Voxel::setPosition(pos);
}

//functions contained in the BLOckTree Class
BLOckTreeNode::BLOckTreeNode(std::string parentID, int nodeID, glm::vec3 center){
	identifier+=parentID+std::to_string(nodeID);
	BLOckTreeNode(center);
}

BLOckTreeNode::BLOckTreeNode(int chunkID, int nodeID, glm::vec3 center){
	identifier+=std::to_string(chunkID)+"#"+std::to_string(nodeID);
	BLOckTreeNode(center); 
}

BLOckTreeNode(glm::vec3 center){
	centerPoint = center;
}

BLOckTreeNode::~BLOckTreeNode(){

}

glm::vec3 BLOckTreeNode::getCenter(){
 	return centerPoint;
}

void BLOckTreeNode::setCenter(glm::vec3 center){
	centerPoint = center;
}

bool BLOckTreeNode::getLeaf(){
	return leaf;
}

void BLOckTreeNode::setLeaf(bool l){
	leaf = l;
}

int BLOckTreeNode::getLevel(){
	return level;
}

void BLOckTreeNode::setLevel(int l){
	level = l;
}

string BLOckTreeNode::getID(){
	return identifier;
}

void BLOckTreeNode::setID(std::string s){
	identifier = s;
}

Block* BLOckTreeNode::getBlockData(){
	return blockData;
}

void BLOckTreeNode::setBlockData(Block* b){
	blockData = b;
}

BLOckTreeNode* BLOckTreeNode::getChild(int index){
	return children[index];
}

void BLOckTreeNode::setChild(int index, BLOckTreeNode* b){
	if(children[index] != nullptr){
		std::cerr << "STOP TRYING TO SET CHILDREN THAT ARE NOT NULL" << std::endl;
		delete children[index];
	}
	children[index] = b;
}

//Functions contained in the BLOcktree Class
BLOckTree::BLOcktree(int chunkID){

}

BLOckTree::~BLOcktree(){

}

bool BLOckTree::createBlockTree(){

}

Mesh*  BLOckTree::generateTreeMesh(){

}

//Constructors for Chunk Class
Chunk::Chunk():Chunk(glm::vec3(0,0,0)){

}

Chunk::Chunk(glm::vec3 pos){
	setID(pos);
	Grid.assign(CHUNKSIZE, nullptr);
	for(int i=0; i<CHUNKSIZE; i++){
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		if(blockPos.y > CHUNKSIDE/2 || blockPos.x == 6 || blockPos.z == 11)
			continue;
		Grid[i] = new Block();
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

void Chunk::updateMesh(){
	for(int i=0; i<6; i++){
		glm::vec3 checkChunkPos = getID()+DirectionVectors[i];
		for(int j=1; j<ChunkContainer.size(); j++){
			if(ChunkContainer[j]->getID() == checkChunkPos){
				adjacentChunks[i] = j;
				break;
			}
		}
	}

	for(int i=0; i<CHUNKSIZE; i++){
		glm::vec3 blockPos = translate1DPos(i,CHUNKSIDE);
		if(Grid[i] == nullptr)
			continue;
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
				if(adjacentChunks[(s*2)+1] != 0)
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
				if(adjacentChunks[(s*2)] != 0)
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

void Chunk::updateBlockMeshes(){
	for(int i=0; i<Grid.size(); i++){
		bool empty = true;
		for(int s=0;s<6;s++){
			if(Grid[i] != nullptr && Grid[i]->getActiveSide(s)){
				empty = false;
				break;
			}
		}
		if(Grid[i] != nullptr && empty == true)
			Grid[i]->setActive(false);
		if(Grid[i] != nullptr && Grid[i]->getActive() == true)
			Grid[i]->GenerateMesh();
	}
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
		if(Grid[i] == nullptr)
			continue;
		blockMesh = Grid[i]->getMesh();
		if(blockMesh == nullptr || Grid[i]->getActive() == false)
			continue;
		//ADD THIS BLOCK MESH TO THE RENDER CONTROLLER.
	}
}

void Chunk::drawChunk(){
	if(chunkMesh == nullptr){
		std::cerr << "Trying to Draw uninitialized Chunk" << std::endl;
		return;
	}
	//THIS WILL INVOLVE QUEUEING THIS CHUNK FOR DRAWING. CHUNK ORGANIZER YAY!!!
}

glm::vec3 Chunk::getID(){
	return ChunkID;
}

void Chunk::setID(glm::vec3 pos){
	ChunkID = pos;
}

int getIdentifier(){
	return chunkIdentifier;
}

void setIdentifier(int id){
	chunkIdentifier = id;
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
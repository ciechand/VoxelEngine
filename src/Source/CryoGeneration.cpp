#include "../Headers/CryoBase.hpp"

Block::Block():OBJ(){
	color = BlockColors[0];
	Position = glm::vec3();
	Type = 0;
	Owner = -1;
	ID = -1;
}

Block::Block(GLint id, GLint mat, int c):OBJ(){
	modelIdentifier = id;
	setTID(mat);
	color = BlockColors[c];
	Position = glm::vec3();
	Type = 0;
	Owner = -1;
	ID = -1;
}

void Block::setColor(int c){
	color = BlockColors[c];
}	
void Block::setColor(glm::vec3 c){
	color = c;
}	

glm::vec3 Block::getColor() const{
	return color;
}

void Block::setPos(glm::vec3 point){
	glm::vec3 dPos = point-Position;
	Position = point;
	TransformMatrix = glm::translate(TransformMatrix, dPos);
}

glm::vec3 Block::getPos() const{
	return Position;
}

void Block::setType(int t){
	Type = t;
}

int Block::getType() const{
	return Type;
}

void Block::setOwner(int o){
	Owner = o;
}

int Block::getOwner() const{
	return Owner;
}

void Block::setID(int id){
	ID = id;
}

int Block::getID() const{
	return ID;
}


Chunk::Chunk(){
	grid.assign(CHUNKDIMS*CHUNKDIMS*CHUNKHEIGHT, nullptr);
	position = glm::vec2();
}

Chunk::Chunk(glm::vec2 p){
	grid.assign(CHUNKDIMS*CHUNKDIMS*CHUNKHEIGHT, nullptr);
	position = p;
}

Chunk::~Chunk(){
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				if(grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]!=nullptr)
					delete grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k];
			}
		}
	}
}

void Chunk::Init(){
	//In this Function k=x;j=z;i=y
	std::uniform_int_distribution<int> udistCol(0,15);
	std::uniform_int_distribution<int> udistTex(1,2);
	int rc = udistCol(randomEng);
	IOBJ * tempOBJ = Renderer.getIObject(0);
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k] = new Block();
				std::cout << "ID: " << tempOBJ->getBlocksSize()-1 << ",Address Outside: " << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k] << std::endl;
				if(grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k] == nullptr) continue;
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setTID(2);
				//rc = udistCol(randomEng);
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setColor(rc);
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setPos(glm::vec3((k+(position.x*CHUNKDIMS))*BLOCKSCALE,i*BLOCKSCALE,(j+(position.y*CHUNKDIMS))*BLOCKSCALE));
				//std::cout << (k+(position.x*CHUNKDIMS))*BLOCKSCALE << ":" << i*BLOCKSCALE << ":" << (j+(position.y*CHUNKDIMS))*BLOCKSCALE << std::endl;
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setID(tempOBJ->getBlocksSize());
				//std::cerr << "Texture of block: " << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getTID() << std::endl;
				//std::cout << "Color of Block: " << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getColor().x << ":" << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getColor().y <<":" << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getColor().z <<std::endl;
				//printMatrix(T);
				tempOBJ->addBlocks(*grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]);
			}
		}
	}
}


void Chunk::setPosition(glm::vec2 pos){
	position = pos;
}

glm::vec2 Chunk::getPosition(){
	return position;
}

bool blockCmp(Block *a, Block *b){
	if(a->getMID() != b->getMID())
		return false;
	if(a->getTID() != b->getTID())
		return false;
	if(a->getColor() != b->getColor())
		return false;
	if(a->getPos() != b->getPos())
		return false;
	if(a->getType() != b->getType())
		return false;
	if(a->getOwner() != b->getOwner())
		return false;
	if(a->getTMatrix() != b->getTMatrix())
		return false;
	return true;
}

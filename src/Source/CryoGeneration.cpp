#include "../Headers/CryoBase.hpp"

Block::Block():OBJ(){
	color = BlockColors[0];
	Position = glm::vec3();
	Type = 0;
	Owner = -1;
}

Block::Block(GLint id, GLint mat, int c):OBJ(){
	modelIdentifier = id;
	setTID(mat);
	color = BlockColors[c];
	Position = glm::vec3();
	Type = 0;
	Owner = -1;
}

void Block::setColor(int c){
	color = BlockColors[c];
}	

glm::vec3 Block::getColor(){
	return color;
}

void Block::setPos(glm::vec3 point){
	glm::vec3 dPos = point-Position;
	Position = point;
	TransformMatrix = glm::translate(TransformMatrix, dPos);
}

glm::vec3 Block::getPos(){
	return Position;
}

void Block::setType(int t){
	Type = t;
}

int Block::getType(){
	return Type;
}

void Block::setOwner(int o){
	Owner = o;
}

int Block::getOwner(){
	return Owner;
}

Chunk::Chunk(){
	grid.assign(CHUNKDIMS*CHUNKDIMS*CHUNKHEIGHT, NULL);
	position = glm::vec2();
}

Chunk::Chunk(glm::vec2 p){
	grid.assign(CHUNKDIMS*CHUNKDIMS*CHUNKHEIGHT, NULL);
	position = p;
}

Chunk::~Chunk(){
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				if(grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]==NULL) continue;
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
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k] = new Block();
				if(grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k] == NULL) continue;
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setTID(2);
				//rc = udistCol(randomEng);
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setColor(rc);
				grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->setPos(glm::vec3((k+(position.x*CHUNKDIMS))*BLOCKSCALE,i*BLOCKSCALE,(j+(position.y*CHUNKDIMS))*BLOCKSCALE));
				//std::cout << (k+(position.x*CHUNKDIMS))*BLOCKSCALE << ":" << i*BLOCKSCALE << ":" << (j+(position.y*CHUNKDIMS))*BLOCKSCALE << std::endl;
				IOBJ * tempOBJ = Renderer.getIObject(grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getMID());
				tempOBJ->addBlocks(*grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]);
				//std::cerr << "Texture of block: " << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getTID() << std::endl;
				//std::cout << "Color of Block: " << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getColor().x << ":" << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getColor().y <<":" << grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getColor().z <<std::endl;
				Renderer.setIObject(grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getMID(), tempOBJ);
				//printMatrix(T);
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
	bool same[7] = {false};
	if(a->getMID() == b->getMID())
		same[0] = true;
	if(a->getTID() == b->getTID())
		same[1] = true;
	if(a->getColor() == b->getColor())
		same[2] = true;
	if(a->getPos() == b->getPos())
		same[3] = true;
	if(a->getType() == b->getType())
		same[4] = true;
	if(a->getOwner() == b->getOwner())
		same[5] = true;
	if(a->getTMatrix() == b->getTMatrix())
		same[6] = true;
	for(int i=0; i<7; i++){
		if(same[i] == false)
			return false;
	}
	return true;
}
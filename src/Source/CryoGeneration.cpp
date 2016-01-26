#include "../Headers/CryoBase.hpp"

Block::Block():OBJ(){
	color = BlockColors[0];
}

Block::Block( GLint id, GLint mat, int c):OBJ(){
	modelIdentifier = id;
	setTID(mat);
	color = BlockColors[c];
}


void Block::setColor(int c){
	color = BlockColors[c];
}	

glm::vec3 Block::getColor(){
	return color;
}

Chunk::Chunk(){
	grid.assign(CHUNKDIMS, std::vector<std::vector<Block*> >(CHUNKDIMS, std::vector<Block*>(CHUNKHEIGHT, new Block())));
	position = glm::vec2();
}

Chunk::~Chunk(){
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				delete grid[k][j][i];
			}
		}
	}
}
void Chunk::Init(){
	//In this Function k=x; j=z;i=y
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				std::uniform_int_distribution<int> udistTex(1,2);
				grid[k][j][i]->setTID(udistTex(randomEng));
				std::uniform_int_distribution<int> udistCol(0,15);
				grid[k][j][i]->setColor(udistCol(randomEng));
				glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3((k+position.x*CHUNKDIMS)*BLOCKSCALE,i*BLOCKSCALE,(j+position.y*CHUNKDIMS)*BLOCKSCALE));
				grid[k][j][i]->setTMatrix(T);
				IOBJ * tempOBJ = Renderer.getIObject(grid[k][j][i]->getMID());
				tempOBJ->addBlocks(*grid[k][j][i]);
				//std::cerr << "Texture of block: " << grid[k][j][i]->getTID() << std::endl;
				//std::cout << "Color of Block: " << grid[k][j][i]->getColor().x << ":" << grid[k][j][i]->getColor().y <<":" << grid[k][j][i]->getColor().z <<std::endl;
				Renderer.setIObject(grid[k][j][i]->getMID(), tempOBJ);
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

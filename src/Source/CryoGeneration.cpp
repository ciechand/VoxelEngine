#include "../Headers/CryoBase.hpp"

Block::Block():OBJ(){
	color = BlockColors[0];
}

Block::Block(int mat, int id, int c):OBJ(){
	modelIdentifier = id;
	textureIdentifier = mat;
	color = BlockColors[c];
}


void Block::setColor(int c){
	color = BlockColors[c];
}	

glm::vec3 Block::getColor(){
	return color;
}


void Block::setID(unsigned long long int id){
	objID = id;
}

unsigned long long int Block::getID(){
	return objID;
}


Chunk::Chunk(){
	std::uniform_int_distribution<int> udistCol(0,15);
	grid.assign(CHUNKDIMS, std::vector<std::vector<Block*> >(CHUNKDIMS, std::vector<Block*>(CHUNKHEIGHT, new Block(1,0,udistCol(randomEng)))));
	position = glm::vec2();
}

void Chunk::Init(){
	//In this Function k=x; j=z;i=y
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				grid[k][j][i]->setID(k + (i << 4) + (j << 8) + ((unsigned long long int)position.x << 16) + ((unsigned long long int)position.y << 36)); //this gives each block a unique ID;
				glm::mat4 T = glm::translate(glm::mat4(1.0f), glm::vec3((k+position.x*CHUNKDIMS)*BLOCKSCALE,i*BLOCKSCALE,(j+position.y*CHUNKDIMS)*BLOCKSCALE));
				grid[k][j][i]->setTMatrix(T);
				IOBJ * tempOBJ = Renderer.getIObject(grid[k][j][i]->getMID());
				tempOBJ->addBlocks(*grid[k][j][i]);
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

#include "../Headers/CryoBase.hpp"

Block::Block(int mat):OBJ(){
	setMaterial(mat);
}

Block::Block(int mat, int id):OBJ(){
	modelIdentifier = id;
	setMaterial(mat);
}


void Block::setMaterial(int mat){
	material = mat;
}	

int Block::getMaterial(){
	return material;
}

void Block::setID(unsigned long long int id){
	objID = id;
}

unsigned long long int Block::getID(){
	return objID;
}


Chunk::Chunk(){
	grid.assign(CHUNKDIMS, std::vector<std::vector<Block*> >(CHUNKDIMS, std::vector<Block*>(CHUNKHEIGHT, new Block(0,0))));
	position = glm::vec2();
}	

void Chunk::Init(){
	//In this Function k=x; j=z;i=y
	//std::cout << "Model Translation Matrices: " << std::endl;
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				grid[k][j][i]->setID(k + (i << 4) + (j << 8) + ((unsigned long long int)position.x << 16) + ((unsigned long long int)position.y << 36)); //this gives each block a unique ID;
				glm::mat4 T = grid[k][j][i]->getTMatrix(Translate);
				T = glm::translate(glm::mat4(1.0), glm::vec3((k+position.x*CHUNKDIMS)*BLOCKSCALE,i*BLOCKSCALE,(j+position.y*CHUNKDIMS)*BLOCKSCALE));
				grid[k][j][i]->setTMatrix(Translate, T);
				IOBJ * tempOBJ = Renderer.getIObject(grid[k][j][i]->getMID());
				tempOBJ->addModelMatrices(grid[k][j][i]->getID(),grid[k][j][i]->getTMatrix(Translate)*grid[k][j][i]->getTMatrix(Scale)*grid[k][j][i]->getTMatrix(Rotate));
				tempOBJ->addMaterial(grid[k][j][i]->getMaterial());
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

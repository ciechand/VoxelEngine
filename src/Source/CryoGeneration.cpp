#include "../Headers/CryoBase.hpp"

Block::Block(int id):OBJ(){
	modelIdentifier = id;
	std::vector<int> mats;
	mats.assign(6,id);
	setSideMats(mats);
}

void Block::setSideMats(int side, int mat){
	sideMaterials[side] = mat;
}

void Block::setSideMats(std::vector<int> mats){
	sideMaterials = mats;
}	

int Block::getSideMats(int side){
	return sideMaterials[side];
}


Chunk::Chunk(){
	grid.assign(CHUNKDIMS, std::vector<std::vector<Block*> >(CHUNKDIMS, std::vector<Block*>(CHUNKHEIGHT, new Block(1))));
	
}	

void Chunk::Init(){
	//std::cout << "Model Translation Matrices: " << std::endl;
	for(int i=0; i<CHUNKHEIGHT; i++){
		for(int j=0; j<CHUNKDIMS; j++){
			for(int k=0;k<CHUNKDIMS; k++){
				glm::mat4 T = grid[k][j][i]->getTMatrix(Translate);
				T = glm::translate(glm::mat4(1.0), glm::vec3(k*BLOCKSCALE,i*BLOCKSCALE,j*BLOCKSCALE));
				grid[k][j][i]->setTMatrix(Translate, T);
				IOBJ * tempOBJ = Renderer.getIObject(grid[k][j][i]->getMID());
				tempOBJ->addModelMatrices(grid[k][j][i]->getTMatrix(Translate)*grid[k][j][i]->getTMatrix(Scale)*grid[k][j][i]->getTMatrix(Rotate));
				Renderer.setIObject(grid[k][j][i]->getMID(), tempOBJ);
				//printMatrix(T);
			}
		}
	}
}
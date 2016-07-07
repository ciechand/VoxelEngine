#include "../Headers/CryoBase.hpp"

Block::Block():Item(){
	Owner = -1;
	bounds.setLimbs(glm::vec3(BLOCKSCALE/2,BLOCKSCALE/2,BLOCKSCALE/2));
}

Block::~Block(){
}

void Block::setPos(glm::vec3 pos){
	baseObj::setPos(pos);
	setBounds(pos);
}

void Block::setOwner(int o){
	Owner = o;
}

int Block::getOwner() const{
	return Owner;
}

void Block::setBounds(BBox & b){
	bounds = b;
}

void Block::setBounds(glm::vec3 a){
	bounds.setPos(a);
}

BBox & Block::getBounds(){
	return bounds;
}


Chunk::Chunk(){
	grid.assign(CHUNKDIMS*CHUNKDIMS*CHUNKHEIGHT, std::make_pair(0,0));
	position = glm::vec2();
	heightMap.resize(CHUNKDIMS);
	for(int i=0; i<CHUNKDIMS; i++){
		heightMap[i].resize(CHUNKDIMS);
		for(int j=0; j<CHUNKDIMS; j++){
			heightMap[i][j] = -1;
		}
	}
	std::uniform_int_distribution<int> chunkDist(1, CHUNKHEIGHT-1);
	heightMap[0][0] = chunkDist(randomEng);
	heightMap[0][CHUNKDIMS-1] = chunkDist(randomEng);
	heightMap[CHUNKDIMS-1][0] = chunkDist(randomEng);
	heightMap[CHUNKDIMS-1][CHUNKDIMS-1] = chunkDist(randomEng);
}

Chunk::Chunk(glm::vec2 p):Chunk(){
	position = p;
}

Chunk::~Chunk(){
}

void Chunk::Init(){
	std::vector<Chunk*> borderingChunks(4,nullptr);
	bool found = false;
	for(int c=0; c < World.size(); c++){
		glm::vec2 cp = World[c]->getPos();
		if(cp == position+glm::vec2(0,1)){//UP
			borderingChunks[0] = World[c];
			found = true;
			continue;
		}		
		if(cp == position+glm::vec2(-1,0)){//LEFT
			borderingChunks[1] = World[c];
			found = true;
			continue;
		}		
		if(cp == position+glm::vec2(1,0)){//RIGHT
			borderingChunks[2] = World[c];
			found = true;
			continue;
		}
		if(cp == position+glm::vec2(0,-1)){//DOWN
			borderingChunks[3] = World[c];
			found = true;
			continue;
		}

	}

	if(found != false){
		std::uniform_real_distribution<> rdist(-0.5f,0.5f);
		for(int bp=0; bp<CHUNKDIMS;bp++){
			if(borderingChunks[3]!=nullptr){
				heightMap[bp][0] = clamp(1,(int)CHUNKHEIGHT-1,(int)(borderingChunks[3]->getHeightMap(glm::vec2(bp,CHUNKDIMS-1))+rdist(randomEng)));
				///std::cout << "A Chunk exists to the Top" << std::endl;
			}
			if(borderingChunks[1]!=nullptr){
				heightMap[0][bp] = clamp(1,(int)CHUNKHEIGHT-1,(int)(borderingChunks[1]->getHeightMap(glm::vec2(CHUNKDIMS-1,bp))+rdist(randomEng)));
				///std::cout << "A Chunk exists to the Left" << std::endl;
			}
			if(borderingChunks[2]!=nullptr){
				heightMap[CHUNKDIMS-1][bp] = clamp(1,(int)CHUNKHEIGHT-1,(int)(borderingChunks[2]->getHeightMap(glm::vec2(0,bp))+rdist(randomEng)));
				///std::cout << "A Chunk exists to the Right" << std::endl;
			}
			if(borderingChunks[0]!=nullptr){
				heightMap[bp][CHUNKDIMS-1] = clamp(1,(int)CHUNKHEIGHT-1,(int)(borderingChunks[0]->getHeightMap(glm::vec2(bp,0))+rdist(randomEng)));
				///std::cout << "A Chunk exists to the Down" << std::endl;
			}
		}
	}


	divide();

	//BLOCK ALLOCATION.
	//In this Function k=x;j=z;i=y
	std::uniform_int_distribution<int> udistCol(0,15);
	std::uniform_int_distribution<int> udistTex(1,2);
	int rc = udistCol(randomEng);
	int cubeIndex = modelNamesFind("Cube");
	IOBJ * tempOBJ = Renderer.getIObject(cubeIndex);
	for(int j=0; j<CHUNKDIMS; j++){
		for(int k=0;k<CHUNKDIMS; k++){
			for(int i=0; i<=heightMap[k][j]; i++){
				int curBlock = (i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k;
				tempOBJ->addBlocks();
				grid[curBlock].first = cubeIndex;
				grid[curBlock].second = tempOBJ->getBlocksSize()-1;
				Block * blockp = tempOBJ->getBlocks(grid[curBlock].second);
				if(blockp == nullptr)
					std::cout <<  "Block is null." << std::endl;
				blockp->setID(grid[curBlock].second);
				blockp->setTOff(2);
				blockp->setMID(cubeIndex);
				//rc = udistCol(randomEng);
				blockp->setColor(Green);
				blockp->setPos(glm::vec3((k+(position.x*CHUNKDIMS))*BLOCKSCALE,i*BLOCKSCALE,(j+(position.y*CHUNKDIMS))*BLOCKSCALE));
				//std::cout << (k+(position.x*CHUNKDIMS))*BLOCKSCALE << ":" << i*BLOCKSCALE << ":" << (j+(position.y*CHUNKDIMS))*BLOCKSCALE << std::endl;

				//std::cerr << "Texture of block: " << grid[curBlock]->getTOff()() << std::endl;
				//std::cout << "Color of Block: " << grid[curBlock]->getColor().x << ":" << grid[curBlock]->getColor().y <<":" << grid[curBlock]->getColor().z <<std::endl;
				//printMatrix(T);
			}
		}
	}
}


void Chunk::setPos(glm::vec2 pos){
	position = pos;
}

glm::vec2 Chunk::getPos(){
	return position;
}

void Chunk::setHeightMap(std::vector<std::vector<int> > hm){
	heightMap = hm;
}

void Chunk::setHeightMap(glm::vec2 pos, int value){
	heightMap[pos.x][pos.y] = value;
}

std::vector<std::vector<int> > Chunk::getHeightMap(){
	return heightMap;
}

int Chunk::getHeightMap(glm::vec2 pos){
	return heightMap[pos.x][pos.y];
}

void Chunk::divide(float size){
	std::uniform_real_distribution<> rdist(-1.0f,1.0f);
	float x, y, half = size/2.0f;
	float Roughness = 0.1;
	float scale = size*Roughness;
	//std::cout << size << std::endl;
	//std::cout << "Scale : " << scale << "\nLargest Offset Possible: " << (float)(scale*CHUNKHEIGHT*0.5f) << std::endl;
	for(y = half; y < CHUNKDIMS-1; y+=size){
		for(x = half; x < CHUNKDIMS-1; x+=size){
			//std::cout << "diamond" << std::endl;
			//std::cout << "X: " << x << " Y: " << y <<std::endl;
			if(heightMap[x][y] == -1)
				diamond(x,y,half,rdist(randomEng)*scale);
		}
	}
	for(y = 0; y <= CHUNKDIMS-1; y+=half){
		for(x = fmod((y+half),size); x <= CHUNKDIMS-1; x+=size){
			//std::cout << "square" << std::endl;
			//std::cout << "X: " << x << " Y: " << y <<std::endl;
			if(heightMap[x][y] == -1)
				square(x,y,half,rdist(randomEng)*scale);
		}
	}
	if(half<1) return;
	divide(size/2);

}

void Chunk::diamond(float x, float y, float size, float offset){
	//std::cout << "Size: "  << size << "\n\tX: " << x << "\n\tY: " << y << "\n\tValue: " << heightMap[(y*CHUNKDIMS)+x] << std::endl;
	float ave = average(std::vector<int>{heightMap[x-size][y-size],heightMap[x+size][y-size],heightMap[x-size][y+size],heightMap[x+size][y+size]});
	float val = ave+offset;
	if(val < 1.0f)
		val = 1.0f;
	if(val > CHUNKHEIGHT-1)
		val = CHUNKHEIGHT-1;
	heightMap[x][y] = round(val);

}

void Chunk::square(float x, float y, float size, float offset){
	float ave = 0.0;
	if(x-size > 0 && x+size < CHUNKDIMS && y-size > 0 && y+size < CHUNKDIMS)
		ave = average(std::vector<int>{heightMap[x][y-size],heightMap[x+size][y],heightMap[x][y+size],heightMap[x-size][y]});
	else if(x-size <= 0 && y-size <= 0)
		ave =  average(std::vector<int>{heightMap[x+size][y],heightMap[x][y+size]});
	else if(x+size >= CHUNKDIMS && y-size <= 0)
		ave =  average(std::vector<int>{heightMap[x][y+size],heightMap[x-size][y]});
	else if(x-size <= 0 && y+size >= CHUNKDIMS)
		ave =  average(std::vector<int>{heightMap[x][y-size],heightMap[x+size][y]});
	else if(x+size >= CHUNKDIMS && y+size >= CHUNKDIMS)
		ave =  average(std::vector<int>{heightMap[x][y-size],heightMap[x-size][y]});
	else if (x-size <= 0)
		ave =  average(std::vector<int>{heightMap[x][y-size],heightMap[x+size][y],heightMap[x][y+size]});
	else if (x+size >= CHUNKDIMS)
		ave =  average(std::vector<int>{heightMap[x][y-size],heightMap[x][y+size],heightMap[x-size][y]});
	else if (y-size <= 0)
		ave =  average(std::vector<int>{heightMap[x+size][y],heightMap[x][y+size],heightMap[x-size][y]});
	else if (y+size >= CHUNKDIMS)
		ave =  average(std::vector<int>{heightMap[x][y-size],heightMap[x+size][y],heightMap[x-size][y]});
	else
		std::cerr << "Missed a possible condition" << std::endl;
	float val = ave+offset;
	if(val < 1.0f)
		val = 1.0f;
	if(val > CHUNKHEIGHT-1)
		val = CHUNKHEIGHT-1;
	heightMap[x][y] = round(val);
}

bool blockCmp(Block *a, Block *b){
	if(a->getMID() != b->getMID())
		return false;
	if(a->getTOff() != b->getTOff())
		return false;
	if(a->getColor() != b->getColor())
		return false;
	if(a->getPos() != b->getPos())
		return false;
	if(a->getOwner() != b->getOwner())
		return false;
	if(a->getTMatrix() != b->getTMatrix())
		return false;
	return true;
}

float average(std::vector<int> vc){
	float sum = 0.0f;
	for(int i=0; i<vc.size(); i++){
		if(std::isnan(vc[i]))
		std::cerr <<  "vc #"<< i <<" is Nan. It actually is: " << vc[i] << std::endl;
		else
			sum+=vc[i];
	}
	return sum/(vc.size());
}

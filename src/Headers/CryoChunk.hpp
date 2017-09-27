#ifndef CryoChunk
#define CryoChunk

//Includes
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "./CryoRenderer.hpp"

//Forward Declarations
class Mesh;
class Voxel;

//Class Definition for Blocks
class Block: public Voxel{
	public:
		Block();
		~Block();

		void GenerateMesh(); 

		Mesh * getMesh();
		
		glm::vec3 getPosition();
		void setPosition(glm::vec3 pos);
	protected:
		Mesh *blockMesh = nullptr;
};

class Chunk{
	public:
		Chunk();
		Chunk(glm::vec3 pos);
		~Chunk();

		void GenerateMesh();
		void initializeMesh();
		void updateMesh();

		void drawChunk();

		glm::vec3 getPos();
		void setPos(glm::vec3 pos);

		std::array<Block*,CHUNKSIZE> getBlock();
		Block* getBlock(unsigned int index);
		Block* getBlock(glm::vec3 pos);

	protected:
		glm::vec3 chunkPos;	
		std::array<Block*, CHUNKSIZE> Grid;	
		Mesh *chunkMesh = nullptr;

	private: 
		std::array<int,6> adjacentChunks;
};

#endif //CryoChunk
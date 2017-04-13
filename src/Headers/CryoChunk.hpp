#ifndef CryoChunk
#define CryoChunk

//Includes
#include <cstdlib>
#include <iostream>
#include <vector>

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

		void drawBlock();

		bool getWhole();
		void setWhole(bool w);

		Voxel * getVoxel(unsigned int index);
		std::vector<Voxel *> getVoxel();

		glm::vec3 getPosition();
		void setPosition(glm::vec3 pos);
	protected:
		std::vector<Voxel *> voxels;
		Mesh *blockMesh = nullptr;
		bool wholeBlock = true;

};

class Chunk{
	public:
		Chunk();
		Chunk(glm::vec3 pos);
		~Chunk();

		void GenerateMesh();

		void drawChunk();

		glm::vec3 getID();
		void setID(glm::vec3 pos);

		glm::mat4 getTMatrix();
		void setTMatrix(glm::mat4 mm);

		std::vector<Block*> getBlock();
		Block* getBlock(unsigned int index);
		Block* getBlock(glm::vec3 pos);

	protected:
		glm::vec3 ChunkID = {0, 0, 0};
		glm::mat4 TMatrix;
		std::vector<Block *> Grid;
		Mesh *chunkMesh = nullptr;


};

#endif //CryoChunk
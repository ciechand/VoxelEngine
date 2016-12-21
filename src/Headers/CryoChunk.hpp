#ifndef CryoChunk
#define CryoChunk

//Includes
#include <cstdlib>
#include <iostream>
#include <vector>

#include <glm/glm.hpp>

//Forward Declarations
class Mesh;
class Voxel;

//Class Definition for Blocks
class Block{
	public:
		Block();
		~Block();

		Mesh GenerateMesh(); 

	protected:
		std::vector<Voxel> voxels;
		Mesh *blockMesh = nullptr;
		bool wholeBlock = true;

};

class Chunk{
	public:
		 Chunk();
		 ~Chunk();

		 Mesh GenerateMesh();

	protected:
		std::vector<Block> Grid;
		Mesh *chunkMesh = nullptr;

};

#endif //CryoChunk
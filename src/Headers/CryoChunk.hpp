#ifndef CryoChunk
#define CryoChunk

//Includes
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>

#include "./CryoRenderer.hpp"

//Precompile definitions
#define MAXBLOCKTREEDEPTH 5

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

//Class Definition for a Block BLOckTreeNode
class BLOckTreeNode{
	public:
		BLOckTreeNode(std::string parentID, int nodeID);
		BLOckTreeNode(int chunkID, int nodeID);
		BLOckTreeNode(glm::vec3 center);
		~BLOckTreeNode();

		glm::vec3 getCenter();
		void setCenter(glm::vec3 center);

		bool getLeaf();
		void setLeaf(bool l);

		int getLevel();
		void setLevel(int l);

		string getID();
		void setID(std::string s);

		Block* getBlockData();
		void setBlockData(Block* b);

		BLOckTreeNode* getChild(int index);
		void setChild(int index, BLOckTreeNode* b);
	private:
		glm::vec3 centerPoint;
		bool leaf = false;
		int level = 0;
		std::string  identifier = "";
		Block* blockData;
		std::array<BLOckTreeNode*, 8> children;
};

//Class Definition for the A Block Octree
class BLOckTree{
	public:
		BLOckTree(int chunkID);
		~BLOckTree();

		bool createBlockTree();

		Mesh*  generateTreeMesh();
	private:
		BLOckTreeNode* Root;

};

class Chunk{
	public:
		Chunk();
		Chunk(glm::vec3 pos);
		~Chunk();

		void GenerateMesh();
		void updateMesh();
		void updateBlockMeshes();

		void drawChunk();

		glm::vec3 getID();
		void setID(glm::vec3 pos);

		int getIdentifier();
		void setIdentifier(int id);

		std::vector<Block*> getBlock();
		Block* getBlock(unsigned int index);
		Block* getBlock(glm::vec3 pos);

	protected:
		glm::vec3 ChunkID = {0, 0, 0};
		int chunkIdentifier = 0;
		BLOckTree * blockTree;
		Mesh *chunkMesh = nullptr;

	private:
		std::array<int, 6> _adjacentChunks;
};

#endif //CryoChunk
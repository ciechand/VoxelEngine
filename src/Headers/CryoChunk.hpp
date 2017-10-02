#ifndef CryoChunk
#define CryoChunk

//Includes
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>

#include <glm/glm.hpp>

class Block;
class Chunk;

#include "./CryoRenderer.hpp"

//Forward Declarations
class Mesh;

//Class Definition for Blocks
class Block{
	public:
		Block();
		~Block();

		void GenerateMesh(); 

		Mesh * getMesh();

		bool getActive();
		void setActive(bool ba);
		
		glm::vec3 getPosition();
		void setPosition(glm::vec3 pos);

		void setRotation(float angle, glm::vec3 axis);

		void setScale(glm::vec3 scale);

		glm::mat4 getObjectMatrix();

		bool getActiveSide(unsigned int n);
		std::array<bool,6> getActiveSide();
		void setActiveSide(unsigned int n, bool s);
		void setActiveSide(std::array<bool,6> s);

		glm::vec3 getSideColor(unsigned int n);
		std::array<glm::vec3,6> getSideColor();
		void setSideColor(unsigned int n, VoxelColor s);
		void setSideColor(std::array<glm::vec3,6> s);
		void setColor(VoxelColor c);
		
		int getSideTexture(unsigned int n);
		std::array<int,6> getSideTexture();
		void setSideTexture(unsigned int n, int s);
		void setSideTexture(std::array<int,6> s);
	protected:
		Mesh *blockMesh = nullptr;
		bool blockActive = true;
		glm::vec3 position = glm::vec3(0, 0, 0);
		std::array<glm::mat4,4> objectMatrix;
		std::array<bool,6> activeSides = {true,true,true,true,true,true};
		std::array<glm::vec3,6> sideColors;
		std::array<int,6> sideTextures = {0,0,0,0,0,0};
};

class Chunk{
	public:
		Chunk();
		Chunk(glm::vec3 pos);
		~Chunk();

		void GenerateMesh();
		void initializeMesh();
		void updateMesh();

		glm::vec3 getPos();
		void setPos(glm::vec3 pos);

		std::array<Block*,CHUNKSIZE> getBlock();
		Block* getBlock(unsigned int index);
		Block* getBlock(glm::vec3 pos);

	protected:
		glm::vec3 chunkPos;	
		std::array<Block*, CHUNKSIZE> Grid;	

	private: 
		std::array<int,6> adjacentChunks;
};

#endif //CryoChunk
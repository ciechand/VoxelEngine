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

//This is the baseMesh class, this contains all the information read in from a 
class BaseMesh{
	public:
		BaseMesh(std::string filename);
		~BaseMesh();

		bool loadModel();

		void addToController();

		unsigned int getID();
		void setID(unsigned int id);

		unsigned int getVertStart();
		void setVertStart(unsigned int index);

		unsigned int getTexStart();
		void setTexStart(unsigned int index);

		unsigned int getNormalStart();
		void setNormalStart(unsigned int index);

		std::vector<glm::vec4> getVertices();
		std::vector<glm::vec2> getTexCoords();
		std::vector<glm::vec4> getNormals();
		std::vector<unsigned int> getIndices();

		void addToColors(std::vector<glm::vec3>::iterator begining, std::vector<glm::vec3>::iterator ending);
		void addToColors(glm::vec3 color);
		std::vector<glm::vec3> getColors();

		void addToModels(std::vector<glm::mat4>::iterator begining, std::vector<glm::mat4>::iterator ending);
		void addToModels(glm::mat4 model);
		std::vector<glm::mat4> getModels();

	private:
		unsigned int ID;
		std::string modelFile = "";
		unsigned int vertexStart = 0;
		std::vector<glm::vec4> vertices;
		unsigned int texStart = 0;
		std::vector<glm::vec2> texCoords;
		unsigned int normalStart = 0;
		std::vector<glm::vec4> normals; 
		std::vector<unsigned int> indices;

		std::vector<glm::vec3> colors;
		std::vector<glm::mat4> modelMatrices;
};


//This is the Mesh Class, It will be generated per chunk. each chunk will contain its own mesh? or each block will be its own mesh and will combine into a chunks final mesh?
class Mesh{
	public:
		Mesh();
		~Mesh();
 
 		void GenerateMesh(Block * parentBlock);

		void GenerateCubeSide(CubeFace face, glm::vec3 c, glm::vec3 offset);

		void addBaseToMesh(int baseID);
		std::vector<int> getBase();
		int getBase(unsigned int index);

		void addTexIDToMesh(int texID);
		std::vector<int> getTexID();
		int getTexID(unsigned int index);

		void addColorToMesh(glm::vec3 c);
		std::vector<glm::vec3> getColors();
		glm::vec3 getColors(unsigned int index);

		void addMatrixToMesh(glm::mat4 mm);
		std::vector<glm::mat4> getMatrix(); 
		glm::mat4 getMatrix(unsigned int index);

		void addMeshToRenderer(glm::mat4 objectMatrix = glm::mat4());

	private:
		//NEW VARIABLES FOR MESHES!!
		std::vector<int> meshIDs; //there should be one for every model.
		std::vector<int> textureIDs; //there should be one for every model.
		std::vector<glm::vec3> colors; // There should be one for every model.
		std::vector<glm::mat4> modelMatrix; //There should be one for ever model.
};


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
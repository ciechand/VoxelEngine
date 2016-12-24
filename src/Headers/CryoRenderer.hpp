#ifndef CryoRender
#define CryoRender

#include <iostream>

#include <vector>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glu.h>

extern const glm::vec3 CubeVerts[8];
extern const glm::vec3 DirectionalVectors[6];

//MACROS
#define BUFFER_OFFSET(offset) ((GLvoid*)(intptr_t)(offset))

//Enum for defining positions of the different cube vertices.
enum CubeVertices{TopBackLeft=0, TopBackRight, TopFrontLeft, TopFrontRight, BottomBackLeft, BottomBackRight, BottomFrontLeft, BottomFrontRight};

//enum for all the Colors that Voxels could be.
enum VoxelColor{None=0, Red, Maroon, Pink, DPink, Purple, Aqua, Blue, Aquamarine, Cyan, Lime, Green, Yellow, Brown, Golden, Orange};

//enum for sides of a cube
enum CubeFace{LeftFace=0, RightFace, TopFace, BottomFace, FrontFace, BackFace};

//Enum for the different vertex buffers labels
enum VertexBufferLabels{LVertexBuffer=0, LTextureBuffer, LModelMatrices};

//typedef for the voxel class, these voxels will comprise blocks. (should )
class Voxel{
	public:
		Voxel();
		~Voxel();

		bool getActive();
		void setActive(bool va);

		VoxelColor getColor();
		void setColor(VoxelColor vc);

	private:
		bool voxActive;
		VoxelColor voxColor;
		char Brightness;
		bool activeSides[6] = {true,true,true,true,true,true};


};

//This is the Mesh Class, It will be generated per chunk. each chunk will contain its own mesh? or each block will be its own mesh and will combine into a chunks final mesh?
class Mesh{
	public:
		Mesh();
		~Mesh();

		void IntiateMesh();
		void updateMesh();

		void GenerateCubeSide(CubeFace face, glm::vec4 offset);
		void addVertexToMesh(glm::vec4 vert);
		//Need some functions here to generate each side of a cube.
		//A function to combine multiple meshes.

		//other functions?

	private:
		//Below Contains all the OpenGL variables needed in order to draw this mesh.
		GLuint VertexArrayObject;
		GLuint VertexBuffer[4];
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> vertexNormals;
		std::vector<glm::vec2> textureCoords;
		std::vector<GLuint> indices;
		std::vector<GLuint> shaderPositions;
		//Below are all the variables not used in drawing.

};


//This is the definition of the Renderer for the Game, This is in process and Will contain alot of pseudocode for quite some time.

//Not sure if this class is needed, maybe to manage chunks and when they get rendered but will decide in the future.

/*class Renderer{
	public:
		Mesh* CreateMesh();
		//storeMesh
		//getMesh
		//updateMesh

		//renderscene



	private:
		std::vector<Mesh*> Meshes;


};*/

#endif //CryoRender
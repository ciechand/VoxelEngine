#ifndef CryoRender
#define CryoRender

#include <iostream>

#include <vector>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glu.h>

extern const glm::vec3 CubeVerts[8];
extern const glm::vec3 DirectionalVectors[6];
extern const glm::vec3 BlockColors[16];

//MACROS
#define BUFFER_OFFSET(offset) ((GLvoid*)(intptr_t)(offset))

//Enum for defining positions of the different cube vertices.
enum CubeVertices{TopBackLeft=0, TopBackRight, TopFrontLeft, TopFrontRight, BottomBackLeft, BottomBackRight, BottomFrontLeft, BottomFrontRight};

//enum for all the Colors that Voxels could be.
enum VoxelColor{None=0, Red, Maroon, Pink, DPink, Purple, Aqua, Blue, Aquamarine, Cyan, Lime, Green, Yellow, Brown, Golden, Orange};

//enum for sides of a cube
enum CubeFace{LeftFace=0, RightFace, TopFace, BottomFace, FrontFace, BackFace};

//Enum for the different vertex buffers labels
enum VertexBufferLabels{LVertexBuffer=0, LTextureBuffer, LColorArray};

//typedef for the voxel class, these voxels will comprise blocks. (should )
class Voxel{
	public:
		Voxel();
		~Voxel();

		bool getActive();
		void setActive(bool va);

		glm::vec3 getPosition();
		void setPosition(glm::vec3 pos);

		bool getActiveSide(unsigned int n);
		std::vector<bool> getActiveSide();
		void setActiveSide(unsigned int n, bool s);
		void setActiveSide(std::vector<bool> s);

		VoxelColor getColor();
		void setColor(VoxelColor vc);

		unsigned char getBrightness(unsigned int n);
		std::vector<unsigned char> getBrightness();
		void setBrightness(unsigned int n, unsigned char c);
		void setBrightness(std::vector<unsigned char> s);

		int getVoxTex();
		void setVoxTex(int t);

	private:
		bool voxActive = true;
		glm::vec3 position;
		std::vector<bool> activeSides = {true,true,true,true,true,true};
		VoxelColor voxColor;
		std::vector<unsigned char> brightness = {255,255,255,255,255,255};//here every value needs to be clamped to 0-255
		int voxTex = 0;
};

//This is the Mesh Class, It will be generated per chunk. each chunk will contain its own mesh? or each block will be its own mesh and will combine into a chunks final mesh?
class Mesh{
	public:
		Mesh();
		Mesh(Voxel v);
		~Mesh();
 
 		void GenerateMesh(Voxel v);
		void UpdateMesh();

		void GenerateCubeSide(CubeFace face, glm::vec3 offset, VoxelColor c);
		void addVertexToMesh(glm::vec3 vert);
		void addNormalTomesh(glm::vec3 norm);
		void addTexCoordToMesh(glm::vec2 tex);
		void addColorToMesh(VoxelColor c);

		void PrintMeshVerts();

		void drawMesh();

	private:
		//Below Contains all the OpenGL variables needed in order to draw this mesh.
		GLuint VertexArrayObject;
		GLuint VertexBuffer[4];
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> vertexNormals;
		std::vector<glm::vec2> textureCoords;
		std::vector<glm::vec3> colors;
		std::vector<GLuint> indices;
		std::vector<GLuint> shaderPositions;
		//Below are all the variables not used in drawing.

};


//This is the definition of the Renderer for the Game, This is in process and Will contain alot of pseudocode for quite some time.

//Not sure if this class is needed, maybe to manage chunks and when they get rendered but will decide in the future.
class RenderController{
	public:
		RenderController();
		~RenderController();

		std::string getVertexShaderPath();
		void setVertexShaderPath(std::string path);

		std::string getFragmentShaderPath();
		void setFragmentShaderPath(std::string path);

		GLuint getProgramVariable();
		void setProgramVariable(GLuint prog);

		void createShaderProgram();

	private:
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
		GLuint programVariable;

};

extern RenderController ShaderController;

#endif //CryoRender
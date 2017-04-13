#ifndef CryoRender
#define CryoRender

#include <iostream>

#include <vector>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glu.h>

extern const glm::vec3 CubeVerts[8];
extern const glm::vec3 DirectionVectors[6];
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
enum VertexBufferLabels{LVertexBuffer=0, LTextureBuffer, LColorArray, LModelMatrices, LBrightnessBuffer};

//Enum for the Transformation Matrices
enum transformMatrixLabels{TranslateMatrix=0, ScaleMatrix, RotationMatrix, CombinedMatrix};

//typedef for the voxel class, these voxels will comprise blocks. (should )
class Voxel{
	public:
		Voxel();
		~Voxel();

		bool getActive();
		void setActive(bool va);

		virtual glm::vec3 getPosition();
		virtual void setPosition(glm::vec3 pos);

		void setScale(unsigned int s);

		void setRotation(float angle, glm::vec3 rot);

		glm::mat4 getTMatrix(unsigned int index);

		bool getActiveSide(unsigned int n);
		std::vector<bool> getActiveSide();
		void setActiveSide(unsigned int n, bool s);
		void setActiveSide(std::vector<bool> s);

		VoxelColor getColor();
		void setColor(VoxelColor vc);

		float getBrightness(unsigned int n);
		std::vector<float> getBrightness();
		void setBrightness(unsigned int n, float c);
		void setBrightness(std::vector<float> s);

		int getVoxTex();
		void setVoxTex(int t);

	private:
		bool voxActive = true;
		glm::vec3 position;
		std::vector<bool> activeSides = {true,true,true,true,true,true};
		VoxelColor voxColor;
		std::vector<float> brightness = {255,255,255,255,255,255};//here every value needs to be clamped to 0-255
		int voxTex = 0;
		std::vector<glm::mat4> transformMatrices;
};

//This is the Mesh Class, It will be generated per chunk. each chunk will contain its own mesh? or each block will be its own mesh and will combine into a chunks final mesh?
class Mesh{
	public:
		Mesh();
		Mesh(Voxel v);
		~Mesh();
 
 		void GenerateMesh(Voxel v);
		void UpdateMesh();

		void GenerateCubeSide(CubeFace face, float b, VoxelColor c, glm::vec3 offset);
		void addVertexToMesh(glm::vec3 vert);
		void addNormalTomesh(glm::vec3 norm);
		void addTexCoordToMesh(glm::vec2 tex);
		void addColorToMesh(VoxelColor c);
		void addMMToMesh(glm::mat4 mm);
		void addBrightnessToMesh(float b);

		void mergeWithMesh(Mesh * m);

		void PrintMeshVerts();

		void drawMesh();

		std::vector<glm::vec4> getVerts();
		std::vector<glm::vec4> getVertNormals();
		std::vector<glm::vec2> getTexCoords();
		std::vector<glm::vec3> getColors();
		std::vector<GLuint> getIndices();
		glm::mat4 getMatrix();
		std::vector<float> getBrightness();

	private:
		//Below Contains all the OpenGL variables needed in order to draw this mesh.
		GLuint VertexArrayObject;
		GLuint VertexBuffer[5];
		std::vector<GLuint> shaderPositions;

		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> vertexNormals;
		std::vector<glm::vec2> textureCoords;
		std::vector<glm::vec3> colors;
		glm::mat4 modelMatrix;
		std::vector<float> brightness;
		std::vector<GLuint> indices;
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
		glm::mat4 projectionMatrix;
		glm::mat4 viewMatrix;
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
		GLuint programVariable;

};

extern RenderController ShaderController;

#endif //CryoRender
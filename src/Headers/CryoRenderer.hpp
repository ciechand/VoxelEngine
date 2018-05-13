#ifndef CryoRender
#define CryoRender

#include <iostream>

#include <string>
#include <vector>
#include <map>

#include <glm/glm.hpp>

#include <GL/glew.h>
#include <GL/glu.h>

extern const glm::vec3 CubeVerts[8];
extern const glm::vec3 DirectionVectors[7];
extern const glm::vec3 BlockColors[16];

//CONTANTS
#define MAXSHADOWMAPS 30

//MACROS
#define BUFFER_OFFSET(offset) ((GLvoid*)(intptr_t)(offset))

//Class predefinition
class Block;

//Enum for defining positions of the different cube vertices.
enum CubeVertices{TopBackLeft=0, TopBackRight, TopFrontLeft, TopFrontRight, BottomBackLeft, BottomBackRight, BottomFrontLeft, BottomFrontRight};

//enum for all the Colors that Voxels could be.
enum VoxelColor{None=0, Red, Maroon, Pink, DPink, Purple, Aqua, Blue, Aquamarine, Cyan, Lime, Green, Yellow, Brown, Golden, Orange};

//enum for sides of a cube
enum CubeFace{LeftFace=0, RightFace, TopFace, BottomFace, FrontFace, BackFace, CenterFace};

//Enum for the different vertex buffers labels
enum VertexBufferLabels{LVertexBuffer=0, LNormalsBuffer,LTextureBuffer, LColorArray, LModelMatrices, LIndexBuffer, NUMBER_OF_LABELS};

//Enum for the Transformation Matrices
enum transformMatrixLabels{TranslateMatrix=0, ScaleMatrix, RotationMatrix, CombinedMatrix};

//Enum outlining the different shaderPrograms
enum shaderTypes{ShaderBase=0, ShaderShadow, ShaderPassThrough};

//forward class declarations go here
class BaseMesh;

//Class for controlling the camera and storing all the camera variables
class CameraController{
	public:
		CameraController();
		~CameraController();

		glm::mat4 getProjMatrix();
		void setProjMatrix(glm::mat4 vm);

		glm::mat4 getViewMatrix();
		void setViewMatrix(glm::mat4 vm);

		glm::vec3 getCamPos();
		void setCamPos(glm::vec3 pos);

		glm::vec3 getCamRot();
		void setCamRot(glm::vec3 rot);

		glm::vec3 getCamAt();
		void setCamAt(glm::vec3 at);

		glm::vec3 getCamUp();
		void setCamUp(glm::vec3 up);

		glm::vec3 getCamFRot();
		void setCamFRot(glm::vec3 frot);

	private:
		glm::mat4 projectionMatrix = glm::mat4();
		glm::mat4 viewMatrix = glm::mat4();
		glm::vec3 cameraPos = glm::vec3(20.0f,20.0f,20.0f);
		glm::vec3 cameraRotation = glm::vec3(0.0f,0.0f,0.0f);
		glm::vec3 cameraAt = glm::vec3(0.0f,0.0f,0.0f);
		glm::vec3 cameraUp = glm::vec3(0.0f,1.0f,0.0f);
		glm::vec3 cameraFinalRotation = glm::vec3(0.0f,0.0f,0.0f);
};


//Class that controlls all rendering for this program.
class RenderController{
	public:
		RenderController();
		~RenderController();

		void initialize();
		int loadBaseMeshes();
		void addBaseMesh(BaseMesh* m);
		void reloadBuffers();
		void reloadShaderBuffers();
		void createNewShaderProgram();

		void setShader(int index);
		void clearShader();

		std::string getVertexShaderPath();
		void setVertexShaderPath(std::string path);

		std::string getFragmentShaderPath();
		void setFragmentShaderPath(std::string path);

		GLuint getProgramVariable(int index);
		void setProgramVariable(GLuint prog, int index);

		std::vector<BaseMesh *> &getBaseMeshes();	

		glm::vec2 getWindowSize();
		void setWindowSize(glm::vec2 ws);

		bool getChanged();
		void setChanged(bool c);

		void drawScene();
	private:
		//Compilation of things needed to draw
		std::vector<GLuint> VertexArrayObject;
		GLuint VertexBufferBase[NUMBER_OF_LABELS];
		GLuint VertexBufferShadow[NUMBER_OF_LABELS];
		GLuint VertexBufferPassThrough[NUMBER_OF_LABELS];
		std::vector<GLuint> shaderPositions;
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> vertexNormals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> colors;
		std::vector<glm::mat4> modelMatrix;
		
		//variable to determine the end of preloaded baseMeshes
		unsigned int meshBorder = 0;
		unsigned int meshCount = 0;

		//Variables needed for other stuff
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
		std::vector<GLuint> programVariables;
		std::vector<BaseMesh *> baseMeshes;
		glm::vec2 windowSize = glm::vec2(SCREENWIDTH,SCREENHEIGHT);

		//variables necessary for shadow mapping
		GLuint shadowBuffer = 0;
		GLuint shadowDepthTexture;
		GLuint lightDataSSBO;
		GLuint lightMatrixSSBO;
		bool lighDataChanged = true;

		//Variables needed for SSAO
		GLuint SSAOKernel = 0;
		GLuint SSAONoise = 0;/*
		GLuint defaultDepthBuffer = 0;*/

		//Variable to dicatate if the scene has changed.
		bool sceneChanged = true;
};

extern RenderController ShaderController;
extern CameraController Camera;

#endif //CryoRender
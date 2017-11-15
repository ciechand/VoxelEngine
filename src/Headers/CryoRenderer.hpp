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
enum VertexBufferLabels{LVertexBuffer=0, LTextureBuffer, LColorArray, LModelMatrices, LIndexBuffer};

//Enum for the Transformation Matrices
enum transformMatrixLabels{TranslateMatrix=0, ScaleMatrix, RotationMatrix, CombinedMatrix};

//Enum outlining the different shaderPrograms
enum shaderTypes{ShaderBase=0, ShaderShadow};

//Classes for bounding boxes goes here, there will be a normal BB which will inherit from an AABB
/*
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*/

//VertexInfo Class, used when reading in the vertices
struct vertexInfo{
	glm::vec4 verts;
	glm::vec4 norms;
	glm::vec2 tex;
	bool operator<(const vertexInfo that) const{
		return memcmp((void*)this, (void*)&that, sizeof(vertexInfo))>0;
	};
};

//Light class, this class contains everythng necessary to create a light. Directional and Point Lights will inherit from this class.
class light{
	public:
		light();
		~light();

		glm::vec4 getPos();
		void setPos(glm::vec4 p);

		float getDist();
		void setDist(float d);
	private:
		glm::vec4 pos = glm::vec4(0.0f,0.0f,0.0f,0.0f);
		float distance = 1.0f;
};

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

//Light SourceController
class lightController{
	public:
	private:

};


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
		void reloadBuffers();
		void createNewShaderProgram();
		void overwriteShaderProgram(int index);

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
		GLuint VertexArrayObject;
		GLuint VertexBuffer[5];
		std::vector<GLuint> shaderPositions;
		std::vector<glm::vec4> vertices;
		std::vector<glm::vec4> vertexNormals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned int> indices;
		std::vector<glm::vec3> colors;
		std::vector<glm::mat4> modelMatrix;
		
		//Variables needed for other stuff
		std::string vertexShaderPath;
		std::string fragmentShaderPath;
		std::vector<GLuint> programVariables;
		std::vector<BaseMesh *> baseMeshes;
		glm::vec2 windowSize = glm::vec2(SCREENWIDTH,SCREENHEIGHT);

		//variables necessary for shadow mapping
		GLuint shadowBuffer = 0;
		GLuint shadowDepthTexture;

		//Variable to dicatate if the scene has changed.
		bool sceneChanged = true;
};

extern RenderController ShaderController;
extern CameraController Camera;

#endif //CryoRender
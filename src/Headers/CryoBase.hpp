#pragma GCC diagnostic ignored "-Wwrite-strings"

#define GLM_FORCE_RADIANS

//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <png.h>

#include <GL/glew.h>
#include <GL/glu.h>

#include <SFML/Main.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>

#include <boost/filesystem.hpp>

// GLOBAL CONSTANT DEFINITIONS
#define PI 3.1415926535f
#define TEXDIMS 16
#define NUMTEX 100
#define BLOCKSCALE 2.0f
#define CHUNKDIMS 5
#define CHUNKHEIGHT 1
#define LOOKSPEED 0.01f

//MACROS
#define BUFFER_OFFSET(offset) ((GLvoid*)(intptr_t)(offset))

#ifndef Base
#define Base

//Forward declaring any classes necessary
typedef class InstancedObject IOBJ;
typedef class Chunk Chunk;

//Enumeration of the states the game can be in.
// [0] = Paused
// [1] = Running
// [2] = Main Menu
// [3] = In game menu
// [4] = Loading

enum GameStates{Paused, Running, Main, Menu, Loading, Exiting};

//Enumeration for transform matrices access.
// [0] = Translate
// [1] = Scale
// [2] = Rotate
//NOTE: this is also the order they should be applied to the vector in, the vector is multiplied on the right.

enum TransformMatrixAtributes{Translate, Scale, Rotate}; 

//Enumeration for vertex attributes
// [0] = Vertex Position
// [1] = Vertex Normals
// [2] = Texture Coords
//NOTE: also the order they are in the VBO array.

enum VertexAttributes{Positions, Normals, Textures, ModelMatrices, TexturePos};

//Enum for faces of cube for texture mapping.
// [0] = Front
// [1] = Back
// [2] = Left
// [3] = Right
// [4] = Top
// [5] = Bottom

enum BlockFace{Front, Back, Left, Right, Top, Bottom};

//enum for the materials that blocks can be made of.
enum Materials{Air, Stone};
/*
	THIS SPACE RESERVED FOR EXTERNAL GLOBAL VARIABLES
*/

//Class for holding the main state of the game.
typedef class GameState{
private:
	unsigned char curState;
	float camPos[2];
public:
	GameState();
	GameState(unsigned char cs);
	
	void setState(unsigned char cs);
	unsigned char getState();

	void setCamPos(float x, float y);
	float * getCamPos();
}GState;

//Class for base object, many other object types such as Item and Block will inherit from this class.
typedef class baseObj{
protected:
	std::vector<glm::mat4> TransformMatrices;
	int modelIdentifier;
	int textureIdentifier;
public:
	baseObj();

	void setTMatrix(int index, glm::mat4 matrix);
	glm::mat4 getTMatrix(int index);
	std::vector<glm::mat4> getTMatrixVec();

	void setMID(int id);
	int getMID();

	void setTID(int id);
	int getTID();
}OBJ;

//Main class for the GameOptions.
typedef class GameOptions{
private:
	float projVar[4];
	float camPos[3];
	std::vector<std::string> modelPaths;
	std::vector<std::string> texturePaths;
public:
	GameOptions();
	GameOptions(const char * optionsFileName);

	void Initialize();

	void setProjVars(float * vars);
	float * getProjVars();

	void setCamPos(float * pos);
	float * getCamPos();

	void addMPaths(std::string path);
	void clearMPaths();
	std::vector<std::string> getMPaths();

	void addTPaths(std::string path);
	void clearTPaths();
	std::vector<std::string> getTPaths();
}OPT;

//The main rendering storage, most of the information that is handled while rendering is housed in game renderer.
typedef class GameRenderer{
private:
	GLuint shaderProgram;
	glm::vec3 camVec[3];
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	std::vector<IOBJ*> InstancedObjectsList;
	std::vector<GLuint> TextureList;
public:
	GameRenderer();

	void Initialize();

	void setShaderProgram(const char * vertexPath, const char * fragmentPath);
	GLuint getShaderProgram();

	void setCamVec(int index, glm::vec3 v);
	glm::vec3 getCamVec(int index);
	glm::vec3 * getCamVec();

	void setViewMatrix(glm::mat4 viewMatrix);
	glm::mat4 getViewMatrix();

	void setProjMatrix(glm::mat4 projMatrix);
	glm::mat4 getProjMatrix();

	void setIObject(int index, IOBJ * object);
	void addToIObjectList(IOBJ * object);
	void clearIObjectList();
	std::vector<IOBJ *> getIObjectList();
	IOBJ * getIObject(int index);

	void addToTextureList(const char * textureFile);
	void clearTextureList();
	std::vector<GLuint> getTextureList();
	GLuint getTextureList(int index);
}GRend;

typedef struct vertexInfo{
	glm::vec4 verts;
	glm::vec4 norms;
	glm::vec2 tex;
	bool operator<(const vertexInfo that) const{
		return memcmp((void*)this, (void*)&that, sizeof(vertexInfo))>0;
	};
}Vinf;

void InitOpenGL();

extern GRend Renderer;
extern OPT Options;
extern GState State;
extern Chunk testingChunk[9];
extern sf::Window mainWindow;

#endif // Base

#include "CryoRendering.hpp"
#include "CryoGeneration.hpp"
#include "CryoUtil.hpp"

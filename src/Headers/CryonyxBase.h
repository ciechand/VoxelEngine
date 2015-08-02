#pragma GCC diagnostic ignored "-Wwrite-strings"

#define _CRT_SECURE_NO_DEPRECATE
#define GLM_FORCE_RADIANS

#define PI 3.14159f

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <map>


#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include "CryonyxUtil.h"
#include "CryonyxRenderer.h"

using namespace glm;


#ifndef base
#define base

//An enumeration defining the different states of the game
// Loading = 0
// Menu = 1
// Paused = 2
// Running = 3
// Possbily more will be needed.
enum States{ Loading, Menu, Paused, Running};
//an enumeration of the attributes
enum Attribs{Position, Normal, Texture};

//A class that will hold the state of the game, if its paused or running etc...
class GameState{
private:
	short state;
	int * mousePos;
public:
	GameState();
	void setMousePos(int x, int y);
	int * getMousePos();
	void SetState(int newState);
};

class Object{
private:
public:
	class ObjectRenderer * renderer;
	Object();
	Object(char * objFileName, char * texFileName);
};


class GameOptions{
private:
	int windowSize[2];
	int windowPos[2];
	float projVar[4];
public:
	glm::mat4 projectionMatrix;
	int * getWindowSize();
	void setWindowSize(int width, int height);
	int * getWindowPos();
	void setWindowPos(int x, int y);
	float * getProjVar();
	void setProjVar(float pov, float aspect, float nearPlane, float farPlane);
	GameOptions();
	GameOptions(int width, int height, int posx, int posy, float pov, float aspect, float np, float fp);
};

class GameRenderer{
private:
	GLuint shaderProgram;
public:
	glm::mat4 mvTrans;
	glm::mat4 mvRot;
	glm::mat4 mvScale;
	glm::mat4 projection;
	std::vector <class Object *> Objects;
	void LoadShaderProgram(char * vShader, char * fShader);
	GLuint getShaderProgram();
};



void InitOpenGL();
void InitOBJ(class Object * obj);


extern class GameState State;
extern class GameRenderer Renderer;
extern class GameOptions Options;
extern double deltaTime;
#endif
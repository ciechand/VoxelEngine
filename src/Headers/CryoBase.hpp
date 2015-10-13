#pragma GCC diagnostic ignored "-Wwrite-strings"

#define GLM_FORCE_RADIANS

//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <cstring>
#include <map>

#include <GL/glew.h>
#include <GL/glu.h>
#include <GL/freeglut.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matric_access.hpp>

#include <boost/filesystem.hpp>

// GLOBAL CONSTANT DEFINITIONS
#define PI 3.1415926535

//MACROS
#define BUFFER_OFFSET(offset) ((GLvoid*) (offset))

#include "CryoRendering.hpp"
#include "CryoUtil.hpp"
#include "CryoGeneration.hpp"

#ifndef Base
#define Base

//Enumeration of the states the game can be in.
// [0] = Paused
// [1] = Running
// [2] = Main Menu
// [3] = In game menu
// [4] = Loading

enum GameStates{Paused, Running, Main, IGM, Loading}

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

enum VertexAttributes{Positions, Normals, Textures};

//Class for holding the main state of the game.
typedef class GameState{
private:
	unsigned char curState;
public:
	GameState();
	GameState(unsigned char cs);
	
	void setState(unsigned char cs);
	unsigned char getState();
}GState;




#endif // Base
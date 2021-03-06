#ifndef CryoMain
#define CryoMain

//Includes
#include <iostream>
#include <vector>
#include <string>
#include <random>

#include <time.h>

#include <GL/glew.h>
#include <GL/glu.h>

#include <SFML/Main.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/integer.hpp>

//Next comes our Defines, these are constants that are necessary for hte game to function
#define SCREENWIDTH 1920
#define SCREENHEIGHT 1080
#define CUBEMAPSIDELENGTH 2048

#define DEBUGMODE false
#define SHADOWMAPDEBUG false

#define CUBESIZE 1.0f
#define HALFSIZE (CUBESIZE/2.0f)+0.003f

#define CHUNKSIZE CHUNKSIDE*CHUNKSIDE*CHUNKSIDE
#define CHUNKSIDE 16

#define PI 3.14f

#define KERNELSIZE 64
#define NOISESIZE 16

#define NEARPLANE 0.05f
#define FARPLANE 200.0f

#define MAXLIGHTS 10

#define GLM_FORCE_RADIANS

#define TO_RADIANS(angle) ((angle*PI)/180.0f)

//Forward Definitions
class Chunk;

//Function Definitions, These are mostly initializations that happen within the main Loop.
void InitOpenGL();
void PrepForRender();
void Render();
void Cleanup();

extern sf::Window MainWindow;
extern sf::Clock MainClock;
extern std::vector<Chunk *> ChunkContainer;

#endif //CryoMain
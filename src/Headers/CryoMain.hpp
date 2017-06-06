#ifndef CryoMain
#define CryoMain

//Includes
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>

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

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>

//Next comes our Defines, these are constants that are necessary for hte game to function
#define SCREENWIDTH 1920
#define SCREENHEIGHT 1080

#define DEBUGMODE true

#define CUBESIZE 1.0f
#define HALFSIZE CUBESIZE/2.0f
#define MINICUBESIZE 1.0f/16.0f

#define CHUNKSIDE 16
#define CHUNKSIZE CHUNKSIDE*CHUNKSIDE*CHUNKSIDE

#define PI 3.14f

//Forward Definitions
class Chunk;

//Function Definitions, These are mostly initializations that happen within the main Loop.
void InitOpenGL();
void PrepForRender();
void Render();

extern sf::Clock MainClock;
extern std::vector<Chunk *> ChunkContainer;

#endif //CryoMain
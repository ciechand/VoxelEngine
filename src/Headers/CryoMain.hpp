#ifndef CryoMain
#define CryoMain

//Includes
#include <iostream>
#include <vector>
#include <string>
#include <random>

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
#define SCREENWIDTH 1024
#define SCREENHEIGHT 768

#define DEBUGMODE true

#define CUBESIZE 1.0f
#define HALFSIZE CUBESIZE/2.0f

//Function Definitions, These are mostly initializations that happen within the main Loop.
void InitOpenGL();
void BasicRender();

#endif //CryoMain
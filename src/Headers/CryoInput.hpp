#ifndef input
#define input

#include <iostream>

#include <GL/glew.h>
#include <GL/glu.h>

#include <SFML/Main.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#define LOOKSPEED 1.5f
#define CAMERADIST 20.0f
#define WALKSPEED 1.0f

void processKeyPress(sf::Event event);
void processMouseMove(sf::Event event);
void processResize(sf::Event event);
void processMouseClicks(sf::Event event);

extern bool mouseBound;

#endif //input
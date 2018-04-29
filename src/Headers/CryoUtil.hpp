#ifndef util
#define util

#include <iostream>

#include <string>

#include <GL/glew.h>
#include <GL/glu.h>

#include <glm/glm.hpp>

std::string readFileToString(const char* filePath);
glm::vec3 translate1DPos(unsigned int index, unsigned int size);
unsigned int translate3DPos(glm::vec3 position, unsigned int size);
unsigned int translate3DPos(int x,int y,int z, unsigned int size);
void print4x4Matrix(glm::mat4 matrix);
float lerp(float a, float b, float f);
#endif //util
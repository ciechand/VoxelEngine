#ifndef util
#define util

//Functions for processing Opengl things such as keyboard and mouse movement.
void ProcessMouseClicks(int button, int state, int x, int y);
void ProcessPassiveMouseMovement(int x, int y);
void ProcessActiveMouseMovement(int x, int y);
void ProcessKeyboard(unsigned char key, int x, int y);

//Reads a file in and interprets it into a NULL-Terminated string.
std::string readFileToString(const char * fileName);

//Function for loading in an OBJ file.
void readOBJFile(IOBJ * Object, const char * fileName);

//File for loading in a BMP and assigning it to a specific texture.
void loadBMP(const char * fileName, GLuint * textureID);

//Function for loading in the two shader programs, compiling them and spitting out the GLuint.
GLuint createShadersProgram(const char* vsFile, const char* fsFile);

//Function for comparing two block distances from the camera.
bool compBlockDist(glm::mat4 a, glm::mat4 b);

//Ways of printing matrices and vertices. (Functions will be added as necessary)
void printMatrix(glm::mat4 matrix);
void printMatrix(glm::mat3 matrix);

#endif //util
#ifndef util
#define util

#define BUFFER_OFFSET(offset) ((GLvoid*) (offset))

void ProcessMouseClicks(int button, int state, int x, int y);
void ProcessPassiveMouseMovement(int x, int y);
void ProcessActiveMouseMovement(int x, int y);
void ProcessKeyboard(unsigned char key, int x, int y);
char* readFile(const char* filename);
GLuint createShadersProgram(const char* vsFile, const char* fsFile);
void printMatrix(glm::mat4 matrix);
#endif
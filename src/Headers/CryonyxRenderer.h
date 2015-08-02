#ifndef render
#define render
#include <string.h>

class ObjectRenderer{
private:
	char * objFileName;
	char * texFileName;
public:
	GLuint vertexArray;
	GLuint elementBuffer;
	GLuint vertexBuffer[3];
	std::vector<GLuint> vertexAttribs;
	std::vector<GLuint> vertexUniforms;
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> vertexNormals;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> indices;
	void ReadOBJFile();
	void SetOBJFile(char * fileName);
	void loadBMP();
	void SetTexFile(char * fileName);
	ObjectRenderer();
	ObjectRenderer(char * objFile, char * texFile);
};

struct vertexInfo{
	glm::vec4 verts;
	glm::vec4 norms;
	glm::vec2 tex;
	bool operator<(const vertexInfo that) const{
		return memcmp((void*)this, (void*)&that, sizeof(vertexInfo))>0;
	};
};

typedef ObjectRenderer objr;

void display();

void drawOBJ(class Object * obj);
#endif
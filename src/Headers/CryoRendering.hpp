#ifndef rendering
#define rendering


typedef class InstancedObject{
private:
	GLuint vertexArrayObject;
	GLuint elementBuffer;
	GLuint vertexBufferObjects[4];
	std::vector<GLuint> vertexAttribs;
	std::vector<GLuint> vertexUniforms;
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> vertexNormals;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> indices;
	std::vector<std::pair<unsigned long long int, glm::mat4> > modelMatrices;
	std::vector<int> materials; 
public:
	InstancedObject();
	void Initialize();
	void drawOBJ();
	void Update();

	void setVertexArrayObject(GLuint VAO);
	GLuint getVertexArrayObject();

	void setElementBuffer(GLuint EB);
	GLuint getElementBuffer();

	void setVertexBufferObject(int index, GLuint VBO);
	GLuint getVertexBufferObject(int index);

	void addVAttributes(GLuint attrib);
	void clearVAttributes();
	std::vector<GLuint> getVAttributes();

	void addVUniforms(GLuint attrib);
	void clearVUniforms();
	std::vector<GLuint> getVUniforms();

	void setVertices(std::vector<glm::vec4> verts);
	void addVertices(glm::vec4 vertex);
	void clearVertices();
	std::vector<glm::vec4> getVertices();
	glm::vec4 getVertices(int index);

	void setVertexNormals(std::vector<glm::vec4> vNormals);
	void addVertexNormals(glm::vec4 vnormal);
	void clearVertexNormals();
	std::vector<glm::vec4> getVertexNormals();
	glm::vec4 getVertexNormals(int index);

	void setTextureCoords(std::vector<glm::vec2> texCoords);
	void addTextureCoords(glm::vec2 texcoord);
	void clearTextureCoords();
	std::vector<glm::vec2> getTextureCoords();
	glm::vec2 getTextureCoords(int index);

	void addIndices(GLuint index);
	void clearIndices();
	std::vector<GLuint> getIndices();

	void addModelMatrices(unsigned long long int identifier, glm::mat4 matrix);
	void clearModelMatrices();
	std::vector<std::pair<unsigned long long int, glm::mat4> > getModelMatrices();

	void addMaterial(int material);
	void clearMaterials();
	std::vector<int> getMaterials();

}IOBJ;

void display();


#endif //rendering
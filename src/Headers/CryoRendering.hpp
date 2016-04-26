#ifndef rendering
#define rendering


typedef class InstancedObject{
private:
	GLuint vertexArrayObject;
	GLuint elementBuffer;
	GLuint vertexBufferObjects[5];
	std::vector<glm::vec4> vertices;
	std::vector<glm::vec4> vertexNormals;
	std::vector<glm::vec2> textureCoords;
	std::vector<GLuint> indices;
	std::vector<Block> blockList;
	GLenum drawType;
public:
	InstancedObject();
	~InstancedObject();

	void Initialize();
	void drawOBJ();
	void Update();

	void setVertexArrayObject(GLuint VAO);
	GLuint getVertexArrayObject();

	void setElementBuffer(GLuint EB);
	GLuint getElementBuffer();

	void setVertexBufferObject(int index, GLuint VBO);
	GLuint getVertexBufferObject(int index);

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

	void addBlocks();
	void setBlockList(std::vector<Block>& B);
	void removeBlock(int blockpos);
	void clearBlocks();
	std::vector<Block>& getBlocks();
	Block * getBlocks(int index);
	int getBlocksSize();

	GLenum getDrawType();
	void setDrawType(GLenum t);
}IOBJ;

void display();



#endif //rendering
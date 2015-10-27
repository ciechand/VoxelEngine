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
	std::vector<glm::mat4> modelMatrices;
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

	void addModelMatrices(glm::mat4 matrix);
	void clearModelMatrices();
	std::vector<glm::mat4> getModelMatrices();

	/*InstancedObject &  operator=(const InstancedObject that){
		if(this == &that) {return *this; 			std::cout << "point?" << std::endl; std::cout.flush();
		}else {

			this->vertexArrayObject = that.vertexArrayObject;
			this->elementBuffer = that.elementBuffer;
			this->vertexBufferObjects[0] = that.vertexBufferObjects[0];
			this->vertexBufferObjects[1] = that.vertexBufferObjects[1];
			this->vertexBufferObjects[2] = that.vertexBufferObjects[2];
			this->vertexBufferObjects[3] = that.vertexBufferObjects[3];
			this->vertexAttribs = that.vertexAttribs;
			this->vertexUniforms = that.vertexUniforms;
			this->vertices = that.vertices;
			this->vertexNormals = that.vertexNormals;
			this->textureCoords = that.textureCoords;
			this->indices = that.indices;
			this->modelMatrices = that.modelMatrices;
		}
	}*/
}IOBJ;

void display();


#endif //rendering
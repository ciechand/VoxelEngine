#include "../Headers/CryoBase.hpp"

InstancedObject::InstancedObject(){
}

void InstancedObject::Initialize(){
	GLuint location = 0;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//setup for the vertex and normal arrays.
	glGenBuffers(3, &vertexBufferObjects[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + vertexNormals.size()*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), &vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), vertexNormals.size()*sizeof(glm::vec4), &vertexNormals[0]);

	GLuint vPos = glGetAttribLocation(Renderer.getShaderProgram(), "vPosition");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormPos = glGetAttribLocation(Renderer.getShaderProgram(), "vNormal");
	glEnableVertexAttribArray(vNormPos);
	glVertexAttribPointer(vNormPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Start with binding and setup of textures.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);

	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size() * sizeof(glm::vec2), &textureCoords[0]);

	GLuint texCoordinatePos = glGetAttribLocation(Renderer.getShaderProgram(), "textureCoordinatesIn");
	glEnableVertexAttribArray(texCoordinatePos);
	glVertexAttribPointer(texCoordinatePos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//model matrices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, (blockList.size()*sizeof(Block)), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (blockList.size()*sizeof(Block)), &blockList[0]);

	GLuint modelMatrixPos = glGetAttribLocation(Renderer.getShaderProgram(), "modelMatrix");

 	GLsizei vec4Size = sizeof(glm::vec4);

    glEnableVertexAttribArray(modelMatrixPos); 
    glVertexAttribPointer(modelMatrixPos, 4, GL_FLOAT, GL_FALSE, sizeof(Block), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(modelMatrixPos+1); 
    glVertexAttribPointer(modelMatrixPos+1, 4, GL_FLOAT, GL_FALSE, sizeof(Block), BUFFER_OFFSET((vec4Size)));
    glEnableVertexAttribArray(modelMatrixPos+2); 
    glVertexAttribPointer(modelMatrixPos+2, 4, GL_FLOAT, GL_FALSE, sizeof(Block), BUFFER_OFFSET((vec4Size*2)));
    glEnableVertexAttribArray(modelMatrixPos+3); 
    glVertexAttribPointer(modelMatrixPos+3, 4, GL_FLOAT, GL_FALSE, sizeof(Block), BUFFER_OFFSET((vec4Size*3)));
    glVertexAttribDivisor(modelMatrixPos, 1);
    glVertexAttribDivisor(modelMatrixPos+1, 1);
    glVertexAttribDivisor(modelMatrixPos+2, 1);
    glVertexAttribDivisor(modelMatrixPos+3, 1);

    //model matrices
    GLuint texPosPos = glGetAttribLocation(Renderer.getShaderProgram(), "texPosIn");
    glEnableVertexAttribArray(texPosPos); 
    glVertexAttribPointer(texPosPos, 1, GL_INT, GL_FALSE, sizeof(Block), BUFFER_OFFSET(sizeof(glm::mat4)+(sizeof(int))));
    glVertexAttribDivisor(texPosPos, 1);

    GLuint ColorPos = glGetAttribLocation(Renderer.getShaderProgram(), "iColor");
    glEnableVertexAttribArray(ColorPos); 
    glVertexAttribPointer(ColorPos, 3, GL_FLOAT, GL_FALSE, sizeof(Block), BUFFER_OFFSET(sizeof(glm::mat4)+(sizeof(int)*2)+sizeof(unsigned long long int)));
    glVertexAttribDivisor(ColorPos, 1);

	GLuint TexSizePos = glGetUniformLocation(Renderer.getShaderProgram(), "TexSize");
	glUniform1i(TexSizePos, (int)NUMTEX);

	GLuint samplePos = glGetUniformLocation(Renderer.getShaderProgram(), "textureSample");
	glUniform1i(samplePos, 0);


	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), &indices[0]);

	glm::mat4 p = glm::perspective(Options.getProjVars()[0],Options.getProjVars()[1],Options.getProjVars()[2],Options.getProjVars()[3]);
	glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "proj"), 1, GL_FALSE, &p[0][0]);
}

void InstancedObject::drawOBJ(){
	if(blockList.size() != 0){
		glBindVertexArray(vertexArrayObject);
		Update();
		glDrawElementsInstanced(GL_TRIANGLES, indices.size()*sizeof(GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(0), blockList.size());
	}
}

void InstancedObject::Update(){
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, (blockList.size()*sizeof(Block)), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (blockList.size()*sizeof(Block)), &blockList[0]);

}

void InstancedObject::setVertexArrayObject(GLuint VAO){
	vertexArrayObject = VAO;
}

GLuint InstancedObject::getVertexArrayObject(){
	return vertexArrayObject;
}

void InstancedObject::setElementBuffer(GLuint EB){
	elementBuffer = EB;
}

GLuint InstancedObject::getElementBuffer(){
	return elementBuffer;
}

void InstancedObject::setVertexBufferObject(int index, GLuint VBO){
	vertexBufferObjects[index] = VBO;
}

GLuint InstancedObject::getVertexBufferObject(int index){
	return vertexBufferObjects[index];
}

void InstancedObject::setVertices(std::vector<glm::vec4> verts){
	vertices = verts;
}

void InstancedObject::addVertices(glm::vec4 vertex){
	vertices.push_back(vertex);
}

void InstancedObject::clearVertices(){
	vertices.clear();
}

std::vector<glm::vec4> InstancedObject::getVertices(){
	return vertices;
}

glm::vec4 InstancedObject::getVertices(int index){
	return vertices[index];
}

void InstancedObject::setVertexNormals(std::vector<glm::vec4> vNormals){
	vertexNormals = vNormals;
}

void InstancedObject::addVertexNormals(glm::vec4 vnormal){
	vertexNormals.push_back(vnormal);
}

void InstancedObject::clearVertexNormals(){
	vertexNormals.clear();
}

std::vector<glm::vec4> InstancedObject::getVertexNormals(){
	return vertexNormals;
}

glm::vec4 InstancedObject::getVertexNormals(int index){
	return vertexNormals[index];
}

void InstancedObject::setTextureCoords(std::vector<glm::vec2> texCoords){
	textureCoords = texCoords;
}

void InstancedObject::addTextureCoords(glm::vec2 texcoord){
	textureCoords.push_back(glm::vec2(texcoord[0],texcoord[1]));
}

void InstancedObject::clearTextureCoords(){
	textureCoords.clear();
}

std::vector<glm::vec2> InstancedObject::getTextureCoords(){
	return textureCoords;
}

glm::vec2 InstancedObject::getTextureCoords(int index){
	return textureCoords[index];
}


void InstancedObject::addIndices(GLuint index){
	indices.push_back(index);
}

void InstancedObject::clearIndices(){
	indices.clear();
}

std::vector<GLuint> InstancedObject::getIndices(){
	return indices;
}

void InstancedObject::addBlocks(Block B){
	blockList.push_back(B);
}

void InstancedObject::clearBlocks(){
	blockList.clear();
}

std::vector<Block> InstancedObject::getBlocks(){
	return blockList;
}


void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "viewMatrix"), 1, GL_FALSE, &Renderer.getViewMatrix()[0][0]);

	std::vector<IOBJ *> Objects = Renderer.getIObjectList();
	for(int i=0; i<Objects.size(); i++){
		Objects[i]->drawOBJ();
	}	

	sf::Vector2u windowSize = mainWindow.getSize();
	if(State.getState() != Menu){
		mainWindow.setMouseCursorVisible(false);
		sf::Mouse::setPosition(sf::Vector2i(windowSize.x/2, windowSize.y/2), mainWindow);
	}else{
		mainWindow.setMouseCursorVisible(true);
	}
	mainWindow.display();
}
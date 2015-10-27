#include "../Headers/CryoBase.hpp"

InstancedObject::InstancedObject(){
	vertexAttribs.assign(3,0);
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

	vertexAttribs[Positions] = glGetAttribLocation(Renderer.getShaderProgram(), "vPosition");
	glEnableVertexAttribArray(vertexAttribs[Positions]);
	glVertexAttribPointer(vertexAttribs[Positions], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	vertexAttribs[Normals] = glGetAttribLocation(Renderer.getShaderProgram(), "vNormal");
	glEnableVertexAttribArray(vertexAttribs[Normals]);
	glVertexAttribPointer(vertexAttribs[Normals], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Start with binding and setup of textures.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);

	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size()*sizeof(glm::vec2), &textureCoords[0]);

	//model matrices
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size()*sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMatrices.size()*sizeof(glm::mat4), &modelMatrices[0]);

 	GLsizei vec4Size = sizeof(glm::vec4);
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, BUFFER_OFFSET(vec4Size));
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, BUFFER_OFFSET(vec4Size*2));
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size,BUFFER_OFFSET(vec4Size*3));
    glVertexAttribDivisor(0, 1);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);

	//vertexUniforms.push_back(glGetUniformLocation(Renderer.getShaderProgram(), "textureSample"));

	//THIS MAY NEED TO BE CHANGED IF YOU END UP USING MORE THAN ONE UNIFORM LIKE THIS
	//glUniform1i(vertexUniforms[0], 0);

	// vertexAttribs[2] = glGetAttribLocation(Renderer.getShaderProgram(), "textureCoordinatesIn");
	// glEnableVertexAttribArray(vertexAttribs[Textures]);
	// glVertexAttribPointer(vertexAttribs[Textures], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// /*TEXTURE CODE WILL GO HERE*/
	// glGenTextures(1, &vertexBufferObjects[3]);
	// glBindTexture(GL_TEXTURE_2D, vertexBufferObjects[3]);
	// glActiveTexture(vertexBufferObjects[3]);

	// // Poor filtering. Needed !
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), &indices[0]);

}

void InstancedObject::drawOBJ(){
	if(modelMatrices.size() != 0){
		Update();
		glBindVertexArray(vertexArrayObject);
		glm::mat4 p = glm::perspective(Options.getProjVars()[0],Options.getProjVars()[1],Options.getProjVars()[2],Options.getProjVars()[3]);
		glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "proj"), 1, GL_FALSE, &p[0][0]);
		glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "viewMatrix"), 1, GL_FALSE, &Renderer.getViewMatrix()[0][0]);

		glDrawElementsInstanced(GL_TRIANGLES, indices.size()*sizeof(GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(0), modelMatrices.size());
	}
}

void InstancedObject::Update(){
	glBindVertexArray(vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrices.size()*sizeof(glm::mat4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, modelMatrices.size()*sizeof(glm::mat4), &modelMatrices[0]);
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

void InstancedObject::addVAttributes(GLuint attrib){
	vertexAttribs.push_back(attrib);
}

void InstancedObject::clearVAttributes(){
	vertexAttribs.clear();
}

std::vector<GLuint> InstancedObject::getVAttributes(){
	return vertexAttribs;
}

void InstancedObject::addVUniforms(GLuint attrib){
	vertexUniforms.push_back(attrib);
}

void InstancedObject::clearVUniforms(){
	vertexUniforms.clear();
}

std::vector<GLuint> InstancedObject::getVUniforms(){
	return vertexUniforms;
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
	textureCoords.push_back(texcoord);
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

void InstancedObject::addModelMatrices(glm::mat4 matrix){
	modelMatrices.push_back(matrix);
}

void InstancedObject::clearModelMatrices(){
	modelMatrices.clear();
}

std::vector<glm::mat4> InstancedObject::getModelMatrices(){
	return modelMatrices;
}


void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	std::vector<IOBJ *> Objects = Renderer.getIObjectList();
	for(int i=0; i<Objects.size(); i++){
		Objects[i]->drawOBJ();
	}	
	//glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
	glutSwapBuffers();
}
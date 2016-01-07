#include "../Headers/CryoBase.hpp"

InstancedObject::InstancedObject(){
	vertexAttribs.assign(4,0);
	vertexUniforms.assign(2,0);
}

void InstancedObject::Initialize(){
	GLuint location = 0;
	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//setup for the vertex and normal arrays.
	glGenBuffers(4, &vertexBufferObjects[0]);
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
	vertexAttribs[Textures] = glGetAttribLocation(Renderer.getShaderProgram(), "textureCoordinatesIn");
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);

	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size() * sizeof(glm::vec2), &textureCoords[0]);

	glEnableVertexAttribArray(vertexAttribs[Textures]);
	glVertexAttribPointer(vertexAttribs[Textures], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//model matrices
	GLsizei ullisize = sizeof(unsigned long long int);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, (modelMatrices.size()*sizeof(glm::mat4)), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (modelMatrices.size()*sizeof(glm::mat4)), &modelMatrices[0]);

	vertexAttribs[ModelMatrices] = glGetAttribLocation(Renderer.getShaderProgram(), "modelMatrix");
 	GLsizei vec4Size = sizeof(glm::vec4);
 	
    glEnableVertexAttribArray(vertexAttribs[ModelMatrices]); 
    glVertexAttribPointer(vertexAttribs[ModelMatrices], 4, GL_FLOAT, GL_FALSE, (4 * vec4Size)+ullisize, BUFFER_OFFSET(ullisize));
    glEnableVertexAttribArray(1); 
    glVertexAttribPointer(vertexAttribs[ModelMatrices]+1, 4, GL_FLOAT, GL_FALSE, (4 * vec4Size)+ullisize, BUFFER_OFFSET((vec4Size)+ullisize));
    glEnableVertexAttribArray(2); 
    glVertexAttribPointer(vertexAttribs[ModelMatrices]+2, 4, GL_FLOAT, GL_FALSE, (4 * vec4Size)+ullisize, BUFFER_OFFSET((vec4Size*2)+ullisize));
    glEnableVertexAttribArray(3); 
    glVertexAttribPointer(vertexAttribs[ModelMatrices]+3, 4, GL_FLOAT, GL_FALSE, (4 * vec4Size)+ullisize, BUFFER_OFFSET((vec4Size*3)+ullisize));
    glVertexAttribDivisor(vertexAttribs[ModelMatrices], 1);
    glVertexAttribDivisor(vertexAttribs[ModelMatrices]+1, 1);
    glVertexAttribDivisor(vertexAttribs[ModelMatrices]+2, 1);
    glVertexAttribDivisor(vertexAttribs[ModelMatrices]+3, 1);

    //model matrices
    vertexAttribs[TexturePos] = glGetAttribLocation(Renderer.getShaderProgram(), "texPosIn");
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[3]);
	glBufferData(GL_ARRAY_BUFFER, (materials.size()*sizeof(int))+(modelMatrices.size()*sizeof(unsigned long long int)), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (materials.size()*sizeof(int))+(modelMatrices.size()*sizeof(unsigned long long int)), &materials[0]);

    glEnableVertexAttribArray(vertexAttribs[TexturePos]); 
    glVertexAttribPointer(vertexAttribs[TexturePos], 1, GL_INT, GL_FALSE, sizeof(int), BUFFER_OFFSET(0));
    glVertexAttribDivisor(vertexAttribs[TexturePos], 1);

	vertexUniforms[1] = glGetUniformLocation(Renderer.getShaderProgram(), "TexSize");
	glUniform1i(vertexUniforms[1], (int)NUMTEX);

	vertexUniforms[0] = glGetUniformLocation(Renderer.getShaderProgram(), "textureSample");
	glUniform1i(vertexUniforms[0], 0);


	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), &indices[0]);

	glm::mat4 p = glm::perspective(Options.getProjVars()[0],Options.getProjVars()[1],Options.getProjVars()[2],Options.getProjVars()[3]);
	glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "proj"), 1, GL_FALSE, &p[0][0]);
}

void InstancedObject::drawOBJ(){
	if(modelMatrices.size() != 0){
		glBindVertexArray(vertexArrayObject);
		Update();
		glDrawElementsInstanced(GL_TRIANGLES, indices.size()*sizeof(GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(0), modelMatrices.size());
	}
}

void InstancedObject::Update(){
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[2]);
	glBufferData(GL_ARRAY_BUFFER, (modelMatrices.size()*sizeof(glm::mat4))+(modelMatrices.size()*sizeof(unsigned long long int)), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (modelMatrices.size()*sizeof(glm::mat4))+(modelMatrices.size()*sizeof(unsigned long long int)), &modelMatrices[0]);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[3]);
	glBufferData(GL_ARRAY_BUFFER, materials.size()*sizeof(int), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, materials.size()*sizeof(int), &materials[0]);

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

void InstancedObject::addModelMatrices(unsigned long long int identifier, glm::mat4 matrix){
	modelMatrices.push_back(std::pair<unsigned long long int, glm::mat4>(identifier, matrix));
}

void InstancedObject::clearModelMatrices(){
	modelMatrices.clear();
}

std::vector<std::pair<unsigned long long int, glm::mat4> > InstancedObject::getModelMatrices(){
	return modelMatrices;
}

void InstancedObject::addMaterial(int material){
	materials.push_back(material);
}

void InstancedObject::clearMaterials(){
	materials.clear();
}

std::vector<int> InstancedObject::getMaterials(){
	return materials;
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
		sf::Mouse::setPosition(sf::Vector2i(windowSize.x/2, windowSize.y/2), mainWindow);
	}
	mainWindow.display();
}
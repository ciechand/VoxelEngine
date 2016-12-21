#include "../Headers/CryoBase.hpp"

InstancedObject::InstancedObject(){
	drawType = GL_TRIANGLES;
	textureSheet = textureNamesFind("Blocks");
	modelID = modelNamesFind("Cube");
	instanced = false;
	changed = true;
}

InstancedObject::~InstancedObject(){
	glDeleteVertexArrays(1, &vertexArrayObject);
	glDeleteBuffers(4, &vertexBufferObjects[0]);
}


void InstancedObject::Init(bool inst){
	instanced = inst;

	size_t stride = sizeof(Block);
	Block * B = new Block();
	size_t listStartingAddress = ((size_t)B->getStartingAddr()-((size_t)B));
  	delete B;

	glGenVertexArrays(1, &vertexArrayObject);
	glBindVertexArray(vertexArrayObject);

	//setup for the vertex and normal arrays.
	glGenBuffers(4, &vertexBufferObjects[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[VBuffer]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec4) + vertexNormals.size()*sizeof(glm::vec4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), vertexNormals.size()*sizeof(glm::vec4), vertexNormals.data());

	GLuint vPos = glGetAttribLocation(Renderer.getShaderProgram(), "vPosition");
	glEnableVertexAttribArray(vPos);
	glVertexAttribPointer(vPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint vNormPos = glGetAttribLocation(Renderer.getShaderProgram(), "vNormal");
	glEnableVertexAttribArray(vNormPos);
	glVertexAttribPointer(vNormPos, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Start with binding and setup of textures.
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[TBuffer]);

	glBufferData(GL_ARRAY_BUFFER, textureCoords.size() * sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size() * sizeof(glm::vec2), textureCoords.data());

	GLuint texCoordinatePos = glGetAttribLocation(Renderer.getShaderProgram(), "textureCoordinatesIn");
	glEnableVertexAttribArray(texCoordinatePos);
	glVertexAttribPointer(texCoordinatePos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//model matrices
	GLuint modelMatrixPos = glGetAttribLocation(Renderer.getShaderProgram(), "modelMatrix");

	//Bind the buffer for drawing Objects
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(baseObj), nullptr, GL_DYNAMIC_DRAW);

	GLsizei vec4Size = sizeof(glm::vec4);

    glEnableVertexAttribArray(modelMatrixPos); 
    glVertexAttribPointer(modelMatrixPos, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+0));
    glEnableVertexAttribArray(modelMatrixPos+1); 
    glVertexAttribPointer(modelMatrixPos+1, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+(vec4Size)));
    glEnableVertexAttribArray(modelMatrixPos+2); 
    glVertexAttribPointer(modelMatrixPos+2, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+(vec4Size*2)));
    glEnableVertexAttribArray(modelMatrixPos+3); 
    glVertexAttribPointer(modelMatrixPos+3, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+(vec4Size*3)));
    glVertexAttribDivisor(modelMatrixPos, 1);
    glVertexAttribDivisor(modelMatrixPos+1, 1);
    glVertexAttribDivisor(modelMatrixPos+2, 1);
    glVertexAttribDivisor(modelMatrixPos+3, 1);

    GLuint texPosPos = glGetAttribLocation(Renderer.getShaderProgram(), "texPosIn");
    glEnableVertexAttribArray(texPosPos); 
    glVertexAttribPointer(texPosPos, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+sizeof(glm::mat4)));
    glVertexAttribDivisor(texPosPos, 1);

    GLuint ColorPos = glGetAttribLocation(Renderer.getShaderProgram(), "iColor");
    glEnableVertexAttribArray(ColorPos); 
    glVertexAttribPointer(ColorPos, 3, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+sizeof(glm::mat4)+sizeof(glm::vec2)));
    glVertexAttribDivisor(ColorPos, 1);
    
    GLuint texSizePos = glGetAttribLocation(Renderer.getShaderProgram(), "texSizeIn");
    glEnableVertexAttribArray(texSizePos); 
    glVertexAttribPointer(texSizePos, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(listStartingAddress+sizeof(glm::mat4)+sizeof(glm::vec2)+sizeof(glm::vec3)));
    glVertexAttribDivisor(texSizePos, 1);


	GLuint TotalTexSizePos = glGetUniformLocation(Renderer.getShaderProgram(), "TotalTexSize");
	glUniform1i(TotalTexSizePos, (int)NUMTEX);

	GLuint samplePos = glGetUniformLocation(Renderer.getShaderProgram(), "textureSample");
	glUniform1i(samplePos, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBufferObjects[ElementBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());

	addBlocks();
}

void InstancedObject::drawOBJ(){
	if(blockList.size() > 1){
		glBindVertexArray(vertexArrayObject);
		Update();
		glDrawElementsInstanced(drawType, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), blockList.size());
	}
}

void InstancedObject::Update(){
	GLuint samplePos = glGetUniformLocation(Renderer.getShaderProgram(), "textureSample");
	glUniform1i(samplePos, textureSheet);


	//if(changed == true){
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, (blockList.size()*sizeof(Block)), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, (blockList.size()*sizeof(Block)), blockList.data());
	changed = false;
	//}
}


void InstancedObject::setVertexArrayObject(GLuint VAO){
	vertexArrayObject = VAO;
}

GLuint InstancedObject::getVertexArrayObject(){
	return vertexArrayObject;
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

Block & InstancedObject::addBlocks(){
	blockList.emplace_back();
	changed = true;
	return blockList[blockList.size()-1];
}

Block & InstancedObject::addBlocks(Block & b){
	blockList.push_back(b);
	changed = true;
	return blockList[blockList.size()-1];
}


void InstancedObject::setBlockList(std::vector<Block>& B){
	blockList = B;
	changed = true;
}	

void InstancedObject::removeBlock(int blockPos){
	//glm::vec3 pos = blockList[blockPos].getPos();
	//std::cerr << "Inside Address of OBJ: " << this << std::endl;
	//std::cerr << "Address of blockList: " << &blockList << std::endl;
	//std::cerr << "Address of block Removed: " << &(blockList[blockpos]) << std::endl;

	if((blockList.end()-1) != (blockList.begin()+blockPos)){
		std::iter_swap(blockList.begin()+blockPos, blockList.end()-1);
		(blockList.begin()+blockPos)->setID((blockList.end()-1)->getID());

		glm::vec3 bpos = (blockList.begin()+blockPos)->getPos();
		glm::vec2 chunk = glm::vec2(floor(bpos.x/(BLOCKSCALE*CHUNKDIMS)),floor(bpos.z/(BLOCKSCALE*CHUNKDIMS)));
		glm::vec3 relpos = glm::vec3(((bpos.x)/BLOCKSCALE)-(chunk.x*CHUNKDIMS),bpos.y/BLOCKSCALE, ((bpos.z)/BLOCKSCALE)-(chunk.y*CHUNKDIMS));
		for(int i=0; i<World.size(); i++){
			if(World[i]->getPos() == chunk){
				World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x].second = blockPos;	
				break;
			}
		}
	}

	blockList.erase(blockList.end()-1);
	changed = true;
	//std::cerr << "\tStart: " << blockPos << "\n\tEnd: " << blockList.size() << std::endl;
}

void InstancedObject::clearBlocks(){
	blockList.clear();
	changed = true;
}

std::vector<Block>& InstancedObject::getBlocks(){
	return blockList;
}

Block * InstancedObject::getBlocks(int index){
	if(index >= blockList.size() || index <= 0)
		return nullptr;
	return &(blockList[index]);	
}

int InstancedObject::getBlocksSize(){
	return blockList.size();
}

GLenum InstancedObject::getDrawType(){
	return drawType;
}

void InstancedObject::setDrawType(GLenum t){
	drawType = t;
}

void InstancedObject::setTexSheet(std::string t){
	textureSheet = textureNamesFind(t);
}

unsigned int InstancedObject::getTexSheet(){
	return textureSheet;
}

void InstancedObject::setMID(unsigned int id){
	modelID = id;
}

unsigned int InstancedObject::getMID(){
	return modelID;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	Renderer.setOBJUniforms();
	std::vector<IOBJ *> Objects = Renderer.getIObjectList();
	for(int i=0; i<Objects.size(); i++){
		Objects[i]->drawOBJ();
	}	


	Renderer.setGuiUniforms();
	std::vector<Window> windows = Renderer.getWindows();
	for(int i=0; i<windows.size(); i++){
		//std::string hid = (windows[i].getHidden())?"Hidden":"Showing";
		//std::cout << "Window is: " << hid << std::endl; 
		if(windows[i].getHidden() != true)
			windows[i].Draw();
	}

	std::vector<Window> swindows = Renderer.getSWindows();
	for(int i=0; i<swindows.size(); i++){
		//std::string hid = (windows[i].getHidden())?"Hidden":"Showing";
		//std::cout << "Window is: " << hid << std::endl; 
		if(swindows[i].getHidden() != true)
			swindows[i].Draw();
	}



	baseItem * b = State.getHItem();
	if(b!=nullptr){
		b->setTMatrix(glm::rotate(b->getTMatrix(Rotate), PI/10 ,glm::vec3(0.0f,1.0f,0.0f)), Rotate);
		b->Draw();
	}

	sf::Vector2u windowSize = mainWindow.getSize();
	if(State.getState() == Running){
		mainWindow.setMouseCursorVisible(false);
		sf::Mouse::setPosition(sf::Vector2i(windowSize.x/2, windowSize.y/2), mainWindow);
	}else{
		mainWindow.setMouseCursorVisible(true);
	}
	mainWindow.display();
}
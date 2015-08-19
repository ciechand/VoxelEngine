#include "../Headers/CryonyxBase.h"

//Initializer for the GameState Object
GameState::GameState(){
	state = Loading;
	mousePos = new float[2];
	mousePos[0] = 0.5f;
	mousePos[1] = 0.5f;
}

//getter and setter for the mousepos
void GameState::setMousePos(float x, float y){
	mousePos[0] = x;
	mousePos[1] = y;
}

float * GameState::getMousePos(){
	return mousePos;
}

//Function to change the state of the game, uses the Enum table in CryonyxUtil.h
void GameState::SetState(int newState){
	state = newState;
}

Object::Object(){
	renderer = NULL;
}

Object::Object(char * objFileName, char * texFileName){
	renderer = new objr(objFileName, texFileName);
}


int * GameOptions::getWindowPos(){
	return windowPos;
}
void GameOptions::setWindowPos(int x, int y){
	windowPos[0] = x;
	windowPos[1] = y;
}
float * GameOptions::getProjVar(){
	return projVar;
}
void GameOptions::setProjVar(float pov, float aspect, float nearPlane, float farPlane){
	projVar[0] = pov;
	projVar[1] = aspect;
	projVar[2] = nearPlane;
	projVar[3] = farPlane;
}
GameOptions::GameOptions(){

}
GameOptions::GameOptions(int posx, int posy, float pov, float aspect, float np, float fp){
	windowPos[0] = posx;
	windowPos[1] = posy;
	projVar[0] = pov;
	projVar[1] = aspect;
	projVar[2] = np;
	projVar[3] = fp;
}

void GameRenderer::LoadShaderProgram(char * vShader, char * fShader){
	shaderProgram = createShadersProgram(vShader, fShader);
	glUseProgram(shaderProgram);
}

GLuint GameRenderer::getShaderProgram(){
	return shaderProgram;
}

GameRenderer::GameRenderer(){
	modelMatrices.push_back(glm::mat4());
	modelMatrices.push_back(glm::mat4());
	modelMatrices.push_back(glm::mat4());
	modelMatrices.push_back(glm::mat4());
	viewMatrix = glm::mat4();
	projection = glm::mat4();

}

void InitOpenGL(){
	glEnable(GL_CULL_FACE);
	Renderer.LoadShaderProgram("./src/Sources/vShader.glsl", "./src/Sources/fShader.glsl");
	Renderer.Objects.push_back(new Object("./Assets/Blocks/cube.obj","./Assets/Blocks/cube.bmp"));
	for (uint i = 0; i < Renderer.Objects.size(); i++){
		InitOBJ(Renderer.Objects[i]);
	}
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0,1.0,1.0,1.0);
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
	glutSetCursor(GLUT_CURSOR_NONE);

}

void InitOBJ(class Object * obj){
	obj->renderer->ReadOBJFile();
	glGenVertexArrays(1, &obj->renderer->vertexArray);
	glBindVertexArray(obj->renderer->vertexArray);

	//setup for the vertex and normal arrays.
	glGenBuffers(2, obj->renderer->vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, obj->renderer->vertexBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, obj->renderer->vertices.size() * sizeof(glm::vec4) + obj->renderer->vertexNormals.size()*sizeof(glm::vec4), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, obj->renderer->vertices.size()*sizeof(glm::vec4), &obj->renderer->vertices[0]);
	glBufferSubData(GL_ARRAY_BUFFER, obj->renderer->vertices.size()*sizeof(glm::vec4), obj->renderer->vertexNormals.size()*sizeof(glm::vec4), &obj->renderer->vertexNormals[0]);

	obj->renderer->vertexAttribs.push_back(glGetAttribLocation(Renderer.getShaderProgram(), "vPosition"));
	glEnableVertexAttribArray(obj->renderer->vertexAttribs[Position]);
	glVertexAttribPointer(obj->renderer->vertexAttribs[Position], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	obj->renderer->vertexAttribs.push_back(glGetAttribLocation(Renderer.getShaderProgram(), "vNormal"));
	glEnableVertexAttribArray(obj->renderer->vertexAttribs[Normal]);
	glVertexAttribPointer(obj->renderer->vertexAttribs[Normal], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(obj->renderer->vertices.size() * sizeof(glm::vec4)));

	//Start with binding and setup of textures.
	glBindBuffer(GL_ARRAY_BUFFER, obj->renderer->vertexBuffer[1]);

	glBufferData(GL_ARRAY_BUFFER, obj->renderer->textureCoords.size() * sizeof(glm::vec2) + obj->renderer->textureCoords.size()*sizeof(glm::vec2), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, obj->renderer->textureCoords.size()*sizeof(glm::vec2), &obj->renderer->textureCoords[0]);

	obj->renderer->vertexUniforms.push_back(glGetUniformLocation(Renderer.getShaderProgram(), "textureSample"));

	//THIS MAY NEED TO BE CHANGED IF YOU END UP USING MORE THAN ONE UNIFORM LIKE THIS
	glUniform1i(obj->renderer->vertexUniforms[0], 0);

	obj->renderer->vertexAttribs.push_back(glGetAttribLocation(Renderer.getShaderProgram(), "tex"));
	glEnableVertexAttribArray(obj->renderer->vertexAttribs[Texture]);
	glVertexAttribPointer(obj->renderer->vertexAttribs[Texture], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));


	/*TEXTURE CODE WILL GO HERE*/
	glGenTextures(1, &obj->renderer->vertexBuffer[2]);
	glBindTexture(GL_TEXTURE_2D, obj->renderer->vertexBuffer[2]);
	glActiveTexture(obj->renderer->vertexBuffer[2]);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glGenBuffers(1, &obj->renderer->elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->renderer->elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->renderer->indices.size() * sizeof(GLuint), NULL, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, obj->renderer->indices.size() * sizeof(GLuint), &obj->renderer->indices[0]);
	obj->renderer->loadBMP();
}
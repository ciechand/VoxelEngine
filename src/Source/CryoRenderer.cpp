#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoUtil.hpp"

RenderController ShaderController;
CameraController Camera;

VoxelColor GlobalColor = Red;

//const int BlockColors[16] = {0x000000, 0xff0000, 0x800000, 0xff0084, 0x9c00ff, 0x000080, 0x0000ff, 0x00baff, 0x00ffcc, 0x00ff30, 0x009900, 0x999900, 0xfcff00, 0xa56900, 0xffa200, 0xff8900};
const glm::vec3 BlockColors[16] = {glm::vec3(255.0f,255.0f,255.0f),glm::vec3(255.0f,0.0f,0.0f),glm::vec3(128.0f,0.0f,0.0f),
									glm::vec3(255.0f,119.0f,173.0f),glm::vec3(255.0f,0.0f,102.0f), glm::vec3(156.0f,0.0f,255.0f),
									glm::vec3(0.0f,0.0f,128.0f),glm::vec3(0.0f,0.0f,255.0f),glm::vec3(0.0f,186.0f,255.0f),
									glm::vec3(0.0f,255.0f,204.0f),glm::vec3(0.0f,255.0f,48.0f),glm::vec3(0.0f,153.0f,0.0f),
									glm::vec3(252.0f,255.0f,0.0f),glm::vec3(165.0f,105.0f,0.0f),glm::vec3(255.0f,204.0f,0.0f),
									glm::vec3(255.0f,137.0f,0.0f)};

//Const Global Array for the different vertexes of a cube. These Can be accessed with the enum CubeVertices.
const glm::vec3 CubeVerts[8] = {
glm::vec3(-HALFSIZE,HALFSIZE,-HALFSIZE),glm::vec3(HALFSIZE,HALFSIZE,-HALFSIZE),
glm::vec3(-HALFSIZE,HALFSIZE, HALFSIZE),glm::vec3(HALFSIZE,HALFSIZE,HALFSIZE),
glm::vec3(-HALFSIZE,-HALFSIZE,-HALFSIZE),glm::vec3(HALFSIZE,-HALFSIZE,-HALFSIZE),
glm::vec3(-HALFSIZE,-HALFSIZE,HALFSIZE),glm::vec3(HALFSIZE,-HALFSIZE,HALFSIZE)
};

//Const GlobaL array for the different directional vectors, these can be accessed with the CubeFace Enum.
const glm::vec3 DirectionVectors[7] = {
glm::vec3(1.0,0.0,0.0), glm::vec3(-1.0,0.0,0.0),
glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,-1.0,0.0),
glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,0.0,0.0)
};


CameraController::CameraController(){
	projectionMatrix = glm::perspective(PI/2.25f, ((float)SCREENWIDTH)/SCREENHEIGHT, 0.01f, 100.0f);
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
}

CameraController::~CameraController(){

}

glm::vec3 CameraController::getCamPos(){
	return cameraPos;
}

void CameraController::setCamPos(glm::vec3 pos){
	cameraPos = pos;
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));}

glm::vec3 CameraController::getCamRot(){
	return cameraRotation;
}

void CameraController::setCamRot(glm::vec3 rot){
	cameraRotation = rot;
}

glm::vec3 CameraController::getCamAt(){
	return cameraAt;
}

void CameraController::setCamAt(glm::vec3 at){
	cameraAt = at;
}

glm::vec3 CameraController::getCamUp(){
	return cameraUp;
}

void CameraController::setCamUp(glm::vec3 up){
	cameraUp = up;
}

glm::vec3 CameraController::getCamFRot(){
	return cameraFinalRotation;
}

void CameraController::setCamFRot(glm::vec3 frot){
	cameraFinalRotation = frot;
}

glm::mat4 CameraController::getProjMatrix(){
	return projectionMatrix;
}

void CameraController::setProjMatrix(glm::mat4 vm){
	projectionMatrix = vm;
}

glm::mat4 CameraController::getViewMatrix(){
	return viewMatrix;
}

void CameraController::setViewMatrix(glm::mat4 vm){
	viewMatrix = vm;
}


RenderController::RenderController(){
	VertexArrayObject.assign(2,0);
}

RenderController::~RenderController(){
	glDeleteVertexArrays(2, &(VertexArrayObject[0]));
	glDeleteBuffers(5, &VertexBufferBase[0]);
	for(int i=0; i<baseMeshes.size(); i++){
		delete baseMeshes[i];
	}
	for(int i=0; i<programVariables.size(); i++){
		glDeleteProgram(programVariables[i]);
	}
}

void RenderController::initialize(){
	glGenVertexArrays(2, &(VertexArrayObject[0]));
	glBindVertexArray(VertexArrayObject[ShaderBase]);

	glGenBuffers(NUMBER_OF_LABELS, &VertexBufferBase[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LVertexBuffer]);

	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), vertices.data());
	
 	shaderPositions.emplace_back();
	shaderPositions[0] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexPosition");

	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LNormalsBuffer]);

	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size()*sizeof(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexNormals.size()*sizeof(glm::vec4), vertexNormals.data());

	shaderPositions.emplace_back();
	shaderPositions[1] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexNormal");
	glEnableVertexAttribArray(shaderPositions[1]);
	glVertexAttribPointer(shaderPositions[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Initializing the texture values for this mesh.
	shaderPositions.emplace_back();
	// glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LTextureBuffer]);
	// glBufferData(GL_ARRAY_BUFFER, textureCoords.size()*sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size()*sizeof(glm::vec2), textureCoords.data());

	// shaderPositions.emplace_back();
	// shaderPositions[2] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "TextureCoords");
	// glEnableVertexAttribArray(shaderPositions[2]);
	// glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
 
	//Initializing the Color values for each vertex of this mesh
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(glm::vec3), colors.data());

	shaderPositions.emplace_back();
	shaderPositions[3] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexColor");
	glEnableVertexAttribArray(shaderPositions[3]);
	glVertexAttribPointer(shaderPositions[3], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribDivisor(shaderPositions[3], 1);

	//Initialize which texture will be utilized for this particular Side.
	
	//Initialize the Model matrices
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data());


	shaderPositions.emplace_back();
	shaderPositions[4] = glGetAttribLocation(programVariables[ShaderBase], "modelMatrix");
    glEnableVertexAttribArray(shaderPositions[4]); 
    glVertexAttribPointer(shaderPositions[4], 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET(0));
    glVertexAttribDivisor(shaderPositions[4], 1);

    glEnableVertexAttribArray(shaderPositions[4]+1); 
    glVertexAttribPointer(shaderPositions[4]+1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET(sizeof(glm::vec4)));
	glVertexAttribDivisor(shaderPositions[4]+1, 1);

    glEnableVertexAttribArray(shaderPositions[4]+2); 
    glVertexAttribPointer(shaderPositions[4]+2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4)*2)));
    glVertexAttribDivisor(shaderPositions[4]+2, 1);

    glEnableVertexAttribArray(shaderPositions[4]+3); 
    glVertexAttribPointer(shaderPositions[4]+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4)*3)));
	glVertexAttribDivisor(shaderPositions[4]+3, 1);
    

    //Initializing the instances to  drawing the Mesh.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferBase[LIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);


	//Set the vertexArray
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glGenBuffers(NUMBER_OF_LABELS, &VertexBufferShadow[0]);

	//Initialize shadow buffers
	glGenFramebuffers(1, &shadowBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);

	//Initialize depth texture
	glGenTextures(1, &shadowDepthTexture);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, windowSize.x, windowSize.y, 0 , GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0);

	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Assigning to shadow shader
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferShadow[LVertexBuffer]);

	//Initializing the vertexs for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), vertices.data());

	shaderPositions[0] = glGetAttribLocation(programVariables[ShaderShadow], "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initialize the Model matrices
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferShadow[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data());

	shaderPositions[4] = glGetAttribLocation(programVariables[ShaderShadow], "modelMatrix");
    glEnableVertexAttribArray(shaderPositions[4]); 
    glVertexAttribPointer(shaderPositions[4], 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET(0));
    glVertexAttribDivisor(shaderPositions[4], 1);

    glEnableVertexAttribArray(shaderPositions[4]+1); 
    glVertexAttribPointer(shaderPositions[4]+1, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET(sizeof(glm::vec4)));
	glVertexAttribDivisor(shaderPositions[4]+1, 1);
// 
    glEnableVertexAttribArray(shaderPositions[4]+2); 
    glVertexAttribPointer(shaderPositions[4]+2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4)*2)));
    glVertexAttribDivisor(shaderPositions[4]+2, 1);

    glEnableVertexAttribArray(shaderPositions[4]+3); 
    glVertexAttribPointer(shaderPositions[4]+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4)*3)));
	glVertexAttribDivisor(shaderPositions[4]+3, 1);

    //Initializing the instances to  drawing the Mesh.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferShadow[LIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

int RenderController::loadBaseMeshes(){
	vertices.clear();
	texCoords.clear();
	vertexNormals.clear();
	indices.clear();
	//loop through the assets folder here and load all the base meshes.
	baseMeshes.emplace_back(new BaseMesh("C:/Users/drago/Documents/Coding-Projects/Cryonyx/Assets/Models/Plane.obj"));
	baseMeshes[0]->setID(baseMeshes.size()-1);
	baseMeshes[0]->setVertStart(vertices.size());
	baseMeshes[0]->setTexStart(texCoords.size());
	baseMeshes[0]->setNormalStart(vertexNormals.size());
	baseMeshes[0]->loadModel();

	std::vector<glm::vec4> tempV;
	std::vector<glm::vec2> tempT;
	std::vector<glm::vec4> tempN;
	std::vector<unsigned int> tempI;
	for(int i=0; i<baseMeshes.size(); i++){
		tempV = baseMeshes[i]->getVertices();
		tempT = baseMeshes[i]->getTexCoords();
		tempN = baseMeshes[i]->getNormals();
		tempI = baseMeshes[i]->getIndices();
		vertices.insert(vertices.end(), tempV.begin(), tempV.end());
		texCoords.insert(texCoords.end(), tempT.begin(), tempT.end());
		vertexNormals.insert(vertexNormals.end(), tempN.begin(), tempN.end());
		indices.insert(indices.end(), tempI.begin(), tempI.end());
	}
}

void RenderController::reloadBuffers(){
	colors.clear();
	modelMatrix.clear();
	//for loop used to recompile all the basemesh colors and model matrices.
	std::vector<glm::vec3> tempC;
	std::vector<glm::mat4> tempM;
	for(int i=0; i<baseMeshes.size(); i++){
		tempC = baseMeshes[i]->getColors();
		tempM = baseMeshes[i]->getModels();

		colors.insert(colors.end(), tempC.begin(), tempC.end());
		modelMatrix.insert(modelMatrix.end(), tempM.begin(), tempM.end());

	}
	//Reload the things for the base shader
	glBindVertexArray(VertexArrayObject[ShaderBase]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data(), GL_DYNAMIC_DRAW);

	//reload all the things for the shadow maps
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferShadow[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

// Create a GLSL program object from vertex and fragment shader files
void RenderController::createNewShaderProgram(){
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shaders
    std::string vertShaderStr = readFileToString(vertexShaderPath.c_str());
    std::string fragShaderStr = readFileToString(fragmentShaderPath.c_str());

    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
   	if(DEBUGMODE)std::cerr << "Compiling vertex shader." << std::endl;
    glShaderSource(vertShader, 1, &vertShaderSrc, nullptr);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, nullptr, &vertShaderError[0]);
    if(logLength > 1) std::cerr << "vert Errors: " << &vertShaderError[0] << std::endl;

    // Compile fragment shader
    if(DEBUGMODE)std::cerr << "Compiling fragment shader." << std::endl;
    glShaderSource(fragShader, 1, &fragShaderSrc, nullptr);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, nullptr, &fragShaderError[0]);
    if(logLength > 1) std::cerr << "Frag Errors: " << &fragShaderError[0] << std::endl;

    std::cerr << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, nullptr, &programError[0]);
    if(logLength > 1) std::cerr << "Prog Errors: " <<&programError[0] << std::endl;


    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    int programVectorSize = programVariables.size();
    programVariables.emplace_back(program);
    glUseProgram(programVariables[programVectorSize]);
    glDetachShader(programVariables[programVectorSize],vertShader);
    glDetachShader(programVariables[programVectorSize],fragShader);
}

void RenderController::setShader(int index){
	glUseProgram(programVariables[index]);
	glBindVertexArray(VertexArrayObject[index]);
}

void RenderController::clearShader(){
	glUseProgram(0);
	glBindVertexArray(0);
}

std::string RenderController::getVertexShaderPath(){
	return vertexShaderPath;
}

void RenderController::setVertexShaderPath(std::string path){
	vertexShaderPath = path;
}

std::string RenderController::getFragmentShaderPath(){
	return fragmentShaderPath;
}

void RenderController::setFragmentShaderPath(std::string path){
	fragmentShaderPath = path;
}

GLuint RenderController::getProgramVariable(int index){
	return programVariables[index];
}

void RenderController::setProgramVariable(GLuint prog, int index){
	programVariables[index] = prog;
}


std::vector<BaseMesh *> &RenderController::getBaseMeshes(){
	return baseMeshes;
}

glm::vec2 RenderController::getWindowSize(){
	return windowSize;
}

void RenderController::setWindowSize(glm::vec2 ws){
	windowSize = ws;
}

bool RenderController::getChanged(){
	return sceneChanged;
}

void RenderController::setChanged(bool c){
	sceneChanged = c;
}

void RenderController::drawScene(){

	//if(sceneChanged == true){
		//Here we reload buffers if the scene changes
		reloadBuffers();
		//render the Shadow maps!
		/*for each light, draw elements instanced.*/

		//Here we setup the depth buffer things that only changes when the scene changes.
		//sceneChanged = false;
	//}
	glUseProgram(programVariables[ShaderShadow]);
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	//glm::mat4 tempProjMatrix = Camera.getProjMatrix();
	glm::mat4 tempProjMatrix =	glm::ortho<float>(-100,100,-100,100,-100,100);
	glm::mat4 tempViewMatrix = glm::lookAt(glm::vec3(10.0f,10.0f,10.0f),glm::vec3(0.0f,0.0f,0.0f),glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 VPMatrix = tempProjMatrix * tempViewMatrix;
	//glm::mat4 tempViewMatrix = Camera.getViewMatrix();
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderShadow], "VP"), 1, GL_FALSE, &(VPMatrix[0][0]));
	
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), modelMatrix.size());

	glUseProgram(programVariables[ShaderBase]); 
	glBindVertexArray(VertexArrayObject[ShaderBase]);

	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderBase], "DepthVP"), 1, GL_FALSE, &(VPMatrix[0][0]));
	tempProjMatrix = Camera.getProjMatrix();
	tempViewMatrix = Camera.getViewMatrix();
	VPMatrix = tempProjMatrix * tempViewMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderBase], "VP"), 1, GL_FALSE, &(VPMatrix[0][0]));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), modelMatrix.size());

	glBindVertexArray(0);
}
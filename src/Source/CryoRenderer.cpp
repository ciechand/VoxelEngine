#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoUtil.hpp"
#include "../Headers/CryoLighting.hpp"

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
glm::vec3(-HALFSIZE,-HALFSIZE,-HALFSIZE),glm::vec3(HALFSIZE,-HALFSIZE,-HALFSIZE),
glm::vec3(-HALFSIZE,HALFSIZE, HALFSIZE),glm::vec3(HALFSIZE,HALFSIZE,HALFSIZE),
glm::vec3(-HALFSIZE,-HALFSIZE,HALFSIZE),glm::vec3(HALFSIZE,-HALFSIZE,HALFSIZE)
};

//Const GlobaL array for the different directional vectors, these can be accessed with the CubeFace Enum.
const glm::vec3 DirectionVectors[7] = {
glm::vec3(1.0,0.0,0.0), glm::vec3(-1.0,0.0,0.0),
glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,-1.0,0.0),
glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,0.0,-1.0), glm::vec3(0.0,0.0,0.0)
};


CameraController::CameraController(){
	cameraAt = glm::vec3(10.0,0.0,0.0);
	projectionMatrix = glm::perspective(PI/2.25f, ((float)SCREENWIDTH)/SCREENHEIGHT, NEARPLANE, FARPLANE);
	//projectionMatrix = glm::perspective(PI/2.0f, 1.0f, NEARPLANE, FARPLANE);
	print4x4Matrix(projectionMatrix);
	viewMatrix = glm::lookAt(cameraPos, cameraAt, cameraUp);
}

CameraController::~CameraController(){

}

glm::vec3 CameraController::getCamPos(){
	return cameraPos;
}

void CameraController::setCamPos(glm::vec3 pos){
	cameraPos = pos;
	viewMatrix = glm::lookAt(cameraPos, cameraAt, cameraUp);
}

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
	std::cerr << "Render Controller" << std::endl;
	VertexArrayObject.assign(NUM_SHADERS,0);
	shaderPositions.assign(5,0);
	shadowProjMatrix = glm::perspective(PI/2.0f, (float)CUBEMAPSIDELENGTH/(float)CUBEMAPSIDELENGTH, NEARPLANE, FARPLANE);
}

RenderController::~RenderController(){
	glDeleteVertexArrays(NUM_SHADERS, &(VertexArrayObject[0]));
	glDeleteBuffers(NUMBER_OF_LABELS, &VertexBufferBase[0]);
	glDeleteBuffers(NUMBER_OF_LABELS, &VertexBufferShadow[0]);
	for(int i=0; i<meshBorder; i++){
		delete baseMeshes[i];
	}
	for(int i=0; i<programVariables.size(); i++){
		glDeleteProgram(programVariables[i]);
	}
}

void RenderController::initialize(){
	glGenVertexArrays(NUM_SHADERS, &(VertexArrayObject[0]));

	//Calling function to initialize the shadowShaders
	std::cerr << "Initializing Shadows" << std::endl;
	shadowPreInit();

	//Calling function to initizlize the base shaders.
	std::cerr << "Initializing Initial Render" << std::endl;
	firstRenderInit();

	//Calling function to initialize SSAO
	std::cerr << "Initializing SSAO" << std::endl;
	SSAOFragInit();

	//Calling functions to initilize SSAOBlur
	std::cerr << "Initializing BlurShaders" << std::endl;
	SSAOBlurInit();
	
	//Calling functions to initilize LightingPass
	std::cerr << "Initializing Lighting" << std::endl;
	lightingInit();

	glBindVertexArray(0);
}


void RenderController::shadowPreInit(){
	glUseProgram(programVariables[ShaderBase]);
	//Set the vertexArray
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glGenBuffers(NUMBER_OF_LABELS, &VertexBufferShadow[0]);

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

    glEnableVertexAttribArray(shaderPositions[4]+2); 
    glVertexAttribPointer(shaderPositions[4]+2, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4)*2)));
    glVertexAttribDivisor(shaderPositions[4]+2, 1);

    glEnableVertexAttribArray(shaderPositions[4]+3); 
    glVertexAttribPointer(shaderPositions[4]+3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), BUFFER_OFFSET((sizeof(glm::vec4)*3)));
	glVertexAttribDivisor(shaderPositions[4]+3, 1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferShadow[LIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	//Initialize shadow buffers
	glGenFramebuffers(1, &shadowBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glDrawBuffer(GL_NONE);

	//Initialize depth texture
	glGenTextures(1, &shadowDepthTexture);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowDepthTexture);
	glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT16, CUBEMAPSIDELENGTH, CUBEMAPSIDELENGTH, MAXLIGHTS*6, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0);
}

void RenderController::firstRenderInit(){
	glUseProgram(programVariables[ShaderBase]);
	glBindVertexArray(VertexArrayObject[ShaderBase]);

	glGenBuffers(NUMBER_OF_LABELS, &VertexBufferBase[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LVertexBuffer]);

	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), vertices.data());
	 
 	shaderPositions[0] = glGetAttribLocation(programVariables[ShaderBase], "VertexPosition");

	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LNormalsBuffer]);

	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size()*sizeof(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertexNormals.size()*sizeof(glm::vec4), vertexNormals.data());

	shaderPositions[1] = glGetAttribLocation(programVariables[ShaderBase], "VertexNormal");
	glEnableVertexAttribArray(shaderPositions[1]);
	glVertexAttribPointer(shaderPositions[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initializing the texture values for this mesh.
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LTextureBuffer]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, texCoords.size()*sizeof(glm::vec2), texCoords.data());

	shaderPositions[2] = glGetAttribLocation(programVariables[ShaderBase], "TextureCoords");
	glEnableVertexAttribArray(shaderPositions[2]);
	glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
 
	//Initializing the Color values for each vertex of this mesh
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(glm::vec3), colors.data());

	shaderPositions[3] = glGetAttribLocation(programVariables[ShaderBase], "VertexColor");
	glEnableVertexAttribArray(shaderPositions[3]);
	glVertexAttribPointer(shaderPositions[3], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initialize which texture will be utilized for this particular Side.
	
	//Initialize the Model matrices
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data());


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


	//Initialize the buffers that will be outputted to by the base shader.
	//Initialize the framebuffer
	glGenFramebuffers(1, &preRenderFrameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, preRenderFrameBuffer);
	
	//Attach Shadowmap to framebuffer
	glGenTextures(1, &baseRenderDepthTexture);
	glActiveTexture(GL_TEXTURE0 + 10);
	glBindTexture(GL_TEXTURE_2D, baseRenderDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SCREENWIDTH, SCREENHEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, baseRenderDepthTexture, 0);

	//Declaring and Defining the NormalBuffer texture
	glGenTextures(1, &NormalBuffer);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, NormalBuffer, 0);

	//Declaring and Defining the ColorBuffer texture
	glGenTextures(1, &ColorBuffer);
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, ColorBuffer, 0);
	
	//Declaring and Defining the PositionBuffer texture
	glGenTextures(1, &PositionBuffer);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, PositionBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, PositionBuffer, 0);
	
	//Declaring and Defining the TextureCoordBuffer texture
	glGenTextures(1, &TextureCoordBuffer);
	glActiveTexture(GL_TEXTURE0 + 9);
	glBindTexture(GL_TEXTURE_2D, TextureCoordBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, TextureCoordBuffer, 0);

	unsigned int bufferAttachments[4] = {GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1,GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3};
	glDrawBuffers(4,bufferAttachments);
}

void RenderController::SSAOFragInit(){
	glUseProgram(programVariables[ShaderSSAO]);
	glBindVertexArray(VertexArrayObject[ShaderSSAO]);

	glGenBuffers(2, &VertexBufferSSAO[0]);

	//Create the quadTexture vertices
	std::vector<glm::vec3> quadVertices = {glm::vec3(-1.0f,1.0f,0.0f),glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f),glm::vec3(1.0f,-1.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f),glm::vec3(-1.0f,-1.0f,0.0f)};

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferSSAO[0]); // Bind the vertex buffer
	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size()*sizeof(glm::vec3), quadVertices.data(), GL_DYNAMIC_DRAW);
	
 	shaderPositions[0] = glGetAttribLocation(programVariables[ShaderSSAO], "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Create the Quad Texture Values
	std::vector<glm::vec2> tempTexCoords = {glm::vec2(0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(1.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(0.0f,0.0f)};

	//Initializing the texture values for this mesh.
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferSSAO[1]);//bind the texture buffer.
	glBufferData(GL_ARRAY_BUFFER, tempTexCoords.size()*sizeof(glm::vec2), tempTexCoords.data(), GL_DYNAMIC_DRAW);

	shaderPositions[2] = glGetAttribLocation(programVariables[ShaderSSAO], "TextureCoords");
	glEnableVertexAttribArray(shaderPositions[2]);
	glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLint NormalBufferLoc = glGetUniformLocation(programVariables[ShaderSSAO], "NormalBuffer");
	glUniform1i(NormalBufferLoc, 4);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);

	GLint positionBufferLoc = glGetUniformLocation(programVariables[ShaderSSAO], "positionBuffer");
	glUniform1i(positionBufferLoc, 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, PositionBuffer);

	//Declaring and Defining the SSAOKernel texture
	glGenTextures(1, &SSAOKernel);
	GLint SSAOKernelLoc = glGetUniformLocation(programVariables[ShaderSSAO], "SSAOKernelMap");	
	glUniform1i(SSAOKernelLoc, 3);
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_1D, SSAOKernel);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16F, KERNELSIZE, 0, GL_RGB, GL_FLOAT, &(lightController.getSSAOKernelData())[0]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Declaring and Defining the SSAONoise texture
	glGenTextures(1, &SSAONoise);
	GLint SSAONoiseLoc = glGetUniformLocation(programVariables[ShaderSSAO], "SSAONoiseMap");	
	glUniform1i(SSAONoiseLoc, 2);
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, SSAONoise);
	int noiseDimensions = sqrt(NOISESIZE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, noiseDimensions, noiseDimensions, 0, GL_RGB, GL_FLOAT, &(lightController.getSSAONoiseData())[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenFramebuffers(1, &SSAOBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBuffer);

	//Declaring and Defining the SSAOOutput texture
	glGenTextures(1, &SSAOOutput);
	glActiveTexture(GL_TEXTURE0+8);
	glBindTexture(GL_TEXTURE_2D, SSAOOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOOutput, 0);

	unsigned int bufferAttachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1,bufferAttachments);
}

void RenderController::SSAOBlurInit(){
	glUseProgram(programVariables[ShaderBlur]);
	glBindVertexArray(VertexArrayObject[ShaderBlur]);

	glGenBuffers(2, &VertexBufferSSAO[0]);

	//Create the quadTexture vertices
	std::vector<glm::vec3> quadVertices = {glm::vec3(-1.0f,1.0f,0.0f),glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f),glm::vec3(1.0f,-1.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f),glm::vec3(-1.0f,-1.0f,0.0f)};

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferSSAO[0]); // Bind the vertex buffer
	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size()*sizeof(glm::vec3), quadVertices.data(), GL_DYNAMIC_DRAW);
	
 	shaderPositions[0] = glGetAttribLocation(programVariables[ShaderBlur], "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Create the Quad Texture Values
	std::vector<glm::vec2> tempTexCoords = {glm::vec2(0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(1.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(0.0f,0.0f)};

	//Initializing the texture values for this mesh.
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferSSAO[1]);//bind the texture buffer.
	glBufferData(GL_ARRAY_BUFFER, tempTexCoords.size()*sizeof(glm::vec2), tempTexCoords.data(), GL_DYNAMIC_DRAW);

	shaderPositions[2] = glGetAttribLocation(programVariables[ShaderBlur], "TextureCoords");
	glEnableVertexAttribArray(shaderPositions[2]);
	glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLint SSAOOutLoc = glGetUniformLocation(programVariables[ShaderBlur], "SSAOInput");
	glUniform1i(SSAOOutLoc, 8);
	glActiveTexture(GL_TEXTURE0 + 8);
	glBindTexture(GL_TEXTURE_2D, SSAOOutput);

	glGenFramebuffers(1, &SSAOBlurBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurBuffer);

	//Declaring and Defining the BlurOutput texture
	glGenTextures(1, &SSAOBlurOutput);
	glActiveTexture(GL_TEXTURE0+7);
	glBindTexture(GL_TEXTURE_2D, SSAOBlurOutput);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SCREENWIDTH, SCREENHEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, SSAOBlurOutput, 0);

	unsigned int bufferAttachments[1] = {GL_COLOR_ATTACHMENT0};
	glDrawBuffers(1,bufferAttachments);
}

void RenderController::lightingInit(){
	glUseProgram(programVariables[ShaderLighting]);
	glBindVertexArray(VertexArrayObject[ShaderLighting]);

	glGenBuffers(2, &VertexBufferSSAO[0]);

	//Create the quadTexture vertices
	std::vector<glm::vec3> quadVertices = {glm::vec3(-1.0f,1.0f,0.0f),glm::vec3(-1.0f,-1.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f),glm::vec3(1.0f,-1.0f,0.0f),glm::vec3(1.0f,1.0f,0.0f),glm::vec3(-1.0f,-1.0f,0.0f)};

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferSSAO[0]); // Bind the vertex buffer
	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size()*sizeof(glm::vec3), quadVertices.data(), GL_DYNAMIC_DRAW);
	
 	shaderPositions[0] = glGetAttribLocation(programVariables[ShaderLighting], "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Create the Quad Texture Values
	std::vector<glm::vec2> tempTexCoords = {glm::vec2(0.0f,1.0f),glm::vec2(0.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(1.0f,0.0f),glm::vec2(1.0f,1.0f),glm::vec2(0.0f,0.0f)};

	//Initializing the texture values for this mesh.
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferSSAO[1]);//bind the texture buffer.
	glBufferData(GL_ARRAY_BUFFER, tempTexCoords.size()*sizeof(glm::vec2), tempTexCoords.data(), GL_DYNAMIC_DRAW);

	shaderPositions[2] = glGetAttribLocation(programVariables[ShaderLighting], "TextureCoords");
	glEnableVertexAttribArray(shaderPositions[2]);
	glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLint ColorBufferLoc = glGetUniformLocation(programVariables[ShaderLighting], "ColorBuffer");
	glUniform1i(ColorBufferLoc, 6);
	glActiveTexture(GL_TEXTURE0 + 6);
	glBindTexture(GL_TEXTURE_2D, ColorBuffer);

	GLint SSAOBlurLoc = glGetUniformLocation(programVariables[ShaderLighting], "SSAOBlurOutput");
	glUniform1i(SSAOBlurLoc, 7);
	glActiveTexture(GL_TEXTURE0 + 7);
	glBindTexture(GL_TEXTURE_2D, SSAOBlurOutput);

	GLint NormalBufferLoc = glGetUniformLocation(programVariables[ShaderLighting], "NormalBuffer");
	glUniform1i(NormalBufferLoc, 4);
	glActiveTexture(GL_TEXTURE0 + 4);
	glBindTexture(GL_TEXTURE_2D, NormalBuffer);

	GLint positionBufferLoc = glGetUniformLocation(programVariables[ShaderLighting], "PositionBuffer");
	glUniform1i(positionBufferLoc, 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, PositionBuffer);

	GLint shadowMapLoc = glGetUniformLocation(programVariables[ShaderLighting], "shadowMap");
	glUniform1i(shadowMapLoc, 5);
	glActiveTexture(GL_TEXTURE0 + 5);
	glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, shadowDepthTexture);

	//Setting up the shader storage Buffers.
	glGenBuffers(1, &lightDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(Light), lightController.getLightData(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightDataSSBO); 

	// glGenBuffers(1, &lightMatrixSSBO); 
	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightMatrixSSBO);
	// glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(glm::mat4), lightController.getLightMatData(), GL_DYNAMIC_COPY);
	// glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, lightMatrixSSBO); 

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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
	std::vector<glm::vec3> tempC;
	std::vector<unsigned int> tempI;
	for(int i=0; i<baseMeshes.size(); i++){
		tempV = baseMeshes[i]->getVertices();
		tempT = baseMeshes[i]->getTexCoords();
		tempN = baseMeshes[i]->getNormals();
		tempI = baseMeshes[i]->getIndices();
		tempC = baseMeshes[i]->getColors();
		vertices.insert(vertices.end(), tempV.begin(), tempV.end());
		texCoords.insert(texCoords.end(), tempT.begin(), tempT.end());
		vertexNormals.insert(vertexNormals.end(), tempN.begin(), tempN.end());
		indices.insert(indices.end(), tempI.begin(), tempI.end());
		colors.insert(colors.end(), tempC.begin(), tempC.end());
	}
	meshBorder = baseMeshes.size();
}

void RenderController::addBaseMesh(BaseMesh* m){
	int id = baseMeshes.size();
	baseMeshes.push_back(m);
	baseMeshes[id]->setID(id);
	baseMeshes[id]->setVertStart(vertices.size());
	baseMeshes[id]->setTexStart(texCoords.size());
	baseMeshes[id]->setNormalStart(vertexNormals.size());
	
	std::vector<glm::vec4> tempV;
	std::vector<glm::vec2> tempT;
	std::vector<glm::vec4> tempN;
	std::vector<glm::vec3> tempC;
	std::vector<unsigned int> tempI;
	tempV = m->getVertices();
	tempT = m->getTexCoords();
	tempN = m->getNormals();
	tempI = m->getIndices();
	tempC = m->getColors();
	//For loop checking all the vertices against the rest of the vertices to optimmize the indices. blegh... maybe fine a way to optimize in the future.
	bool found = false;
	unsigned int index = 0;
	for(int i=0; i<tempI.size(); i++){
		found = false;
		index = tempI[i];
		for(int j=0; j<vertices.size(); j++){
			//std::cerr << "~~~~~~~~~~~~~~~~~~~~~" << std::endl;
			//std::cerr << "i: " << i <<  std::endl;
			//std::cerr << "tempVertex"<< index <<":\n\tX: " <<tempV[index].x << "\n\tY: " <<tempV[index].y << "\n\tZ: " <<tempV[index].z << std::endl;
			//std::cerr << "vertex"<< j <<":\n\tX: " <<vertices[j].x << "\n\tY: " <<vertices[j].y << "\n\tZ: " <<vertices[j].z << std::endl;
			//std::cerr << "tempNormal"<< index <<":\n\tX: " <<tempN[index].x << "\n\tY: " <<tempN[index].y << "\n\tZ: " <<tempN[index].z << std::endl;
			//std::cerr << "vertexNormal"<< j <<":\n\tX: " <<vertexNormals[j].x << "\n\tY: " <<vertexNormals[j].y << "\n\tZ: " <<vertexNormals[j].z << std::endl;
			if(tempV[index] == vertices[j] && tempT[index] == texCoords[j] && tempN[index] == vertexNormals[j] && tempC[index] == colors[j]){
				indices.emplace_back(j);
				found = true;
				break;
			}
			//std::cerr << "Found: False" <<  std::endl;
		}
		if(found == true) continue;
		unsigned int vSize = vertices.size();
		indices.emplace_back(vSize);
		vertices.emplace_back(tempV[index]);
		texCoords.emplace_back(tempT[index]);
		vertexNormals.emplace_back(tempN[index]);
		colors.emplace_back(tempC[index]);
	}

}

void RenderController::reloadBuffers(){
	modelMatrix.clear();
	//for loop used to recompile all the basemesh colors and model matrices.
	std::vector<glm::vec3> tempC;
	std::vector<glm::mat4> tempM;
	for(int i=0; i<baseMeshes.size(); i++){
		tempM = baseMeshes[i]->getModels();

		modelMatrix.insert(modelMatrix.end(), tempM.begin(), tempM.end());

	}
	//Reload the things for the base shader
	glBindVertexArray(VertexArrayObject[ShaderBase]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LVertexBuffer]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LNormalsBuffer]);
	glBufferData(GL_ARRAY_BUFFER, vertexNormals.size()*sizeof(glm::vec4), vertexNormals.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LTextureBuffer]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), texCoords.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data(), GL_DYNAMIC_DRAW);


	//reload all the things for the shadow maps
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferShadow[LVertexBuffer]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), vertices.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferShadow[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void RenderController::reloadShaderBuffers(){
	glBindVertexArray(VertexArrayObject[ShaderLighting]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(Light), lightController.getLightData(), GL_DYNAMIC_COPY);
	//GLvoid* pl = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//memcpy(pl, lightController.getLightData(), lightController.getNumLights()*sizeof(Light));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	// glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightMatrixSSBO);
	// glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(glm::mat4), lightController.getLightMatData(), GL_DYNAMIC_COPY);
	//GLvoid* pm = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//memcpy(pm, lightController.getLightMatData(), lightController.getNumLights()*sizeof(glm::mat4));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

// Create a GLSL program object from vertex and fragment shader files
void RenderController::createNewShaderProgram(){
	createNewShaderProgram(vertexShaderPath,fragmentShaderPath);
}
void RenderController::createNewShaderProgram(std::string vertexPath, std::string fragmentPath,std::string geometryPath){
	GLuint vertShader = compileShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragShader = compileShader(fragmentPath, GL_FRAGMENT_SHADER);
	GLuint geoShader = compileShader(geometryPath, GL_GEOMETRY_SHADER);

	if(vertShader == 0 && fragShader == 0) std::cerr << "BOTH SHADERS ARE INVALID" << std::endl;

    std::cerr << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    if(vertShader != 0) glAttachShader(program, vertShader);
    if(fragShader != 0) glAttachShader(program, fragShader);
    if(geoShader != 0) glAttachShader(program, geoShader);
    glLinkProgram(program);

 	GLint result = GL_FALSE;
	int logLength;

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, nullptr, &programError[0]);
    if(logLength > 1) std::cerr << "Prog Errors: " <<&programError[0] << std::endl;


    if(vertShader != 0)glDeleteShader(vertShader);
    if(fragShader != 0) glDeleteShader(fragShader);
   if(geoShader != 0) glDeleteShader(geoShader);
    int programVectorSize = programVariables.size();
    programVariables.emplace_back(program);
    glUseProgram(programVariables[programVectorSize]);
    if(vertShader != 0) glDetachShader(programVariables[programVectorSize],vertShader);
    if(fragShader != 0) glDetachShader(programVariables[programVectorSize],fragShader);
    if(geoShader != 0) glDetachShader(programVariables[programVectorSize],geoShader);
}

GLuint RenderController::compileShader(std::string path, GLenum shaderType){
 	if (path.empty()) return 0;
 	GLuint shader = glCreateShader(shaderType);

   std::string shaderStr = readFileToString(path.c_str());
   const char *shaderSrc = shaderStr.c_str();

   GLint result = GL_FALSE;
	int logLength;

	std::string type;
	switch(shaderType){
		case GL_GEOMETRY_SHADER:
			type = "Geometry";
			break;
		case GL_VERTEX_SHADER:
			type = "Vertex";
			break;
		case GL_FRAGMENT_SHADER:
			type = "Fragment";
			break;
		default:
			type = "????";
			break;
	}
	if((DEBUGMODE == true))std::cerr << "Compiling " << type<<" shader." << std::endl;
    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    // Check vertex shader
    glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> shaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(shader, logLength, nullptr, &shaderError[0]);
    if(logLength > 1) std::cerr << "vert Errors: " << &shaderError[0] << std::endl;

    return shader;
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
	//Going to need more then one change variable, will need an enum of bools that define the flags for change.
	if(sceneChanged == true){
		//Here we reload buffers if the scene changes
		reloadShaderBuffers();
		reloadBuffers();
		//render the Shadow maps!
		//Here we setup the depth buffer things that only changes when the scene changes.
		//sceneChanged = false;
	}

	glUseProgram(programVariables[ShaderShadow]);
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glCullFace(GL_FRONT);

	glFramebufferTexture( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0);
	glClear(GL_DEPTH_BUFFER_BIT);

	glViewport(0,0,CUBEMAPSIDELENGTH, CUBEMAPSIDELENGTH);

	for(int i = 0; i<lightController.getNumLights(); i++){
		//Light * l = lightController.getLight(i);
		//std::cerr << "Light "<< i <<" Color: \n\tX: " << l->getColor().r << "\n\tY: " << l->getColor().g << "\n\tZ: " << l->getColor().b << std::endl;

		//glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0, i*6);

		glm::mat4 tempMatrix;
		for(int j=0; j<6; j++){
			tempMatrix = shadowProjMatrix * lightController.getMatrix(i,j);
			glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderShadow], ("shadowMatrices["+std::to_string(j)+"]").c_str()), 1, GL_FALSE, &(tempMatrix[0][0]));
		}
		glUniform1f(glGetUniformLocation(programVariables[ShaderShadow], "farPlane"), FARPLANE);
		glUniform1i(glGetUniformLocation(programVariables[ShaderShadow], "index"), i*6);
		glm::vec4 tempLightPos = lightController.getLight(i)->getPos();
		glUniform3f(glGetUniformLocation(programVariables[ShaderShadow], "lightPos"), tempLightPos[0],tempLightPos[1],tempLightPos[2]);

		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), 1);
	}

	glUseProgram(programVariables[ShaderBase]); 
	glBindVertexArray(VertexArrayObject[ShaderBase]);
	glCullFace(GL_BACK);

	glViewport(0,0,SCREENWIDTH, SCREENHEIGHT);

	glm::mat4 BaseProjMatrix = Camera.getProjMatrix();
	glm::mat4 BaseViewMatrix = Camera.getViewMatrix();

	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderBase], "P"), 1, GL_FALSE, &(BaseProjMatrix[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderBase], "V"), 1, GL_FALSE, &(BaseViewMatrix[0][0]));

	glBindFramebuffer(GL_FRAMEBUFFER, preRenderFrameBuffer);
	// glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0, 0);
	//glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), 1);

	glUseProgram(programVariables[ShaderSSAO]);
	glBindVertexArray(VertexArrayObject[ShaderSSAO]);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBuffer);
	//glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderSSAO], "P"), 1, GL_FALSE, &(BaseProjMatrix[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderSSAO], "V"), 1, GL_FALSE, &(BaseViewMatrix[0][0]));
	glUniform2f(glGetUniformLocation(programVariables[ShaderSSAO], "windowSize"), windowSize.x, windowSize.y);
	glUniform1ui(glGetUniformLocation(programVariables[ShaderSSAO], "kernelSize"), KERNELSIZE);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(programVariables[ShaderBlur]);
	glBindVertexArray(VertexArrayObject[ShaderBlur]);
	glBindFramebuffer(GL_FRAMEBUFFER, SSAOBlurBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glUseProgram(programVariables[ShaderLighting]);
	glBindVertexArray(VertexArrayObject[ShaderLighting]);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderSSAO], "P"), 1, GL_FALSE, &(BaseProjMatrix[0][0]));
	glUniform1f(glGetUniformLocation(programVariables[ShaderLighting], "farPlane"), FARPLANE);
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderLighting], "V"), 1, GL_FALSE, &(BaseViewMatrix[0][0]));
	glUniform1ui(glGetUniformLocation(programVariables[ShaderLighting], "numLights"), lightController.getNumLights());

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}
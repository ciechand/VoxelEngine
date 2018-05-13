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
	cameraAt = glm::vec3(23,28,21);
	projectionMatrix = glm::perspective(PI/2.25f, ((float)SCREENWIDTH)/SCREENHEIGHT, 0.1f, 100.0f);
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
	VertexArrayObject.assign(3,0);
}

RenderController::~RenderController(){
	glDeleteVertexArrays(3, &(VertexArrayObject[0]));
	glDeleteBuffers(5, &VertexBufferBase[0]);
	for(int i=0; i<meshBorder; i++){
		delete baseMeshes[i];
	}
	for(int i=0; i<programVariables.size(); i++){
		glDeleteProgram(programVariables[i]);
	}
}

void RenderController::initialize(){
	glGenVertexArrays(3, &(VertexArrayObject[0]));
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
	glVertexAttribPointer(shaderPositions[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initializing the texture values for this mesh.
	shaderPositions.emplace_back();
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LTextureBuffer]);
	glBufferData(GL_ARRAY_BUFFER, texCoords.size()*sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, texCoords.size()*sizeof(glm::vec2), texCoords.data());

	shaderPositions.emplace_back();
	shaderPositions[2] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "TextureCoords");
	glEnableVertexAttribArray(shaderPositions[2]);
	glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
 
	//Initializing the Color values for each vertex of this mesh
	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferBase[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(glm::vec3), colors.data());

	shaderPositions.emplace_back();
	shaderPositions[3] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexColor");
	glEnableVertexAttribArray(shaderPositions[3]);
	glVertexAttribPointer(shaderPositions[3], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

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


	//Setting up the shader storage Buffers.
	glGenBuffers(1, &lightDataSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(Light), lightController.getLightData(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, lightDataSSBO); 

	glGenBuffers(1, &lightMatrixSSBO); 
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightMatrixSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(glm::mat4), lightController.getLightMatData(), GL_DYNAMIC_COPY);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, lightMatrixSSBO); 

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	//Set the vertexArray
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glGenBuffers(NUMBER_OF_LABELS, &VertexBufferShadow[0]);

	glUseProgram(programVariables[ShaderBase]);

	//Initialize shadow buffers
	glGenFramebuffers(1, &shadowBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);

	//Initialize depth texture
	glGenTextures(1, &shadowDepthTexture);
	GLint shadowMapLoc = glGetUniformLocation(programVariables[ShaderBase], "shadowMap");
	glUniform1i(shadowMapLoc, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowDepthTexture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT16, windowSize.x, windowSize.y, 10, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Declaring and Defining the SSAOKernel texture
	glGenTextures(1, &SSAOKernel);
	GLint SSAOKernelLoc = glGetUniformLocation(programVariables[ShaderBase], "SSAOKernelMap");
	glUniform1i(SSAOKernelLoc, 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, SSAOKernel);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGB16F, KERNELSIZE, 0, GL_RGB, GL_FLOAT, &(lightController.getSSAOKernelData())[0]);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0);

	glDrawBuffer(GL_NONE);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Initialize default depth Texture, Hopefully eliminating the need to bind the first camera to the depth buffer.
/*	glGenTextures(1, &defaultDepthBuffer);
	GLint shadowMapDefault = glGetUniformLocation(programVariables[ShaderBase], "previousDepthBuffer");
	glUniform1i(shadowMapDefault, 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, defaultDepthBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, windowSize.x, windowSize.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, defaultDepthBuffer, 0);*/

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

    //Initializing the instances to  drawing the Mesh.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferShadow[LIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);

	glBindVertexArray(VertexArrayObject[ShaderPassThrough]);
	glGenBuffers(NUMBER_OF_LABELS, &VertexBufferPassThrough[0]);

	std::vector<glm::vec2> quadVertices = {
		glm::vec2(-1.0f, -1.0f),
		glm::vec2( 1.0f, -1.0f),
		glm::vec2(-1.0f,  1.0f),
		glm::vec2(-1.0f,  1.0f),
		glm::vec2( 1.0f, -1.0f),
		glm::vec2( 1.0f,  1.0f)
	};

	glBindBuffer(GL_ARRAY_BUFFER, VertexBufferPassThrough[LVertexBuffer]);
	glBufferData(GL_ARRAY_BUFFER, quadVertices.size()*sizeof(glm::vec2), quadVertices.data(), GL_DYNAMIC_DRAW);
	

	GLuint shaderPos = glGetAttribLocation(programVariables[ShaderPassThrough], "vertexPosition");
	glEnableVertexAttribArray(shaderPos);
	glVertexAttribPointer(shaderPos, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint texShaderPos = glGetUniformLocation(programVariables[ShaderPassThrough], "textureSample");

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
	glBindVertexArray(VertexArrayObject[ShaderBase]);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightDataSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(Light), lightController.getLightData(), GL_DYNAMIC_COPY);
	//GLvoid* pl = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//memcpy(pl, lightController.getLightData(), lightController.getNumLights()*sizeof(Light));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, lightMatrixSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER , lightController.getNumLights()*sizeof(glm::mat4), lightController.getLightMatData(), GL_DYNAMIC_COPY);
	//GLvoid* pm = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//memcpy(pm, lightController.getLightMatData(), lightController.getNumLights()*sizeof(glm::mat4));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);


	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
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
	//Going to need more then one change variable, will need an enum of bools that define the flags for change.
	if(sceneChanged == true){
		//Here we reload buffers if the scene changes
		reloadShaderBuffers();
		reloadBuffers();
		//render the Shadow maps!
		// /*for each light, draw elements instanced.*/
		//Here we setup the depth buffer things that only changes when the scene changes.
		//sceneChanged = false;
	}
	glUseProgram(programVariables[ShaderShadow]);
	glBindVertexArray(VertexArrayObject[ShaderShadow]);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glCullFace(GL_FRONT);
	glClear(GL_DEPTH_BUFFER_BIT);


	for(int i = 1; i<lightController.getNumLights(); i++){
		//Light * l = lightController.getLight(i);
		//std::cerr << "Light "<< i <<" Color: \n\tX: " << l->getColor().r << "\n\tY: " << l->getColor().g << "\n\tZ: " << l->getColor().b << std::endl;
	
		glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0, i);

		glm::mat4 tempLightMatrix = lightController.getMatrix(i);

		glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderShadow], "PV"), 1, GL_FALSE, &(tempLightMatrix[0][0]));

		glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), 1);
	}


	glm::mat4 BaseProjMatrix = Camera.getProjMatrix();
	glm::mat4 BaseViewMatrix = Camera.getViewMatrix();

	glUseProgram(programVariables[ShaderBase]); 
	glBindVertexArray(VertexArrayObject[ShaderBase]);
	glCullFace(GL_BACK);

	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderBase], "P"), 1, GL_FALSE, &(BaseProjMatrix[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(programVariables[ShaderBase], "V"), 1, GL_FALSE, &(BaseViewMatrix[0][0]));
	glUniform1ui(glGetUniformLocation(programVariables[ShaderBase], "numLights"), lightController.getNumLights());
	glUniform1ui(glGetUniformLocation(programVariables[ShaderBase], "kernelSize"), KERNELSIZE);
	glUniform2f(glGetUniformLocation(programVariables[ShaderBase], "windowSize"), windowSize.x, windowSize.y);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowBuffer);
	glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, shadowDepthTexture, 0, 0);

	//glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), 1);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//glViewport(0,0,SCREENWIDTH,SCREENHEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), 1);
/*
	if(SHADOWMAPDEBUG){
		glViewport(SCREENWIDTH-500, SCREENHEIGHT-500,500,500); 
		glUseProgram(programVariables[ShaderPassThrough]);
		glBindVertexArray(VertexArrayObject[ShaderPassThrough]);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glViewport(0, 0,SCREENWIDTH,SCREENHEIGHT);
	}*/

	glBindVertexArray(0);
}
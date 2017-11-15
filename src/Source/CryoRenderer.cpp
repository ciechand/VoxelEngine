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

//Start of definitions for BaseMesh Class!
BaseMesh::BaseMesh(std::string filename){
	modelFile = filename;
}

BaseMesh::~BaseMesh(){

}

bool BaseMesh::loadModel(){
	vertices.clear();
	normals.clear();
	texCoords.clear();
	indices.clear();
	//Make sure the object file has a name already assigned
	if (modelFile.empty()){
		std::cerr << "Have not set a modelFile, Please do so before trying to load an obj file again." << std::endl;
		return false;
	}
//std::cerr << "Loading in Asset as : " << modelFile << std::endl;
	//opening the OBJ file for reading
	FILE * objFile = fopen(modelFile.c_str(), "r");
	if (objFile == nullptr){
		perror("THAT FILE DOES NOT EXIST");
		exit(EXIT_FAILURE);
	}
	//creating a buffer to read the file into
	char fileBuffer[128];
	std::vector<GLuint> vertexIndices;
	std::vector<GLuint> normalIndices;
	std::vector<GLuint> textureIndices;
	while (fgets(fileBuffer, 128, objFile) != nullptr){
		//checking which identifier you are currently reading and assigning appropriately.
		char identifier[3];
		sscanf(fileBuffer, "%s", identifier);
		if (strcmp(identifier, "#") == 0){
			continue;
		} else if (strcmp(identifier, "v") == 0){
			glm::vec4 verts;
			sscanf(fileBuffer, "%*s %f %f %f", &verts.x, &verts.y, &verts.z);
			verts.w = 1.0;
			vertices.emplace_back(verts);
		} else if (strcmp(identifier, "vn") == 0){
			glm::vec4 norm;
			sscanf(fileBuffer, "%*s %f %f %f", &norm.x, &norm.y, &norm.z);
			norm.w = 0.0;
			normals.emplace_back(norm);
		} else if (strcmp(identifier, "vt") == 0){
			glm::vec2 tex;
			sscanf(fileBuffer, "%*s %f %f", &tex.x, &tex.y);
			texCoords.emplace_back(tex);
		} else if (strcmp(identifier, "f") == 0){
			GLuint indices[9];
			sscanf(fileBuffer, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5], &indices[6], &indices[7], &indices[8]);
			for (int j = 0; j < 3; j++){
				vertexIndices.push_back(indices[(3 * j)] - 1);
				normalIndices.push_back(indices[(3 * j) + 2] - 1);
				textureIndices.push_back(indices[(3 * j) + 1] - 1);
			}
		}else{
			perror("Incorrect Identifier!"); //WTF KINDA OBJ FILE ARE YOU READING?!
		}
	}

	fclose(objFile);
	std::map<vertexInfo, GLuint> indexMap;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> tex;

	//Fix all the indices so that it doesnt have to be done later.
	//This needs to be done since open gl only allows one index array for each object so, you need to reorganize the indices that are read in from the OBJ file.

	for (size_t i = 0; i < vertexIndices.size(); i++){
		struct vertexInfo v = {vertices[vertexIndices[i]], normals[normalIndices[i]], texCoords[textureIndices[i]]};
		bool exists = false;
		GLuint index = 0;
		//Checking for unique (vertex,normal,texture) triplets.
		std::map<vertexInfo, GLuint>::iterator it = indexMap.find(v);
		 
		if (it == indexMap.end()){
			exists = false;
		} else{
			index = it->second;
			exists = true;
		}
		if (exists){
			indices.emplace_back(index);
		} else{
			verts.push_back(vertices[vertexIndices[i]]);
			norms.push_back(normals[normalIndices[i]]);
			tex.push_back(texCoords[textureIndices[i]]);
			index = (GLuint)verts.size() - 1;
			indices.emplace_back(index);
			indexMap[v] = index;
		}
	};

	vertices.clear();
	normals.clear();
	texCoords.clear();
	//The vectors must be re-applied so that the order is correct in correspondence with the new index array.
	vertices = verts;
	normals = norms;
	texCoords = tex;
	//std::cerr << "Finnished Loading in Asset of : " << modelFile << std::endl;
}

unsigned int BaseMesh::getID(){
	return ID;
}

void BaseMesh::setID(unsigned int id){
	ID = id;
}

unsigned int BaseMesh::getVertStart(){
	return vertexStart;
}

void BaseMesh::setVertStart(unsigned int index){
	vertexStart = index;
}

unsigned int BaseMesh::getTexStart(){
	return texStart;
}

void BaseMesh::setTexStart(unsigned int index){
	texStart = index;
}

unsigned int BaseMesh::getNormalStart(){
	return normalStart;
}

void BaseMesh::setNormalStart(unsigned int index){
	normalStart = index;
}


std::vector<glm::vec4> BaseMesh::getVertices(){
	return vertices;
}

std::vector<glm::vec2> BaseMesh::getTexCoords(){
	return texCoords;
}

std::vector<glm::vec4> BaseMesh::getNormals(){
	return normals;
}

std::vector<unsigned int> BaseMesh::getIndices(){
	return indices;
}

void BaseMesh::addToColors(std::vector<glm::vec3>::iterator begining, std::vector<glm::vec3>::iterator ending){
	colors.insert(colors.end(), begining, ending);
}

void BaseMesh::addToColors(glm::vec3 color){
	colors.emplace_back(color);
}

std::vector<glm::vec3> BaseMesh::getColors(){
	return colors;
}

void BaseMesh::addToModels(std::vector<glm::mat4>::iterator begining, std::vector<glm::mat4>::iterator ending){
	modelMatrices.insert(modelMatrices.end(), begining, ending);
}

void BaseMesh::addToModels(glm::mat4 model){
	modelMatrices.emplace_back(model);
}

std::vector<glm::mat4> BaseMesh::getModels(){
	return modelMatrices;
}

//Start of MESH Class
Mesh::Mesh(){
}

Mesh::~Mesh(){

}

void Mesh::GenerateMesh(Block * parentBlock){
	if(meshIDs.size() != 0 || textureIDs.size() != 0 || colors.size()  != 0 || modelMatrix.size() != 0)
			std::cerr << "DONT TRY AND GENERATE A MESH WHEN THE PREVIOUS ONE HAS NOT BEEN CLEARED!!!" << std::endl;
	std::array<bool,6> tempActiveArr = parentBlock->getActiveSide();
	for(int i=0; i<6; i++){
		if(tempActiveArr[i] == true){
			GenerateCubeSide((CubeFace)i, parentBlock->getSideColor(i), parentBlock->getPosition());
		}
	}
}

void Mesh::GenerateCubeSide(CubeFace face, glm::vec3 c, glm::vec3 offset){
	//std::cerr << "Offset: \n\tX: " << offset.x << "\n\tY: " << offset.y << "\n\tZ: " << offset.z << std::endl; 
	glm::mat4 translated = glm::translate(glm::mat4(), DirectionVectors[face]-(DirectionVectors[face]*0.001f));
	glm::mat4 rotated;
	switch(face){
		case RightFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			addColorToMesh(c);
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(-90.0f),glm::vec3(0,1,0));
			addMatrixToMesh(translated*rotated);
			break;
		case LeftFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			addColorToMesh(c);
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(90.0f),glm::vec3(0,1,0));
			addMatrixToMesh(translated*rotated);
			break;
		case TopFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			addColorToMesh(c);
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(-90.0f),glm::vec3(1,0,0));
			addMatrixToMesh(translated*rotated);
			break;
		case BottomFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			addColorToMesh(c);
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(90.0f),glm::vec3(1,0,0));
			addMatrixToMesh(translated*rotated);
			break;
		case FrontFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			//No rotation needed
			addColorToMesh(c);
			rotated = glm::mat4();
			addMatrixToMesh(translated*rotated);
			break;
		case BackFace:
			addBaseToMesh(0);
			addTexIDToMesh(0);
			addColorToMesh(c);
			rotated = glm::rotate(glm::mat4(),TO_RADIANS(180.0f),glm::vec3(0,1,0));
			addMatrixToMesh(translated*rotated);
			break;
		default:
			std::cerr << "INVALID SIDE GENERATION ATTEMPT." << std::endl;
			break;
	}
}

void Mesh::addBaseToMesh(int baseID){
	meshIDs.emplace_back(baseID);
}

std::vector<int> Mesh::getBase(){
	return meshIDs;
}

int Mesh::getBase(unsigned int index){
	return meshIDs[index];
}

void Mesh::addTexIDToMesh(int texID){
	textureIDs.emplace_back(texID);
}

std::vector<int> Mesh::getTexID(){
	return textureIDs;
}

int Mesh::getTexID(unsigned int index){
	return textureIDs[index];
}

void Mesh::addColorToMesh(glm::vec3 c){
	colors.emplace_back(c);
}

std::vector<glm::vec3> Mesh::getColors(){
	return colors;
}

glm::vec3 Mesh::getColors(unsigned int index){
	return colors[index];
}

void Mesh::addMatrixToMesh(glm::mat4 mm){
	modelMatrix.emplace_back(mm);
}

std::vector<glm::mat4> Mesh::getMatrix(){
	return modelMatrix;
}

glm::mat4 Mesh::getMatrix(unsigned int index){
	return modelMatrix[index];
}

void Mesh::addMeshToRenderer(glm::mat4 objectMatrix){
	if(colors.size() == 0)
		std::cerr << "Trying to add a mesh with nothing in it." << std::endl;

	std::vector<BaseMesh *> controllerBases = ShaderController.getBaseMeshes();
	if(controllerBases.size() == 0)
		std::cerr << "attempting to add a mesh without any base meshes." << std::endl;

	for(int i=0; i<meshIDs.size(); i++){
		controllerBases[meshIDs[i]]->addToColors(colors[i]);
		controllerBases[meshIDs[i]]->addToModels(objectMatrix*modelMatrix[i]);
	}
}

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
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(ShaderBase), "viewMatrix"), 1, GL_FALSE, &(viewMatrix[0][0]));
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
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(ShaderBase), "projectionMatrix"), 1, GL_FALSE, &(projectionMatrix[0][0]));
}

glm::mat4 CameraController::getViewMatrix(){
	return viewMatrix;
}

void CameraController::setViewMatrix(glm::mat4 vm){
	viewMatrix = vm;
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(ShaderBase), "viewMatrix"), 1, GL_FALSE, &(viewMatrix[0][0]));
}


RenderController::RenderController(){

}

RenderController::~RenderController(){
	glDeleteVertexArrays(1, &VertexArrayObject);
	glDeleteBuffers(5, &VertexBuffer[0]);
	for(int i=0; i<baseMeshes.size(); i++){
		delete baseMeshes[i];
	}
	for(int i=0; i<programVariables.size(); i++){
		glDeleteProgram(programVariables[i]);
	}
}

void RenderController::initialize(){
	glGenVertexArrays(1, &VertexArrayObject);
	glBindVertexArray(VertexArrayObject);

	glGenBuffers(5, &VertexBuffer[0]);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LVertexBuffer]);

	//Initializing the vertex and normals for this particular mesh.
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4) + vertexNormals.size()*sizeof(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), vertexNormals.size()*sizeof(glm::vec4), vertexNormals.data());

	shaderPositions.emplace_back();
	shaderPositions[0] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	shaderPositions.emplace_back();
	shaderPositions[1] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexNormal");
	glEnableVertexAttribArray(shaderPositions[1]);
	glVertexAttribPointer(shaderPositions[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Initializing the texture values for this mesh.
	shaderPositions.emplace_back();
	// glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LTextureBuffer]);
	// glBufferData(GL_ARRAY_BUFFER, textureCoords.size()*sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size()*sizeof(glm::vec2), textureCoords.data());

	// shaderPositions.emplace_back();
	// shaderPositions[2] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "TextureCoords");
	// glEnableVertexAttribArray(shaderPositions[2]);
	// glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initializing the Color values for each vertex of this mesh
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(glm::vec3), colors.data());

	shaderPositions.emplace_back();
	shaderPositions[3] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "VertexColor");
	glEnableVertexAttribArray(shaderPositions[3]);
	glVertexAttribPointer(shaderPositions[3], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribDivisor(shaderPositions[3], 1);

	//Initialize which texture will be utilized for this particular Side.
	
	//Initialize the Model matrices
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data());


	shaderPositions.emplace_back();
	shaderPositions[4] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderBase), "modelMatrix");
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
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //Initializing the instances to  drawing the Mesh.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBuffer[LIndexBuffer]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

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

    glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(ShaderShadow), "projectionMatrix"), 1, GL_FALSE, &(Camera.getProjMatrix()[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(ShaderShadow), "viewMatrix"), 1, GL_FALSE, &(Camera.getViewMatrix()[0][0]));

	shaderPositions[0] = glGetAttribLocation(ShaderController.getProgramVariable(ShaderShadow), "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
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

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    glUseProgram(program);
    int programVectorSize = programVariables.size();
    programVariables.emplace_back(program);
    glDetachShader(programVariables[programVectorSize],vertShader);
    glDetachShader(programVariables[programVectorSize],fragShader);
    glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(programVectorSize), "projectionMatrix"), 1, GL_FALSE, &(Camera.getProjMatrix()[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(programVectorSize), "viewMatrix"), 1, GL_FALSE, &(Camera.getViewMatrix()[0][0]));
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
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LVertexBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBuffer[LIndexBuffer]);
	glUseProgram(programVariables[ShaderShadow]);
	//if(sceneChanged == true){
		//Here we reload buffers if the scene changes
		reloadBuffers();
		//render the Shadow maps!
		/*for each light, draw elements instanced.*/

		//Here we setup the depth buffer things that only changes when the scene changes.
		sceneChanged = false;
	//}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), modelMatrix.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
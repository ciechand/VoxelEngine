#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoUtil.hpp"

RenderController ShaderController;

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
	std::cerr << "Offset: \n\tX: " << offset.x << "\n\tY: " << offset.y << "\n\tZ: " << offset.z << std::endl; 
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


RenderController::RenderController(){
	projectionMatrix = glm::perspective(PI/2.25f, ((float)SCREENWIDTH)/SCREENHEIGHT, 0.01f, 100.0f);
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));

}

RenderController::~RenderController(){
	glDeleteVertexArrays(1, &VertexArrayObject);
	glDeleteBuffers(5, &VertexBuffer[0]);
	for(int i=0; i<baseMeshes.size(); i++){
		delete baseMeshes[i];
	}
	glDeleteProgram(programVariable);
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
	shaderPositions[0] = glGetAttribLocation(ShaderController.getProgramVariable(), "VertexPosition");
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	shaderPositions.emplace_back();
	shaderPositions[1] = glGetAttribLocation(ShaderController.getProgramVariable(), "VertexNormal");
	glEnableVertexAttribArray(shaderPositions[1]);
	glVertexAttribPointer(shaderPositions[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Initializing the texture values for this mesh.
	shaderPositions.emplace_back();
	// glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LTextureBuffer]);
	// glBufferData(GL_ARRAY_BUFFER, textureCoords.size()*sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, textureCoords.size()*sizeof(glm::vec2), textureCoords.data());

	// shaderPositions.emplace_back();
	// shaderPositions[2] = glGetAttribLocation(ShaderController.getProgramVariable(), "TextureCoords");
	// glEnableVertexAttribArray(shaderPositions[2]);
	// glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initializing the Color values for each vertex of this mesh
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size()*sizeof(glm::vec3), colors.data());

	shaderPositions.emplace_back();
	shaderPositions[3] = glGetAttribLocation(ShaderController.getProgramVariable(), "VertexColor");
	glEnableVertexAttribArray(shaderPositions[3]);
	glVertexAttribPointer(shaderPositions[3], 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glVertexAttribDivisor(shaderPositions[3], 1);

	//Initialize which texture will be utilized for this particular Side.
	
	//Initialize the Model matrices
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0,  modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data());


	shaderPositions.emplace_back();
	shaderPositions[4] = glGetAttribLocation(ShaderController.getProgramVariable(), "modelMatrix");
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

	// for(int i=0; i<indices.size(); i++){
	// 	std::cerr << "index " << i << ":" << indices[i] << std::endl; 
	// }
	// for(int i=0; i<vertices.size(); i++){
	// 	std::cerr << "Vertices " << i << ": \n\tX: " << vertices[i].x << "\n\tY: " <<vertices[i].y << "\n\tZ: " <<vertices[i].z << std::endl; 
	// 	std::cerr << "vertexNormals " << i << ": \n\tX: " << vertexNormals[i].x << "\n\tY: " <<vertexNormals[i].y << "\n\tZ: " <<vertexNormals[i].z << std::endl; 
	// }
	//for(int i=0; i<colors.size(); i++){
	//	std::cerr << "colors " << i << ": \n\tX: " << colors[i].x << "\n\tY: " <<colors[i].y << "\n\tZ: " <<colors[i].z << std::endl; 
		//print4x4Matrix(modelMatrix[i]);
	//}
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LColorArray]);
	glBufferData(GL_ARRAY_BUFFER, colors.size()*sizeof(glm::vec3), colors.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LModelMatrices]);
	glBufferData(GL_ARRAY_BUFFER, modelMatrix.size()*sizeof(glm::mat4), modelMatrix.data(), GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// Create a GLSL program object from vertex and fragment shader files
void RenderController::createShaderProgram(){
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
    programVariable = program;
    glDetachShader(programVariable,vertShader);
    glDetachShader(programVariable,fragShader);
    glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(), "projectionMatrix"), 1, GL_FALSE, &(projectionMatrix[0][0]));
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(), "viewMatrix"), 1, GL_FALSE, &(viewMatrix[0][0]));
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

GLuint RenderController::getProgramVariable(){
	return programVariable;
}

void RenderController::setProgramVariable(GLuint prog){
	programVariable = prog;
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

glm::vec3 RenderController::getCamPos(){
	return cameraPos;
}

void RenderController::setCamPos(glm::vec3 pos){
	cameraPos = pos;
	viewMatrix = glm::lookAt(cameraPos, glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(), "viewMatrix"), 1, GL_FALSE, &(viewMatrix[0][0]));
}

glm::vec3 RenderController::getCamRot(){
	return cameraRotation;
}

void RenderController::setCamRot(glm::vec3 rot){
	cameraRotation = rot;
}

void RenderController::setViewMatrix(glm::mat4 mat){
	viewMatrix = mat;
	glUniformMatrix4fv(glGetUniformLocation(ShaderController.getProgramVariable(), "viewMatrix"), 1, GL_FALSE, &(viewMatrix[0][0]));
}

void RenderController::drawScene(){
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LVertexBuffer]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBuffer[LIndexBuffer]);

	glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0), modelMatrix.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
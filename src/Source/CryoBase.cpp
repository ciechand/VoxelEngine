#include "../Headers/CryoBase.hpp"

GameState::GameState(){
	curState = Loading;
	camPos[0] = 0.0f;
	camPos[1] = 0.0f;
}

GameState::GameState(unsigned char cs){
	curState = cs;
	camPos[0] = 0.0f;
	camPos[1] = 0.0f;
}

void GameState::setState(unsigned char cs){
	curState = cs;
}

unsigned char GameState::getState(){
	return curState;
}

void GameState::setCamPos(float x, float y){
	camPos[0] = x;
	camPos[1] = y;
}

float * GameState::getCamPos(){
	return camPos;
}

baseObj::baseObj(){
	TransformMatrices = std::vector<glm::mat4>(3, glm::mat4(1.0));
	//TransformMatrices[Scale] = glm::scale(glm::mat4(1.0), glm::vec3(BLOCKSCALE,BLOCKSCALE,BLOCKSCALE));
	modelIdentifier = 0;
	textureIdentifier = 0;
}

void baseObj::setTMatrix(int index, glm::mat4 matrix){
	TransformMatrices[index] = matrix;
}

glm::mat4 baseObj::getTMatrix(int index){
	return TransformMatrices[index];
}

std::vector<glm::mat4> baseObj::getTMatrixVec(){
	return TransformMatrices;
}

void baseObj::setMID(int id){
	modelIdentifier = id;
}

int baseObj::getMID(){
	return modelIdentifier;
}

void baseObj::setTID(int id){
	textureIdentifier = id;
}

int baseObj::getTID(){
	return textureIdentifier;
}

GameOptions::GameOptions(){

	using namespace boost::filesystem;
	boost::filesystem::path p ("./Assets/");

	texturePaths.clear();
	modelPaths.clear();

	if(exists(p)){
		if(is_directory(p)){
			for(directory_entry& d : directory_iterator(p)){
				if(is_directory(d)){
					for(directory_entry& d2 : directory_iterator(d)){
						if(is_regular_file(d2)){
							if(d2.path().string().compare(d2.path().string().size()-4,4,".png") == 0 ||d2.path().string().compare(d2.path().string().size()-4,4,".BMP") == 0 ){
								texturePaths.push_back(d2.path().string());
							}else if(d2.path().string().compare(d2.path().string().size()-4,4,".obj") == 0 ){
								modelPaths.push_back(d2.path().string());
							}else{
								std::cout << "NOT ONE OF THE TWO VALID TYPES!!!" << std::endl;
							}
						}
					}
				}
			}
		}else{
			printf("Path provided is not a directory.\n");
		}
	}else{
		printf("Path does not exist.\n");
	}

	projVar[0] = 1.04f;
	projVar[1] = 1.0f;
	projVar[2] = 0.01f;
	projVar[3] = 200.0f;

	camPos[0] = 5.0f;
	camPos[1] = 2.0f;
	camPos[2] = 20.0f;
}

GameOptions::GameOptions(const char * optionsFileName){
	GameOptions();
}

void GameOptions::Initialize(){

	std::cout << "loading "<< modelPaths.size() <<" Models: " << std::endl;
	for(int i=0; i<modelPaths.size(); i++){
		IOBJ * tempOBJ = new IOBJ();
		readOBJFile(tempOBJ, modelPaths[i].c_str());
		tempOBJ->Initialize();
		Renderer.addToIObjectList(tempOBJ);
	} 

	std::cout << "Finished Loading Models" << std::endl << "Loading Textures: " << std::endl;
	for(int i=0; i<texturePaths.size(); i++){
		std::cout << texturePaths[i] << " Loaded" << std::endl;
		Renderer.addToTextureList(texturePaths[i].c_str());
	} 
	std::cout << "Finished Loading Textures" <<  std::endl;
}

void GameOptions::setProjVars(float * vars){
	projVar[0] = vars[0];
	projVar[1] = vars[1];
	projVar[2] = vars[2];
	projVar[3] = vars[3];
}

float * GameOptions::getProjVars(){
	return projVar;
}


void GameOptions::setCamPos(float * pos){
	camPos[0] = pos[0];
	camPos[1] = pos[1];
	camPos[2] = pos[2];
}

float * GameOptions::getCamPos(){
	return camPos;
}


void GameOptions::addMPaths(std::string path){
	modelPaths.push_back(path);
}
void GameOptions::clearMPaths(){
	modelPaths.clear();
}
std::vector<std::string> GameOptions::getMPaths(){
	return modelPaths;
}

void GameOptions::addTPaths(std::string path){
	texturePaths.push_back(path);
}
void GameOptions::clearTPaths(){
	texturePaths.clear();
}
std::vector<std::string> GameOptions::getTPaths(){
	return texturePaths;
}

GameRenderer::GameRenderer(){
	ViewMatrix = glm::mat4();
	ProjectionMatrix = glm::mat4();
}

void GameRenderer::Initialize(){
	shaderProgram = createShadersProgram("./src/Source/VertexShader.glsl", "./src/Source/FragmentShader.glsl");
	glActiveTexture(GL_TEXTURE0);
	camVec[0] = glm::vec3(0.0f,0.0f,0.0f);
	camVec[1] = glm::vec3(0.0f,0.0f,0.0f);
	camVec[2] = glm::vec3(0.0f,1.0f,0.0f);
}

void GameRenderer::setShaderProgram(const char * vertexPath, const char * fragmentPath){
	shaderProgram = createShadersProgram(vertexPath, fragmentPath);
}

GLuint GameRenderer::getShaderProgram(){
	return shaderProgram;
}

void GameRenderer::setCamVec(int index, glm::vec3 v){
	camVec[index] = v;
}

glm::vec3 GameRenderer::getCamVec(int index){
	return camVec[index];
}

glm::vec3 * GameRenderer::getCamVec(){
	return camVec;
}

void GameRenderer::setViewMatrix(glm::mat4 view){
	ViewMatrix = view;
}

glm::mat4 GameRenderer::getViewMatrix(){
	return ViewMatrix;
}

void GameRenderer::setProjMatrix(glm::mat4 proj){
	ProjectionMatrix = proj;
}

glm::mat4 GameRenderer::getProjMatrix(){
	return ProjectionMatrix;
}

void GameRenderer::setIObject(int index, IOBJ * object){
	InstancedObjectsList[index] = object;
}

void GameRenderer::addToIObjectList(IOBJ * object){
	InstancedObjectsList.push_back(object);
}

void GameRenderer::clearIObjectList(){
	InstancedObjectsList.clear();
}

std::vector<IOBJ *> GameRenderer::getIObjectList(){
	return InstancedObjectsList;
}

IOBJ * GameRenderer::getIObject(int index){
	return InstancedObjectsList[index];

} 

void GameRenderer::addToTextureList(const char * textureFile){
	GLuint tempTex;
	glGenTextures(1, &tempTex);
	glBindTexture(GL_TEXTURE_2D, tempTex);
	loadBMP(textureFile, &tempTex);
	TextureList.push_back(tempTex);
}

void GameRenderer::clearTextureList(){
	TextureList.clear();
}

std::vector<GLuint> GameRenderer::getTextureList(){
	return TextureList;
}

GLuint GameRenderer::getTextureList(int index){
	return TextureList[index];
}

void InitOpenGL(){
	std::cout << "Begin Init" << std::endl;
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LESS);
	glDepthRange(0.0f,1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClearDepth(1.0f);

	//Lock cursor to window and make it invisible with sfml?

	Renderer.Initialize();
	Options.Initialize();
	for(int i=0; i<9; i++){
		testingChunk[i].setPosition(glm::vec2(i%3, i/3));
		testingChunk[i].Init();
	}
	std::cout << "End Init" << std::endl;
	std::cout.flush();

}
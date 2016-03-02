#include "../Headers/CryoBase.hpp"

//const int BlockColors[16] = {0x000000, 0xff0000, 0x800000, 0xff0084, 0x9c00ff, 0x000080, 0x0000ff, 0x00baff, 0x00ffcc, 0x00ff30, 0x009900, 0x999900, 0xfcff00, 0xa56900, 0xffa200, 0xff8900};
const glm::vec3 BlockColors[16] = {glm::vec3(255.0f,255.0f,255.0f),glm::vec3(255.0f,0.0f,0.0f),glm::vec3(128.0f,0.0f,0.0f),
									glm::vec3(255.0f,119.0f,173.0f),glm::vec3(255.0f,0.0f,102.0f), glm::vec3(156.0f,0.0f,255.0f),
									glm::vec3(0.0f,0.0f,128.0f),glm::vec3(0.0f,0.0f,255.0f),glm::vec3(0.0f,186.0f,255.0f),
									glm::vec3(0.0f,255.0f,204.0f),glm::vec3(0.0f,255.0f,48.0f),glm::vec3(0.0f,153.0f,0.0f),
									glm::vec3(252.0f,255.0f,0.0f),glm::vec3(165.0f,105.0f,0.0f),glm::vec3(255.0f,204.0f,0.0f),
									glm::vec3(255.0f,137.0f,0.0f)};

GameState::GameState(){
	curState = Loading;
	camPos[0] = 0.0f;
	camPos[1] = 0.0f;
	moving = std::vector<bool>(6, false);
	Player Self;
	Self.setPos(glm::vec3(8.0f,0.0f,8.0f));
	Self.setID(Players.size());
	Players.push_back(Self);
	masterClock.restart();
	timeElapsed = masterClock.getElapsedTime();
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

void GameState::setMoving(int index, bool tag){
	moving[index] = tag;
}

bool GameState::getMoving(int index){
	return moving[index];
}

std::vector<bool> GameState::getMoving(){
	return moving;
}

void GameState::addPlayer(Player p){
	Players.push_back(p);
}

void GameState::setPlayer(int index, Player& p){
	Players[index] = p;
}

Player& GameState::getPlayer(int index){
	return Players[index];
}

std::vector<Player> GameState::getPlayerList(){
	return Players;
}

sf::Time GameState::getTime(){
	return timeElapsed;
}

void GameState::setTime(sf::Time t){
	timeElapsed = t;
}

baseObj::baseObj(){
	TransformMatrix = glm::mat4(1.0f);
	modelIdentifier = 0;
	textureIdentifier = glm::vec2(0,0);
}

void baseObj::setTMatrix(glm::mat4 matrix){
	TransformMatrix = matrix;
}

glm::mat4 baseObj::getTMatrix() const{
	return TransformMatrix;
}


void baseObj::setMID(GLint id){
	modelIdentifier = id;
}

GLint baseObj::getMID() const{
	return modelIdentifier;
}

void baseObj::setTID(int id){
	textureIdentifier = glm::vec2(glm::mod(id,NUMTEX), floor((float)id/(float)NUMTEX));
}

void baseObj::setTID(glm::vec2 id){
	textureIdentifier = id;
}


glm::vec2 baseObj::getTID() const{
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
							if(d2.path().string().compare(d2.path().string().size()-4,4,".png") == 0 || d2.path().string().compare(d2.path().string().size()-4,4,".BMP") == 0 ){
								texturePaths.push_back(d2.path().string());
							}else if(d2.path().string().compare(d2.path().string().size()-4,4,".obj") == 0 ){
								modelPaths.push_back(d2.path().string());
							}else{
								std::cerr << "NOT ONE OF THE TWO VALID TYPES!!!" << std::endl;
							}
						}else{
							std::cerr << "this is not a regular file." << std::endl;
						}
					}
				}
			}
		}else{
			std::cerr << "Path provided is not a directory." << std::endl;
		}
	}else{
		std::cerr <<"Path does not exist."<< std::endl;
	}
	projVar[0] = 1.04f;
	projVar[1] = 1.0f;
	projVar[2] = 0.01f;
	projVar[3] = 2000.0f;

	camRot[0] = 5.0f;
	camRot[1] = 2.0f;
	camRot[2] = 20.0f;
}

GameOptions::GameOptions(const char * optionsFileName){
	GameOptions();
}

GameOptions::~GameOptions(){
}

void GameOptions::Initialize(){
	IOBJ * tempOBJ;
	std::cout << "loading "<< modelPaths.size() <<" Models: " << std::endl;
	for(int i=0; i<modelPaths.size(); i++){
		tempOBJ = new IOBJ();
		readOBJFile(tempOBJ, modelPaths[i].c_str());
		tempOBJ->Initialize();
		Renderer.addToIObjectList(tempOBJ);
	}
/*	tempOBJ = Renderer.getIObject(0);
	Block *playerPos;
	tempOBJ->addBlocks(playerPos);
	playerPos->setTID(2)*/

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


void GameOptions::setCamRot(glm::vec3 rot){
	camRot[0] = rot[0];
	camRot[1] = rot[1];
	camRot[2] = rot[2];
}

glm::vec3 GameOptions::getCamRot(){
	return camRot;
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
	shaderProgram = 0;
}

GameRenderer::~GameRenderer(){
	for(int i=0; i<TextureList.size(); i++){
		glDeleteTextures(1, &TextureList.data()[i]);
	}
	for(int i=0; i<InstancedObjectsList.size(); i++){
		delete InstancedObjectsList[i];
	}
}

void GameRenderer::Initialize(){
	std::cout << "Starting to Load Shaders" << std::endl;
	shaderProgram = createShadersProgram("./src/Source/VertexShader.glsl", "./src/Source/FragmentShader.glsl");
	std::cout << "Finnished Loading Shaders" << std::endl;
	camVec[0] = glm::vec3(0.0f,0.0f,0.0f);
	camVec[1] = glm::vec3(16.0f,0.0f,16.0f);
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

std::vector<IOBJ *>& GameRenderer::getIObjectList(){
	return InstancedObjectsList;
}

IOBJ * GameRenderer::getIObject(int index){
	if(index >= InstancedObjectsList.size())
			return nullptr;
	return InstancedObjectsList[index];
} 

int GameRenderer::getIObjectSize(){
	return InstancedObjectsList.size();
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
	std::cerr << "Begin Init" << std::endl;
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(1.0f,1.0f,1.0f,1.0f);

	Renderer.Initialize();
	Options.Initialize();

	std::cerr << "End Init" << std::endl;

}


void tickUpdate(){
	Player self = State.getPlayer(0);
	glm::vec3 selfPos = self.getPos();
	glm::vec2 prevChunk = self.getChunk();
	glm::vec2 curChunk =  glm::vec2(floor(selfPos.x/(CHUNKDIMS*BLOCKSCALE)), floor(selfPos.z/(CHUNKDIMS*BLOCKSCALE)));
	//std::cout << "PrevChunk: x->" << prevChunk.x << ": Y->" << prevChunk.y << std::endl;
	//std::cout << "CurChunk: x->" << curChunk.x << ": Y->" << curChunk.y << std::endl;
	Chunk * c;
	if(World.size() == 0){
		for(int i=0; i<RENDERRADIUS*RENDERRADIUS; i++){
				c = new Chunk(prevChunk+glm::vec2((i%RENDERRADIUS)-floor(RENDERRADIUS/2),(i/RENDERRADIUS)-floor(RENDERRADIUS/2)));
				c->Init();
				World.push_back(c);
		}
	}
	if(curChunk!=prevChunk){
		self.setChunk(curChunk);
		glm::vec2 chunkDif = curChunk-prevChunk;
		//std::cout << chunkDif.x <<" : " << chunkDif.y << std::endl;
		bool found[RENDERRADIUS*RENDERRADIUS] = {false};

		//std::cout << "CurChunk: " << curChunk.x << " : " << curChunk.y << std::endl;
		for(int i=0; i<World.size(); i++){
			glm::vec2 Wpos = World[i]->getPosition();
			std::cout << "Chunk " << i << ": X->" << Wpos.x << " ; Y->" << Wpos.y << std::endl;
			for(int i=0; i<RENDERRADIUS*RENDERRADIUS; i++){
				if(Wpos == curChunk+glm::vec2((i%RENDERRADIUS)-floor(RENDERRADIUS/2),(i/RENDERRADIUS)-floor(RENDERRADIUS/2))) found[i] = true;
			}
		}
		//std::cout << std::endl;
		//std::cout << "Found:" <<std::endl;
		for(int i=0; i<RENDERRADIUS*RENDERRADIUS; i++){
			if(found[i] != true){
				c = new Chunk(curChunk+glm::vec2((i%RENDERRADIUS)-floor(RENDERRADIUS/2),(i/RENDERRADIUS)-floor(RENDERRADIUS/2)));
				c->Init();
				World.push_back(c);
			}
			//std::cout <<"\t" << found[i] << std::endl;

		}	
		//std::cout << std::endl;
	}
	//std::cout << "Before Pick." << std::endl;
	Block * b = pickBlock();
	self = State.getPlayer(0);
	//std::cout << "After Pick." << std::endl;
	self.setSelected(b);
	self.removeSelect(Selector);
	if(b!=nullptr){
		//std::cout << b->getID() << std::endl << std::flush;
		self.addSelect(b->getTMatrix(), Red, Selector);
	}
	State.setPlayer(0, self);
}
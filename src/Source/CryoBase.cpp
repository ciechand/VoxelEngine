#include "../Headers/CryoBase.hpp"

//const int BlockColors[16] = {0x000000, 0xff0000, 0x800000, 0xff0084, 0x9c00ff, 0x000080, 0x0000ff, 0x00baff, 0x00ffcc, 0x00ff30, 0x009900, 0x999900, 0xfcff00, 0xa56900, 0xffa200, 0xff8900};
const glm::vec3 BlockColors[16] = {glm::vec3(255.0f,255.0f,255.0f),glm::vec3(255.0f,0.0f,0.0f),glm::vec3(128.0f,0.0f,0.0f),
									glm::vec3(255.0f,119.0f,173.0f),glm::vec3(255.0f,0.0f,102.0f), glm::vec3(156.0f,0.0f,255.0f),
									glm::vec3(0.0f,0.0f,128.0f),glm::vec3(0.0f,0.0f,255.0f),glm::vec3(0.0f,186.0f,255.0f),
									glm::vec3(0.0f,255.0f,204.0f),glm::vec3(0.0f,255.0f,48.0f),glm::vec3(0.0f,153.0f,0.0f),
									glm::vec3(252.0f,255.0f,0.0f),glm::vec3(165.0f,105.0f,0.0f),glm::vec3(255.0f,204.0f,0.0f),
									glm::vec3(255.0f,137.0f,0.0f)};

//A Constant Array for Directional Vectors. See CryoBase.hpp - BlockFace enum for which number are which direction.
const glm::vec3 DirectionalVectors[6] = {glm::vec3(1.0f*BLOCKSCALE,0.0f,0.0f),
									  glm::vec3(-1.0f*BLOCKSCALE,0.0f,0.0f),
									  glm::vec3(0.0f,1.0f*BLOCKSCALE,0.0f),
									  glm::vec3(0.0f,-1.0f*BLOCKSCALE,0.0f),
									  glm::vec3(0.0f,0.0f,1.0f*BLOCKSCALE),
									  glm::vec3(0.0f,0.0f,-1.0f*BLOCKSCALE)};


std::vector<std::string> BlockNames;


GameState::GameState(){
	curState = Loading;
	projChange = false;
	camPos[0] = 0.0f;
	camPos[1] = 0.0f;
	moving = std::vector<bool>(NUMBER_OF_FLAGS, false);
	Player Self;
	Self.setPos(glm::vec3(8.5f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),8.5f));
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

void GameState::setProjChange(bool pc){
	projChange = pc;
}

bool GameState::getProjChange(){
	return projChange;
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

Player * GameState::getPlayer(int index){
	return &(Players[index]);
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

float GameState::getdTime(){
	return deltaTime;
}

void GameState::setdTime(float t){
	deltaTime = t;
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
				if(is_directory(d) && d.path().generic_string().compare(0,d.path().string().size(),"./Assets/Shaders") != 0){
					std::cerr << "Path: " << d.path().generic_string() << std::endl;
					for(directory_entry& d2 : directory_iterator(d)){
						if(is_regular_file(d2)){
							if(d2.path().string().compare(d2.path().string().size()-4,4,".png") == 0 || d2.path().string().compare(d2.path().string().size()-4,4,".BMP") == 0 ){
								texturePaths.push_back(d2.path().string());
							}else if(d2.path().string().compare(d2.path().string().size()-4,4,".obj") == 0 ){
								modelPaths.push_back(d2.path().string());

								int pathSize = d2.path().parent_path().string().size();
								BlockNames.push_back(d2.path().string().substr(pathSize+1,d2.path().string().size()-4-(pathSize+1))); 
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
	projVar[0] = PI/2.25;
	projVar[1] = 1.0f;
	projVar[2] = 0.01f;
	projVar[3] = 200.0f;

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
	std::cout << "Model Names:\n"; 
	for(int i=0; i<modelPaths.size(); i++){
		std::cout << "\t->" << BlockNames[i] << std::endl;
		tempOBJ = new IOBJ();
		readOBJFile(tempOBJ, modelPaths[i].c_str());
		tempOBJ->Initialize();
		//Need to add a blank block at 0 so that the uint which is an index at 0 is never pointing to a block.
		Renderer.addToIObjectList(tempOBJ);
	}
	std::cout << std::endl;

	int sphereIndex = blockNamesFind("Sphere");
	tempOBJ = Renderer.getIObject(sphereIndex);
	tempOBJ->addBlocks();
	PlayerPos = std::make_pair(sphereIndex,tempOBJ->getBlocksSize()-1);
	Block * bp = tempOBJ->getBlocks(PlayerPos.second);
	bp->setMID(sphereIndex);
	bp->setTID(2);
	bp->setPos(glm::vec3(8.5f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),8.5f));
	bp->setID(tempOBJ->getBlocksSize()-1);

	int lineIndex = blockNamesFind("Line");
	tempOBJ = Renderer.getIObject(lineIndex);
	tempOBJ->addBlocks();
	ViewLinePos = std::make_pair(lineIndex,tempOBJ->getBlocksSize()-1);
	bp = tempOBJ->getBlocks(ViewLinePos.second);
	bp->setMID(lineIndex);
	bp->setTID(2);
	bp->setPos(glm::vec3(8.5f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),8.5f));
	bp->setID(tempOBJ->getBlocksSize()-1);
	bp->setColor(Pink);

	std::cout << "Finished Loading Models" << std::endl << "Loading Textures: " << std::endl;
	for(int i=0; i<texturePaths.size(); i++){
		std::cout << "\t->" <<  texturePaths[i] << " Loaded" << std::endl;
		Renderer.addToTextureList(texturePaths[i].c_str());
	} 
	std::cout << std::endl;
	std::cout << "Finished Loading Textures" <<  std::endl;
}

void GameOptions::setProjVars(float * vars){
	projVar[0] = vars[0];
	projVar[1] = vars[1];
	projVar[2] = vars[2];
	projVar[3] = vars[3];
}

void GameOptions::setProjVars(int index, float vars){
	projVar[index] = vars;
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
//std::cout << "Starting to Load Shaders" << std::endl;
	shaderProgram = createShadersProgram("./Assets/Shaders/VertexShader.glsl", "./Assets/Shaders/FragmentShader.glsl");
//std::cout << "Finnished Loading Shaders" << std::endl;
	camVec[0] = glm::vec3(0.0f,0.0f,0.0f);
	camVec[1] = glm::vec3(8.0f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),8.0f);
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
	State.setState(Running);
}


void tickUpdate(){
	sf::Time curTime = masterClock.getElapsedTime();
	sf::Time lastTime = State.getTime();
	float deltaTime = curTime.asSeconds() - lastTime.asSeconds();
	//std::cout << "Cur: " << curTime.asSeconds() << "\n\tLast: " << lastTime.asSeconds() << "\n\tDelta: " << deltaTime << std::endl;
	State.setTime(curTime);
	State.setdTime(deltaTime);
	Player * self = State.getPlayer(0);

	glm::vec3 selfPos = self->getPos();
	glm::vec2 prevChunk = self->getChunk();
	glm::vec2 curChunk =  glm::vec2(floor(selfPos.x/(CHUNKDIMS*BLOCKSCALE)), floor(selfPos.z/(CHUNKDIMS*BLOCKSCALE)));
	//std::cout << "PrevChunk: x->" << prevChunk.x << ": Y->" << prevChunk.y << std::endl;
	//std::cout << "CurChunk: x->" << curChunk.x << ": Y->" << curChunk.y << std::endl;

	//std::cout << "Before Chunk" << std::endl;
	Chunk * c;
	if(World.size() == 0){
		for(int i=0; i<RENDERRADIUS*RENDERRADIUS; i++){
				c = new Chunk(prevChunk+glm::vec2((i%RENDERRADIUS)-floor(RENDERRADIUS/2),(i/RENDERRADIUS)-floor(RENDERRADIUS/2)));
				c->Init();
				World.push_back(c);
		}
	}
	if(curChunk!=prevChunk){
		self->setChunk(curChunk);
		glm::vec2 chunkDif = curChunk-prevChunk;
		//std::cout << chunkDif.x <<" : " << chunkDif.y << std::endl;
		bool found[RENDERRADIUS*RENDERRADIUS] = {false};

		//std::cout << "CurChunk: " << curChunk.x << " : " << curChunk.y << std::endl;
		for(int i=0; i<World.size(); i++){
			glm::vec2 Wpos = World[i]->getPos();
			//std::cout << "Chunk " << i << ": X->" << Wpos.x << " ; Y->" << Wpos.y << std::endl;
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
	glm::vec3 camPos = Renderer.getCamVec(0);
	glm::vec3 lookingDirection = selfPos-camPos;
	lookingDirection = glm::normalize(lookingDirection)*FOCUSDIST;

	Ray sight(selfPos,lookingDirection,0.0f,FOCUSDIST);

	IOBJ * tempOBJ = Renderer.getIObject(ViewLinePos.first);
	Block * bp = tempOBJ->getBlocks(ViewLinePos.second);
	glm::mat4 mats = glm::scale(glm::mat4(), glm::vec3(FOCUSDIST));
	glm::mat4 matt = glm::translate(glm::mat4(), selfPos);
	glm::mat4 matr = alignVec(glm::vec3(0.0f,1.0f,0.0f),lookingDirection);
	bp->setPos(selfPos);
	bp->setTMatrix(matt*glm::transpose(matr)*mats);

	std::pair<Block *, int> pickedBlock = pickBlock(sight);
	//std::cout << "After Pick." << std::endl;
	self->setSelected(pickedBlock.first);
	self->setSelectedSide(pickedBlock.second);
	self->removeSelect();
	if(pickedBlock.first != nullptr){
		//std::cout << b->getID() << std::endl << std::flush;
		self->addSelect(pickedBlock.first->getTMatrix(), Red, Selector);
	}

	//std::cerr << side << std::endl;
	if(pickedBlock.second != -1 && pickedBlock.first != nullptr){
		glm::mat4 trans = glm::translate(pickedBlock.first->getTMatrix(), DirectionalVectors[pickedBlock.second]);
		self->addSelect(trans, Green, Highlight);
	}
}


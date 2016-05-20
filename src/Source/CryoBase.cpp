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


std::vector<std::string> AssetPaths = {"./Assets/Models/Blocks/", "./Assets/Models/Misc/", "./Assets/Textures/"};
std::vector<std::string> BlockNames;
std::vector<std::string> TextureNames;

baseObj::baseObj(){
	TransformMatrix = glm::mat4(1.0f);
	textureOffset = glm::vec2(0,0);
	color = BlockColors[0];
	textureSize = glm::vec2(1.0f,1.0f);

	modelIdentifier = -1;
	position = glm::vec3();
	ID = -1;
}

baseObj::~baseObj(){

}

glm::mat4 * baseObj::getStartingAddr(){
	return &TransformMatrix;
}

void baseObj::setColor(int c){
	color = BlockColors[c];
}	

void baseObj::setColor(glm::vec3 c){
	color = c;
}	

glm::vec3 baseObj::getColor() const{
	return color;
}

int baseObj::getColorNumber() const{
	for(int i=0; i<16; i++){
		if(BlockColors[i] == color)
			return i;
	}
}

void baseObj::setPos(glm::vec3 point){
	position = point;
	setTMatrix(glm::translate(glm::mat4(),point),Translate);
}

glm::vec3 baseObj::getPos() const{
	return position;
}


void baseObj::setTMatrix(glm::mat4 matrix,int index){
	Matrices[index] = matrix;
	TransformMatrix = Matrices[Translate]*Matrices[Scale]*Matrices[Rotate];
}

glm::mat4 baseObj::getTMatrix(int index) const{
	return Matrices[index];
}

glm::mat4 baseObj::getTMatrix() const{
	return Matrices[Translate]*Matrices[Scale]*Matrices[Rotate];
}

void baseObj::setMID(GLint id){
	modelIdentifier = id;
}

GLint baseObj::getMID() const{
	return modelIdentifier;
}

void baseObj::setTOff(int offset){
	textureOffset = glm::vec2(glm::mod(offset,NUMTEX), floor((float)offset/(float)NUMTEX));
}

void baseObj::setTOff(glm::vec2 offset){
	textureOffset = offset;
}

glm::vec2 baseObj::getTOff() const{
	return textureOffset;
}

void baseObj::setTSize(glm::vec2 size){
	textureSize = size;
}

glm::vec2 baseObj::getTSize() const{
	return textureSize;
}

void baseObj::setID(int id){
	ID = id;
}

int baseObj::getID() const{
	return ID;
}

baseItem::baseItem():baseObj(){
	StackInfo = std::make_pair(1, 0);
}

baseItem::baseItem(unsigned int maxStack):baseObj(){
	StackInfo = std::make_pair(maxStack, 0);
}

baseItem::~baseItem(){

}

void baseItem::Interact(){
	std::cout << "Interacting with Item" << std::endl;
}

std::pair<unsigned int, unsigned int> baseItem::getStackInfo(){
	return StackInfo;
}

void baseItem::setStackInfo(unsigned int ms){
	StackInfo.first = ms;
}

void baseItem::addItem(int count){
	int res = StackInfo.second + count;
	if(res <= 0)
		StackInfo.first = 0;
	StackInfo.second = res;
}



GameState::GameState(){
	curState = Loading;
	projChange = false;
	camPos[0] = 0.0f;
	camPos[1] = 0.0f;
	moving = std::vector<bool>(NUMBER_OF_FLAGS, false);
	Player Self;
	Self.setPos(glm::vec3(0.0f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),0.0f));
	Self.setID(Players.size());
	Players.push_back(Self);
	masterClock.restart();
	timeElapsed = masterClock.getElapsedTime();
}

void GameState::setState(unsigned int cs){
	curState = cs;
}

unsigned int GameState::getState(){
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


baseItem * GameState::getHItem(){
	return heldItem;
}

void GameState::setHItem(baseItem * i){
	heldItem = i;
}

Window * GameState::getSWindow(){
	return selectedWindow;
}

void GameState::setSWindow(Window * w){
	selectedWindow = w;
}

GameOptions::GameOptions(){

	using namespace boost::filesystem;

	texturePaths.clear();
	modelPaths.clear();
	
	for(std::string &s : AssetPaths){
		path p (s);
		if(is_directory(p) && exists(p)){
			std::cerr << "Path: " << p.generic_string() << std::endl;
			for(directory_entry& d2 : directory_iterator(p)){
				if(is_regular_file(d2)){
					if(d2.path().string().compare(d2.path().string().size()-4,4,".png") == 0){
						texturePaths.push_back(d2.path().generic_string());

						int pathSize = d2.path().parent_path().string().size();
						TextureNames.push_back(d2.path().string().substr(pathSize+1,d2.path().string().size()-4-(pathSize+1))); 
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
		}else{
			std::cerr << "Path provided is not a directory." << std::endl;
		}
	}
	projVar[0] = PI/2.25;
	projVar[1] = 1024.0f/768.0f;
	projVar[2] = 0.0001f;
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
		if(modelPaths[i].substr(0,modelPaths[i].find_last_of('\\',modelPaths[i].size())).compare(0,modelPaths[i].find_last_of('\\',modelPaths[i].size()),"./Assets/Models/Blocks") == 0)
			tempOBJ->Initialize(true);
		else
			tempOBJ->Initialize();
		tempOBJ->setMID(i);
		Renderer.addToIObjectList(tempOBJ);
	}
	std::cout << "Finished Loading Models" <<  std::endl;
	std::cout << std::endl;
	std::cout << "Loading Textures: " << std::endl;
	for(int i=0; i<texturePaths.size(); i++){
		std::cout << "\tT"<<i<<"->" <<  texturePaths[i] << " Loaded" << std::endl;
		Renderer.addToTextureList(texturePaths[i].c_str());
	} 
	glActiveTexture(GL_TEXTURE0);
	std::cout << "Finished Loading Textures" <<  std::endl;
	std::cout << std::endl;
	std::cout << "Loading Basic Objects" << std::endl;

	int sphereIndex = modelNamesFind("Sphere");
	tempOBJ = Renderer.getIObject(sphereIndex);
	tempOBJ->addBlocks();
	PlayerPos = std::make_pair(sphereIndex,tempOBJ->getBlocksSize()-1);
	Block * bp = tempOBJ->getBlocks(PlayerPos.second);
	bp->setMID(sphereIndex);
	bp->setTOff(2);
	bp->setPos(glm::vec3(0.0f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),0.0f));
	bp->setID(tempOBJ->getBlocksSize()-1);

	int lineIndex = modelNamesFind("Line");
	tempOBJ = Renderer.getIObject(lineIndex);
	tempOBJ->addBlocks();
	ViewLinePos = std::make_pair(lineIndex,tempOBJ->getBlocksSize()-1);
	bp = tempOBJ->getBlocks(ViewLinePos.second);
	bp->setMID(lineIndex);
	bp->setTOff(2);
	bp->setPos(glm::vec3(0.0f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),0.0f));
	bp->setID(tempOBJ->getBlocksSize()-1);
	bp->setColor(Pink);

	Renderer.addWindow(glm::vec2(3.0f,3.0f), glm::vec2(100.0f, 100.0f), glm::vec2(600.0f, 900.0f), true);
	Window * curW = Renderer.getWindows(0);
	curW->setWType(PInv);
	std::cout << "Finnished Loading Basic Objects" << std::endl;
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
	camVec[1] = glm::vec3(0.0f,(CHUNKHEIGHT*BLOCKSCALE)+(BLOCKSCALE*2),0.0f);
	camVec[2] = glm::vec3(0.0f,1.0f,0.0f);
}

void GameRenderer::setGuiUniforms(){
	GLuint samplePos = glGetUniformLocation(shaderProgram, "textureSample");
	glUniform1i(samplePos, textureNamesFind("Gui"));

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, &(glm::mat4())[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &(glm::mat4())[0][0]);
}

void GameRenderer::setOBJUniforms(){
	float * projV = Options.getProjVars();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "proj"), 1, GL_FALSE, &(glm::perspective(projV[0],projV[1],projV[2],projV[3]))[0][0]);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "viewMatrix"), 1, GL_FALSE, &(ViewMatrix)[0][0]);
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
	if(index >= InstancedObjectsList.size() || index < 0)
			return nullptr;
	return InstancedObjectsList[index];
} 

int GameRenderer::getIObjectSize(){
	return InstancedObjectsList.size();
} 


void GameRenderer::addToTextureList(const char * textureFile){
	GLuint tempTex;
	glGenTextures(1, &tempTex);
	glActiveTexture(GL_TEXTURE0+TextureList.size());
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

void GameRenderer::addWindow(){
	Windows.emplace_back();
}

void GameRenderer::addWindow(Window w){
	Windows.push_back(w);
}

void GameRenderer::addWindow(glm::vec2 tsize, glm::vec2 pos, glm::vec2 wsize, bool hide){
	Windows.emplace_back(tsize, pos, wsize, hide);
}

void GameRenderer::setWindow(int index, Window w){
	Windows[index] = w;
}

Window * GameRenderer::getWindows(int index){
	if(index < 0 || index >= Windows.size())
		return nullptr;
	return &(Windows[index]);
}

std::vector<Window> & GameRenderer::getWindows(){
	return Windows;
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

	unsigned int curState = State.getState();
	if(curState == Running){
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
		bp->setTMatrix(matt,Translate);
		bp->setTMatrix(glm::transpose(matr),Rotate);
		bp->setTMatrix(mats,Scale);

		std::pair<Block *, int> pickedBlock = pickBlock(sight);

		//std::cout << "After Pick." << std::endl;
		self->setSelected(pickedBlock.first);
		self->setSelectedSide(pickedBlock.second);
		self->removeSelect();
		if(pickedBlock.first != nullptr){
			//std::cout << b->getID() << std::endl << std::flush;
			self->addSelect(*(pickedBlock.first), Red, Selector);
		}

		//std::cerr << side << std::endl;
		if(pickedBlock.second != -1 && pickedBlock.first != nullptr){
			Block tempB = *pickedBlock.first;
			tempB.setTMatrix(glm::translate(tempB.getTMatrix(Translate), DirectionalVectors[pickedBlock.second]), Translate);
			self->addSelect(tempB, Green, Highlight);
		}
	}else if (curState == Menu){
		sf::Vector2u mwsize = mainWindow.getSize();
		sf::Vector2i mpos = sf::Mouse::getPosition(mainWindow);
		mpos.x *=2;
		mpos.y *=2;
		//std::cout << "Mouse Pos: \n\tX:" << mpos.x << "\n\tY:" << mpos.y << std::endl;
		std::vector<Window> & winds = Renderer.getWindows();

		Window * overWindow = nullptr;
		int depth = -1;

		for(Window & w:winds){
			if(w.getHidden() == false){
				glm::vec3 wpos = w.getPos();
				glm::vec2 wsize = w.getSize();
				int curDepth = w.getDepth();
				//std::cout << "Window Pos: \n\tX:" << wpos.x << "\n\tY:" << wpos.y << std::endl;
				//std::cout << "Window Size: \n\tX:" << wsize.x << "\n\tY:" << wsize.y << std::endl;
				//std::cout << "Depth: " << depth << std::endl;
				//std::cout << "CurDepth: " << curDepth << std::endl;
				if(mpos.x >= wpos.x && mpos.x <= wpos.x+wsize.x && mpos.y >= wpos.y && mpos.y <= wpos.y+wsize.y && curDepth >= depth){
					overWindow = &w;
					depth = curDepth;

					std::vector<Pane> & panes = w.getPane();
					for(Pane&p:panes){
						glm::vec3 ppos = p.getPos();
						glm::vec2 psize = p.getSize();
						curDepth = p.getDepth();
						//std::cout << "\tDepth: " << curDepth << std::endl;
						if(mpos.x >= ppos.x && mpos.x <= ppos.x+psize.x && mpos.y >= ppos.y && mpos.y <= ppos.y+psize.y && curDepth >= depth){
							p.setColor(Red);
						}else{
							p.setColor(None);
						}
					}

					std::vector<Slot> & slots = w.getSlot();
					for(Slot&s:slots){
						glm::vec3 spos = s.getPos();
						glm::vec2 ssize = s.getSize();
						curDepth = s.getDepth();
						//std::cout << "\tDepth: " << curDepth << std::endl;
						if(mpos.x >= spos.x && mpos.x <= spos.x+ssize.x && mpos.y >= spos.y && mpos.y <= spos.y+ssize.y && curDepth >= depth){
							s.setColor(Red);
						}else{
							s.setColor(None);
						}
					}
				}else{
					std::vector<Pane> & panes = w.getPane();
					for(Pane&p:panes){p.setColor(None);}
					std::vector<Slot> & slots = w.getSlot();
					for(Slot&s:slots){s.setColor(None);}
				}
			}
		}
		State.setSWindow(overWindow);
	}
}


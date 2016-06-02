#include "../Headers/CryoBase.hpp"

Player::Player(){
	position = glm::vec3();
	curChunk = glm::vec2();
	movingDirection = glm::vec3();
	ID = 0;
	selected = nullptr;
	selectedSide = -1;
	jumping = false;
	bounds.setLimbs(glm::vec3((PLAYERWIDTH/2),(PLAYERHEIGHT/2),(PLAYERWIDTH/2)));
	Inventory.assign(DEFAULTPLAYERSLOTS, nullptr);

	Renderer.addWindow(glm::vec2(3.0f,3.0f), glm::vec2(100.0f, 100.0f), glm::vec2(600.0f, 900.0f), true, PInv);

	std::vector<Window> & tempI = Renderer.getWindows();
	inventoryWindow = Renderer.getWindows(tempI.size()-1);

	baseItem * b = new Block();

	setItem(0,b);
}

Player::~Player(){

}

void Player::addSelect(Block b, Colors16 c, SelectTypes type){
	int size = State.Selectors.size();
	State.Selectors.push_back(b);
	State.Selectors[size].setType(type);
	State.Selectors[size].setTOff(1);
	State.Selectors[size].setColor(c);
	State.Selectors[size].setOwner(ID);
	State.Selectors[size].setTMatrix(glm::scale(State.Selectors[size].getTMatrix(Scale), glm::vec3(1.01f,1.01f,1.01f)), Scale);
}

void Player::removeSelect(){
	for(int i=0; i<State.Selectors.size(); i++){
		if(State.Selectors[i].getOwner() == ID){
			State.Selectors.erase(State.Selectors.begin()+i);
		}
	}
}

void Player::removeSelect(SelectTypes type){
	for(int i=0; i<State.Selectors.size(); i++){
		if(State.Selectors[i].getOwner() == ID && State.Selectors[i].getType() == type){
			State.Selectors.erase(State.Selectors.begin()+i);
		}
	}
}

void Player::setPos(glm::vec3 p){
	position = p;
	//setChunk(glm::vec2(floor(position.x/(CHUNKDIMS*BLOCKSCALE)), floor(position.z/(CHUNKDIMS*BLOCKSCALE))));
	setBounds(position);
}

glm::vec3 Player::getPos(){
	return position;
}


void Player::setSight(Ray r){
	SightLine = r;
}

Ray Player::getSight(){
	return SightLine;
}

void Player::setMDir(glm::vec3 dir){
	movingDirection = dir;
}

glm::vec3 Player::getMDIR(){
 return movingDirection;
}


void Player::setChunk(glm::vec2 c){
	curChunk = c;
}

glm::vec2 Player::getChunk(){
	return curChunk;
}

void Player::setRelBlock(glm::vec3 b){
	relBlock = b;
}

glm::vec3 Player::getRelBlock(){
	return relBlock;
}

void Player::setID(int id){
	ID = id;
}

int Player::getID(){
	return ID;
}

void Player::setVelocity(glm::vec3 a){
	velocity = a;
}

glm::vec3 Player::getVelocity(){
	return velocity;
}


void Player::setSelected(Block * b){
	selected = b;
}

void Player::clearSelected(){
	selected = nullptr;
}

Block * Player::getSelected(){
	return selected;
}

void Player::setSelectedSide(int side){
	selectedSide = side;
}

int Player::getSelectedSide(){
	return selectedSide;
}

void Player::setJumping(bool j){
	jumping = j;
}

bool Player::getJumping(){
	return jumping;
}

void Player::setBounds(BBox & b){
	bounds = b;
}

void Player::setBounds(glm::vec3 a){
	bounds.setPos(a);
}

BBox & Player::getBounds(){
	return bounds;
}

baseItem * Player::getItem(unsigned int index){
	return Inventory[index];
}

void Player::setItem(unsigned int index, baseItem * item){
	if(inventoryWindow == nullptr){
		std::cout << "Player " << ID << " Inventory has no window." << std::endl;
		return;
	}
	if(Inventory[index] != nullptr){
		std::cerr << "Trying to Add Item to Slot that has something already." << std::endl;
		delete Inventory[index];
	}
	Inventory[index] = item;
	Slot & s = inventoryWindow->getSlot(index);

	glm::vec3 ItemPos = s.getTruePos();
	item->setPos(ItemPos-glm::vec3(0.0f,0.0f,0.035f));
	item->setTMatrix(glm::rotate(glm::mat4(), PI/4 ,glm::vec3(1.0f,1.0f,1.0f)), Rotate);
	item->setTMatrix(glm::scale(glm::mat4(), glm::vec3(0.02f,0.02f,0.02f)), Scale);

	//std::cout << "Position of Item: \n\tX: "<< ItemPos.x <<  "\n\tY: "<< ItemPos.y <<  "\n\tZ: "<< ItemPos.z << std::endl;
	s.setObj(item);
}

void Player::addItem(baseItem * item){
	//This will need to be actually implemented.
}


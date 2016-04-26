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
}

Player::~Player(){
}

void Player::addSelect(glm::mat4 position){
	int size = State.Selectors.size();
	State.Selectors.emplace_back();
	State.Selectors[size].setType(Selector);
	State.Selectors[size].setTID(1);
	State.Selectors[size].setColor(Red);
	State.Selectors[size].setOwner(ID);
	State.Selectors[size].setTMatrix(glm::scale(position, glm::vec3(1.01f,1.01f,1.01f)));
}

void Player::addSelect(glm::mat4 position, Colors16 c){
	int size = State.Selectors.size();
	State.Selectors.emplace_back();
	State.Selectors[size].setType(Selector);
	State.Selectors[size].setTID(1);
	State.Selectors[size].setColor(c);
	State.Selectors[size].setOwner(ID);
	State.Selectors[size].setTMatrix(glm::scale(position, glm::vec3(1.01f,1.01f,1.01f)));
}

void Player::addSelect(glm::mat4 position, Colors16 c, SelectTypes type){
	int size = State.Selectors.size();
	State.Selectors.emplace_back();
	State.Selectors[size].setType(type);
	State.Selectors[size].setTID(1);
	State.Selectors[size].setColor(c);
	State.Selectors[size].setOwner(ID);
	State.Selectors[size].setTMatrix(glm::scale(position, glm::vec3(1.01f,1.01f,1.01f)));
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

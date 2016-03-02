#include "../Headers/CryoBase.hpp"

Player::Player(){
	Position = glm::vec3();
	curChunk = glm::vec2();
	movingDirection = glm::vec3();
	ID = 0;
	selected = nullptr;
	selectedSide = -1;
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
	Position = p;
}

glm::vec3 Player::getPos(){
	return Position;
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

void Player::setID(int id){
	ID = id;
}

int Player::getID(){
	return ID;
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

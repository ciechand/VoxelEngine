#include "../Headers/CryoBase.hpp"

Player::Player(){
	Position = glm::vec3();
	curChunk = glm::vec2();
	movingDirection = glm::vec3();
	ID = 0;
}

void Player::addSelect(Block * b){
	for(int i=0; i<State.Selectors.size(); i++){	
		if(blockCmp(b,&State.Selectors[i])) // need to add == operator overloaded funtion to BLOCK
			return;
	}
	Block *newB = new Block();
	newB->setType(Selector);
	newB->setTID(1);
	newB->setColor(Red);
	newB->setOwner(0);
	newB->setTMatrix(glm::scale(b->getTMatrix(), glm::vec3(1.01f,1.01f,1.01f)));
	State.Selectors.push_back(*newB);
}

void Player::removeSelect(){
	for(int i=0; i<State.Selectors.size(); i++){
		if(State.Selectors[i].getOwner() == ID){
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

#include "../Headers/CryoBase.hpp"

Player::Player(){
	position = glm::vec3();
	curChunk = glm::vec2();
	movingDirection = glm::vec3();
	ID = 0;
	selected = nullptr;
	selectedSide = -1;
	jumping = false;
	hotbarSlot = 0;

	bounds.setLimbs(glm::vec3((PLAYERWIDTH/2),(PLAYERHEIGHT/2),(PLAYERWIDTH/2)));

	//Get Window Size.
	sf::Vector2u mwsize = mainWindow.getSize();
	//Initiate Player Inventory Window and Link.
	Renderer.addWindow(glm::vec2(3.0f,3.0f), glm::vec2(0.0f,0.0f), glm::vec2(100.0f, 100.0f), glm::vec2(300.0f, 450.0f), true, PInv);

	inventoryWindow = Renderer.getWindows(Renderer.getWindows().size()-1);

	//Initiate Player Hotbar and Storage.
	Renderer.addSWindow(glm::vec2(3.0f, 1.0f), glm::vec2(3.0f,0.0f), glm::vec2((mwsize.x/2)-(mwsize.x/6),mwsize.y-48.0f), glm::vec2(mwsize.x/3, 50.0f), false, Hotbar);

	HotBar = Renderer.getSWindows(Renderer.getSWindows().size()-1);
	Slot & tempSlot = HotBar->getSlot(hotbarSlot);
	tempSlot.setColor(Green);

	//Add Test Items.
	baseItem * b = new Block();

	setItem(0,b);
	b = new Block();
	b->setItemType(DestructionTool);
	b->setColor(Pink);
	setItem(1,b);
	b = new Block();
	b->setItemType(Placeable);
	b->setColor(Green);
	setItem(2,b);
	b = new Block();
	b->setItemType(Placeable);
	b->setColor(Green);
	setItem(3,b);
	b = new Block();
	b->setItemType(Placeable);
	b->setColor(Green);
	setItem(4,b);
}

Player::~Player(){

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

void Player::setHSlot(unsigned int s){
	if(HotBar == nullptr){
		std::cerr <<  "HOTBAR IS NULL IN ATTEMPT TO SET" << std::endl; 
		return;
	}

	Slot & tempSlotB = HotBar->getSlot(hotbarSlot);
	tempSlotB.setColor(None);

	while(s >= DEFAULTHOTBARSLOTS)
		s-=DEFAULTHOTBARSLOTS;
	hotbarSlot = s;

	Slot & tempSlotA = HotBar->getSlot(hotbarSlot);
	tempSlotA.setColor(Green);
}

void Player::incrementHSlot(unsigned int n){
	//std::cerr << "increment" << std::endl;
	//std::cerr << "Slot Num Before: " << hotbarSlot << std::endl;

	if(HotBar == nullptr){
		std::cerr <<  "HOTBAR IS NULL IN ATTEMPT TO INCREMENT" << std::endl; 
		return;
	}

	Slot & tempSlotB = HotBar->getSlot(hotbarSlot);
	tempSlotB.setColor(None);

	int curSlot = hotbarSlot;
	curSlot+=n;
	while(curSlot >= DEFAULTHOTBARSLOTS)
		curSlot-=DEFAULTHOTBARSLOTS;
	hotbarSlot = curSlot;

	Slot & tempSlotA = HotBar->getSlot(hotbarSlot);
	tempSlotA.setColor(Green);
	//std::cerr << "Slot Num After: " << hotbarSlot << "\n" << std::endl;
}

void Player::decrementHSlot(unsigned int n){
	if(HotBar == nullptr){
		std::cerr <<  "HOTBAR IS NULL IN ATTEMPT TO DECREMENT" << std::endl; 
		return;
	}

	//std::cerr << "decrement" << std::endl;
	//std::cerr << "Slot Num Before: " << hotbarSlot << std::endl;
	Slot & tempSlotB = HotBar->getSlot(hotbarSlot);
	tempSlotB.setColor(None);

	int curSlot = hotbarSlot;
	curSlot-=n;
	while(curSlot < 0)
		curSlot+=DEFAULTHOTBARSLOTS;
	hotbarSlot = curSlot;

	Slot & tempSlotA = HotBar->getSlot(hotbarSlot);
	tempSlotA.setColor(Green);
	//std::cerr << "Slot Num After: " << hotbarSlot << "\n" << std::endl;
}

unsigned int Player::getHSlot(){
	return hotbarSlot;
}

baseItem * Player::getItem(unsigned int index){
	if(index < DEFAULTHOTBARSLOTS){
		Slot & s = HotBar->getSlot(index);
		return s.getObj();
	}else if(index < DEFAULTPLAYERSLOTS+DEFAULTHOTBARSLOTS){
		Slot & s = inventoryWindow->getSlot(index-DEFAULTHOTBARSLOTS);
		return s.getObj();
	}else{
		std::cerr << "Getting an Item from an index outside of bounds" << std::endl;
	}
}

void Player::setItem(unsigned int index, baseItem * item){
	if(index < DEFAULTPLAYERSLOTS){
		if(inventoryWindow == nullptr){
			std::cerr << "Player " << ID << " Inventory has no window Bound." << std::endl;
			return;
		}

		Slot & s = inventoryWindow->getSlot(index);
		if(s.getObj() != nullptr && item != nullptr){
			std::cerr << "Trying to Add Item to Slot that has something already." << std::endl;
		}
		
		s.setObj(item);
	}else if(index < DEFAULTPLAYERSLOTS+DEFAULTHOTBARSLOTS){
		index-=DEFAULTPLAYERSLOTS;
		if(HotBar == nullptr){
			std::cerr << "Player " << ID << " Inventory has no Hotbar Bound." << std::endl;
			return;
		}

		Slot & s = HotBar->getSlot(index);
		if(s.getObj() != nullptr && item != nullptr){
			std::cerr << "Trying to Add Item to Slot that has something already." << std::endl;
		}
		
		s.setObj(item);
	}else{
		std::cerr << "Adding an Item to an index outside of bounds" << std::endl;
	}

}

void Player::addItem(baseItem * item){
	//This will need to be actually implemented.
	//Need to write item equallity first
}

Block * Player::getSelector(unsigned int index){
	return Selectors[index];
}

std::vector<Block*> Player::getSelector(){
	return Selectors;
}

void Player::setSelector(unsigned int index, Block * b){
	Selectors[index] = b;
}

void Player::moveSelector(Block * b){
	bool found = false;
	unsigned int index = 0;
	if(b == nullptr){
		std::cerr << "b is Null?" << std::endl;
		return;
	}
	for(int i=0; i<Selectors.size(); i++){
		if(Selectors[i]->getItemType() == BaseSelector){
			index = i;
			found = true;
		}
	}
	if(found == true)
		Selectors[index]->setPos(b->getPos());
	else
		std::cerr << "Not Found" << std::endl;
}

void Player::addSelector(Block * b, unsigned int color){
	IOBJ * tempI = Renderer.getIObject(b->getMID());
	Block & addedB = tempI->addBlocks(*b);
	Selectors.push_back(&addedB);
	addedB.setID(tempI->getBlocksSize()-1);
	addedB.setItemType(BaseSelector);
	addedB.setTOff(1);
	if(color == None)
		addedB.setColor(b->getColor());
	else
		addedB.setColor(color);
	addedB.setOwner(ID);	
	addedB.setTMatrix(glm::scale(addedB.getTMatrix(Scale), glm::vec3(1.2f,1.2f,1.2f)), Scale);
}

void Player::removeSelector(Block * b){
	std::cerr << "Removing Selector" << std::endl;
	glm::vec3 blockPos = b->getPos();
	for(int i=0; i<Selectors.size(); i++){
		glm::vec3 bvec = Selectors[i]->getPos();	
		//std::cerr << "b Pos: \n\tX: "<< blockPos.x <<  "\n\tY: "<< blockPos.y <<  "\n\tZ: "<< blockPos.z << std::endl;
		//std::cerr << "selecVec: \n\tX: "<< bvec.x <<  "\n\tY: "<< bvec.y <<  "\n\tZ: "<< bvec.z << std::endl;	
		IOBJ * tempI = Renderer.getIObject(Selectors[i]->getMID());
		if(tempI != nullptr){
			tempI->removeBlock(Selectors[i]->getID());
			Selectors.erase(Selectors.begin()+i);
		}else
			std::cerr << "Why is IOBJ NULL?" << std::endl;

	}
}
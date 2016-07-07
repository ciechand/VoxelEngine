#include "../Headers/CryoBase.hpp"

Window::BaseWindow():OBJ(){
	parent = nullptr;
	modelIdentifier = modelNamesFind("Plane");
	hidden = true;
	depth = 0;
	textureSize = glm::vec2(3.0f,3.0f);
	windowType = SInv;

	textureID = textureNamesFind("Gui");
	setPos(glm::vec3(0.0f,0.0f,-20.0f));
	setSize(glm::vec2(100.0f,100.0f));

}

Window::BaseWindow(glm::vec2 tsize,  glm::vec2 tpos, glm::vec2 pos, glm::vec2 wsize, bool hide, unsigned int wType,  Window * p):OBJ(){
	modelIdentifier = modelNamesFind("Plane");
	hidden = hide;
	textureSize = tsize;
	parent = p;
	windowType = wType;
	textureID = textureNamesFind("Gui");
	setTOff(tpos);

	if(parent != nullptr)
		depth = parent->getDepth()+1;
	else
		depth = 0;

	bool bts = (p == nullptr)?true:false;


	setSize(wsize, bts);
	setPos(glm::vec3(pos,-20.0f));

	if(tsize.x != 1.0f || tsize.y != 1.0f){
		for(int i=0; i<=(size.x/(DEFAULTWINDOWPANEL))-1; i++){
			for(int j=0; j<=(size.y/(DEFAULTWINDOWPANEL))-1; j++){
				glm::vec2 realWMinSize(fmin(DEFAULTWINDOWPANEL,size.x),fmin(DEFAULTWINDOWPANEL,size.y));
				windowDraw.emplace_back(glm::vec2(1.0f,1.0f), tpos, glm::vec2(pos.x+(i*((DEFAULTWINDOWPANEL))),  pos.y+(j*((DEFAULTWINDOWPANEL)))), realWMinSize, false, FalseWindow);
				glm::vec2 preOff = arrayBounds(i,j,(size.x/(DEFAULTWINDOWPANEL))-1,(size.y/(DEFAULTWINDOWPANEL))-1);
				glm::vec2 Offset = glm::vec2(floor(preOff.x*(tsize.x-1)),(tsize.y-1.0f-floor(preOff.y*(tsize.y-1))));
				windowDraw[windowDraw.size()-1].setTOff(tpos+Offset);
			}
		}
		if(windowType == PInv){
			addSlots(DEFAULTPLAYERSLOTS,glm::vec2(0.0f,((size.y)/2)+(SLOTSEPSIZE*5)));
			for(int s=0;s<Slots.size(); s++){
				Slots[s].setID(s);
			}
			addPane(glm::vec2(size.x-((DEFAULTSLOTSIZE*0.75f)+SLOTSEPSIZE*2.5), (SLOTSEPSIZE*2.5)), ExitP);
		}else if(windowType == SInv){
			addSlots(DEFAULTPLAYERSLOTS,glm::vec2(SLOTSEPSIZE,SLOTSEPSIZE));
			for(int s=0;s<Slots.size(); s++){
				Slots[s].setID(s);
			}
			addPane(glm::vec2(size.x-((DEFAULTSLOTSIZE*0.75f)+SLOTSEPSIZE*2.5), (SLOTSEPSIZE*2.5)), ExitP);
		}else if(windowType == Hotbar){
			addSlots(DEFAULTHOTBARSLOTS,glm::vec2(SLOTSEPSIZE,SLOTSEPSIZE));
			for(int s=0;s<Slots.size(); s++){
				Slots[s].setID(s);
			}
		}


	}
}

Window::~BaseWindow(){

}

bool Window::getHidden(){
	return hidden;
}

void Window::setHidden(bool flag){
	hidden = flag;
}

glm::vec2 Window::getSize(){
	return size;
}

void Window::setSize(glm::vec2 s, bool boundToTexSize){
	if(boundToTexSize){
		glm::vec2 ds(fmod(s.x,TEXDIMS),fmod(s.y,TEXDIMS));
		size = s-ds;
		if(s.x < TEXDIMS)
			size.x = s.x;
		if(s.y < TEXDIMS)
			size.y = s.y;
	}else{
		size = s;
	}
	setTMatrix(glm::scale(glm::mat4(),glm::vec3(size.x/BLOCKSCALE,size.y/BLOCKSCALE,1.0f)), Scale);
	//std::cout << "Size: \n\tx:" << s.x << "\n\ty: " << s.y << std::endl;
	//std::cout << "new Size: \n\tx:" << size.x << "\n\ty: " << size.y << std::endl;
}

unsigned int Window::getDepth(){
	return depth;
}

void Window::setDepth(unsigned int d){
	depth = d;
}

void Window::setPos(glm::vec3 pos){
	sf::Vector2u wsize = mainWindow.getSize();
	baseObj::setPos(glm::vec3(pos.x+(size.x/2),wsize.y-(pos.y+(size.y/2)),pos.z+(depth*0.001f)));
	//std::cout << "Position of Window: \n\tX: "<< position.x <<  "\n\tY: "<< position.y <<  "\n\tZ: "<< position.z << std::endl;
	position = pos;
}

glm::vec3 Window::getTruePos(){
	sf::Vector2u wsize = mainWindow.getSize();
	return glm::vec3(position.x+(size.x/2),wsize.y-(position.y+(size.y/2)),position.z+(depth*0.001f));
}

void Window::setColor(unsigned int c){
	baseObj::setColor(c);
	for(Window & w:windowDraw){
		w.setColor(c);
	}
}

void Window::setColor(glm::vec3 c){
	baseObj::setColor(c);
	for(Window & w:windowDraw){
		w.setColor(c);
	}
}

void Window::addPane(glm::vec2 pos, unsigned int pt){
	float paneSize = DEFAULTSLOTSIZE;
	glm::vec2 tpos = glm::vec2(0.0f,0.0f);
	if(pt == ExitP){
		paneSize*=0.75f;
		tpos = glm::vec2(2.0f,3.0f);
	}
	int psize = Panes.size();
	Panes.emplace_back(glm::vec2(1.0f,1.0f), tpos, glm::vec2(position)+pos, glm::vec2(paneSize, paneSize), false, this);
	Panes[psize].setType(pt);
	Panes[psize].setID(psize);

	//std::cout << "Position of Pane: \n\tX: "<< (glm::vec2(position)+pos).x <<  "\n\tY: "<< (glm::vec2(position)+pos).y << std::endl;
}

Pane& Window::getPane(unsigned int index){
	return Panes[index];
}

std::vector<Pane> & Window::getPane(){
	return Panes;
}

void Window::addSlots(unsigned int count, glm::vec2 spos){
	int xSlots = (size.x-spos.x+SLOTSEPSIZE)/(DEFAULTSLOTSIZE+SLOTSEPSIZE);
	int ySlots = (size.y-spos.y+SLOTSEPSIZE)/(DEFAULTSLOTSIZE+SLOTSEPSIZE);

	if(count > xSlots*ySlots){
		count = xSlots*ySlots;
		std::cout << "NOT ENOUGH ROOM FOR THE SLOT COUNT!" <<  std::endl;
	}
	if(count < xSlots){
		xSlots = count;
		ySlots = 1;
	}

	float xSize = (DEFAULTSLOTSIZE*(xSlots))+(SLOTSEPSIZE*(xSlots-1));
	float ySize = (DEFAULTSLOTSIZE*(ySlots))+(SLOTSEPSIZE*(ySlots-1));

	for(int j=0; j<ySlots; j++){
		for(int i=0; i<xSlots; i++){
			addSlot(glm::vec2(spos.x+(((size.x-spos.x)-xSize)/2)+((DEFAULTSLOTSIZE*i)+(SLOTSEPSIZE*(i))), spos.y+(((size.y-spos.y)-ySize)/2)+(((DEFAULTSLOTSIZE*j)+(SLOTSEPSIZE*(j))))));
			//std::cout << "Y: " <<  spos.y+(((size.y-spos.y)-ySize)/2)+(((DEFAULTSLOTSIZE*j)+(SLOTSEPSIZE*(j)))) << std::endl;
			//std::cout << "spos Y: " << spos.y << std::endl;
			//std::cout << "size Y: " << size.y << std::endl;
			//std::cout << "yslots: " << ySlots << std::endl;
			//std::cout << "ysize Y: " << ySize << std::endl;
			//std::cout << "(size-spos) Y: " << (size.y-spos.y) << std::endl;
			//std::cout << "(size-spos)-ysize Y: " << (size.y-spos.y)-ySize << std::endl;
			//std::cout << std::endl;
		}
		count -= xSlots;
		if(count < 0)
			return;
		if(count < xSlots)
			xSlots = count;
		xSize = (DEFAULTSLOTSIZE*(xSlots))+(SLOTSEPSIZE*(xSlots-1));
	}
}

void Window::addSlot(){
	int slotsize = Slots.size();
	addSlot(glm::vec2(size.x/2,size.y/2));
	Slots[slotsize].setID(slotsize);
}

void Window::addSlot(glm::vec2 pos){
	int slotsize = Slots.size();
	Slots.emplace_back(glm::vec2(position)+pos, this);
	Slots[slotsize].setID(slotsize);
}

Slot& Window::getSlot(unsigned int index){
	return Slots[index];
}

std::vector<Slot> & Window::getSlot(){
	return Slots;
}

unsigned int Window::getWType(){
	return windowType;
}

void Window::setWType(unsigned int t){
	windowType = t;
}

unsigned int Window::getRotation(){
	return rotation;
}

void Window::setRotation(unsigned int r){
	rotation = r;
}

void Window::Draw(){
	if(windowDraw.size() == 0){
		baseObj::Draw();
	}else{
		for(Window &w:windowDraw){
			w.Draw();
		}
	}
	for(Slot &s:Slots){
		if(s.getHidden() == false)
			s.Draw();
	}
	for(Pane &p:Panes){
		if(p.getHidden() == false)
			p.Draw();
	}
	rotation++;
	if(rotation>40)
		rotation = 0;
}

void Window::Interact(){

}

WindowPane::WindowPane():Window(){
	windowType = InventoryP;
}

WindowPane::WindowPane(glm::vec2 tsize,  glm::vec2 tpos,  glm::vec2 pos, glm::vec2 wsize, bool hide,  Window * p):Window(tsize, tpos,  pos,   wsize, hide, SInv, p){
	paneType = InventoryP;
}

WindowPane::~WindowPane(){

}

unsigned int WindowPane::getType(){
	return paneType;
}

void WindowPane::setType(unsigned int t){
	paneType = t;
}

void WindowPane::Interact(){
	if(paneType == ExitP){
		parent->setHidden(true);
		bool anyVisible = false;
		for(Window &w:Renderer.getWindows()){
			if(w.getHidden() == false)
				anyVisible = true;
		}
		if(anyVisible == false)
			State.setState(Running);
		State.setFlags(OpenInv, false);
	}
}

WindowSlot::WindowSlot(glm::vec2 pos, Window* p):Pane(glm::vec2(1.0f,1.0f), glm::vec2(0.0f,3.0f), pos,  glm::vec2(DEFAULTSLOTSIZE, DEFAULTSLOTSIZE), false, p){
	windowType = SlotP;
}

WindowSlot::~WindowSlot(){
	/*if(containedObj != nullptr)
		delete containedObj;*/
}

baseItem *  WindowSlot::getObj(){
	return containedObj;
}

void  WindowSlot::setObj(baseItem * b){
	//std::cout << "Position of Slot: \n\tX: "<< position.x <<  "\n\tY: "<< position.y <<  "\n\tZ: "<< position.z << std::endl;
	containedObj = b;
	if(containedObj != nullptr){
		containedObj->setPos(position+glm::vec3(0.0f,0.0f,(BLOCKSCALE*8.0f)/2));
		containedObj->setTMatrix(glm::rotate(glm::mat4(), PI/4 ,glm::vec3(1.0f,1.0f,1.0f)), Rotate);
		containedObj->setTMatrix(glm::scale(glm::mat4(), glm::vec3(8.0f,8.0f,8.0f)), Scale);
	}
}

void WindowSlot::Draw(){
	Window::Draw();
	if(containedObj != nullptr){
		containedObj->setTMatrix(glm::rotate(containedObj->getTMatrix(Rotate), PI/20 ,glm::vec3(0.0f,1.0f,0.0f)), Rotate);
		containedObj->Draw();
	}
}

void WindowSlot::Interact(){
	baseItem * tempb = State.getHItem();
	// std::cout << "Interaction: " << std::endl;
	// if(tempb == nullptr)
	// 	std::cout << "\tHeld Item is NULL" << std::endl;
	// if(containedObj == nullptr)
	// 	std::cout << "\tContained Item is NULL" << std::endl;
	State.setHItem(containedObj);
	setObj(tempb);
}

glm::vec2 arrayBounds(int x, int y, float maxx, float maxy){
	maxx = floor(maxx);
	maxy = floor(maxy);
	//std::cout << "X:" << x << std::endl;
	//std::cout << "y:" << y << std::endl;
	//std::cout << "MaxX:" << maxx << std::endl;
	//std::cout << "Maxy:" << maxy << std::endl;
	if(x == 0 && y == 0)
		return glm::vec2(0.0f,0.0f);
	else if(x == maxx && y == 0)
		return glm::vec2(1.0f, 0.0f);
	else if(x == 0 && y == maxy)
		return glm::vec2(0.0f,1.0f);
	else if(x == maxx && y == maxy)
		return glm::vec2(1.0f, 1.0f);
	else if(x == 0)
		return glm::vec2(0.0f,0.5f);
	else if(x == maxx)
		return glm::vec2(1.0f, 0.5f);
	else if(y ==0)
		return glm::vec2(0.5f,0.0f);
	else if(y == maxy)
		return glm::vec2(0.5f,1.0f);
	return glm::vec2(0.5f,0.5f);
}
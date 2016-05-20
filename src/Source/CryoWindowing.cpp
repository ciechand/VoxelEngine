#include "../Headers/CryoBase.hpp"

Window::BaseWindow():OBJ(){
	parent = nullptr;
	modelIdentifier = modelNamesFind("Plane");
	hidden = true;
	depth = 0;
	textureSize = glm::vec2(3.0f,3.0f);
	setPos(glm::vec3(0.0f,0.0f,-0.01f));
	setSize(glm::vec2(100.0f,100.0f));
}

Window::BaseWindow(glm::vec2 tsize, glm::vec2 pos, glm::vec2 wsize, bool hide, Window * p):OBJ(){
	modelIdentifier = modelNamesFind("Plane");
	hidden = hide;
	textureSize = tsize;
	parent = p;

	if(parent != nullptr)
		depth = parent->getDepth()+1;
	else
		depth = 0;

	setSize(wsize);
	setPos(glm::vec3(pos,-0.01));


	if(tsize.x != 1.0f || tsize.y != 1.0f){
		for(int i=0; i<wsize.x/(TEXDIMS*10); i++){
			for(int j=0; j<wsize.y/(TEXDIMS*10); j++){
				windowDraw.emplace_back(glm::vec2(1.0f,1.0f), glm::vec2(100.0f+(i*((TEXDIMS*10))), 100.0f+(j*((TEXDIMS*10)))), glm::vec2((TEXDIMS*10), (TEXDIMS*10)), false);
				glm::vec2 preOff = arrayBounds(i,j,(wsize.x/(TEXDIMS*10)),(wsize.y/(TEXDIMS*10)));
				glm::vec2 Offset = glm::vec2(floor(preOff.x*(tsize.x-1)),(tsize.y-1.0f-floor(preOff.y*(tsize.y-1))));
				windowDraw[windowDraw.size()-1].setTOff(Offset);
			}
		}
		addSlots(50,glm::vec2(0.0f,(size.y)/2));
		addPane(glm::vec2(size.x-(SLOTSEPSIZE*4), (SLOTSEPSIZE*4)), ExitP);
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

void Window::setSize(glm::vec2 s){
	sf::Vector2u wsize = mainWindow.getSize();
	glm::vec2 ds(fmod(s.x,TEXDIMS),fmod(s.y,TEXDIMS));
	glm::vec2 newSize((s.x-ds.x)/wsize.x, (s.y-ds.y)/wsize.y);
	size = s-ds;
	setTMatrix(glm::scale(glm::mat4(),glm::vec3(newSize.x/2,newSize.y/2,1.0f)), Scale);
}

unsigned int Window::getDepth(){
	return depth;
}

void Window::setDepth(unsigned int d){
	depth = d;
}

void Window::setPos(glm::vec3 pos){
	sf::Vector2u wsize = mainWindow.getSize();
	baseObj::setPos(glm::vec3((((pos.x/wsize.x)+(size.x/(2*wsize.x))))-1.0f,1.0f-(((pos.y/wsize.y)+(size.y/(2*wsize.y)))),pos.z-(depth*0.0001)));
	position = pos;
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
	if(pt == ExitP)
		paneSize*=0.75f;
	int psize = Panes.size();
	Panes.emplace_back(glm::vec2(1.0f,1.0f), glm::vec2(position)+pos, glm::vec2(paneSize, paneSize), false, this);
	Panes[psize].setType(pt);
	Panes[psize].setID(psize);
	Panes[psize].setTOff(glm::vec2(2.0f,3.0f));

	//std::cout << "Position of Pane: \n\tX: "<< (glm::vec2(position)+pos).x <<  "\n\tY: "<< (glm::vec2(position)+pos).y << std::endl;
}

Pane& Window::getPane(int index){
	return Panes[index];
}

std::vector<Pane> & Window::getPane(){
	return Panes;
}

void Window::addSlots(unsigned int count, glm::vec2 spos){
	sf::Vector2u wsize = mainWindow.getSize();
	glm::vec2 trueWSize = glm::vec2(size.x,size.y);
	glm::vec2 trueWPos = glm::vec2(spos.x,spos.y);
	int xSlots = (trueWSize.x+SLOTSEPSIZE)/(DEFAULTSLOTSIZE+SLOTSEPSIZE);
	int ySlots = ((trueWSize.y-trueWPos.y)+SLOTSEPSIZE)/(DEFAULTSLOTSIZE+SLOTSEPSIZE);

	float xSize = (DEFAULTSLOTSIZE*(xSlots))+(SLOTSEPSIZE*(xSlots-1));
	float ySize = (DEFAULTSLOTSIZE*(ySlots))+(SLOTSEPSIZE*(ySlots-1));

	if(count > xSlots*ySlots)
		count = xSlots*ySlots;

	for(float j=0; j<ySlots; j+=1.0f){
		for(float i=0; i<xSlots; i+=1.0f){
			addSlot(glm::vec2((trueWSize.x/2)+((DEFAULTSLOTSIZE*i)+(SLOTSEPSIZE*(i+3)))-(xSize/2), (trueWSize.y)-(ySize)+(((DEFAULTSLOTSIZE*j)+(SLOTSEPSIZE*(j+3))))));
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
	addSlot(glm::vec2(size.x/2,size.y/2));
}

void Window::addSlot(glm::vec2 pos){
	int slotsize = Slots.size();
	Slots.emplace_back(glm::vec2(position)+pos, this);
	Slots[slotsize].setID(slotsize);
}

Slot& Window::getSlot(int index){
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

void Window::Draw(){
	if(windowDraw.size() == 0){
		IOBJ * tempOBJ = Renderer.getIObject(modelIdentifier);

		glBindVertexArray(tempOBJ->getVertexArrayObject());

		glBindBuffer(GL_ARRAY_BUFFER, tempOBJ->getVertexBufferObject(2));
		glBufferData(GL_ARRAY_BUFFER, sizeof(baseObj), this, GL_DYNAMIC_DRAW);

		std::vector<GLuint> ind = tempOBJ->getIndices();

		glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
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
}

WindowPane::WindowPane():Window(){

}

WindowPane::WindowPane(glm::vec2 tsize, glm::vec2 pos, glm::vec2 wsize, bool hide, Window * p):Window(tsize,  pos,  wsize, hide, p){
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

WindowSlot::WindowSlot(glm::vec2 pos, Window* p):Pane(glm::vec2(1.0f,1.0f),  pos,  glm::vec2(DEFAULTSLOTSIZE, DEFAULTSLOTSIZE), false, p){
	setTOff(glm::vec2(0.0f,3.0f));
}

WindowSlot::~WindowSlot(){

}

baseItem *  WindowSlot::getObj(){
	return containedObj;
}

void  WindowSlot::setObj(baseItem * b){
	containedObj = b;
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
#include "../Headers/CryoBase.hpp"


//Function that will process the mouse inputs.
void processMouseClicks(sf::Event e){
	Player self = State.getPlayer(0);
	Block * selec = self.getSelected();
	//std::cerr << "Block: " << World[0]->grid[10]->getPos().x << " : " << World[0]->grid[10]->getPos().y << std::endl;
	if(selec == nullptr)
		return;
	if(e.mouseButton.button == sf::Mouse::Left){
		IOBJ * tempOBJ = Renderer.getIObject(selec->getMID());
		if(tempOBJ == nullptr)
			return;
		//std::cerr << std::endl;
		//std::cerr << "Outside Address of OBJ: " << tempOBJ << std::endl;
		glm::vec3 bpos = selec->getPos();
		glm::vec2 chunk = glm::vec2(floor(bpos.x/(BLOCKSCALE*CHUNKDIMS)),floor(bpos.z/(BLOCKSCALE*CHUNKDIMS)));
		glm::vec3 relpos = glm::vec3(((bpos.x)/BLOCKSCALE)-(chunk.x*CHUNKDIMS),bpos.y/BLOCKSCALE, ((bpos.z)/BLOCKSCALE)-(chunk.y*CHUNKDIMS));
		//std::cerr << "Chunk: " << chunk.x << ": " << chunk.y << ";\n\tPos: " << relpos.x << ": " << relpos.y << ": " << relpos.z << ";" << std::endl; 
		for(int i=0; i<World.size(); i++){
			if(World[i]->getPosition() == chunk){
				if(World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x] == nullptr)
					return;
				if(World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x]->getID() != selec->getID())
					std::cerr << "ID's DO NOT MATCH!" << std::endl;
				//std::cerr << "Address of world being removed: " << World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x] << std::endl;
				World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x] = nullptr;	
			}
		}
		if(selec->getOwner() == 0 || selec->getOwner() == -1){
			tempOBJ->removeBlock(selec->getID());
		}
		self.clearSelected();
		self.removeSelect(Selector);
	}
	if(e.mouseButton.button == sf::Mouse::Right){
		int side = self.getSelectedSide();
		//std::cerr << side << std::endl;
		if(side != -1){
			glm::mat4 trans = glm::mat4();
			if(side == 0)
				trans = glm::translate(trans, glm::vec3(-1.0f*BLOCKSCALE,0.0f,0.0f));
			if(side == 1)
				trans = glm::translate(trans, glm::vec3(1.0f*BLOCKSCALE,0.0f,0.0f));
			if(side == 2)
				trans = glm::translate(trans, glm::vec3(0.0f,-1.0f*BLOCKSCALE,0.0f));
			if(side == 3)
				trans = glm::translate(trans, glm::vec3(0.0f,1.0f*BLOCKSCALE,0.0f));
			if(side == 4)
				trans = glm::translate(trans, glm::vec3(0.0f,0.0f,-1.0f*BLOCKSCALE));
			if(side == 5)
				trans = glm::translate(trans, glm::vec3(0.0f,0.0f,1.0f*BLOCKSCALE));
			trans = trans * selec->getTMatrix();
			self.removeSelect(Highlight);
			self.addSelect(trans, Green, Highlight);
		}
	}
	State.setPlayer(0, self);
}

//function that will process the passive mouse movement.
void processMouseMovement(sf::Event e){
	if(State.getState() != Menu){
		sf::Vector2u windowSize = mainWindow.getSize();
		glm::vec3 camRot = Options.getCamRot();

		float rotationPer[2] = {(((e.mouseMove.x-((int)windowSize.x/2))/((float)windowSize.x/2.0f))*LOOKSPEED), (((e.mouseMove.y-((int)windowSize.y/2))/((float)windowSize.y/2.0f))*LOOKSPEED)}; 

		camRot[0] += rotationPer[0];
		camRot[1] += rotationPer[1];

		(camRot[0]>1.0f)?camRot[0] = -1.0f:(camRot[0]<0.0f)?camRot[0] +=1.0f:camRot[0]=camRot[0];	
		(camRot[1]>1.0f)?camRot[1] = 0.9999f:(camRot[1]<0.0f)?camRot[1] = 0.0001f:camRot[1]=camRot[1];

		rotationPer[0] = camRot[0]*PI*2;
		rotationPer[1] = (camRot[1]*PI)+PI/2;

 		glm::vec3 camAt = Renderer.getCamVec(1);
 		glm::vec3 rotations = glm::vec3(-sin(rotationPer[0])*cos(rotationPer[1])*CAMERADIST, -sin(rotationPer[1])*CAMERADIST, cos(rotationPer[0])*cos(rotationPer[1])*CAMERADIST);
 		//std::cout << "Rotation vector:\n X: " << rotations.x << "\n Y: " << rotations.y << "\n Z: " << rotations.z << std::endl;
 		glm::vec3 camUp = Renderer.getCamVec(2);
 		Renderer.setViewMatrix(glm::lookAt(camAt+rotations, camAt+glm::vec3(0.0f,0.0f,0.0f), camUp));

 		Renderer.setCamVec(0, camAt+rotations);

		Options.setCamRot(camRot);

	}
}
//Function for processing the keyboard inputs on press.
void processKeyboardDown(sf::Event e){
	switch (e.key.code){
		case sf::Keyboard::Escape:	
			State.setState(Exiting);
			break;
		case sf::Keyboard::W:	
			State.setMoving(Forward, true);
			break;
		case sf::Keyboard::S:	
			State.setMoving(Backward, true);
			break;
		case sf::Keyboard::A:	
			State.setMoving(MLeft, true);
			break;
		case sf::Keyboard::D:	
			State.setMoving(MRight, true);
			break;
		case sf::Keyboard::E:	
			State.setMoving(Down, true);
			break;
		case sf::Keyboard::Q:	
			State.setMoving(Up, true);
			break;
		case sf::Keyboard::R:
			(State.getState() != Menu)? State.setState(Menu):State.setState(Loading);
			break;
		default:
			break;
	}
}
//Function for processing the keyboard inputs on Release.
void processKeyboardUp(sf::Event e){
	switch (e.key.code){
		case sf::Keyboard::W:	
			State.setMoving(Forward, false);
			break;
		case sf::Keyboard::S:	
			State.setMoving(Backward, false);
			break;
		case sf::Keyboard::A:	
			State.setMoving(MLeft, false);
			break;
		case sf::Keyboard::D:	
			State.setMoving(MRight, false);
			break;
		case sf::Keyboard::E:	
			State.setMoving(Down, false);
			break;
		case sf::Keyboard::Q:	
			State.setMoving(Up, false);
			break;
		default:
			break;
	}
}


void processMovement(){
	sf::Time curTime = masterClock.getElapsedTime();
	sf::Time lastTime = State.getTime();
	float deltaTime = curTime.asSeconds() - lastTime.asSeconds();
	//std::cout << "Cur: " << curTime.asSeconds() << "\n\tLast: " << lastTime.asSeconds() << "\n\tDelta: " << deltaTime << std::endl;
	State.setTime(curTime);
	//std::cout << "Start Movement Process." << std::endl;
	glm::vec3 camPos = Renderer.getCamVec(0);
	glm::vec3 camAt = Renderer.getCamVec(1);
	glm::vec3 camUp = Renderer.getCamVec(2);
	glm::vec3 camRot = Options.getCamRot();
	std::vector<bool> movement = State.getMoving();
	glm::vec3 cross = glm::cross(camAt-camPos, camUp);
	if(cross == glm::vec3(0.0f))
		cross = glm::cross(camAt-camPos, glm::vec3(camAt.x-camPos.x,0.0f, camAt.z-camPos.z));
	if(movement[Forward] == true){
		camAt += glm::normalize(glm::vec3(camAt.x-camPos.x,0.0f,camAt.z-camPos.z))*(MOVESPEED*deltaTime*TIMECOMPENSATE);
	}
	if(movement[Backward] == true){
		camAt -= glm::normalize(glm::vec3(camAt.x-camPos.x,0.0f,camAt.z-camPos.z))*(MOVESPEED*deltaTime*TIMECOMPENSATE);
	}
    if(movement[MLeft] == true){
		camAt -= glm::normalize(cross)*(MOVESPEED*deltaTime*TIMECOMPENSATE);
	}
	if(movement[MRight] == true){
		camAt += glm::normalize(cross)*(MOVESPEED*deltaTime*TIMECOMPENSATE);
	}
	if(movement[Down] == true){
		camAt -= camUp*(MOVESPEED*deltaTime*TIMECOMPENSATE);
	}
	if(movement[Up] == true){
		camAt += camUp*(MOVESPEED*deltaTime*TIMECOMPENSATE);
	}
	Renderer.setCamVec(1, camAt);
	Player self = State.getPlayer(0);
	self.setPos(camAt);
	State.setPlayer(0, self);
}


void windowResized(sf::Event e){
	sf::Vector2u windowSize = mainWindow.getSize();
	glViewport(0,0,e.size.width, e.size.height);
}

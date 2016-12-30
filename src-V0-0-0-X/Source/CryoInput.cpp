#include "../Headers/CryoBase.hpp"


//Function that will process the mouse inputs.
void processMouseClicks(sf::Event e){
	Player * self = State.getPlayer(0);
	unsigned char curState = State.getState();
	if(e.mouseButton.button == sf::Mouse::Left){

		baseItem * curHotbarItem = self->getItem(self->getHSlot());
		unsigned int tempIType = NullItemType;
		if(curHotbarItem != nullptr) 
			tempIType = curHotbarItem->getItemType();

		if(curState == Running && tempIType == DestructionTool){
			Block * selec = self->getSelected();
			if(selec == nullptr)
				return;
			IOBJ * tempOBJ = Renderer.getIObject(selec->getMID());
			if(tempOBJ == nullptr)
				return;
			glm::vec3 bpos = selec->getPos();
			glm::vec2 chunk = glm::vec2(floor(bpos.x/(BLOCKSCALE*CHUNKDIMS)),floor(bpos.z/(BLOCKSCALE*CHUNKDIMS)));
			glm::vec3 relpos = glm::vec3(((bpos.x)/BLOCKSCALE)-(chunk.x*CHUNKDIMS),bpos.y/BLOCKSCALE, ((bpos.z)/BLOCKSCALE)-(chunk.y*CHUNKDIMS));
			//std::cerr << "Chunk: " << chunk.x << ": " << chunk.y << ";\n\tPos: " << relpos.x << ": " << relpos.y << ": " << relpos.z << ";" << std::endl; 
			for(int i=0; i<World.size(); i++){
				if(World[i]->getPos() == chunk){
						//std::cerr << "Address of world being removed: " << World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x] << std::endl;
						if((selec->getOwner() == 0 || selec->getOwner() == -1) && selec->getPos().y >0.0f){
							tempOBJ->removeBlock(selec->getID());
							World[i]->grid[(relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x] = std::make_pair(0,0);
							break;
						}	
				}	
			}
			//std::cerr << "Left Clicked" << std::endl;
			self->removeSelector(self->getSelected());
			self->clearSelected();
		}else if(curState == Menu){
			Pane * curSPane = State.getSPane();
			if(curSPane != nullptr){
				curSPane->Interact();
			}
		}
	}else if(e.mouseButton.button == sf::Mouse::Right){

		baseItem * curHotbarItem = self->getItem(self->getHSlot());
		unsigned int tempIType = NullItemType;
		if(curHotbarItem != nullptr) 
			tempIType = curHotbarItem->getItemType();
		
		if(curState == Running  && tempIType == Placeable){
			Block * selec = self->getSelected();
			//std::cerr << "Block: " << World[0]->grid[10]->getPos().x << " : " << World[0]->grid[10]->getPos().y << std::endl;
			if(selec == nullptr)
				return;
			IOBJ * tempOBJ = Renderer.getIObject(selec->getMID());
			glm::vec3 blockPos = selec->getPos();
			int side = self->getSelectedSide();
			glm::vec3 newPos =  blockPos + DirectionalVectors[side];

			if(newPos.y >= (CHUNKHEIGHT-1)*BLOCKSCALE || newPos.y < 0.0f || glm::length(newPos - self->getPos()) <= BLOCKSCALE)
				return;
			glm::vec2 chunk = glm::vec2(floor(newPos.x/(BLOCKSCALE*CHUNKDIMS)),floor(newPos.z/(BLOCKSCALE*CHUNKDIMS)));
			glm::vec3 relpos = glm::vec3(((newPos.x)/BLOCKSCALE)-(chunk.x*CHUNKDIMS),newPos.y/BLOCKSCALE, ((newPos.z)/BLOCKSCALE)-(chunk.y*CHUNKDIMS));
			
			for(int i=0; i<World.size(); i++){
				if(World[i]->getPos() == chunk){
					int curBlock = (relpos.y*CHUNKDIMS*CHUNKDIMS)+(relpos.z*CHUNKDIMS)+relpos.x;
					Block * blockp = tempOBJ->getBlocks(World[i]->grid[curBlock].second);
					if(blockp != nullptr)
						break;

					tempOBJ->addBlocks();
					World[i]->grid[curBlock].second = tempOBJ->getBlocksSize()-1;
					blockp = tempOBJ->getBlocks(World[i]->grid[curBlock].second);
					blockp->setID(World[i]->grid[curBlock].second);
					blockp->setTOff(1);
					blockp->setColor(None);
					blockp->setPos(newPos);
					break;
				}
			}
		}
	}else if(e.mouseButton.button == sf::Mouse::Middle){
		if(curState == Running){
			Block * selec = self->getSelected();
			if(selec == nullptr)
				return;
			int col = selec->getColorNumber();
			col++;
			if(col>=16)
				col = col%16;
			selec->setColor(col);
		}
	}
}

//function that will process the passive mouse movement.
void processMouseMovement(sf::Event e){
	if(State.getState() == Running){
		sf::Vector2u windowSize = mainWindow.getSize();
		glm::vec3 camRot = Options.getCamRot();

		std::vector<float> rotationPer = {(((e.mouseMove.x-((int)windowSize.x/2))/((float)windowSize.x/2.0f))*LOOKSPEED), (((e.mouseMove.y-((int)windowSize.y/2))/((float)windowSize.y/2.0f))*LOOKSPEED)}; 

		camRot[0] += rotationPer[0];
		camRot[1] += rotationPer[1];

		(camRot[0]>1.0f)?camRot[0] = -1.0f:(camRot[0]<0.0f)?camRot[0] +=1.0f:camRot[0]=camRot[0];	
		(camRot[1]>1.0f)?camRot[1] = 0.9999f:(camRot[1]<0.0f)?camRot[1] = 0.0001f:camRot[1]=camRot[1];

		rotationPer[0] = camRot[0]*PI*2;
		rotationPer[1] = (camRot[1]*PI)+PI/2;

 		glm::vec3 camAt = Renderer.getCamVec(1);
 		glm::vec3 rotations = glm::vec3(-sin(rotationPer[0])*cos(rotationPer[1])*CAMERADIST, -sin(rotationPer[1])*CAMERADIST, cos(rotationPer[0])*cos(rotationPer[1])*CAMERADIST);
 		//std::cerr << "Rotation vector:\n X: " << rotations.x << "\n Y: " << rotations.y << "\n Z: " << rotations.z << std::endl;
 		glm::vec3 camUp = Renderer.getCamVec(2);
 		Renderer.setViewMatrix(glm::lookAt(camAt+rotations, camAt, camUp));

 		Renderer.setCamVec(0, camAt+rotations);

		Options.setCamRot(camRot);

	}
}

void processMouseWheelMovement(sf::Event e){
	Player * self = State.getPlayer(0);
	unsigned char curState = State.getState();
	int ticks = e.mouseWheel.delta;
	if(ticks<0){
		self->incrementHSlot(abs(ticks));
	}else if(ticks>0){
		self->decrementHSlot(abs(ticks));
	}
}

//Function for processing the keyboard inputs on press.
void processKeyboardDown(sf::Event e){
	unsigned char curState = State.getState();
	switch (e.key.code){
		case sf::Keyboard::Escape:
			State.setState(Exiting);
			break;
		case sf::Keyboard::W:
			if(curState == Running)
				State.setMoving(Forward, true);
			break;
		case sf::Keyboard::S:
			if(curState == Running)
				State.setMoving(Backward, true);
			break;
		case sf::Keyboard::A:
			if(curState == Running)
				State.setMoving(MLeft, true);
			break;
		case sf::Keyboard::D:
			if(curState == Running)
				State.setMoving(MRight, true);
			break;
		case sf::Keyboard::Space:
			if(curState == Running)
				State.setMoving(Up, true);
			break;
		case sf::Keyboard::E:
			if(State.getFlags(InvDown) == false){
				if(State.getFlags(OpenInv) == false){
					for(int i=0; i<NUMBER_OF_MOVEMENT; i++){
						State.setMoving(i,false);
					}
					State.setFlags(OpenInv, true);
					State.setState(Menu);
					std::vector<Window> & winds = Renderer.getWindows();
					for(Window & w:winds){
						if(w.getWType() == PInv)
							w.setHidden(false);
					}
				}else{
					State.setFlags(OpenInv, false);
					State.setState(Running);
					Window * selw = State.getSWindow();
					if(selw !=nullptr){
						selw->setColor(None);
						State.setSWindow(nullptr);
					}
					Pane * p = State.getSPane();
					if(p !=nullptr){
						p->setColor(None);
						State.setSPane(nullptr);
					}
					std::vector<Window> & winds = Renderer.getWindows();
					for(Window & w:winds){
						if(w.getWType() == PInv)
							w.setHidden(true);
					}
				}
				State.setFlags(InvDown, true);
			}
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
		case sf::Keyboard::Space:	
			State.setMoving(Up, false);
			break;
		case sf::Keyboard::E:
			State.setFlags(InvDown, false);
		default:
			break;
	}
}


void processMovement(){
//std::cerr << std::endl;
//std::cerr << "Beginning of Everything" << std::endl;
	float deltaTime = State.getdTime();
	Player * self = State.getPlayer(0);
	//std::cerr << "Start Movement Process." << std::endl;
	glm::vec3 camPos = Renderer.getCamVec(0);
	glm::vec3 camAt = Renderer.getCamVec(1);
	glm::vec3 camUp = Renderer.getCamVec(2);
	glm::vec3 camRot = Options.getCamRot();
	std::vector<bool> movement = State.getMoving();
	glm::vec3 cross = glm::cross(camAt-camPos, camUp);
	glm::vec3  playerVel = self->getVelocity();
	glm::vec3 vel = glm::vec3(0.0f, playerVel.y, 0.0f);

	bool jump = self->getJumping();

	glm::vec3 ppos = self->getPos();

	glm::vec3 tempMove = ppos;
	vel.y += GRAVITY*deltaTime;
	if(cross == glm::vec3(0.0f))
		cross = glm::cross(ppos-camPos, glm::vec3(ppos.x-camPos.x,0.0f, ppos.z-camPos.z));
	if(movement[Forward] == true){
		vel += glm::normalize(glm::vec3(ppos.x-camPos.x,0.0f,ppos.z-camPos.z))*MOVESPEED*deltaTime;
	}
	if(movement[Backward] == true){
		vel -= glm::normalize(glm::vec3(ppos.x-camPos.x,0.0f,ppos.z-camPos.z))*MOVESPEED*deltaTime;
	}
	if(movement[MLeft] == true){
		vel -= glm::normalize(cross)*MOVESPEED*deltaTime;
	}
	if(movement[MRight] == true){
		vel += glm::normalize(cross)*MOVESPEED*deltaTime;
	}
	if(movement[Up] == true && jump == false){
	//std::cerr << "Jumping" << std::endl;
		vel.y += (-GRAVITY)*0.5f;
		jump = true;
	}

	if(vel.y > TVEL)
		vel.y = TVEL;
	if(vel.y < -TVEL)
		vel.y = -TVEL;
	if(vel.x > TVEL)
		vel.x = TVEL;
	if(vel.x < -TVEL)
		vel.x = -TVEL;
	if(vel.z > TVEL)
		vel.z = TVEL;
	if(vel.z < -TVEL)
		vel.z = -TVEL;

	tempMove += vel;
	//camAt += vel;
	//std::cerr << "Vel Before: \n\tX: "<< vel.x <<  "\n\tY: "<< vel.y <<  "\n\tZ: "<< vel.z << std::endl;

	BoundingBox B = self->getBounds();
	glm::vec3 moveTime(-1.0f);
	int axis = -1;

	if(tempMove.y-B.getLimbs().y <= ((CHUNKHEIGHT-1)*BLOCKSCALE)+(BLOCKSCALE/2) && tempMove.y+B.getLimbs().y >= -(BLOCKSCALE/2))
		moveTime = playerCollideTerrain(B, vel, axis);

	//std::cerr << "moveTime: \n\tX: "<< moveTime.x <<  "\n\tY: "<< moveTime.y <<  "\n\tZ: "<< moveTime.z << std::endl;
	//std::cerr << moveTime << std::endl;

	//glm::vec3 prevVel = vel;
	if(moveTime != glm::vec3(-1.0f)){
		if((moveTime.y >= -(float)std::numeric_limits<float>::min() && moveTime.y <= (float)std::numeric_limits<float>::min()) && vel.y < 0.0f){
			//std::cerr << "jumpReady" << std::endl;
			jump = false;
		}
		vel *= moveTime;
	}
	camAt += vel;
	self->setVelocity(vel);
	//std::cerr << "Vel After: \n\tX: "<< vel.x <<  "\n\tY: "<< vel.y <<  "\n\tZ: "<< vel.z << std::endl;
	//std::cerr << "Position of Char: \n\tX: "<< camAt.x <<  "\n\tY: "<< camAt.y <<  "\n\tZ: "<< camAt.z << std::endl;

	self->setJumping(jump);
	self->setPos(camAt);
	IOBJ * tempOBJ = Renderer.getIObject(PlayerPos.first);
	Block * bp = tempOBJ->getBlocks(PlayerPos.second);
	bp->setPos(camAt);

	camRot[0] = camRot[0]*PI*2;
	camRot[1] = (camRot[1]*PI)+PI/2;
	
	glm::vec3 rotations = glm::vec3(-sin(camRot[0])*cos(camRot[1])*CAMERADIST, -sin(camRot[1])*CAMERADIST, cos(camRot[0])*cos(camRot[1])*CAMERADIST);
	Renderer.setViewMatrix(glm::lookAt(camAt+rotations, camAt, camUp));

	Renderer.setCamVec(0, camAt+rotations);

	Renderer.setCamVec(1, camAt);

}


void windowResized(sf::Event e){
	Options.setProjVars(1, (float)e.size.width/(float)e.size.height);
	State.setProjChange(true);
	glViewport(0,0,e.size.width, e.size.height);
}

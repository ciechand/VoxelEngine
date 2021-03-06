#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoBase.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoInput.hpp"
#include "../Headers/CryoLighting.hpp"

bool mouseBound = true;

void processKeyPress(sf::Event event){
	glm::vec3 ca = Camera.getCamAt();
	glm::vec3 cp = Camera.getCamPos();
	glm::vec3 cu = Camera.getCamUp();
	glm::vec3 cv = ca-cp;
	switch(event.key.code){
		case sf::Keyboard::Escape:
			MainController.setCurrentGameState(GameExiting);
			break;
		case sf::Keyboard::K:
			mouseBound = !mouseBound;
			break;
		case sf::Keyboard::F:
			lightController.incRotation();
			break;
		case sf::Keyboard::L:
			lightController.addLight(glm::vec4(ca,1.0),BlockColors[rand()%16],50);
			ShaderController.lightingInit();
		default:
			break;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		glm::vec3 temp = glm::normalize(glm::vec3(cv.x,0.0,cv.z));
		ca += temp;
		printf("Forward Vector:\nX: %f\nY: %f\nZ: %f\n", temp.x, temp.y, temp.z);
	}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
		ca -= glm::normalize(glm::vec3(cv.x,0.0,cv.z));
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
		ca -= glm::normalize(glm::cross(glm::vec3(cv.x,0.0,cv.z),cu));
	}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
		ca += glm::normalize(glm::cross(glm::vec3(cv.x,0.0,cv.z),cu));
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){	
		ca += cu;
	}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
		ca -= cu;
	}

	Camera.setCamAt(ca);
	Camera.setCamPos(ca+Camera.getCamFRot());
	//std::cerr << "Camera Pos: \n\tx: " << ShaderController.cameraPos.x << "\n\ty: " << ShaderController.cameraPos.y << "\n\tz: " << ShaderController.cameraPos.z << std::endl;
}

void processMouseMove(sf::Event event){
		if(!mouseBound)
			return;
		sf::Vector2u windowSize = MainWindow.getSize();
		glm::vec3 camRot = Camera.getCamRot();

		std::array<float, 2> rotationPer = {(((event.mouseMove.x-((int)windowSize.x/2))/((float)windowSize.x/2.0f))*LOOKSPEED), (((event.mouseMove.y-((int)windowSize.y/2))/((float)windowSize.y/2.0f))*LOOKSPEED)}; 

		camRot[0] += rotationPer[0];
		camRot[1] += rotationPer[1];

		(camRot[0]>1.0f)?camRot[0] -= 1.0f:(camRot[0]<0.0f)?camRot[0] +=1.0f:camRot[0]=camRot[0];	
		(camRot[1]>1.0f)?camRot[1] = 0.999f:(camRot[1]<0.0f)?camRot[1] = 0.001f:camRot[1]=camRot[1];

		rotationPer[0] = camRot[0]*PI*2;
		rotationPer[1] = (camRot[1]*PI)+PI/2;

 		glm::vec3 rotations = glm::vec3(-sin(rotationPer[0])*cos(rotationPer[1])*CAMERADIST, -sin(rotationPer[1])*CAMERADIST, cos(rotationPer[0])*cos(rotationPer[1])*CAMERADIST);
 		//std::cerr << "Rotation vector:\n X: " << camRot.x << "\n Y: " << camRot.y << "\n Z: " << camRot.z << std::endl;
 		Camera.setCamPos(Camera.getCamAt()+rotations);
 		//std::cerr << " Camera Pos: \n\tX:" << (Camera.getCamAt()+rotations).x << "\n\tY: " << (Camera.getCamAt()+rotations).y << "\n\tZ: " << (Camera.getCamAt()+rotations).z << std::endl;

 		Camera.setCamFRot(rotations);
		Camera.setCamRot(camRot);
}

void processMouseClicks(sf::Event Event){
	//process mouse click by ray tracing and figure out what block face it intersects
}


void processResize(sf::Event event){
	glViewport(0,0,event.size.width, event.size.height);

}
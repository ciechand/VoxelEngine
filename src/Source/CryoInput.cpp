#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoBase.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoInput.hpp"


void processKeyPress(sf::Event event){
	glm::vec3 cp = ShaderController.getCamPos();
	switch(event.key.code){
		case sf::Keyboard::Escape:
			MainController.setCurrentGameState(GameExiting);
			break;
		case sf::Keyboard::S:
			cp.z -= 0.2f;
			break;
		case sf::Keyboard::W:
			cp.z += 0.2f;
			break;
		case sf::Keyboard::A:
			cp.x -= 0.2f;
			break;
		case sf::Keyboard::D:
			cp.x += 0.2f;
			break;
		case sf::Keyboard::Q:
			cp.y -= 0.2f;
			break;
		case sf::Keyboard::E:
			cp.y += 0.2f;
			break;
		default:
			break;
	}
	ShaderController.setCamPos(cp);
	//std::cerr << "Camera Pos: \n\tx: " << ShaderController.cameraPos.x << "\n\ty: " << ShaderController.cameraPos.y << "\n\tz: " << ShaderController.cameraPos.z << std::endl;
}

void processMouseMove(sf::Event event){
		sf::Vector2u windowSize = MainWindow.getSize();
		glm::vec3 camRot = ShaderController.getCamRot();

		std::array<float, 2> rotationPer = {(((event.mouseMove.x-((int)windowSize.x/2))/((float)windowSize.x/2.0f))*LOOKSPEED), (((event.mouseMove.y-((int)windowSize.y/2))/((float)windowSize.y/2.0f))*LOOKSPEED)}; 

		camRot[0] += rotationPer[0];
		camRot[1] += rotationPer[1];

		(camRot[0]>1.0f)?camRot[0] = -1.0f:(camRot[0]<0.0f)?camRot[0] +=1.0f:camRot[0]=camRot[0];	
		(camRot[1]>1.0f)?camRot[1] = 0.9999f:(camRot[1]<0.0f)?camRot[1] = 0.0001f:camRot[1]=camRot[1];

		rotationPer[0] = camRot[0]*PI*2;
		rotationPer[1] = (camRot[1]*PI)+PI/2;

 		glm::vec3 camAt = glm::vec3(0.0f,0.0f,0.0f); //Store this value somewhere.
 		glm::vec3 rotations = glm::vec3(-sin(rotationPer[0])*cos(rotationPer[1])*CAMERADIST, -sin(rotationPer[1])*CAMERADIST, cos(rotationPer[0])*cos(rotationPer[1])*CAMERADIST);
 		//std::cerr << "Rotation vector:\n X: " << rotations.x << "\n Y: " << rotations.y << "\n Z: " << rotations.z << std::endl;
 		glm::vec3 camUp = glm::vec3(0.0f,1.0f,0.0f); //Store this value somewhere.
 		ShaderController.setViewMatrix(glm::lookAt(camAt+rotations, camAt, camUp));

		ShaderController.setCamRot(camRot);
}

void processResize(sf::Event event){
	glViewport(0,0,event.size.width, event.size.height);

}
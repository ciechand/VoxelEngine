#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoBase.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoInput.hpp"


void processKeyPress(sf::Event event){
	glm::vec3 ca = Camera.getCamAt();
	switch(event.key.code){
		case sf::Keyboard::Escape:
			MainController.setCurrentGameState(GameExiting);
			break;
		case sf::Keyboard::S:
			ca.z += WALKSPEED;
			break;
		case sf::Keyboard::W:
			ca.z -= WALKSPEED;
			break;
		case sf::Keyboard::A:
			ca.x -= WALKSPEED;
			break;
		case sf::Keyboard::D:
			ca.x += WALKSPEED;
			break;
		case sf::Keyboard::Q:
			ca.y -= WALKSPEED;
			break;
		case sf::Keyboard::E:
			ca.y += WALKSPEED;
			break;
		default:
			break;
	}
	Camera.setCamAt(ca);
	Camera.setViewMatrix(glm::lookAt(ca+Camera.getCamFRot(), ca, Camera.getCamUp()));
	//std::cerr << "Camera Pos: \n\tx: " << ShaderController.cameraPos.x << "\n\ty: " << ShaderController.cameraPos.y << "\n\tz: " << ShaderController.cameraPos.z << std::endl;
}

void processMouseMove(sf::Event event){
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
 		//std::cerr << "Rotation vector:\n X: " << rotations.x << "\n Y: " << rotations.y << "\n Z: " << rotations.z << std::endl;
 		Camera.setViewMatrix(glm::lookAt(Camera.getCamAt()+rotations, Camera.getCamAt(), Camera.getCamUp()));

 		Camera.setCamFRot(rotations);
		Camera.setCamRot(camRot);
}

void processResize(sf::Event event){
	glViewport(0,0,event.size.width, event.size.height);

}
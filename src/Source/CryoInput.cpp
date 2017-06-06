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

void processResize(sf::Event event){
	glViewport(0,0,event.size.width, event.size.height);

}
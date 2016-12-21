#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"

sf::Window MainWindow;
sf::Clock MainClock;

int main(int argc, char ** argv){
	if(DEBUGMODE)std::cerr << "Starting Program" << std::endl;
	//Firstly, We need to initiate SFML and create our main window.
	sf::ContextSettings setting;
	setting.depthBits = 24;
	MainWindow.create(sf::VideoMode(SCREENWIDTH,SCREENHEIGHT), "Cryonyx", sf::Style::Default, setting);

	if(DEBUGMODE)std::cerr << "Window Created" << std::endl;

	glewExperimental = GL_TRUE;
	glewInit();

	InitOpenGL();

	BasicRender();

	MainWindow.close();
	if(DEBUGMODE)std::cerr << "Program Complete" << std::endl;
	return 0;
}

void InitOpenGL(){
	if(DEBUGMODE)std::cerr << "Initializing OpenGL" << std::endl;
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(77.0f/256.0f,166.0f/255.0f,255.0f/255.0f,1.0f);
	if(DEBUGMODE)std::cerr << "OpenGL Init Complete" << std::endl;
}

void BasicRender(){	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	MainWindow.display();
}
#include "../Headers/CryoBase.hpp"

sf::Window mainWindow;
std::default_random_engine randomEng(time(0));

GRend Renderer;
OPT Options;
GState State;
Chunk testingChunk[9];

int main (int argc, char ** argv){
	//First things first, int sfml (The main windowing extention that I am using)
	sf::ContextSettings set;
	set.depthBits = 24;
	mainWindow.create(sf::VideoMode(1024,768), "Cryonyx", sf::Style::Default, set);

	std::cerr << "Starting Program" << std::endl; 

	//Create and Initiate the global options for the program, all the reading from file is done within the constructor.
	//Options = *new class GameOptions("./Cryoptions.config");

	//setting up glew
	glewExperimental = GL_TRUE; //This is so that experimental drivers will be checked as well.
	glewInit();

	InitOpenGL();

	while(State.getState() != Exiting){
		sf::Event event;
		while(mainWindow.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					mainWindow.close();
					return 0;
					break;
				case sf::Event::KeyPressed:
					processKeyboardDown(event);
					break;
				case sf::Event::KeyReleased:
					processKeyboardUp(event);
					break;
				case sf::Event::MouseButtonPressed:
					processMouseClicks(event);
					break;
				case sf::Event::MouseMoved:
					processMouseMovement(event);
					break;
				case sf::Event::Resized:
					windowResized(event);
					break;
				case sf::Event::LostFocus:
					State.setState(Menu);
					break;
				case sf::Event::GainedFocus:
					State.setState(Running);
					break;
				default:
					break;
			}
		}
		processMovement();
		display();
	}

	mainWindow.close();
	return 0;
}	
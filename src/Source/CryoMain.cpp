#include "../Headers/CryoBase.hpp"

sf::Window mainWindow;
std::default_random_engine randomEng(time(0));

std::pair<glm::uint, glm::uint> PlayerPos;
GRend Renderer;
OPT Options;
GState State;
std::vector<Chunk*> World;
sf::Clock masterClock;

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
	std::cerr << "InitComplete" << std::endl;
	while(State.getState() != Exiting){
		sf::Event event;
		while(mainWindow.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					State.setState(Exiting);
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
				case sf::Event::MouseWheelMoved:
					processMouseWheelMovement(event);
					break;
				case sf::Event::Resized:
					windowResized(event);
					break;
				case sf::Event::LostFocus:
					//This will need to be changed!
					State.setState(Menu);
					break;
				case sf::Event::GainedFocus:
					//This too.
					State.setState(Running);
					break;
				default:
					break;
			}
		}
		tickUpdate();
		//std::cout << "Movement Processing" << std::endl;
		processMovement();
		//std::cout << "Display" << std::endl;
		display();
	}
	for(int i=0; i<World.size(); i++){
		delete World[i];
	}

	mainWindow.close();
	return 0;
}	
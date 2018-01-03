#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoBase.hpp"
#include "../Headers/CryoInput.hpp"

sf::Window MainWindow;
sf::Clock MainClock;

std::vector<Chunk *> ChunkContainer;

int main(int argc, char ** argv){
	std::cerr << "Beginning of Program" << std::endl;

	if(DEBUGMODE)std::cerr << "Starting Program" << std::endl;

	//Firstly, We need to initiate SFML and create our main window.
	sf::ContextSettings setting;
	setting.depthBits = 24;
	MainWindow.create(sf::VideoMode(SCREENWIDTH,SCREENHEIGHT), "Cryonyx", sf::Style::Default, setting);

	if(DEBUGMODE)std::cerr << "Window Created" << std::endl;


	glewExperimental = GL_TRUE;
	glewInit();

	InitOpenGL();

	//THIS SECTION IS LOADING ALL THE DIFFERENT SHADERS USED
	if(DEBUGMODE)std::cerr << "Loading Base Vertex Shader Path" << std::endl;
	ShaderController.setVertexShaderPath(std::string("./Assets/Shaders/BaseVertexShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Base Fragment Shader Path" << std::endl;
	ShaderController.setFragmentShaderPath(std::string("./Assets/Shaders/BaseFragmentShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Base Shader Program" << std::endl;
	ShaderController.createNewShaderProgram();

	if(DEBUGMODE)std::cerr << "Loading Shadow Vertex Shader Path" << std::endl;
	ShaderController.setVertexShaderPath(std::string("./Assets/Shaders/ShadowVertexShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Shadow Fragment Shader Path" << std::endl;
	ShaderController.setFragmentShaderPath(std::string("./Assets/Shaders/ShadowFragmentShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Shadow Shader Program" << std::endl;
	ShaderController.createNewShaderProgram();
	
	if(DEBUGMODE)std::cerr << "Done Loading Shader Programs" << std::endl;
	//THIS IS THE END OF LOADING SHADERS

	//ShaderController.loadBaseMeshes();



	unsigned int iterations = 1;
	sf::Time t1 = MainClock.getElapsedTime();
	for(int i=6; i>(6-iterations); i--){
		if(DEBUGMODE == true){
			glm::vec3 tempV = glm::vec3(0,0,0)+DirectionVectors[i];
			std::cerr << "Setting up Chunk #" << i  << " at:  " << tempV.x << ", " <<tempV.y  << ", " << tempV.z << ";" << std::endl;
		}
		ChunkContainer.emplace_back(new Chunk(glm::vec3(0,0,0)+DirectionVectors[i]));
	}

	sf::Time t2 = MainClock.getElapsedTime();
	sf::Time dt = t2-t1;
	std::cerr << "Time to generate Chunks: " << dt.asSeconds() << std::endl;
	t1 = MainClock.getElapsedTime();

	for(int i=6; i>(6-iterations); i--){
		if(DEBUGMODE == true){
			glm::vec3 tempV = glm::vec3(0,0,0)+DirectionVectors[i];
			std::cerr << "Configuring Mesh for Chunk #" << i  << " at:  " << tempV.x << ", " <<tempV.y  << ", " << tempV.z << ";" << std::endl;
		}
		ChunkContainer[(6-i)]->GenerateMesh();
	}

	ShaderController.initialize();

	t2 = MainClock.getElapsedTime();
	dt = t2-t1;
	std::cerr << "Time to generate Chunk Meshes: " << dt.asSeconds() << std::endl;
	while(MainController.getCurrentGameState() != GameExiting){
		sf::Event event;
		while(MainWindow.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					MainController.setCurrentGameState(GameExiting);
					break;
				case sf::Event::KeyPressed:
					processKeyPress(event);
					break;
				case sf::Event::Resized:
					processResize(event);
					break;
				case sf::Event::MouseMoved:
					processMouseMove(event);
					break;
				default:
					break;
			}
		}
		//Do the light Mapping rendering.

		//Render the actual chunks. 
		PrepForRender();
		Render();
		Cleanup();
	}
	for(int i=0; i<ChunkContainer.size(); i++){
		delete ChunkContainer[i];
	}
	ChunkContainer.clear();
	MainWindow.close();
	if(DEBUGMODE)std::cerr << "Program Complete" << std::endl;
	return 0;
}

void InitOpenGL(){
	if(DEBUGMODE)std::cerr << "Initializing OpenGL" << std::endl;
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(77.0f/256.0f,166.0f/255.0f,255.0f/255.0f,1.0f);
	if(DEBUGMODE)std::cerr << "OpenGL Init Complete" << std::endl;
}

void PrepForRender(){
	
}

void Render(){
	ShaderController.drawScene();
	MainWindow.display();
}

void Cleanup(){
	sf::Vector2u windowSize = MainWindow.getSize();
	if(mouseBound)
		sf::Mouse::setPosition(sf::Vector2i(windowSize.x/2, windowSize.y/2), MainWindow);
	MainWindow.setMouseCursorVisible(false);
}
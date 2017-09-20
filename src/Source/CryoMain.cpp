#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoBase.hpp"
#include "../Headers/CryoInput.hpp"

sf::Window MainWindow;
sf::Clock MainClock;

std::vector<Chunk *> ChunkContainer;

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
	ChunkContainer.assign(1, nullptr);

	if(DEBUGMODE)std::cerr << "Loading Vertex Shader Path" << std::endl;
	ShaderController.setVertexShaderPath(std::string("./Assets/Shaders/VertexShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Fragment Shader Path" << std::endl;
	ShaderController.setFragmentShaderPath(std::string("./Assets/Shaders/FragmentShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Shader Program" << std::endl;
	ShaderController.createShaderProgram();
	
	if(DEBUGMODE)std::cerr << "Done Loading Shader Program" << std::endl;


	unsigned int iterations = 3;
	sf::Time t1 = MainClock.getElapsedTime();
	for(int i=0; i<iterations; i++){
		glm::vec3 tempV = glm::vec3(0,0,0)+DirectionVectors[i];
		if(DEBUGMODE == true)std::cerr << "Setting up Chunk #" << i  << " at:\t " << tempV.x << ", " <<tempV.y  << ", " << tempV.z << ";" << std::endl;
		int numChunks = ChunkContainer.size();
		if(i!=0){
			ChunkContainer.emplace_back(new Chunk(glm::vec3(0,0,0)+DirectionVectors[i-1]));
			ChunkContainer[numChunks].setIdentifier(numChunks);
		}else{
			ChunkContainer.emplace_back(new Chunk(glm::vec3(0,0,0)));
			ChunkContainer[numChunks].setIdentifier(numChunks);
		}
	}
	for(int i=0; i<iterations; i++){
		ChunkContainer[i]->GenerateMesh();
	}
	sf::Time t2 = MainClock.getElapsedTime();
	sf::Time dt = t2-t1;
	std::cerr << "Time to generate Chunks: " << dt.asSeconds() << std::endl;
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
				default:
					break;
			}
		}
		//Do the light Mapping rendering.

		//Render the actual chunks. 
		PrepForRender();
		for(int i=0; i<iterations; i++){
			ChunkContainer[i]->drawChunk();
		}
		Render();
	}
	for(int i=0; i<iterations; i++){
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render(){	
	MainWindow.display();
}
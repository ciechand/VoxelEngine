#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoBase.hpp"
#include "../Headers/CryoInput.hpp"
#include "../Headers/CryoLighting.hpp"

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
	if(DEBUGMODE)std::cerr << "Loading Base Shader Program" << std::endl;
	ShaderController.createNewShaderProgram(std::string("./Assets/Shaders/BaseVertexShader.glsl"),std::string("./Assets/Shaders/BaseFragmentShader.glsl"));

	if(DEBUGMODE)std::cerr << "Loading Shadow Shader Program" << std::endl;
	ShaderController.createNewShaderProgram(std::string("./Assets/Shaders/ShadowVertexShader.glsl"),std::string("./Assets/Shaders/ShadowFragmentShader.glsl"),std::string("./Assets/Shaders/ShadowGeometryShader.glsl"));

	if(DEBUGMODE)std::cerr << "Loading SSAO Shader Program" << std::endl;
	ShaderController.createNewShaderProgram(std::string("./Assets/Shaders/QuadVertexShader.glsl"),std::string("./Assets/Shaders/SSAOFragmentShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Blur Shader Program" << std::endl;
	ShaderController.createNewShaderProgram(std::string("./Assets/Shaders/QuadVertexShader.glsl"),std::string("./Assets/Shaders/SSAOBlurShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Lighting Shader Program" << std::endl;
	ShaderController.createNewShaderProgram(std::string("./Assets/Shaders/QuadVertexShader.glsl"),std::string("./Assets/Shaders/LightingFragmentShader.glsl"));

	if(DEBUGMODE)std::cerr << "Done Loading Shader Programs" << std::endl;
	//THIS IS THE END OF LOADING SHADERS

	//ShaderController.loadBaseMeshes();
	
	//Load all the noise Settings
	noiseSampler.SetNoiseType(FastNoise::Perlin);
	//noiseSampler.SetSeed(time(NULL));


	sf::Time t1 = MainClock.getElapsedTime();
	unsigned int chunkRadius = 10;

	float minHeight = std::numeric_limits<float>::max();
	float maxHeight = -std::numeric_limits<float>::max();

	for(int i=0; i<chunkRadius*chunkRadius; i++){
		float xAxis = i%chunkRadius;
		float zAxis = i/chunkRadius;
		minHeight = std::numeric_limits<float>::max();
		maxHeight = -std::numeric_limits<float>::max();
		for(int x=0; x<CHUNKSIDE; x++){
			for(int z=0; z<CHUNKSIDE; z++){
				float height = noiseSampler.GetNoise(x+(xAxis*CHUNKSIDE),z+(zAxis*CHUNKSIDE));
				if(floor(height) < minHeight)
						minHeight = floor(height);
				if(ceil(height) > maxHeight)
						maxHeight = ceil(height);
			}
		}
		for(int y=minHeight; y<=maxHeight; y++){
			if(DEBUGMODE == true){
				std::cerr << "Min Height: " << minHeight << "\nMax Height: " << maxHeight << std::endl;
				glm::vec3 tempV = glm::vec3(xAxis,y,zAxis);
				std::cerr << "Setting up Chunk #" << i+1  << " at:  " << tempV.x << ", " <<tempV.y  << ", " << tempV.z << ";" << std::endl;
			}
			ChunkContainer.emplace_back(new Chunk(glm::vec3(xAxis,y,zAxis)));
		}
	}
	sf::Time t2 = MainClock.getElapsedTime();
	sf::Time dt = t2-t1;
	std::cerr << "Time to generate and Mesh Chunks: " << dt.asSeconds() << std::endl;
	t1 = MainClock.getElapsedTime();

	for(int i=0; i<ChunkContainer.size(); i++){
		if(DEBUGMODE == true){
			glm::vec3 tempV = ChunkContainer[i]->getPos();
			std::cerr << "Meshing Chunk #" << i+1  << " at:  " << tempV.x << ", " <<tempV.y  << ", " << tempV.z << ";" << std::endl;
		}
		ChunkContainer[i]->GenerateMesh();
	}
	
	lightController.initialize();
	ShaderController.initialize();

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
	glViewport(0,0,SCREENWIDTH, SCREENHEIGHT);
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
	if(mouseBound){
		sf::Mouse::setPosition(sf::Vector2i(windowSize.x/2, windowSize.y/2), MainWindow);
		MainWindow.setMouseCursorVisible(false);
	}
}
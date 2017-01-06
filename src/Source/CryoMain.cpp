#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoChunk.hpp"
#include "../Headers/CryoRenderer.hpp"
#include "../Headers/CryoBase.hpp"
//#include "../Headers/CryoInput.hpp"

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

	if(DEBUGMODE)std::cerr << "Loading Vertex Shader Path" << std::endl;
	ShaderController.setVertexShaderPath(std::string("./Assets/Shaders/VertexShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Fragment Shader Path" << std::endl;
	ShaderController.setFragmentShaderPath(std::string("./Assets/Shaders/FragmentShader.glsl"));
	
	if(DEBUGMODE)std::cerr << "Loading Shader Program" << std::endl;
	ShaderController.createShaderProgram();
	
	if(DEBUGMODE)std::cerr << "Done Loading Shader Program" << std::endl;

	Voxel v;
	Mesh *m = new Mesh(v);
	while(MainController.getCurrentGameState() != GameExiting){
		sf::Event event;
		while(MainWindow.pollEvent(event)){
			switch(event.type){
				case sf::Event::Closed:
					MainController.setCurrentGameState(GameExiting);
					break;
				case sf::Event::KeyPressed:
					//processKeyPress(event);
					break;
				default:
					break;
			}
		}
		PrepForRender();
		m->drawMesh();
		Render();
	}
	delete m;
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

void PrepForRender(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Render(){	
	MainWindow.display();
}
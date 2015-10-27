#include "../Headers/CryoBase.hpp"

GRend Renderer;
OPT Options;
GState State;
Chunk testingChunk;

int main (int argc, char ** argv){
	//First things first, init Glut (The main windowing extention that I am using)
	glutInit(&argc, argv);

	std::cout << "Starting Program" << std::endl; 
	std::cout.flush();
	
	//Setup the way glut will be displaying things.
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

	//init some glut version things.
	glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);

	//Create and Initiate the global options for the program, all the reading from file is done within the constructor.
	//Options = *new class GameOptions("./Cryoptions.config");

	//setup glut window, in the future the hard-coded values will be replaces with the options from the options class.
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(10,10);
	glutCreateWindow("Cryonyx");

	//setting up glew
	glewExperimental = GL_TRUE; //This is so that experimental drivers will be checked as well.
	glewInit();

	InitOpenGL();

	//Processing the glut functions.
	glutKeyboardFunc(ProcessKeyboard);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(ProcessPassiveMouseMovement);
	glutMotionFunc(ProcessActiveMouseMovement);


	glutMainLoop();

	return 0;
}	
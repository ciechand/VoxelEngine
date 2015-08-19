#include "../Headers/CryonyxBase.h"

class GameState State;
class GameRenderer Renderer;
class GameOptions Options;
double deltaTime;

int main(int argc, char ** argv){
	//init glut itself
	glutInit(&argc, argv);
	
	//init the way that glut is going to display things
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	
	//init some glut version things.
	glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
	
	/*
	Reading from options file goes here.	
	*/
	Options = *new class GameOptions(0,0,1.04f,1.0f,0.01f,15.0f);

	//setup glut window
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(Options.getWindowPos()[0], Options.getWindowPos()[1]);
	glutCreateWindow("Cryonyx");

	//setting up glew
	glewExperimental = GL_TRUE; //This is so that experimental drivers will be checked as well.
	glewInit();

	/* 
	Init will go here, once i set it up
	*/
	InitOpenGL();
	
	glutKeyboardFunc(ProcessKeyboard);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(ProcessPassiveMouseMovement);
	glutMotionFunc(ProcessActiveMouseMovement);

	glutMainLoop();

	return 0;
}
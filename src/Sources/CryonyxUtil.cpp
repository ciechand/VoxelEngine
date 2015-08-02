#include "../Headers/CryonyxBase.h"

//Function that will process the mouse inputs.
void ProcessMouseClicks(int button, int state, int x, int y){


}

//function that will process the passive mouse movement.
void ProcessPassiveMouseMovement(int x, int y){
	int mousePos[2] = { x-State.getMousePos()[0], y-State.getMousePos()[1] };
	(mousePos[0] >= 30)? mousePos[0] = 30: (mousePos[0] <= -30)? mousePos[0] = -30: mousePos[0] = mousePos[0];
	(mousePos[1] >= 30) ? mousePos[1] = 30 : (mousePos[1] <= -30) ? mousePos[1] = -30 : mousePos[1] = mousePos[1];
	Renderer.mvRot = glm::rotate(Renderer.mvRot,  mousePos[0]* (PI/180)*0.3f, vec3(glm::inverse(Renderer.mvRot) * glm::inverse(Renderer.mvTrans) *  vec4(0.0f, 1.0f, 0.0f,0.0f)));
	Renderer.mvRot = glm::rotate(Renderer.mvRot, mousePos[1] * (PI / 180)*0.3f, vec3(glm::inverse(Renderer.mvRot) * glm::inverse(Renderer.mvTrans) *  vec4(1.0f, 0.0f, 0.0f, 0.0f)));
	State.setMousePos(x,y);
	glutPostRedisplay();
}

void ProcessActiveMouseMovement(int x, int y){
	ProcessPassiveMouseMovement(x, y);
}

//Function for processing the keyboard inputs utilizing glut.
void ProcessKeyboard(unsigned char key, int x, int y){
	switch (key){
		case 033:	
			exit(EXIT_SUCCESS);
			break;
	}
}

// Create a NULL-terminated string by reading the provided file
char* readFile(const char* filename)
{
	FILE* fp = fopen(filename, "r");

	if (fp == NULL)
		return NULL;

	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';
	fclose(fp);
	return buf;
}

// Create a GLSL program object from vertex and fragment shader files
GLuint createShadersProgram(const char* vsFile, const char* fsFile){
	struct Shader
	{
		const char* filename;
		GLenum type;
		GLchar* source;
	};

	struct Shader shaders[] = {
			{ vsFile, GL_VERTEX_SHADER, NULL },
			{ fsFile, GL_FRAGMENT_SHADER, NULL }
	};

	GLuint program = glCreateProgram();

	for (int i = 0; i < 2; ++i)
	{
		Shader& s = shaders[i];
		s.source = readFile(s.filename);

		if (shaders[i].source == NULL)
		{
			fprintf(stderr, "Failed to read %s\n", s.filename);
			//exit(EXIT_FAILURE);
		}

		GLuint shader = glCreateShader(s.type);

		glShaderSource(shader, 1, (const GLchar**)&s.source, NULL);
		glCompileShader(shader);
		GLint  compiled;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint  logSize;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shader, logSize, NULL, logMsg);
			fprintf(stderr, "%s failed to compile:\n%s\n", s.filename, logMsg);
			delete[] logMsg;
			//exit(EXIT_FAILURE);
		}

		delete[] s.source;
		glAttachShader(program, shader);
	}

	// link and error check
	glLinkProgram(program);
	GLint  linked;
	glGetProgramiv(program, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint  logSize;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logSize);
		char* logMsg = new char[logSize];
		glGetProgramInfoLog(program, logSize, NULL, logMsg);
		fprintf(stderr, "Shader program failed to link:\n%s\n", logMsg);
		delete[] logMsg;
		//exit(EXIT_FAILURE);
	}

	// use program object
	glUseProgram(program);
	return program;
}

void printMatrix(glm::mat4 matrix){
	printf("\n");
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			printf("%f ", matrix[i][j]);
		}
		printf("\n");
	}	
	printf("\n");
}
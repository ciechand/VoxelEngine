#include "../Headers/CryonyxBase.h"

//Function that will process the mouse inputs.
void ProcessMouseClicks(int button, int state, int x, int y){


}

//function that will process the passive mouse movement.
void ProcessPassiveMouseMovement(int x, int y){
	printf("%d\n", y);
	int ws[2] = {glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)};
	float * mousepos = State.getMousePos();
	int curx=(x-(ws[0]/2)), cury=(y-(ws[1]/2));
	float curxper=(((float)curx)/ws[0]), curyper=(((float)cury)/ws[1]);


	glm::mat4 rotations = glm::mat4();
	if(x<(ws[0]/2)-1 || x>(ws[0]/2)+1 || y<(ws[1]/2)-1 || y>(ws[1]/2)+1){
		mousepos[0] += curxper*0.5;
		mousepos[1] += curyper*0.5;
	}
	(mousepos[0] > 1.0f)? mousepos[0] -= 1.0f: (mousepos[0] < 0.0f)? mousepos[0] += 1.0f: mousepos[0] = mousepos[0];
	(mousepos[1] > 1.0f)? mousepos[1] = 1.0f: (mousepos[1] < 0.0f)? mousepos[1] = 0.0f: mousepos[1] = mousepos[1];
	
	rotations = glm::rotate(rotations, (mousepos[0]*2*PI), glm::vec3(0.0f,1.0f,0.0f));
	rotations = glm::rotate(rotations, (mousepos[1]*PI)-(PI/2), glm::vec3(1.0f,0.0f,0.0f));

	Renderer.modelMatrices[Rotate] = rotations;
	State.setMousePos(mousepos[0], mousepos[1]);

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
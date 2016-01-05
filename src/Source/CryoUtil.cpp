#include "../Headers/CryoBase.hpp"

//Function that will process the mouse inputs.
void ProcessMouseClicks(int button, int state, int x, int y){


}

//function that will process the passive mouse movement.
void ProcessPassiveMouseMovement(int x, int y){

	int ws[2] = {glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)};
	float * mousepos = State.getCamPos();
	int curx=(x-(ws[0]/2)), cury=(y-(ws[1]/2));
	float curxper=(((float)curx)/ws[0]), curyper=(((float)cury)/ws[1]);

	//mousepos[0] = curx*0.1f;
	//mousepos[1] = cury*0.1f;
	//glm::mat4 rotations = glm::mat4();
	//if(x<(ws[0]/2)-1 || x>(ws[0]/2)+1 || y<(ws[1]/2)-1 || y>(ws[1]/2)+1){
		//mousepos[0] += curxper*LOOKSPEED;
		//mousepos[1] += curyper*LOOKSPEED;
	//}
	//(mousepos[0] > 1.0f)? mousepos[0] -= 1.0f: (mousepos[0] < 0.0f)? mousepos[0] += 1.0f: mousepos[0] = mousepos[0];
	//(mousepos[1] > 1.0f)? mousepos[1] = 1.0f: (mousepos[1] < 0.0f)? mousepos[1] = 0.0f: mousepos[1] = mousepos[1];
	
	float * cam = Options.getCamPos();

	//Renderer.setViewMatrix(glm::lookAt(glm::vec3(cam[0],cam[1],cam[2]), glm::vec3(cam[0],cam[1],cam[2])+glm::vec3(cos(mousepos[0])*sin(mousepos[1]),sin(mousepos[1]), cos(mousepos[0])*cos(mousepos[1])), glm::vec3(0.0f,1.0f,0.0f)));
	Renderer.setViewMatrix(glm::lookAt(glm::vec3(10.0f, 20.0f, 14.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,1.0f,0.0f)));
	// State.setCamPos(mousepos[0], mousepos[1]);

	glutPostRedisplay();

}

void ProcessActiveMouseMovement(int x, int y){
	ProcessPassiveMouseMovement(x, y);
}

//Function for processing the keyboard inputs utilizing glut.
void ProcessKeyboard(unsigned char key, int x, int y){
	glm::mat4 vmat = Renderer.getViewMatrix();
	switch (key){
		case 033:	
			exit(EXIT_SUCCESS);
			break;
		case 'a':
			Renderer.setViewMatrix(glm::translate(vmat, glm::vec3(glm::inverse(vmat)*glm::vec4(0.1f,0.0f,0.0f,0.0f))));
			break;
		case 'd':
			Renderer.setViewMatrix(glm::translate(vmat, glm::vec3(glm::inverse(vmat)*glm::vec4(-0.1f,0.0f,0.0f,0.0f))));
			break;
		case 'w':
			Renderer.setViewMatrix(glm::translate(vmat, glm::vec3(glm::inverse(vmat)*glm::vec4(0.0f,0.0f,0.1f,0.0f))));
			break;
		case 's':
			Renderer.setViewMatrix(glm::translate(vmat, glm::vec3(glm::inverse(vmat)*glm::vec4(0.0f,0.0f,-0.1f,0.0f))));
			break;
	}
	glutPostRedisplay();
}

// Create a NULL-terminated string by reading the provided file
std::string readFileToString(const char* filePath)
{
	std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cout << "Could not read file " << filePath << ". File does not exist." << std::endl;
        return "";
    }

    std::string line = "";
    while(!fileStream.eof()) {
        std::getline(fileStream, line);
        content.append(line + "\n");
    }

    fileStream.close();
    return content;
}

void readOBJFile(IOBJ * Object, const char * fileName){
	//Make sure the object file has a name already assigned
	if (fileName == NULL){
		perror("Have not set a filename, Please do so before trying to load an obj file again.");
		return;
	}
	std::cout << "Loading in Asset as : " << fileName << std::endl;
	//opening the OBJ file for reading
	FILE * objFile = fopen(fileName, "r");
	if (objFile == NULL){
		perror("THAT FILE DOES NOT EXIST");
		exit(EXIT_FAILURE);
	}
	//creating a buffer to read the file into
	char fileBuffer[128];
	std::vector<GLuint> vertexIndices;
	std::vector<GLuint> normalIndices;
	std::vector<GLuint> textureIndices;
	while (fgets(fileBuffer, 128, objFile) != NULL){
		//checking which identifier you are currently reading and assigning appropriately.
		char identifier[3];
		sscanf(fileBuffer, "%s", identifier);
		if (strcmp(identifier, "#") == 0){
			continue;
		} else if (strcmp(identifier, "v") == 0){
			glm::vec4 verts;
			sscanf(fileBuffer, "%*s %f %f %f", &verts.x, &verts.y, &verts.z);
			verts.w = 1.0;
			Object->addVertices(verts);
		} else if (strcmp(identifier, "vn") == 0){
			glm::vec4 norm;
			sscanf(fileBuffer, "%*s %f %f %f", &norm.x, &norm.y, &norm.z);
			norm.w = 0.0;
			Object->addVertexNormals(norm);
		} else if (strcmp(identifier, "vt") == 0){
			glm::vec2 tex;
			sscanf(fileBuffer, "%*s %f %f", &tex.x, &tex.y);
			Object->addTextureCoords(tex);
		} else if (strcmp(identifier, "f") == 0){
			GLuint indices[9];
			sscanf(fileBuffer, "%*s %d/%d/%d %d/%d/%d %d/%d/%d", &indices[0], &indices[1], &indices[2], &indices[3], &indices[4], &indices[5], &indices[6], &indices[7], &indices[8]);
			for (int j = 0; j < 3; j++){
				vertexIndices.push_back(indices[(3 * j)] - 1);
				normalIndices.push_back(indices[(3 * j) + 2] - 1);
				textureIndices.push_back(indices[(3 * j) + 1] - 1);

			}
		} else{
			perror("Incorrect Identifier!");//WTF KINDA OBJ FILE ARE YOU READING?!
		}
	}

	fclose(objFile);
	std::map<vertexInfo, GLuint> indexMap;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> tex;

	//Fix all the indices so that it doesnt have to be done later.
	//This needs to be done since open gl only allows one index array for each object so, you need to reorganize the indices that are read in from the OBJ file.

	for (size_t i = 0; i < vertexIndices.size(); i++){
		struct vertexInfo v = { Object->getVertices(vertexIndices[i]), Object->getVertexNormals(normalIndices[i]), Object->getTextureCoords(textureIndices[i]) };
		bool exists = false;
		GLuint index = 0;
		//Checking for unique (vertex,normal,texture) triplets.
		std::map<vertexInfo, GLuint>::iterator it = indexMap.find(v);
		 
		if (it == indexMap.end()){
			exists = false;
		} else{
			index = it->second;
			exists = true;
		}
		if (exists){
			Object->addIndices(index);
		} else{
			verts.push_back(Object->getVertices(vertexIndices[i]));
			norms.push_back(Object->getVertexNormals(normalIndices[i]));
			tex.push_back(Object->getTextureCoords(textureIndices[i]));
			index = (GLuint)verts.size() - 1;
			Object->addIndices(index);
			indexMap[v] = index;
		}
	};
	//The vectors must be re-applied so that the order is correct in correspondence with the new index array.
	Object->setVertices(verts);
	Object->setVertexNormals(norms);
	Object->setTextureCoords(tex);
	std::cout << "Finnished Loading in Asset of : " << fileName << std::endl;
}

void loadBMP(const char * fileName, GLuint * textureID){
	//header for testing if it is a png
	png_byte header[8];

	//open file as binary
	FILE *fp = fopen(fileName, "rb");
	if (!fp) {
		
	}

	//read the header
	fread(header, 1, 8, fp);

	//test if png
	int is_png = !png_sig_cmp(header, 0, 8);
	if (!is_png) {
		fclose(fp);
		perror("TEXTURE LOAD ERROR 1");
	}

	//create png struct
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL,
	NULL, NULL);
	if (!png_ptr) {
		fclose(fp);
		perror("TEXTURE LOAD ERROR 2");
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 3");
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 4");
	}

	//png error stuff, not sure libpng man suggests this.
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 5");
	}

	//init png reading
	png_init_io(png_ptr, fp);

	//let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	//variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 twidth, theight;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &twidth, &theight, &bit_depth, &color_type,
	NULL, NULL, NULL);

	//update width and height based on png info
	int width = twidth;
	int height = theight;

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte *image_data = new png_byte[rowbytes * height];
	if (!image_data) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 6");
	}

	//row_pointers is for pointing to image_data for reading the png with libpng
	png_bytep *row_pointers = new png_bytep[height];
	if (!row_pointers) {
		//clean up memory and close stuff
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		delete[] image_data;
		fclose(fp);
		perror("TEXTURE LOAD ERROR 7");
	}
	// set the individual row_pointers to point at the correct offsets of image_data
	for (int i = 0; i < height; ++i)
		row_pointers[height - 1 - i] = image_data + i * rowbytes;

	//read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	//Now generate the OpenGL texture object
	glBindTexture(GL_TEXTURE_2D, *textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//clean up memory and close stuff
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	delete[] image_data;
	delete[] row_pointers;
	fclose(fp);
 
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
	GLuint shaderArr[2];
	for (int i = 0; i < 2; ++i)
	{
		Shader& s = shaders[i];
		std::string tempStr = readFileToString(s.filename);
		s.source = (GLchar *)tempStr.c_str();


		if (shaders[i].source == NULL)
		{
			fprintf(stderr, "Failed to read %s\n", s.filename);
			//exit(EXIT_FAILURE);
		}

		shaderArr[i] = glCreateShader(s.type);

		glShaderSource(shaderArr[i], 1, (const GLchar**)&s.source, NULL);
		glCompileShader(shaderArr[i]);
		GLint  compiled;
		glGetShaderiv(shaderArr[i], GL_COMPILE_STATUS, &compiled);
		if (!compiled)
		{
			GLint  logSize;
			glGetShaderiv(shaderArr[i], GL_INFO_LOG_LENGTH, &logSize);
			char* logMsg = new char[logSize];
			glGetShaderInfoLog(shaderArr[i], logSize, NULL, logMsg);
			fprintf(stderr, "%s failed to compile:\n%s\n", s.filename, logMsg);
			delete[] logMsg;
			//exit(EXIT_FAILURE);
		}

		delete[] s.source;
		glAttachShader(program, shaderArr[i]);
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
	for(int i=0; i<2; i++){
		glDeleteShader(shaderArr[i]);
	}
	// use program object
	glUseProgram(program);
	return program;
}

bool compBlockDist(glm::mat4 a, glm::mat4 b){
	float * camPos = Options.getCamPos();
	float dista = sqrt((a[3][0]-camPos[0])+(a[3][1]-camPos[1])+(a[3][2]-camPos[2]));
	float distb = sqrt((b[3][0]-camPos[0])+(b[3][1]-camPos[1])+(b[3][2]-camPos[2]));
	return (dista>distb)?true:false;

}

//Below are functions for printing matrices.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void printMatrix(glm::mat4 matrix){
	using namespace std;
	cout << "4X4 Matrix: " << endl;
	for (int i = 0; i < 4; i++){
		for (int j = 0; j < 4; j++){
			cout << matrix[j][i];
		}
		cout << endl;
	}	
	cout << endl;
	cout.flush();
}

void printMatrix(glm::mat3 matrix){
	using namespace std;
	cout << "4X4 Matrix: " << endl;
	for (int i = 0; i < 3; i++){
		for (int j = 0; j < 3; j++){
			cout << matrix[j][i];
		}
		cout << endl;
	}	
	cout << endl;
	cout.flush();
}
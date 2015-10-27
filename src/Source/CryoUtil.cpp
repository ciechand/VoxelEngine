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


	glm::mat4 rotations = glm::mat4();
	if(x<(ws[0]/2)-1 || x>(ws[0]/2)+1 || y<(ws[1]/2)-1 || y>(ws[1]/2)+1){
		mousepos[0] += curxper*0.5;
		mousepos[1] += curyper*0.5;
	}
	(mousepos[0] > 1.0f)? mousepos[0] -= 1.0f: (mousepos[0] < 0.0f)? mousepos[0] += 1.0f: mousepos[0] = mousepos[0];
	(mousepos[1] > 1.0f)? mousepos[1] = 1.0f: (mousepos[1] < 0.0f)? mousepos[1] = 0.0f: mousepos[1] = mousepos[1];
	
	rotations = glm::rotate(rotations, (mousepos[0]*2*PI), glm::vec3(0.0f,1.0f,0.0f));
	rotations = glm::rotate(rotations, (mousepos[1]*PI)-(PI/2), glm::vec3(1.0f,0.0f,0.0f));

	//Renderer.setViewMatrix(rotations * glm::lookAt(glm::vec3(10.0f,10.0f,2.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f)));
	Renderer.setViewMatrix( glm::lookAt(glm::vec3(0.0f,0.0f,20.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f)));

	State.setCamPos(mousepos[0], mousepos[1]);

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
	std::cout << "Printing size of Objects: " << Object->getModelMatrices().size() << std::endl;
	//The vectors must be re-applied so that the order is correct in correspondence with the new index array.
	Object->setVertices(verts);
	Object->setVertexNormals(norms);
	Object->setTextureCoords(tex);
	std::cout << "Finnished Loading in Asset of : " << fileName << std::endl;
}

//Function to load in a BMP for texture mapping
void loadBMP(const char * fileName, GLuint * TextureID){
	int width = 0;
	int height = 0;
	short BitsPerPixel = 0;
	std::vector<unsigned char> Pixels;

	std::cout << "Adding Texture " << fileName << std::endl;

	 std::fstream hFile(fileName, std::ios::in | std::ios::binary);
    if (!hFile.is_open()) throw std::invalid_argument("Error: File Not Found.");

    hFile.seekg(0, std::ios::end);
    int Length = hFile.tellg();
    hFile.seekg(0, std::ios::beg);
    std::vector<std::uint8_t> FileInfo(Length);
    hFile.read(reinterpret_cast<char*>(FileInfo.data()), 54);

    if(FileInfo[0] != 'B' && FileInfo[1] != 'M')
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. Bitmap Required.");
    }

    if (FileInfo[28] != 24 && FileInfo[28] != 32)
    {
        hFile.close();
        throw std::invalid_argument("Error: Invalid File Format. 24 or 32 bit Image Required.");
    }

    BitsPerPixel = FileInfo[28];
    width = FileInfo[18] + (FileInfo[19] << 8);
    height = FileInfo[22] + (FileInfo[23] << 8);
    std::uint32_t PixelsOffset = FileInfo[10] + (FileInfo[11] << 8);
    std::uint32_t size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
    Pixels.resize(size);

    hFile.seekg (PixelsOffset, std::ios::beg);
    hFile.read(reinterpret_cast<char*>(Pixels.data()), size);
    hFile.close();


	glBindTexture(GL_TEXTURE_2D, *TextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, Pixels.data());

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
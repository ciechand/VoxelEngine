#include "../Headers/CryoBase.hpp"

//Function that will process the mouse inputs.
void processMouseClicks(sf::Event e){


}

//function that will process the passive mouse movement.
void processMouseMovement(sf::Event e){
	if(State.getState() != Menu){
		sf::Vector2u windowSize = mainWindow.getSize();
		float * camRot = Options.getCamPos();

		float rotationPer[2] = {(((e.mouseMove.x-((int)windowSize.x/2))/((float)windowSize.x/2.0f))), (((e.mouseMove.y-((int)windowSize.y/2))/((float)windowSize.y/2.0f)))}; 

		camRot[0] += rotationPer[0];
		camRot[1] += rotationPer[1];

		(camRot[0]>1.0f)?camRot[0] = -1.0f:(camRot[0]<0.0f)?camRot[0] +=1.0f:camRot[0]=camRot[0];	
		(camRot[1]>1.0f)?camRot[1] = 0.9999f:(camRot[1]<0.0f)?camRot[1] = 0.0001f:camRot[1]=camRot[1];

		rotationPer[0] = camRot[0]*PI*2;
		rotationPer[1] = (camRot[1]*PI)+PI/2;

 		glm::vec3 camPos = Renderer.getCamVec(0);
 		glm::vec3 rotations = glm::vec3(-sin(rotationPer[0])*cos(rotationPer[1])*10.0f, -sin(rotationPer[1])*10.0f, cos(rotationPer[0])*cos(rotationPer[1])*10.0f);
 		//std::cout << "Rotation vector:\n X: " << rotations.x << "\n Y: " << rotations.y << "\n Z: " << rotations.z << std::endl;
 		glm::vec3 camUp = Renderer.getCamVec(2);
 		Renderer.setViewMatrix(glm::lookAt(camPos+rotations, camPos+glm::vec3(0.0f,5.0f,0.0f), camUp));

 		Renderer.setCamVec(1, camPos+rotations);

		Options.setCamPos(camRot);

	}
}
//Function for processing the keyboard inputs.
void processKeyboard(sf::Event e){
	glm::vec3 camPos = Renderer.getCamVec(0);
	glm::vec3 camDir = Renderer.getCamVec(1);
	glm::vec3 camUp = Renderer.getCamVec(2);
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
		camPos -= glm::normalize(glm::vec3(camDir.x,0.0f,camDir.z))*0.5f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
		camPos += glm::normalize(glm::vec3(camDir.x,0.0f,camDir.z))*0.5f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
		camPos -= glm::normalize(glm::cross(camPos-camDir, camUp))*0.5f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
		camPos += glm::normalize(glm::cross(camPos-camDir, camUp))*0.5f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::E)){
		camPos -= camUp*0.5f;
	}
	if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q)){
		camPos += camUp*0.5f;
	}
	switch (e.key.code){
		case sf::Keyboard::Escape:	
			State.setState(Exiting);
			break;
		case sf::Keyboard::R:
			(State.getState() != Menu)? State.setState(Menu):State.setState(Loading);
			break;
		default:
			break;
	}
	Renderer.setCamVec(0, camPos);
}

void windowResized(sf::Event e){
	sf::Vector2u windowSize = mainWindow.getSize();
	glViewport(0,0,e.size.width, e.size.height);
}

// Create a NULL-terminated string by reading the provided file
std::string readFileToString(const char* filePath)
{

	std::string content;
    std::ifstream fileStream(filePath, std::ios::in);

    if(!fileStream.is_open()) {
        std::cerr << "Could not read file " << filePath << ". File does not exist." << std::endl;
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
			perror("Incorrect Identifier!"); //WTF KINDA OBJ FILE ARE YOU READING?!
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
		struct vertexInfo v = { Object->getVertices(vertexIndices[i]), Object->getVertexNormals(normalIndices[i]), Object->getTextureCoords(textureIndices[i])};
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
	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

    // Read shaders
    std::string vertShaderStr = readFileToString(vsFile);
    std::string fragShaderStr = readFileToString(fsFile);
    const char *vertShaderSrc = vertShaderStr.c_str();
    const char *fragShaderSrc = fragShaderStr.c_str();

    GLint result = GL_FALSE;
    int logLength;

    // Compile vertex shader
    std::cout << "Compiling vertex shader." << std::endl;
    glShaderSource(vertShader, 1, &vertShaderSrc, NULL);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, NULL, &vertShaderError[0]);
    if(logLength > 1)std::cout << "vert Errors: " << &vertShaderError[0] << std::endl;

    // Compile fragment shader
    std::cout << "Compiling fragment shader." << std::endl;
    glShaderSource(fragShader, 1, &fragShaderSrc, NULL);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, NULL, &fragShaderError[0]);
    if(logLength > 1) std::cout << "Frag Errors: " << &fragShaderError[0] << std::endl;

    std::cout << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, NULL, &programError[0]);
    if(logLength > 1)std::cout << "Prog Errors: " <<&programError[0] << std::endl;

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
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

void printShaderInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
   		std::cerr << "Printing " << infologLength << " Shader Problems: " << infoLog << std::endl;
        free(infoLog);
    }else {
    	std::cerr << "No Shader Problems." << std::endl;
    }
}
 
void printProgramInfoLog(GLuint obj)
{
    int infologLength = 0;
    int charsWritten  = 0;
    char *infoLog;
 
    glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
 
    if (infologLength > 0)
    {
        infoLog = (char *)malloc(infologLength);
        glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
    	std::cerr << "Printing " << infologLength << " Program Problems: " << infoLog << std::endl;
        free(infoLog);
    }else{
    	std::cerr << "No Program Problems." << std::endl;
    }
}
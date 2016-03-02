#include "../Headers/CryoBase.hpp"

Ray::Ray(){
	Origin = glm::vec3();
	Direction = glm::vec3();
	InverseDirection = glm::vec3(1/Direction.x,1/Direction.y,1/Direction.z);
	sign[0] = (InverseDirection.x < 0);
	sign[1] = (InverseDirection.y < 0);
	sign[2] = (InverseDirection.z < 0);
	times[0] = 0.0f;
	times[1] = 1.0f;

}

Ray::Ray(glm::vec3 O, glm::vec3 D, float t0, float t1){
	Origin = O;
	Direction = D;
	InverseDirection = glm::vec3(1/Direction.x,1/Direction.y,1/Direction.z);
	sign[0] = (InverseDirection.x < 0);
	sign[1] = (InverseDirection.y < 0);
	sign[2] = (InverseDirection.z < 0);
	times[0] = t0;
	times[1] = t1;
}

int Intersection(Block * b, Ray r){
	//std::cout << b << std::endl;
	int side[2] = {-1,-1};
	float tmin, tmax, tymin, tymax, tzmin,tzmax;
	if(b == nullptr)
		return -1;
	int bID = b->getID();
	//std::cout << "Block ID: " << bID << std::endl;
	glm::vec3 bpos = b->getPos();
	//std::cout << "Block Coords: " << bpos.x << ": " << bpos.y << ": " << bpos.z << std::endl;
	glm::vec3 bounds[2] = {glm::vec3(bpos.x-(BLOCKSCALE/2),bpos.y-(BLOCKSCALE/2),bpos.z-(BLOCKSCALE/2)), glm::vec3(bpos.x+(BLOCKSCALE/2),bpos.y+(BLOCKSCALE/2),bpos.z+(BLOCKSCALE/2))};


	//std::cout << "Intersection Block xs\n\tX: " << tmin << " \n\tY: " << tmax << std::endl;
	//std::cout << "Intersection Block xs\n\tX: " << tmin << " \n\tY: " << tmax << std::endl;

	tmin = (bounds[r.sign[0]].x - r.Origin.x) * r.InverseDirection.x;
	tmax = (bounds[1-r.sign[0]].x - r.Origin.x) * r.InverseDirection.x;
	side[0] = 0;
	side[1] = 0;
	tymin = (bounds[r.sign[1]].y - r.Origin.y) * r.InverseDirection.y;
	tymax = (bounds[1-r.sign[1]].y - r.Origin.y) * r.InverseDirection.y;
	//std::cout << "Intersection Block xs\n\tX: " << tmin << " \n\tY: " << tmax << std::endl; 
	//std::cout << "Intersection Block ys\n\tX: " << tymin << " \n\tY: " << tymax << std::endl; 
	if((tmin > tymax) || (tymin > tmax))
		return -1;
	if(tymin > tmin){
		tmin = tymin;
		side[0] = 1;
	}
	if(tymax < tmax){
		tmax = tymax;
		side[1] = 1;
	}
	tzmin = (bounds[r.sign[2]].z - r.Origin.z) * r.InverseDirection.z;
	tzmax = (bounds[1-r.sign[2]].z - r.Origin.z) * r.InverseDirection.z;
	//std::cout << "Intersection Block zs\n\tX: " << tzmin << " \n\tY: " << tzmax << std::endl << std::endl; 
	if((tmin > tzmax) || (tzmin > tmax))
		return -1;
	if(tzmin > tmin){
		tmin = tzmin;
		side[0] = 2;
	}
	if(tzmax < tmax){
		tmax = tzmax;
		side[1] = 2;
	}
	int finalSide = (r.sign[side[0]])+(side[0]*2);
	if ((tmin < r.times[1]) && (tmax > r.times[0]))
		return finalSide;
	else
		return -1;
}

Block * pickBlock(){
	Ray sight;
	std::vector<Block*> viableBlockList;
	std::vector<int> viableSides;
	Player self = State.getPlayer(0);
	glm::vec3 playerPos = self.getPos();
	//playerPos = glm::vec3(playerPos.x,playerPos.y,playerPos.z);
	glm::vec3 camPos = Renderer.getCamVec(0);

	glm::vec3 lookingDirection = playerPos-camPos;
	lookingDirection = glm::normalize(lookingDirection)*FOCUSDIST;
	glm::vec3 finalPoint = playerPos+lookingDirection;

	//finalPoint = glm::normalize(finalPoint)*FOCUSDIST;
	//finalPoint = glm::vec3(floor(finalPoint.x),floor(finalPoint.y),floor(finalPoint.z));

	sight = Ray(playerPos,lookingDirection,0.0f,FOCUSDIST);

	//std::cout << "After Declarations." << std::endl;

	//std::cout << std::endl;

	//std::cout << "PlayerPos: " << playerPos.x << ":" << playerPos.y << ":" << playerPos.z << std::endl;
	//std::cout << "finalPos: " << finalPoint.x << ":" << finalPoint.y << ":" << finalPoint.z << std::endl;

	if(playerPos.y/BLOCKSCALE >= (CHUNKHEIGHT) && finalPoint.y/BLOCKSCALE >= (CHUNKHEIGHT) )
		return nullptr;
	if (playerPos.y/BLOCKSCALE <= 0.0f && finalPoint.y/BLOCKSCALE <= 0.0f)
		return nullptr;

	glm::vec2 finalChunk = glm::vec2(floor(finalPoint.x/(CHUNKDIMS*BLOCKSCALE)), floor(finalPoint.z/(CHUNKDIMS*BLOCKSCALE)));
	glm::vec2 startChunk = glm::vec2(floor(playerPos.x/(CHUNKDIMS*BLOCKSCALE)), floor(playerPos.z/(CHUNKDIMS*BLOCKSCALE)));

	glm::vec2 chunkDif = finalChunk-startChunk;
	std::vector<glm::vec2> CheckedChunks;
	CheckedChunks.push_back(startChunk);
	//std::cout << "Pushed Back Start Chunk: " << startChunk.x << ": " << startChunk.y << std::endl;
	for(int cx=fminf(startChunk.x,finalChunk.x); cx<=fmaxf(startChunk.x,finalChunk.x); cx++){
		for(int cy=fminf(startChunk.y,finalChunk.y); cy<=fmax(startChunk.y,finalChunk.y); cy++){
			//std::cout << "Pushed Back Chunk: " << startChunk.x << ": " << startChunk.y << std::endl;
			if(startChunk != glm::vec2(cx,cy))
				CheckedChunks.push_back(glm::vec2(cx,cy));
		}
	}
	//This needs to scope out an area in between PlayerPos and FinalPoint, need to control signs Correctly so this doesnt exit automatically. Also, make sure to use the dimentions of the chunk as boundries. (fminf(),fmaxf)
	//std::cout << "Chunkmath Done, Checking " << CheckedChunks.size() <<" world chunks." << std::endl;
	Chunk * tempChunk = nullptr;
	glm::vec2 chunkPos = glm::vec2();
	int inter = -1;

	for(int c=0; c<CheckedChunks.size(); c++){
		//std::cout << "CheckedChunk\n\tX: " << CheckedChunks[c].x << " \n\tY: " << CheckedChunks[c].y << std::endl; 
		for(int i=0; i<World.size(); i++){
			chunkPos = World[i]->getPosition();
			//std::cerr << "Chunk\n\tX: " << chunkPos.x << " \n\tY: " << chunkPos.y << std::endl; 
			if(chunkPos == CheckedChunks[c]){
				tempChunk = World[i];
				break;
			}
		}
		if(tempChunk == nullptr) {
			std::cerr << "tempChunk is nullptr, WTF?!" << std::endl;
			return nullptr;
		}
		int Chunkx = (int)CheckedChunks[c].x*CHUNKDIMS*BLOCKSCALE;
		int Chunkz = (int)CheckedChunks[c].y*CHUNKDIMS*BLOCKSCALE;
		int beginBoundx = (int)fmaxf(fminf(playerPos.x, finalPoint.x), Chunkx);
		int endBoundx = (int)fminf(fmaxf(playerPos.x, finalPoint.x), (Chunkx+(CHUNKDIMS*BLOCKSCALE))-1);
		int beginBoundz = (int)fmaxf(fminf(playerPos.z, finalPoint.z), Chunkz); 
		int endBoundz = (int)fminf(fmaxf(playerPos.z, finalPoint.z), (Chunkz+(CHUNKDIMS*BLOCKSCALE))-1);
/*		std::cout << "Boundx: " << beginBoundx << ":" << endBoundx << std::endl;
		std::cout << "Boundz: " << beginBoundz << ":" << endBoundz << std::endl;
		std::cout << "chunkx: " << CheckedChunks[c].x << std::endl; 
		std::cout << "chunkz: " << CheckedChunks[c].y << std::endl; */
		glm::vec2 Boundx;
		glm::vec2 Boundz;
		Boundx = glm::vec2(beginBoundx-Chunkx,endBoundx-Chunkx);
		Boundz = glm::vec2(beginBoundz-Chunkz,endBoundz-Chunkz);
		Boundx = Boundx/BLOCKSCALE;
		Boundz = Boundz/BLOCKSCALE;
/*		std::cout << "Boundx: " << Boundx.x << ":" << Boundx.y << std::endl;
		std::cout << "Boundz: " << Boundz.x << ":" << Boundz.y << std::endl;*/

		for(int i=fmaxf(fminf(playerPos.y/BLOCKSCALE,finalPoint.y/BLOCKSCALE),0.0f); i<fminf(fmaxf(playerPos.y/BLOCKSCALE,finalPoint.y/BLOCKSCALE),CHUNKHEIGHT); i++){
			for(int j=(int)(Boundz.x); j<=(int)(Boundz.y); j++){
				for(int k=(int)(Boundx.x); k<=(int)(Boundx.y); k++){
					inter = -1;
					//std::cout << "\tk: " << k << "\n\ti: " << i << "\n\tj: " << j << std::endl;
					if(tempChunk->grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k] == nullptr) 
						continue;
					//std::cout << "Chunk Number: " << CheckedChunks[c].x << ":" << CheckedChunks[c].y << std::endl;
					//std::cout << "Block Number: " << k << ":" << i << ":" << j << std::endl;
					//tempChunk->grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]->getPos();
					inter = Intersection(tempChunk->grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k], sight);
					if(inter!=-1){
						viableBlockList.push_back(tempChunk->grid[(i*CHUNKDIMS*CHUNKDIMS)+(j*CHUNKDIMS)+k]);
						viableSides.push_back(inter);
					}
				}
			}
		}
	}
	//std::cout << "After Intersection." << std::endl;
	if(viableBlockList.size() == 0)
		return nullptr;

	glm::vec3 blockPos;
	glm::vec3 camDist;
	float tempDist = 0.0f;
	float finalDist = 99999.0f;
	int minBlock = viableBlockList.size();
	for(int i=0; i<viableBlockList.size(); i++){
		blockPos = viableBlockList[i]->getPos();
		camDist = blockPos-playerPos;
		tempDist = sqrt(pow(camDist.x,2)+pow(camDist.y,2)+pow(camDist.z,2));
		if(tempDist <= finalDist){
			finalDist = tempDist;
			minBlock = i;
		}
	} 
 	blockPos = viableBlockList[minBlock]->getPos();
 	self.setSelectedSide(viableSides[minBlock]);
 	State.setPlayer(0, self);
 	//std::cout << "There Exists a Viable Block at" << std::endl;
 	//std::cout << "\tX: " << blockPos.x <<"\n\tY: " << blockPos.y << "\n\t Z: " << blockPos.z << std::endl;  
	return viableBlockList[minBlock];

}


// Create a nullptr-terminated string by reading the provided file
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
	if (fileName == nullptr){
		perror("Have not set a filename, Please do so before trying to load an obj file again.");
		return;
	}
	std::cout << "Loading in Asset as : " << fileName << std::endl;
	//opening the OBJ file for reading
	FILE * objFile = fopen(fileName, "r");
	if (objFile == nullptr){
		perror("THAT FILE DOES NOT EXIST");
		exit(EXIT_FAILURE);
	}
	//creating a buffer to read the file into
	char fileBuffer[128];
	std::vector<GLuint> vertexIndices;
	std::vector<GLuint> normalIndices;
	std::vector<GLuint> textureIndices;
	while (fgets(fileBuffer, 128, objFile) != nullptr){
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
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr,
	nullptr, nullptr);
	if (!png_ptr) {
		fclose(fp);
		perror("TEXTURE LOAD ERROR 2");
	}

	//create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_read_struct(&png_ptr, (png_infopp) nullptr, (png_infopp) nullptr);
		fclose(fp);
		perror("TEXTURE LOAD ERROR 3");
	}

	//create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info) {
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) nullptr);
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
	nullptr, nullptr, nullptr);

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
    glShaderSource(vertShader, 1, &vertShaderSrc, nullptr);
    glCompileShader(vertShader);

    // Check vertex shader
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> vertShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(vertShader, logLength, nullptr, &vertShaderError[0]);
    if(logLength > 1)std::cout << "vert Errors: " << &vertShaderError[0] << std::endl;

    // Compile fragment shader
    std::cout << "Compiling fragment shader." << std::endl;
    glShaderSource(fragShader, 1, &fragShaderSrc, nullptr);
    glCompileShader(fragShader);

    // Check fragment shader
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, nullptr, &fragShaderError[0]);
    if(logLength > 1) std::cout << "Frag Errors: " << &fragShaderError[0] << std::endl;

    std::cout << "Linking program" << std::endl;
    GLuint program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &result);
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError( (logLength > 1) ? logLength : 1 );
    glGetProgramInfoLog(program, logLength, nullptr, &programError[0]);
    if(logLength > 1)std::cout << "Prog Errors: " <<&programError[0] << std::endl;

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
	glUseProgram(program);
    return program;
}

bool compBlockDist(glm::mat4 a, glm::mat4 b){
	glm::vec3 camPos = Options.getCamRot();
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
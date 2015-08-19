#include "../Headers/CryonyxBase.h"


void ObjectRenderer::ReadOBJFile(){
	//Make sure the object file has a name already assigned
	if (objFileName == NULL){
		perror("Have not set a filename, Please do so before trying to load an obj file again.");
		return;
	}
	printf("Loading in Asset at: %s\n", objFileName);
	//opening the OBJ file for reading
	FILE * objFile = fopen(objFileName, "r");
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
			vertices.push_back(verts);
		} else if (strcmp(identifier, "vn") == 0){
			glm::vec4 norm;
			sscanf(fileBuffer, "%*s %f %f %f", &norm.x, &norm.y, &norm.z);
			norm.w = 0.0;
			vertexNormals.push_back(norm);
		} else if (strcmp(identifier, "vt") == 0){
			glm::vec2 tex;
			sscanf(fileBuffer, "%*s %f %f", &tex.x, &tex.y);
			textureCoords.push_back(tex);
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
	std::map<vertexInfo, GLuint> indexMap;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> tex;

	//Fix all the indices so that it doesnt have to be done later.
	//This needs to be done since open gl only allows one index array for each object so, you need to reorganize the indices that are read in from the OBJ file.

	for (uint i = 0; i < vertexIndices.size(); i++){
		struct vertexInfo v = { vertices[vertexIndices[i]], vertexNormals[normalIndices[i]], textureCoords[textureIndices[i]] };
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
			indices.push_back(index);
		} else{
			verts.push_back(vertices[vertexIndices[i]]);
			norms.push_back(vertexNormals[normalIndices[i]]);
			tex.push_back(textureCoords[textureIndices[i]]);
			index = (GLuint)verts.size() - 1;
			indices.push_back(index);
			indexMap[v] = index;
		}
	}

	//The vectors must be re-applied so that the order is correct in correspondence with the new index array.
	vertices = verts;
	vertexNormals = norms;
	textureCoords = tex;

	fclose(objFile);
}

void ObjectRenderer::SetOBJFile(char * fileName){
	objFileName = fileName;
}
//Function to load in a BMP for texture mapping
void ObjectRenderer::loadBMP(){
	//variables declarations
	unsigned char header[54];
	unsigned int width, height;
	unsigned char * data;
	unsigned int imageSize, dataPos;

	FILE * BMPfile = fopen(texFileName, "rb");
	if (!BMPfile){
		perror("error reading bmp.");
	}
	if (fread(header, 1, 54, BMPfile) != 54){
		perror("fread on header failed.");
	}
	if (header[0] != 'B' || header[1] != 'M'){
		perror("NOT A CORRECT BMP FILE.");
	}
	dataPos = *(int*)&(header[0x0A]);
	imageSize = *(int*)&(header[0x22]);
	width = *(int*)&(header[0x12]);
	height = *(int*)&(header[0x16]);

	if (imageSize == 0) imageSize = width*height;
	if (dataPos == 0) dataPos = 54;

	data = new unsigned char[imageSize];
	fread(data, 1, imageSize, BMPfile);

	fclose(BMPfile);
	glBindTexture(GL_TEXTURE_2D, vertexBuffer[2]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, data);
}


void ObjectRenderer::SetTexFile(char * fileName){
	texFileName = fileName;
}

ObjectRenderer::ObjectRenderer(){
	objFileName = NULL;
	texFileName = NULL;
}

ObjectRenderer::ObjectRenderer(char * objFile, char * texFile){
	objFileName = objFile;
	texFileName = texFile;
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	for (uint i = 0; i < Renderer.Objects.size(); i++){
		drawOBJ(Renderer.Objects[i]);
	}
	
	glutWarpPointer(glutGet(GLUT_WINDOW_WIDTH)/2, glutGet(GLUT_WINDOW_HEIGHT)/2);
	glutSwapBuffers();
}

void drawOBJ(class Object * obj){
	glBindVertexArray(obj->renderer->vertexArray);
	Renderer.viewMatrix =  glm::lookAt(glm::vec3(2.0f,2.0f,2.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.0f,1.0f,0.0f));
	glm::mat4 mv = Renderer.modelMatrices[Translate]  * Renderer.modelMatrices[Scale] * Renderer.modelMatrices[Rotate] * Renderer.viewMatrix;
	float * vars = Options.getProjVar();
	glm::mat4 p = glm::perspective(vars[0], vars[1], vars[2], vars[3]);
	glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "mv"), 1, GL_FALSE, &mv[0][0]);
	glUniformMatrix4fv(glGetUniformLocation(Renderer.getShaderProgram(), "proj"), 1, GL_FALSE, &p[0][0]);
	//Loading the tex to gpu will happen here.
	glDrawElements(GL_TRIANGLES, obj->renderer->indices.size() * sizeof(GLuint), GL_UNSIGNED_INT, BUFFER_OFFSET(0));
}
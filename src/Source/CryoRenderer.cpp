#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoRenderer.hpp"


//Const Global Array for the different vertexes of a cube. These Can be accessed with the enum CubeVertices.
const glm::vec3 CubeVerts[8] = {
glm::vec3(-HALFSIZE,HALFSIZE,-HALFSIZE),glm::vec3(HALFSIZE,HALFSIZE,-HALFSIZE),
glm::vec3(-HALFSIZE,HALFSIZE, HALFSIZE),glm::vec3(HALFSIZE,HALFSIZE,HALFSIZE),
glm::vec3(-HALFSIZE,-HALFSIZE,-HALFSIZE),glm::vec3(HALFSIZE,-HALFSIZE,-HALFSIZE),
glm::vec3(-HALFSIZE,-HALFSIZE,HALFSIZE),glm::vec3(HALFSIZE,-HALFSIZE,HALFSIZE)
};

//Const GlobaL array for the different directional vectors, these can be accessed with the CubeFace Enum.
const glm::vec3 DirectionVectors[6] = {
glm::vec3(-1.0,0.0,0.0), glm::vec3(1.0,0.0,0.0),
glm::vec3(0.0,1.0,0.0), glm::vec3(0.0,-1.0,0.0),
glm::vec3(0.0,0.0,1.0), glm::vec3(0.0,0.0,-1.0)
};

//Functions for the Voxel Class
Voxel::Voxel(){

}

Voxel::~Voxel(){

	
}

bool Voxel::getActive(){
	return voxActive;	
}

void Voxel::setActive(bool va){
	voxActive = va;
}

VoxelColor Voxel::getColor(){
	return voxColor;
}

void Voxel::setColor(VoxelColor vc){
	voxColor = vc;
}

Mesh::Mesh(bool activeSides[6] = {true,true,true,true,true,true}, glm::vec3 offset){
	glGenvertexArrays(1, &VertexArrayObject);
	glBindVertexArray(VertexArrayObject);

	glGenBuffers(4, &VertexBuffer[0]);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LVertexBuffer]);



	updateMesh();
}

Mesh::~Mesh(){

}

void Mesh::InitiateMesh(){
	glBindVertexArray(VertexArrayObject);

	//Initializing the vertex and normals for this particular mesh.
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LVertexBuffer]);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4) + vertexNormals.size()*sizeo(glm::vec4),nullptr, GL_DYNAMIC_DRAW);
	glbufferSubData(GL_ARRAY_BUFFER, 0, vertices.size()*sizeof(glm::vec4), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec4), vertexNormals.size()*sizeof(glm::vec4), vertexNormals.data());

	shaderPositions.emplace_back();
	shaderPositions[0] = glGetAttribLocation(/*Shader pointer goes here*/, /*Name within shader will go here.*/);
	glEnableVertexAttribArray(shaderPositions[0]);
	glVertexAttribPointer(shaderPositions[0], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	shaderPositions.emplace_back();
	shaderPositions[1] = glGetAttribLocation(/*Shader pointer goes here*/, /*Name within shader will go here.*/);
	glEnableVertexAttribArray(shaderPositions[1]);
	glVertexAttribPointer(shaderPositions[1], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertices.size() * sizeof(glm::vec4)));

	//Initializing the texture values for this mesh.
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LTextureBuffer]);
	glBufferData(GL_ARRAY_BUFFER, textureCoords.size()*sizeof(glm::vec2), nullptr, GL_DYNAMIC_DRAW);
	glBufferData(GL_ARRAY_BUFFER, 0, textureCoords.size()*sizeof(glm::vec2), textureCoords.data());

	shaderPositions.emplace_back();
	shaderPositions[2] = glGetAttribLocation(/*Shader pointer goes here*/, /*Name within shader will go here.*/);
	glEnableVertexAttribArray(shaderPositions[2]);
	glVertexAttribPointer(shaderPositions[2], 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	//Initialize which texture will be utilized for this particular Side.
	
	
	//Initialize the Model matrices / Will I even need this? Or can this be passed in as a uniform for this particular Mesh?
	// glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer[LModelMatrices]);
	// glBufferData(GL_ARRAY_BUFFER, /*Size betwen matrices*/, nullptr, GL_DYNAMIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, 0, textureCoords.size()*sizeof(glm::vec2), textureCoords.data());

	// shaderPositions.emplace_back();
	// shaderPositions[3] = glGetAttribLocation(/*Shader pointer goes here*/, /*Name within shader will go here.*/);
 //    glEnableVertexAttribArray(shaderPositions[3]); 
 //    glVertexAttribPointer(shaderPositions[3], 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(listStartingAddress+0));
 //    glEnableVertexAttribArray(shaderPositions[3]+1); 
 //    glVertexAttribPointer(shaderPositions[3]+1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(listStartingAddress+(vec4Size)));
 //    glEnableVertexAttribArray(shaderPositions[3]+2); 
 //    glVertexAttribPointer(shaderPositions[3]+2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(listStartingAddress+(vec4Size*2)));
 //    glEnableVertexAttribArray(shaderPositions[3]+3); 
 //    glVertexAttribPointer(shaderPositions[3]+3, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(listStartingAddress+(vec4Size*3)));
 //    glVertexAttribDivisor(shaderPositions[3], 1);
 //    glVertexAttribDivisor(shaderPositions[3]+1, 1);
 //    glVertexAttribDivisor(shaderPositions[3]+2, 1);
 //    glVertexAttribDivisor(shaderPositions[3]+3, 1);


}

void Mesh::updateMesh(){
	for(int i=0; i<6; i++){
		if(activeSides[i] == true)
			GenerateCubeSide(i,offset);
	}	
	InitiateMesh();
}

void Mesh::GenerateCubeSide(CubeFace face, glm::vec3 offset){
	switch(face){
		case LeftFace:
			addVertexToMesh(offset+CubeVerts[TopBackLeft]);
			addVertexToMesh(offset+CubeVerts[TopFrontLeft]);
			addVertexToMesh(offset+CubeVerts[BottomBackLeft]);
			addVertexToMesh(offset+CubeVerts[BottomFrontLeft]);
			break;
		case RightFace:
			addVertexToMesh(offset+CubeVerts[TopBackRight]);
			addVertexToMesh(offset+CubeVerts[TopFrontRight]);
			addVertexToMesh(offset+CubeVerts[BottomBackRight]);
			addVertexToMesh(offset+CubeVerts[BottomFrontRight]);
			break;
		case TopFace:
			addVertexToMesh(offset+CubeVerts[TopBackLeft]);
			addVertexToMesh(offset+CubeVerts[TopFrontLeft]);
			addVertexToMesh(offset+CubeVerts[TopBackRight]);
			addVertexToMesh(offset+CubeVerts[TopFrontRight]);
			break;
		case BottomFace:
			addVertexToMesh(offset+CubeVerts[BottomBackLeft]);
			addVertexToMesh(offset+CubeVerts[BottomFrontLeft]);
			addVertexToMesh(offset+CubeVerts[BottomBackRight]);
			addVertexToMesh(offset+CubeVerts[BottomFrontRight]);
			break;
		case FrontFace:
			addVertexToMesh(offset+CubeVerts[TopFrontLeft]);
			addVertexToMesh(offset+CubeVerts[TopFrontRight]);
			addVertexToMesh(offset+CubeVerts[BottomFrontLeft]);
			addVertexToMesh(offset+CubeVerts[BottomFrontRight]);
			break;
		case BackFace:
			addVertexToMesh(offset+CubeVerts[TopBackLeft]);
			addVertexToMesh(offset+CubeVerts[TopBackRight]);
			addVertexToMesh(offset+CubeVerts[BottomBackLeft]);
			addVertexToMesh(offset+CubeVerts[BottomBackRight]);
			break;
		default:
			std::cout << "INVALID SIDE GENERATION ATTEMPT." << std::endl;
			break;
	}
}

void Mesh::addVertexToMesh(glm::vec3 vert){
	glm::vec4 finalVertex(vert, 1.0f);

}

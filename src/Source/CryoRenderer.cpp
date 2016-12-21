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

void Mesh::GenerateCubeSide(CubeFace face, glm::vec4 offset){
	switch(face){
		case LeftFace:
			//addVertexToMesh();
			break;
		case RightFace:
			break;
		case TopFace:
			break;
		case BottomFace:
			break;
		case FrontFace:
			break;
		case BackFace:
			break;
		default:
			std::cout << "INVALID SIDE GENERATION ATTEMPT." << std::endl;
			break;
	}
}

void Mesh::addVertexToMesh(glm::vec4 vert){


}
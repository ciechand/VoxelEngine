#include "../Headers/CryoMain.hpp"
#include "../Headers/CryoUtil.hpp"
#include "../Headers/CryoLighting.hpp"
#include "../Headers/CryoRenderer.hpp"

LightController lightController;

Light::Light(glm::vec4 pos, glm::vec3 color, float intensity){
	lightpos = pos;
	lightcolor = glm::vec4(color,intensity);
}

Light::Light(){
}

Light::~Light(){

}

glm::vec4 Light::getPos(){
	return lightpos;
}

void Light::setPos(glm::vec4 p){
	lightpos = p;
}

glm::vec4 Light::getColor(){
	return lightcolor;
}

void Light::setColor(glm::vec4 c){
	lightcolor = c;
}
/*
float Light::getInt(){
	return lightintensity;
}

void Light::setInt(float d){
	lightintensity = d;
}*/

//Start of functions for light controller.
LightController::LightController(){
	addLight();
	addLight(glm::vec4(CHUNKSIDE*0.5f,20.0f,CHUNKSIDE*0.5f,1.0f), glm::vec3(0.0,0.0,255.0), 0.9f);
	addLight(glm::vec4(CHUNKSIDE*2.0f,13.0f,CHUNKSIDE*2.0f,1.0f), glm::vec3(255.0,0.0,0.0), 0.3f);
	//addLight(glm::vec4(CHUNKSIDE*5.0f,17.0f,CHUNKSIDE*8.0f,1.0f), glm::vec3(0.0,255.0,0.0), 1.5f);

	//Setup the SSAO Kernel Samples
	std::random_device r;
	std::default_random_engine randE(r());
	std::uniform_real_distribution<> uniform_distxy(-1.0, 1.0);
	std::uniform_real_distribution<> uniform_distz(0.0, 1.0);
	SSAOKernel.assign(KERNELSIZE, glm::vec3());
	SSAONoise.assign(KERNELSIZE, glm::vec3());
	for (int i = 0; i < KERNELSIZE; ++i) {
		SSAOKernel[i] = glm::vec3(uniform_distxy(randE), uniform_distxy(randE), uniform_distz(randE));
		SSAOKernel[i] = glm::normalize(SSAOKernel[i]);
		SSAOKernel[i] *= uniform_distz(randE);
		float scale = float(i) / float(KERNELSIZE);
		scale = lerp(0.1f, 1.0f, scale * scale);
		SSAOKernel[i] *= scale;

		SSAONoise[i] = glm::vec3(uniform_distxy(randE),uniform_distxy(randE),0.0f);
		SSAONoise[i] = SSAONoise[i];
	}

}

LightController::~LightController(){
}

unsigned int LightController::getNumLights(){
	return lightList.size();
}

Light* LightController::getLightData(){
	return lightList.data();
}

glm::mat4* LightController::getLightMatData(){
	return matrixList.data();
}

glm::vec3 * LightController::getSSAOKernelData(){
	return SSAOKernel.data();	
}

glm::vec3 * LightController::getSSAONoiseData(){
	return SSAONoise.data();	
}

void LightController::addLight(){
	addLight(glm::vec4(0.0,0.0,0.0,1.0), glm::vec3(255.0f,255.0,255.0), 0.0f);
}

void LightController::addLight(Light l){
	//Repair this function
	lightList.emplace_back(l);
}

void LightController::addLight(glm::vec4 pos, glm::vec3 color, float intensity){
	lightList.emplace_back(pos, color, intensity);

	//Add the lights coorisponding matrices to the vectors;
	glm::vec3 center = (Camera.getCamAt()-glm::vec3(pos));
	float halfSideLength = (100.0f*glm::normalize(glm::length(Camera.getCamAt()-glm::vec3(pos)))*(float)sqrt(3))/(3.0f);
	glm::mat4 tempProjShadowMatrix = glm::ortho<float>(center.x-halfSideLength, center.x+halfSideLength, center.y-halfSideLength,center.y+halfSideLength, center.z-halfSideLength, center.z+halfSideLength);
	//At some point in the future I will need to figure out what to change the at value to such that it actually cooresponds properly.
	glm::mat4 tempViewShadowMatrix = glm::lookAt(glm::vec3(pos),Camera.getCamAt(),glm::vec3(0.0f,1.0f,0.0f));

	lightMatrices.emplace_back(std::pair<glm::mat4,glm::mat4>(tempProjShadowMatrix, tempViewShadowMatrix));

	matrixList.emplace_back(tempProjShadowMatrix*tempViewShadowMatrix);
}

void LightController::setLight(unsigned int index, glm::vec4 pos, glm::vec3 color, float inten){
	lightList[index].setPos(pos);
	lightList[index].setColor(glm::vec4(color,inten));

	//Add the lights coorisponding matrices to the vectors;
	glm::vec3 center = (Camera.getCamAt()-glm::vec3(pos));
	float halfSideLength = (100.0f*glm::normalize(glm::length(Camera.getCamAt()-glm::vec3(pos)))*(float)sqrt(3))/(3.0f);
	glm::mat4 tempProjShadowMatrix = glm::ortho<float>(center.x-halfSideLength, center.x+halfSideLength, center.y-halfSideLength,center.y+halfSideLength, center.z-halfSideLength, center.z+halfSideLength);
	//At some point in the future I will need to figure out what to change the at value to such that it actually cooresponds properly.
	glm::mat4 tempViewShadowMatrix = glm::lookAt(glm::vec3(pos),Camera.getCamAt(),glm::vec3(0.0f,1.0f,0.0f));

	lightMatrices[index] = (std::pair<glm::mat4,glm::mat4>(tempProjShadowMatrix, tempViewShadowMatrix));

	matrixList[index] = (tempProjShadowMatrix*tempViewShadowMatrix);
}

void LightController::setLight(unsigned int index, glm::vec4 pos, glm::vec3 color, float inten, glm::mat4 proj, glm::mat4 view){
	lightList[index].setPos(pos);
	lightList[index].setColor(glm::vec4(color,inten));

	lightMatrices[index] = (std::pair<glm::mat4,glm::mat4>(proj, view));

	matrixList[index] = (proj*view);
}

Light * LightController::getLight(unsigned int index){
	return &(lightList[index]);
}

std::vector<glm::mat4> LightController::getMatrix(){
	return matrixList;
}

glm::mat4 LightController::getMatrix(unsigned int index){
	return matrixList[index];
}

std::pair<glm::mat4,glm::mat4> LightController::getMatrixPair(unsigned int index){
	return lightMatrices[index];
}

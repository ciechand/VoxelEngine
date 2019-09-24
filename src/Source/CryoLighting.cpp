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
	//Setup the SSAO Kernel Samples
	std::random_device r;
	std::default_random_engine randE(r());
	std::uniform_real_distribution<> uniform_distxy(-1.0, 1.0);
	std::uniform_real_distribution<> uniform_distz(0.5, 1.0);
	SSAOKernel.assign(KERNELSIZE, glm::vec3());
	for (int i = 0; i < KERNELSIZE; ++i) {
		glm::vec3 tempKernel;
		tempKernel = glm::vec3(uniform_distxy(randE), uniform_distxy(randE), uniform_distz(randE));
		tempKernel = glm::normalize(tempKernel);
		tempKernel *= uniform_distz(randE);
		float scale = float(i) / float(KERNELSIZE);
		scale = lerp(0.1f, 1.0f, scale * scale);
		tempKernel *= scale;
		SSAOKernel[i] = tempKernel;
	}
	SSAONoise.assign(NOISESIZE, glm::vec3());
	for(int i=0; i<NOISESIZE; i++){
		SSAONoise[i] = glm::vec3(uniform_distxy(randE),uniform_distxy(randE),0.0f);
	}
}

LightController::~LightController(){
}

void LightController::initialize(){
	addLight(glm::vec4(CHUNKSIDE*1.0f,20.0f,CHUNKSIDE*1.0f,1.0f), glm::vec3(255.0,0.0,0.0), 50.0f);
	addLight(glm::vec4(CHUNKSIDE*5.0f,20.0f,CHUNKSIDE*8.0f,1.0f), glm::vec3(0.0,255.0,0.0), 100.0f);
}

unsigned int LightController::getNumLights(){
	return lightList.size();
}

Light* LightController::getLightData(){
	glm::mat4 rotateMat = glm::rotate(0.001f, glm::vec3(0.0f,1.0f,0.0f));
	for(int i=0; i<lightList.size(); i++){
		setLightPos(i,rotateMat*lightList[i].getPos());
	}
	return lightList.data();
}

glm::mat4* LightController::getLightMatData(unsigned int index){
	return viewMatricesList[index].data();
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
	viewMatricesList.emplace_back();
	setLight(viewMatricesList.size()-1, pos, color, intensity);
}

void LightController::setLight(unsigned int index, glm::vec4 pos, glm::vec3 color, float inten){
	lightList[index].setPos(pos);
	lightList[index].setColor(glm::vec4(color,inten));

	//Add the lights coorisponding matrices to the vectors;
	viewMatricesList[index].assign(6, glm::mat4());
	for(int i=0; i<6; i++){
		glm::vec3 up = glm::vec3();
		if(i == 0 || i == 1 || i == 4 || i == 5)
			up = glm::vec3(0.0f,-1.0f,0.0f);
		else if (i == 2)
			up = glm::vec3(0.0f,0.0f,1.0f);
		else if (i == 3)
			up = glm::vec3(0.0f, 0.0f,-1.0f);
		if((DEBUGMODE == true) == true)  std::cerr << pos.x << "," << pos.y << "," << pos.z << std::endl;
		viewMatricesList[index][i] = glm::lookAt(glm::vec3(pos), glm::vec3(pos) + DirectionVectors[i], up);
	}
}

void LightController::setLightPos(unsigned int index, glm::vec4 pos){
	setLight(index, pos, glm::vec3(lightList[index].getColor()), (lightList[index]).getColor().w);
}

Light * LightController::getLight(unsigned int index){
	return &(lightList[index]);
}

std::vector<glm::mat4> LightController::getMatrix(unsigned int index){
	return viewMatricesList[index];
}

glm::mat4 LightController::getMatrix(unsigned int index, unsigned int dir){
	return viewMatricesList[index][dir];
}

void LightController::incRotation(){
	for(int i=0; i<lightList.size(); i++){
		glm::vec4 lightPos = lightList[i].getPos();
	/*	glm::vec4 tempVec = lightPos - glm::vec4(centerOfRotation,1.0);
		glm::vec4 newlightPos = glm::rotate(tempVec, glm::radians(3.0f), centerOfRotation);*/
		glm::mat4 translateMat = glm::translate(glm::mat4(), glm::vec3(0.0f,0.0f,-1.0f));
		glm::vec4 newlightPos = translateMat*lightPos;
		setLightPos(i,newlightPos);
	}
}
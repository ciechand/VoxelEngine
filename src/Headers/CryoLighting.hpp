#ifndef CryoLighting
#define CryoLighting

#include <vector>
#include <random>

#include <glm/glm.hpp>


//Light class, this class contains everythng necessary to create a light. Directional and Point Lights will inherit from this class.
class Light{
	public:
		Light(glm::vec4 pos, glm::vec3 color, float inten);
		Light();
		~Light();

		glm::vec4 getPos();
		void setPos(glm::vec4 p);

		glm::vec4 getColor();
		void setColor(glm::vec4 c);
	private:
		glm::vec4 lightpos = glm::vec4(0.0f,0.0f,0.0f,1.0f);
		glm::vec4 lightcolor = glm::vec4(255.0f,255.0f,255.0f,100.0f);
};

//Light SourceController : this will need one array of lights and one array of std:pair<glm::mat4,glm::mat4>(viewMatrix, projectionMatrix)
class LightController{
	public:
		LightController();
		~LightController();

		unsigned int getNumLights();
		Light* getLightData();
		glm::mat4* getLightMatData();
		glm::vec3 * getSSAOKernelData();
		glm::vec3 * getSSAONoiseData();

		void addLight();
		void addLight(Light l);
		void addLight(glm::vec4 pos, glm::vec3 color, float inten);
		void setLight(unsigned int index, glm::vec4 pos, glm::vec3 color, float inten);		
		void setLight(unsigned int index, glm::vec4 pos, glm::vec3 color, float inten, glm::mat4 proj, glm::mat4 view);
		Light * getLight(unsigned int index);

		std::vector<glm::mat4> getMatrix();
		glm::mat4 getMatrix(unsigned int index);
		std::pair<glm::mat4,glm::mat4> getMatrixPair(unsigned int index);

	private:
		std::vector<glm::vec3> SSAOKernel;
		std::vector<glm::vec3> SSAONoise;
		std::vector<Light> lightList;
		std::vector<glm::mat4> matrixList;
		std::vector<std::pair<glm::mat4,glm::mat4> > lightMatrices;
};

extern LightController lightController;
#endif //CryoLighting
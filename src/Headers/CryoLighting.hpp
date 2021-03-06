#ifndef CryoLighting
#define CryoLighting

#include <vector>
#include <random>

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>

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

		void initialize();

		unsigned int getNumLights();
		Light* getLightData();
		glm::mat4* getLightMatData(unsigned int index);
		glm::vec3 * getSSAOKernelData();
		glm::vec3 * getSSAONoiseData();

		void incRotation();

		void addLight();
		void addLight(Light l);
		void addLight(glm::vec4 pos, glm::vec3 color, float inten);
		void setLight(unsigned int index, glm::vec4 pos, glm::vec3 color, float inten);
		void setLightPos(unsigned int index, glm::vec4 pos);
		Light * getLight(unsigned int index);

		std::vector<glm::mat4> getMatrix(unsigned int index);
		glm::mat4 getMatrix(unsigned int index,unsigned int dir);

	private:
		glm::vec3 centerOfRotation = glm::vec3(CHUNKSIDE,20.0f,CHUNKSIDE);
		float globalLightRotation = 0;
		std::vector<glm::vec3> SSAOKernel;
		std::vector<glm::vec3> SSAONoise;
		std::vector<Light> lightList;
		std::vector<std::vector<glm::mat4> > viewMatricesList;
};

extern LightController lightController;
#endif //CryoLighting
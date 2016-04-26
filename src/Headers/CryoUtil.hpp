#ifndef util
#define util

typedef class Ray{
	public:
		glm::vec3 Origin;
		glm::vec3 Direction;
		glm::vec3 InverseDirection;
		float times[2];
		int sign[3];
		Ray();
		Ray(glm::vec3 O, glm::vec3 D, float t0, float t1);

}Ray;

typedef class CollisionResult{
	public:
		int side;
		float minCol;
		float maxCol;
		CollisionResult();
		CollisionResult(int s, float n, float x);
}ColRes;

typedef class BoundingBox{
	private:
		glm::vec3 Position;
		glm::vec3 prevPos;
		glm::vec3 Limbs;
	public:
		BoundingBox();
		
		glm::vec3 getPrevPos();

		void setPos(glm::vec3 pos);
		glm::vec3 getPos();
		
		void setLimbs(glm::vec3 limb);
		glm::vec3 getLimbs();

		glm::vec3 getMin();
		glm::vec3 getMax();
}BBox;


ColRes Intersection(BBox bounds, Ray r);
ColRes Intersection(BBox a, BBox b);

glm::vec3 playerCollideTerrain(BoundingBox playerBox, glm::vec3 moveDir, int & axis);

std::pair<Block *, int> pickBlock(Ray sight);

//Reads a file in and interprets it into a nullptr-Terminated string.
std::string readFileToString(const char * fileName);

//Function for loading in an OBJ file.
void readOBJFile(IOBJ * Object, const char * fileName);

//File for loading in a BMP and assigning it to a specific texture.
void loadBMP(const char * fileName, GLuint * textureID);

//Function for loading in the two shader programs, compiling them and spitting out the GLuint.
GLuint createShadersProgram(const char* vsFile, const char* fsFile);

//Function for comparing two block distances from the camera.
bool compBlockDist(glm::mat4 a, glm::mat4 b);

//Ways of printing matrices and vertices. (Functions will be added as necessary)
void printMatrix(glm::mat4 matrix);
void printMatrix(glm::mat3 matrix);
void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);

float clamp(float min, float max, float f);
double clamp(double min, double max, double f);
int clamp(int min, int max, int f);

int blockNamesFind(std::string s);
glm::mat4 alignVec(glm::vec3 a, glm::vec3 b);


#endif //util
#include "CryoUtil.hpp"

#ifndef gen
#define gen

typedef class Block:public OBJ{
private:
	glm::vec3 color;
	glm::vec3 position;
	int Type; 
	int Owner;
	int ID;
	BBox bounds;
public:
	Block();
	Block(GLint id, GLint tex, int c); 
	~Block();

	void setColor(int c);	
	void setColor(glm::vec3 c);
	glm::vec3 getColor() const;
	int getColorNumber() const;

	void setPos(glm::vec3 point);
	glm::vec3 getPos() const;

	void setType(int t);
	int getType() const;

	void setOwner(int o);
	int getOwner() const;

	void setID(int id);
	int getID() const;

	void setBounds(BBox& b);
	void setBounds(glm::vec3 a);
	BBox& getBounds();
}Block;

typedef class Chunk{
private:
	glm::vec2 position;
	std::vector<std::vector<int> > heightMap;
public:
	std::vector<std::pair<glm::uint, glm::uint>> grid;
	Chunk();
	Chunk(glm::vec2 p);
	~Chunk();

	void Init();

	void setPos(glm::vec2 pos);
	glm::vec2 getPos();

	void setHeightMap(std::vector<std::vector<int> > hm);
	void setHeightMap(glm::vec2 pos, int value);
	std::vector<std::vector<int> > getHeightMap();
	int getHeightMap(glm::vec2 pos);

	void divide(float size = CHUNKDIMS-1);
	void diamond(float x, float y, float size, float offset);
	void square(float x, float y, float size, float offset);
}Chunk;

bool blockCmp(Block *a, Block *b);
float average(std::vector<int> vc);

#endif //gen
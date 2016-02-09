#ifndef gen
#define gen

typedef class Block:public OBJ{
private:
	//unsigned long long int objID; //This object ID is a unique identifier for every block, the first 16 bits are for the block identifier(4 for x, 4 for z, and 8 for y), the next 6 are the x coords of the chunk, then 6 for y coord of chunk, then the last 4 are for the dimension. 
	glm::vec3 color;
	glm::vec3 Position;
	int Type; 
	int Owner;
public:
	Block();
	Block(GLint id, GLint mat, int c); 
	
	void setColor(int c);	
	glm::vec3 getColor();

	void setPos(glm::vec3 point);
	glm::vec3 getPos();

	void setType(int t);
	int getType();

	void setOwner(int o);
	int getOwner();
}Block;

typedef class Chunk{
private:
	glm::vec2 position;
public:
	std::vector<Block*> grid;
	Chunk();
	Chunk(glm::vec2 p);
	~Chunk();

	void Init();

	void setPosition(glm::vec2 pos);
	glm::vec2 getPosition();
}Chunk;

bool blockCmp(Block *a, Block *b);

#endif //gen
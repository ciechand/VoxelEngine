#ifndef gen
#define gen

typedef class Block:public OBJ{
private:
	glm::vec3 color;
	glm::vec3 Position;
	int Type; 
	int Owner;
	int ID;
public:
	Block();
	Block(GLint id, GLint tex, int c); 
/*	Block& operator=(const Block& B){
		this->setTMatrix(B.getTMatrix());
		this->setMID(B.getMID());
		this->setTID(B.getTID());
		this->setColor(B.getColor());
		this->setPos(B.getPos());
		this->setType(B.getType());
		this->setOwner(B.getOwner());
		this->setID(B.getID());
		return *this;	
	}*/
	void setColor(int c);	
	void setColor(glm::vec3 c);
	glm::vec3 getColor() const;

	void setPos(glm::vec3 point);
	glm::vec3 getPos() const;

	void setType(int t);
	int getType() const;

	void setOwner(int o);
	int getOwner() const;

	void setID(int id);
	int getID() const;
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
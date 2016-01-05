#ifndef gen
#define gen

typedef class Block:public OBJ{
private:
	unsigned long long int objID; //This object ID is a unique identifier for every block, the first 16 bits are for the block identifier(4 for x, 4 for z, and 8 for y), the next 6 are the x coords of the chunk, then 6 for y coord of chunk, then the last 4 are for the dimension. 
	int material;
public:
	Block(int mat);
	Block(int mat, int id);

	void setMaterial(int mat);	
	int getMaterial();

	void setID(unsigned long long int id);
	unsigned long long int getID();

}Block;

typedef class Chunk{
private:
	glm::vec2 position;
public:
	std::vector<std::vector<std::vector<Block*> > > grid;
	Chunk();
	void Init();

	void setPosition(glm::vec2 pos);
	glm::vec2 getPosition();
}Chunk;

#endif //gen
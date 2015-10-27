#ifndef gen
#define gen

typedef class Block:public OBJ{
private:
	std::vector<int> sideMaterials;
public:
	Block(int id);

	void setSideMats(int side, int mat);
	void setSideMats(std::vector<int> mats);	
	int getSideMats(int side);

}Block;

typedef class Chunk{
private:
public:
	std::vector<std::vector<std::vector<Block*> > > grid;
	Chunk();
	void Init();
}Chunk;

#endif //gen
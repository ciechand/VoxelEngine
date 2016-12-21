#ifndef player
#define player

typedef class Player{
	private:
		int ID;
		glm::vec3 position;
		glm::vec2 curChunk;
		glm::vec3 relBlock;
		Ray SightLine;
		glm::vec3 movingDirection;
		glm::vec3 velocity;
		Block *selected;
		int selectedSide;
		bool jumping;
		BBox bounds;
		unsigned int hotbarSlot;
		Window * inventoryWindow;
		Window * HotBar;
		std::vector<Block*> Selectors;
	public:
		Player();
		~Player();

		void setPos(glm::vec3 p);
		glm::vec3 getPos();

		void setSight(Ray r);
		Ray getSight();

		void setMDir(glm::vec3 dir);
		glm::vec3 getMDIR();

		void setChunk(glm::vec2 c);
		glm::vec2 getChunk();

		void setRelBlock(glm::vec3 bp);
		glm::vec3 getRelBlock();

		void setID(int id);
		int getID();

		void setVelocity(glm::vec3 a);
		glm::vec3 getVelocity();

		void setSelected(Block * b);
		void clearSelected();
		Block * getSelected();

		void setSelectedSide(int side);
		int getSelectedSide();

		void setJumping(bool j);
		bool getJumping();

		void setBounds(BBox & b);
		void setBounds(glm::vec3 a);
		BBox & getBounds();

		void setHSlot(unsigned int s);
		void incrementHSlot(unsigned int n = 1);
		void decrementHSlot(unsigned int n =1);
		unsigned int getHSlot();

		baseItem * getItem(unsigned int index);
		void setItem(unsigned int index, baseItem * items);
		void addItem(baseItem * item);

		Block * getSelector(unsigned int index);
		std::vector<Block*> getSelector();
		void setSelector(unsigned int index, Block * b);
		void moveSelector(Block * b);
		void addSelector(Block * b, unsigned int color = Pink);
		void removeSelector(Block * b);
}Player;

#endif //player
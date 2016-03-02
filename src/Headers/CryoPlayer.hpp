#ifndef player
#define player

typedef class Player{
	private:
		int ID;
		glm::vec3 Position;
		glm::vec2 curChunk;
		Ray SightLine;
		glm::vec3 movingDirection;
		Block *selected;
		int selectedSide;
	public:
		Player();

		void addSelect(glm::mat4 position);
		void addSelect(glm::mat4 position, Colors16 c);
		void addSelect(glm::mat4 position, Colors16 c, SelectTypes type);
		void removeSelect();
		void removeSelect(SelectTypes type);

		void setPos(glm::vec3 p);
		glm::vec3 getPos();

		void setSight(Ray r);
		Ray getSight();

		void setMDir(glm::vec3 dir);
		glm::vec3 getMDIR();

		void setChunk(glm::vec2 c);
		glm::vec2 getChunk();

		void setID(int id);
		int getID();

		void setSelected(Block * b);
		void clearSelected();
		Block * getSelected();

		void setSelectedSide(int side);
		int getSelectedSide();
}Player;

#endif //player
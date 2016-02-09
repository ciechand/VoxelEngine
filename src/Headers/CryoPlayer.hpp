#ifndef player
#define player

typedef class Player{
	private:
		int ID;
		glm::vec3 Position;
		glm::vec2 curChunk;
		Ray SightLine;
		glm::vec3 movingDirection;
	public:
		Player();

		void addSelect(Block * b);
		void removeSelect();

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
}Player;

#endif //player
#ifndef window
#define window

typedef class BaseWindow:public OBJ{
	protected:
		bool hidden;
		glm::vec2 size;
		unsigned int depth;
		std::vector<Window> windowDraw;
		std::vector<Pane> Panes;
		std::vector<Slot> Slots;
		Window * parent;
		unsigned int windowType = SInv;
		unsigned int rotation = 0;
	public:	
		BaseWindow();
		BaseWindow(glm::vec2 tsize, glm::vec2 pos, glm::vec2 wsize, bool hide, unsigned int wType, Window * p = nullptr);
		~BaseWindow();

		bool getHidden();
		void setHidden(bool flag);

		glm::vec2 getSize();
		void setSize(glm::vec2 s);

		unsigned int getDepth();
		void setDepth(unsigned int d);

		void setPos(glm::vec3 pos);
		glm::vec3 getTruePos();

		void setColor(unsigned int c);
		void setColor(glm::vec3 c);

		void addPane(glm::vec2 pos, unsigned int pt);
		Pane& getPane(int index);
		std::vector<Pane> & getPane();

		void addSlots(unsigned int count, glm::vec2 spos);
		void addSlot();
		void addSlot(glm::vec2 pos);
		Slot& getSlot(int index);
		std::vector<Slot> & getSlot();

		unsigned int getWType();
		void setWType(unsigned int t);

		unsigned int getRotation();
		void setRotation(unsigned int r);

		void Draw();
		virtual void Interact();
}Window;

typedef class WindowPane:public Window{
	protected:
		unsigned int paneType;
	public:
		WindowPane();
		WindowPane(glm::vec2 tsize, glm::vec2 pos, glm::vec2 wsize, bool hide, Window * p = nullptr);
		~WindowPane();

		unsigned int getType();
		void setType(unsigned int t);

		void Interact();

}Pane;

typedef class WindowSlot:public Pane{
	private:
		baseItem * containedObj = nullptr;
	public:
		WindowSlot(glm::vec2 pos, Window * p = nullptr);
		~WindowSlot();

		baseItem * getObj();
		void setObj(baseItem * b);

		void Draw();
}Slot;

glm::vec2 arrayBounds(int x, int y, float maxx, float maxy);

#endif //window
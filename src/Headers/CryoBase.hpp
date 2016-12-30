#ifndef base
#define base

//enum defining the different game states
enum GameStates{GameExiting=0, GameLoading, GameRunning};

class StateController{
	public:
		StateContoller();
		~StateController();

		GameStates getCurrentGameState();
		void setCurrentGameState(GameStates state);
	private:
		GameStates CurrentGameState = GameLoading;
};


extern StateController MainController;

#endif //base
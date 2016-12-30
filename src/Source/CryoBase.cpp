#include "../Headers/CryoBase.hpp"

StateController MainController;

StateController::StateContoller(){

}

StateController::~StateController(){

}

GameStates StateController::getCurrentGameState(){
	return CurrentGameState;
}

void StateController::setCurrentGameState(GameStates state){
	CurrentGameState = state;
}

#ifndef input
#define input

//Functions for processing Opengl things such as keyboard and mouse movement.
void processMouseClicks(sf::Event e);
void processMouseMovement(sf::Event e);
void processMouseWheelMovement(sf::Event e);
void processKeyboardDown(sf::Event e);
void processKeyboardUp(sf::Event e);
void windowResized(sf::Event e);

//functions for processing things that will tick in the world e.g. movement.
void processMovement();


#endif //input
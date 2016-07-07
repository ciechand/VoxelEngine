#pragma GCC diagnostic ignored "-Wwrite-strings"

#define GLM_FORCE_RADIANS

//INCLUDES
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <random>
#include <png.h>
#include <utility>
#include <limits>

#include <GL/glew.h>
#include <GL/glu.h>

#include <SFML/Main.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/integer.hpp>

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>


// GLOBAL CONSTANT DEFINITIONS
#define PI 3.1415926535f
#define TIMECOMPENSATE 60.0f
#define TEXDIMS 16
#define NUMTEX 100
#define BLOCKSCALE 2.0f
#define CHUNKDIMS 17
#define CHUNKHEIGHT 20.0f
#define LOOKSPEED 1.0f
#define MOVESPEED 10.0f
#define CAMERADIST 20.0f
#define FOCUSDIST 15.0f
#define RRADIUS 2
#define RENDERRADIUS (2*(RRADIUS)+1)
#define GRAVITY -2.0f
#define TVEL 100.0f
#define PLAYERHEIGHT BLOCKSCALE*1.5f
#define PLAYERWIDTH BLOCKSCALE*0.9f
#define DEFAULTSLOTSIZE 32.0f
#define SLOTSEPSIZE DEFAULTSLOTSIZE/10.0f
#define DEFAULTWINDOWPANEL TEXDIMS*3
#define DEFAULTPLAYERSLOTS 40
#define DEFAULTHOTBARSLOTS 9

//MACROS
#define BUFFER_OFFSET(offset) ((GLvoid*)(intptr_t)(offset))

#ifndef Base
#define Base

//Forward declaring any classes necessary
typedef class BoundingBox BBox;
typedef class Block Block;
typedef class Chunk Chunk;
typedef class InstancedObject IOBJ;
typedef class Player Player;
typedef class BaseWindow Window;
typedef class WindowPane Pane;
typedef class WindowSlot Slot; 


//Enumeration of the states the game can be in.
// [0] = Paused
// [1] = Running
// [2] = Main Menu
// [3] = In game menu
// [4] = Loading

enum GameStates{Paused, Running, Main, Menu, Loading, Exiting};

//Enumeration for transform matrices access.
// [0] = Translate
// [1] = Scale
// [2] = Rotate
//NOTE: this is also the order they should be applied to the vector in, the vector is multiplied on the right.

enum TransformMatrixAtributes{Translate, Scale, Rotate}; 

//Enumeration for vertex attributes
// [0] = Vertex Position
// [1] = Vertex Normals
// [2] = Texture Coords
//NOTE: also the order they are in the VBO array.

enum VertexAttributes{Positions, Normals, Textures, ModelMatrices, TexturePos};

//Enumeration for Vertex Array Objects. There will only be two, one for singly draw things and one for the lists. so that the size doesnt need to be adjusted constantly.
// [0] = List Draw
// [1] = single Draw

enum VertexArrayObjects{ListBuffer, SingleBuffer};


//Enumeration for Vertex Buffer Objects
// [0] = vertex + vertexNormals
// [1] = Texture
// [2] = Block List
// [3] = Single Block

enum VertexBufferObjects{VBuffer, TBuffer, ArrayBuffer, ElementBuffer};

//Enum for faces of cube for texture mapping.
// [0] = Left = +x
// [1] = Right = -x
// [2] = Top = +y
// [3] = Bottom = -y
// [4] = Front = +z
// [5] = Back = -z

enum BlockFace{Right, Left, Top, Bottom, Front, Back};

//enum for the materials that blocks can be made of.
enum Materials{Air, Stone};

//Enum for Names of all the colors. may change from array to map or vector later.
enum Colors16{None, Red, Maroon, Pink, DPink, Purple, Aqua, Blue, Aquamarine, Cyan, Lime, Green, Yellow, Brown, Golden, Orange};

//Enum for the directions of movement
enum Movement{Forward, Backward, MLeft, MRight, Up, Down, NUMBER_OF_MOVEMENT};

//Enum defining which flags are which in GameState.
enum StateFlags{OpenInv, InvDown, NUMBER_OF_FLAGS};

//Enum for typesof Selector
enum SelectTypes{BaseSelector, Selector, Highlighter};

//enum for different Pane Types. OUtlined Below:
//Inventory Pane
//Equipment Pane
//Exit Pane
enum PaneType{InventoryP, EquipP, SlotP, ExitP};

//Enum for Window Types
//Player Inventory
//Storage inventory
//..
enum WindowTypes{FalseWindow, PInv, SInv, Hotbar};

//Enum for Item Types:
//
enum ItemTypes{Placeable, DestructionTool};

//Class for base object render info, many other object types such as Item and Block and window will inherit from this class.
typedef class baseObj{
	protected:
		//These variables need to be in this specific order becuase this is the order that they are passed to the shader.
		glm::mat4 TransformMatrix;
		glm::vec2 textureOffset;
		glm::vec3 color;
		glm::vec2 textureSize;
		unsigned int textureID;
		
		//The Variables below are not passed to shaders
		GLint modelIdentifier;
		glm::vec3 position;
		glm::mat4 Matrices[3];
		int ID;
	public:
		baseObj();
		~baseObj();

		glm::mat4 * getStartingAddr();

		void setColor(int c);	
		void setColor(glm::vec3 c);
		glm::vec3 getColor() const;
		int getColorNumber() const;

		void setPos(glm::vec3 point, bool alignToWindow = false);
		glm::vec3 getPos() const;

		void setTMatrix(glm::mat4 matrix, int index);
		glm::mat4 getTMatrix(int index) const;
		glm::mat4 getTMatrix() const;

		void setMID(GLint id);
		GLint getMID() const;

		void setTOff(GLint offset);
		void setTOff(glm::vec2 offset);
		glm::vec2 getTOff() const;

		void setTSize(glm::vec2 size);
		glm::vec2 getTSize() const;

		void setID(int id);
		int getID() const;

		virtual void Interact();
		virtual void Draw();
}OBJ;

//base Item class for anything that will be able to be stored in an inventory slot. 
//inherits from BaseObj so every item has the info for a renderer. 
typedef class baseItem:public baseObj{
	protected:
		std::pair<unsigned int, unsigned int> StackInfo;
		unsigned int itemType;
	public:
		baseItem();
		baseItem(unsigned int maxStack);
		~baseItem();

		void setPos(glm::vec3 pos);

		std::pair<unsigned int, unsigned int> getStackInfo();
		void setStackInfo(unsigned int ms);
		void addItem(int count);

		unsigned int getItemType();
		void setItemType(unsigned int type);

		void Interact();
		void Draw();
}Item;


//Class for holding the main state of the game.
typedef class GameState{
private:
	unsigned int curState;
	bool projChange;
	std::vector<bool> Flags;
	std::vector<bool> moving;
	float camPos[2];
	std::vector<Player> Players;
	sf::Time timeElapsed;
	float deltaTime;
	baseItem * heldItem = nullptr;
	Window * selectedWindow = nullptr;
	Pane * selectedPane = nullptr;
	unsigned int prevSlotColor;
public:
	GameState();
	GameState(unsigned int cs);

	void Init();
	
	void setState(unsigned int cs);
	unsigned int getState();

	void setProjChange(bool pc);
	bool getProjChange();

	void setCamPos(float x, float y);
	float * getCamPos();

	void setFlags(int index, bool tag);
	bool getFlags(int index);
	std::vector<bool> getFlags();

	void setMoving(int index, bool tag);
	bool getMoving(int index);
	std::vector<bool> getMoving();

	void addPlayer(Player p);
	void setPlayer(int index, Player& p);
	Player * getPlayer(int index);
	std::vector<Player> getPlayerList();

	sf::Time getTime();
	void setTime(sf::Time t);

	float getdTime();
	void setdTime(float t);

	baseItem * getHItem();
	void setHItem(baseItem * i);

	Window * getSWindow();
	void setSWindow(Window * w);

	Pane * getSPane();
	void setSPane(Pane * p);

	unsigned int getPrevSlotColor();
	void setPrevSlotColor(unsigned int c);
}GState;


//Main class for the GameOptions.
typedef class GameOptions{
private:
	float projVar[4];
	glm::vec3 camRot;
	std::vector<std::string> modelPaths;
	std::vector<std::string> texturePaths;
public:
	GameOptions();
	GameOptions(const char * optionsFileName);
	~GameOptions();

	void Init();

	void setProjVars(float * vars);
	void setProjVars(int index, float vars);
	float * getProjVars();

	void setCamRot(glm::vec3 rot);
	glm::vec3 getCamRot();

	void addMPaths(std::string path);
	void clearMPaths();
	std::vector<std::string> getMPaths();

	void addTPaths(std::string path);
	void clearTPaths();
	std::vector<std::string> getTPaths();
}OPT;

//The main rendering storage, most of the information that is handled while rendering is housed in game renderer.
typedef class GameRenderer{
private:
	GLuint shaderProgram;
	glm::vec3 camVec[3];
	glm::mat4 ViewMatrix;
	glm::mat4 ProjectionMatrix;
	std::vector<GLuint> TextureList;
	std::vector<IOBJ*> InstancedObjectsList;
	std::vector<Window> Windows;
	std::vector<Window> StaticWindows;
public:
	GameRenderer();

	~GameRenderer();
	
	void Init();

	void setGuiUniforms();
	void setOBJUniforms();

	void setShaderProgram(const char * vertexPath, const char * fragmentPath);
	GLuint getShaderProgram();

	void setCamVec(int index, glm::vec3 v);
	glm::vec3 getCamVec(int index);
	glm::vec3 * getCamVec();

	void setViewMatrix(glm::mat4 viewMatrix);
	glm::mat4 getViewMatrix();

	void setProjMatrix(glm::mat4 projMatrix);
	glm::mat4 getProjMatrix();

	void setIObject(int index, IOBJ * object);
	void addToIObjectList(IOBJ * object);
	void clearIObjectList();
	std::vector<IOBJ *>& getIObjectList();
	IOBJ * getIObject(int index);
	int getIObjectSize();

	void addToTextureList(const char * textureFile);
	void clearTextureList();
	std::vector<GLuint> getTextureList();
	GLuint getTextureList(int index);

	void addWindow();
	void addWindow(Window w);
	void addWindow(glm::vec2 tsize, glm::vec2 tpos, glm::vec2 pos, glm::vec2 wsize, bool hide, unsigned int wType);
	void setWindow(int index, Window w);
	Window * getWindows(int index);
	std::vector<Window> & getWindows();

	void addSWindow();
	void addSWindow(Window w);
	void addSWindow(glm::vec2 tsize, glm::vec2 tpos, glm::vec2 pos, glm::vec2 wsize, bool hide, unsigned int wType);
	void setSWindow(int index, Window w);
	Window * getSWindows(int index);
	std::vector<Window> & getSWindows();
}GRend;

typedef struct vertexInfo{
	glm::vec4 verts;
	glm::vec4 norms;
	glm::vec2 tex;
	bool operator<(const vertexInfo that) const{
		return memcmp((void*)this, (void*)&that, sizeof(vertexInfo))>0;
	};
}Vinf;

void InitOpenGL();
void tickUpdate();


extern GRend Renderer;
extern OPT Options;
extern GState State;
extern std::vector<Chunk*> World;
extern sf::Clock masterClock;
extern std::pair<glm::uint,glm::uint> PlayerPos;

extern sf::Window mainWindow;
extern std::default_random_engine randomEng;

extern const glm::vec3 BlockColors[16];
extern const glm::vec3 DirectionalVectors[6];
extern std::vector<std::string> BlockNames;
extern std::vector<std::string> TextureNames;


#endif //Base

#include "CryoGeneration.hpp"
#include "CryoRendering.hpp"
#include "CryoUtil.hpp"
#include "CryoPlayer.hpp"
#include "CryoInput.hpp"
#include "CryoWindowing.hpp"
TARGETC = ./src/Sources/Cryonyx.cpp ./src/Sources/CryonyxBase.cpp ./src/Sources/CryonyxRenderer.cpp ./src/Sources/CryonyxUtil.cpp
TARGETEXE = ./Cryonyx.exe
FLAGS = 
INC =  -lglu32 -lopengl32 -lgdi32 -lglew32 -lfreeglut 

all: compile

compile: $(TARGETC)
	g++ $(FLAGS) $(TARGETC) -o $(TARGETEXE) $(INC)

clean:
	rm -rf *.o *.exe


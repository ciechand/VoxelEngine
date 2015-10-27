TARGETC = $(wildcard ./src/Source/*.cpp)
TARGETEXE = ./Cryonyx.exe
FLAGS = -std=c++11
INC =  -lboost_filesystem-mgw48-mt-1_59 -lboost_system-mgw48-mt-1_59 -lglu32 -lopengl32 -lgdi32 -lglew32 -lfreeglut

all: compile

compile: $(TARGETC)
	g++ $(FLAGS) $(TARGETC) -o $(TARGETEXE) $(INC)

clean:
	rm -rf *.o *.exe


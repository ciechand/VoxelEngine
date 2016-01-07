TARGETC = $(wildcard ./src/Source/*.cpp)
TARGETEXE = ./Cryonyx.exe
FLAGS = -std=c++11 -DSFML_STATIC
INC = -lsfml-graphics -lsfml-window -lsfml-system -lwinmm -lboost_filesystem-mt -lboost_system-mt -lglu32 -lopengl32 -lgdi32 -lglew32 -lpng -lz -ljpeg -lfreetype

all: compile

compile: $(TARGETC)
	g++ $(FLAGS) $(TARGETC) -o $(TARGETEXE) $(INC)

clean:
	rm -rf *.o *.exe


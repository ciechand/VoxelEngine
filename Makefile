TARGETC = $(wildcard ./src/Source/*.cpp)
TARGETEXE = ./Cryonyx.exe
CPPFLAGS = -std=c++11 -g -DSFML_STATIC
INC = -lsfml-graphics -lsfml-window -lsfml-system -lwinmm -lboost_filesystem-mt -lboost_system-mt -lglu32 -lopengl32 -lgdi32 -lglew32 -lpng -lz -ljpeg -lfreetype

all: compile

compile: $(TARGETC)
	g++ $(CPPFLAGS) $(TARGETC) -o $(TARGETEXE) $(INC)

clean:
	rm -rf *.o *.exe


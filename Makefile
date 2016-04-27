TARGETC = $(wildcard ./src/Source/*.cpp)
TARGETO = $(TARGETC:./src/Source/%.cpp=./src/Objects/%.o)
TARGETD = $(TARGETO:./src/Objects/%.o=./src/Dependencies/%.d)
HPPS = $(wildcard ./src/Headers/*.hpp)
TARGETEXE = ./Cryonyx.exe
CPPFLAGS = -std=c++11 -g -DSFML_STATIC
INC = -lsfml-graphics -lsfml-window -lsfml-system -lwinmm -lboost_filesystem-mt -lboost_system-mt -lglu32 -lopengl32 -lgdi32 -lglew32 -lpng -lz -ljpeg -lfreetype

all: compile

compile: $(TARGETO)
	$(CXX) $(CPPFLAGS) -o $(TARGETEXE) $^ $(INC)

-include $(TARGETD)

./src/Objects/%.o:./src/Source/%.cpp $(HPPS)
	$(CXX) $(CPPFLAGS) -o $@ -c $<

./src/Dependencies/%.d:./src/Objects/%.o $(HPPS)
	$(CPP) $(CFLAGS) $< -MM -MT $(@:./src/Dependencies/%.d=./src/Objects/%.o) >$@

clean:
	rm -rf *.o *.exe
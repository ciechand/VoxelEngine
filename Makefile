TARGETC = $(wildcard ./src/Source/*.cpp)
TARGETC += $(wildcard ./Includes/*.cpp)
TARGETO = $(addprefix ./src/Objects/,$(notdir $(TARGETC:%.cpp=%.o)))
HPPS = $(wildcard ./src/Headers/*.hpp)
TARGETEXE = ./Cryonyx.exe
CPPFLAGS = -std=c++17 -g -static-libstdc++ -static-libgcc -DSFML_STATIC
INC = -lsfml-graphics -lsfml-window -lsfml-system -lwinmm -lboost_filesystem-mt -lboost_system-mt -lglu32 -lopengl32 -lgdi32 -lglew32 -lpng -lz -ljpeg -lfreetype -lpthread

PHONY := clean

all: compile

compile: $(TARGETO)
	$(CXX) $(CPPFLAGS) -o $(TARGETEXE) $^ $(INC)
	@echo "Compilation Complete"

./src/Objects/%.o:./src/Source/%.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

./src/Objects/%.o:./Includes/%.cpp
	$(CXX) $(CPPFLAGS) -o $@ -c $<

clean:
	rm $(TARGETO) $(TARGETD)
# compiler and flags
COMPILER = g++
FLAGS = -std=c++17 -g
DEFINES = -DGLEW_STATIC
INCLUDE_PATH = -I./dependencies/glew/include -I./dependencies/stb_image -I./dependencies/imgui -I./src
LIB_PATH = -L./dependencies/glew/lib/x64
LIBS = -lglew32s -lglfw3 -lopengl32

# source, object, and output binary files
SRC = $(wildcard src/*.cpp) $(wildcard src/tests/*.cpp)
OBJ = $(patsubst src/%.cpp, obj/%.o, $(SRC)) # intermediate object files for my own src
OUTPUT = application

DEP_OBJ = $(wildcard dependencies/obj/*.o)
DEP_TIMESTAMP = dependencies/obj/.deps_timestamp

# default rule that builds and runs
all: $(OUTPUT)
	./$(OUTPUT)

# build the executable
$(OUTPUT): $(DEP_TIMESTAMP) $(OBJ) $(DEP_OBJ)
	$(COMPILER) -o $@ $(OBJ) $(DEP_OBJ) $(LIB_PATH) $(LIBS) $(DEFINES)

# compile source files
obj/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(COMPILER) $(FLAGS) $(INCLUDE_PATH) $(DEFINES) -c $< -o $@

clean:
	rm -rf obj $(OUTPUT)

cleanall:
	rm -rf obj dependencies/obj $(OUTPUT) && clear

# rule to compile dependency source code if the timestamp file doesnt exist
$(DEP_TIMESTAMP):
	bash ./dependencies.sh
	@touch $(DEP_TIMESTAMP)

.PHONY: all clean deps
# builds the program and runs it; assumes that dependencies are present in dependencies folder with their header files, library files, or compiled object files

INCLUDE_PATH="-I./dependencies/glew/include -I./dependencies/stb_image -I./dependencies/imgui -I./src"
LIB_PATH="-L./dependencies/glew/lib/x64"
LIBS="-lglew32s -lglfw3 -lopengl32"
DEFINES="-DGLEW_STATIC"
FLAGS="-std=c++17 -g"

SRC="src/*.cpp dependencies/obj/*.o"
OUTPUT="application"

g++ $SRC -o $OUTPUT $INCLUDE_PATH $FLAGS $LIB_PATH $LIBS $DEFINES \
    && ./$OUTPUT
# for compiling src_jamie code
# builds the program and runs it; assumes that dependencies are present in dependencies folder with their header files, library files, or compiled object files
# on linux, install glfw and glew using package manager globally, then run this script
# on windows, install glfw globally somehow, but other libraries are present in `dependencies/`
# script only deals with windows and linux, not mac
# global installations of libraries is required as well as running of `dependencies.sh` before running this script!

DEFINES="-DGLEW_STATIC"
FLAGS="-std=c++17 -g"
SRC="src_jamie/*.cpp dependencies/obj/*.o"
OUTPUT="application"

INCLUDE_PATH="-I./dependencies/glew/include -I./dependencies -I./dependencies/stb_image -I./dependencies/imgui -I./src_jamie"
LIB_PATH="-L./dependencies/glew/lib/x64"

OS="$(uname -s)"

if [ "$OS" = "Linux" ]; then
    LIBS="-lGLEW -lglfw -lGL"
else
    LIBS="-lglew32s -lglfw3 -lopengl32"
fi

g++ $SRC -o $OUTPUT $INCLUDE_PATH $FLAGS $LIB_PATH $LIBS $DEFINES \
    && ./$OUTPUT
# for compiling src code, deals with windows and linux only
# builds the program and runs it
# assumes that dependencies are present in dependencies folder with their header files, library files, or compiled object files
# run dependencies.sh before this
# on linux, install glfw using the package manager

cc="g++"
flags="-std=c++11 -g"
src="src/main.cpp dependencies/obj/*.o"
output="application"
includes="-Idependencies -Idependencies/stb_image -Idependencies/imgui -Isrc"

mkdir -p build

if [[ "$(uname)" == "Linux" ]]; then
    echo "Building for Linux..."
    libs="-lglfw -lGL -lX11"
    
elif [[ "$(uname)" == "MINGW"* ]] || [[ "$(uname)" == "MSYS"* ]]; then
    echo "Building for Windows..."
    lib_path="-Ldependencies/GLFW"
    libs="-lglfw3 -lopengl32 -lgdi32"
    
else
    echo "Unsupported platform: $(uname)"
    exit 1
fi

$cc $src -o build/$output $includes $flags $lib_path $libs
echo "Built"
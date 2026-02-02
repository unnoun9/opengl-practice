# for compiling src_cherno code
# works only for windows by default
# assumes that dependencies are present in dependencies folder with their header files, library files, or compiled object files

cc="g++"
flags="-std=c++11 -g"
src="src_cherno/*.cpp src_cherno/tests/*.cpp dependencies/obj/*.o"
output="application_cherno"

includes="-Idependencies -Idependencies/stb_image -Idependencies/imgui -Isrc_cherno"

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
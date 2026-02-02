# used to only compile imgui and stb_image as object files in the "dependencies/obj" directory
# manadatory to be done at least once before building via build.sh

echo "Compiling dependencies source code into dependencies/obj..."
mkdir -p dependencies/obj
cc="g++"
flags="-std=c++11 -O3 -c"

# Compile glad
$cc dependencies/glad.c -o dependencies/obj/glad.o $flags -Idependencies
echo "✓ glad.o"

# Compile stb_image
$cc dependencies/stb_image/stb_image.cpp -o dependencies/obj/stb_image.o $flags -Idependencies/stb_image
echo "✓ stb_image.o"

# Compile imgui files
includes="-Idependencies/imgui -Idependencies"
for file in dependencies/imgui/*.cpp; do
    basename=$(basename "$file" .cpp)
    $cc "$file" -o "dependencies/obj/${basename}.o" $includes $flags
    echo "✓ ${basename}.o"
done
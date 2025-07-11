# used to only compile imgui and stb_image as object files in the "dependencies/obj" directory
# manadatory to be done at least once before building via build.sh or make

# 1. compile imgui files
mkdir -p "dependencies/obj"
IMGUI_LOCATION="dependencies/imgui"
FLAGS="-std=c++17 -O3 -c"
INCLUDE_PATH="-Idependencies/imgui"

for file in $IMGUI_LOCATION/*.cpp; do
    g++ "$file" -o "dependencies/obj/$(basename "$file" .cpp).o" $INCLUDE_PATH $FLAGS 
done
echo "ImGui should be compiled in \"dependencies/obj\""

# 2. compile stb_image files
INCLUDE_PATH="-I./dependencies/stb_image"
SRC="dependencies/stb_image/stb_image.cpp"
OUTPUT="dependencies/obj/stb_image.o"

g++ $SRC -o $OUTPUT $INCLUDE_PATH $FLAGS
echo "stb_image should be compiled in \"dependencies/obj\""
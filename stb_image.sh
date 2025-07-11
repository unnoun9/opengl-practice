# used to only compile stb_image; manadatory to be done before building via build.sh

FLAGS="-std=c++17 -O3 -c"
INCLUDE_PATH="-I./dependencies/stb_image"
SRC="dependencies/stb_image/stb_image.cpp"
OUTPUT="dependencies/stb_image/stb_image.o"

g++ $SRC -o $OUTPUT $INCLUDE_PATH $FLAGS
mkdir -p bin

g++ -std=c++17 -I$(pwd) -o bin/Application src/*.cpp src/glad.c -lglfw3

./bin/Application
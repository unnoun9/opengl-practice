g++ -I./glew/include -L./glew/lib/x64 -DGLEW_STATIC -o application src/*.cpp -lglew32s -lglfw3 -lopengl32

./application && rm application.exe

FLAGS = "-g3" "-std=c++20" "-I" "src/includes"
CPP_FILES = "src/Main.cpp" "src/includes/Utils/*.cpp" "src/includes/Tokenizer/*.cpp" "src/includes/Parser/*.cpp" "src/includes/Generator/*.cpp"

default: build

build:
	g++ $(FLAGS) $(CPP_FILES) -o out/Bi.exe

run: build
	cd out && Bi.exe ../test/prova.bi && cd ..

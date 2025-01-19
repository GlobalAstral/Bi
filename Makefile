includes = "-I" "src"
libs = "src/*.cpp"


build:
	g++ "-g3" "-std=c++11" ${includes} ${libs} "Main.cpp" "-o" "out/Main.exe"
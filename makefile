s = ./source/
files = $(s)main/main.cpp 
flags = -Wall -std=c++23
cc = g++
binary = low
testFile = test.low

main: $(files)
	$(cc) $(flags) $(files) -o $(binary)

clean: $(binary)
	rm $(binary)

run: $(binary)
	./(binary)

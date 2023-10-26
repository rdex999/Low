s = ./source/

files = $(s)main/main.cpp $(s)lexer/lexer.cpp $(s)parseArgs/parseArgs.cpp \
	$(s)parse/parse.cpp

flags = -Wall -std=c++23
cc = g++
binary = low
testFile = test.low

$(binary): $(files)
	$(cc) $(flags) $(files) -o $(binary)

clean: $(binary)
	rm $(binary)

run: $(binary)
	./$(binary) $(testFile)

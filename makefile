s = ./source/
flags = -Wall -std=c17
files = $(s)main/main.c $(s)fileFuncs/fileFuncs.c $(s)lexer/lexer.c
binaryName = low

main: $(files)
	gcc $(flags) $(files) -o $(binaryName)

run: $(binaryName)
	./$(binaryName)

clean: $(binaryName)
	rm $(binaryName)
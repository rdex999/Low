# Low
The "Low" programming language. \
I will probably change the name in the future.
Please note that this project was made just for fun, nothing serious.

# Compiling
compile a .low file with the following command:
```
low main.low -o main
```

# Contributing
I have stopped this project, please do not contribute. \
I might create a new compiler in the future, and this time start it right so i wont mess it up.

# Syntax
Every statement MUST end with a semicolon ( ; ) \
If you miss a semicolon the compiler might throw an error, but usualy it doesnt and will just generate bad code.

## Variables
There are currently int, char, and float. \
You can declare variables with, or without a value. \
Note that when declaring a variable without a value it will have the old value that was used by other programs in its memory location. \
Usage: `type varName;` \
Example: `int x;`

#### int
Integers are 4 bytes, and are used to store numbers. \
Examples: \
`int x;` \
`int x = 10;` 

#### char
char, in short for character is one byte and can store a single character. \
Examples: \
`char ch;` \
`char ch = 'a';` 

#### float
Float (floating point) is a data type for fractional numbers. \
Examples: \
`float x;` \
`float x = 12.1234;`

### Arrays
You can use arrays for each type, examples: 
```
int[10] myArr;
myArr[0] = 35211;
myArr[1] = 683242;
```

## Printing
There are function from the low standard library to print to the console, explained below.
### stdout
For printing values with text, use the print function. Example: \
`print("value is %i \n");` 
#### Formating options
%i -> signed 32 bit int \
%c -> character (char) \
%s -> string (zero terminated) \
%f -> signed 32 bit float

For printing values, you can use the following functions. \
For printing a zero terminated string: \
`printStr("Hello, world!\n");` \
You can also print a single character with the printChar function, example: \
`printChar('a');` \
And for printing integers use the function printInt, example: \
`printInt(2345);` \
For printing a floating point (32 bit): \
`printFloat32(13.1242, 5); // NOTE: second argument is the amount of digits to print after the point.`

### stderr
Currently not supported.

## If statements
if statements are supported, with all basic boolean operators (==, !=, <, >, <=, >=) \
Btw, you can use the "is" keyword instead of the == operator. \
Examples:
```
if 5 is 5{
    printStr("5 is 5\n");
}else if 2 is 4{
    printStr("2 is 4\n");
}else{
    printStr("None of the above\n");
}
```

## Loops
### while
Syntax: `while <condition> { code; }` \
Example:
```
int i = 0;
while i < 30{
    printInt(i);
    printChar('\n');
    i++;
}
```

### for
Syntax: `for int i=0; i<10; i++ { code; }` \
Example:
```
for int i=0; i<10; i++ {
    printInt(i);
    printChar('\n');
}
```

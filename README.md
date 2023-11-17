# Low
The "Low" programming language. \
I will probably change the name in the future.

# Compiling
compile a .low file with the following command:
```
low main.low -o main
```

# Syntax
Every statement MUST end with a semicolon ( ; ) \
Usualy if you miss a semicolon the compiler will throw an error, but it might not, and can just generate bad code. (will be fixed in the future)

# Contributing
As this languege is currently in development, i will not accept pull requests

## Variables
there are currently 2 types of variables, int, and char \
You can declare variables with, or without a value. \
Nnote that when declaring a variable without a value it will have the old value that wa used by other programs in that location. \
Usage: `type varName;` \
Example: `int x;`

#### int
Integers are 4 bytes, and are used to store numbers. \
Examples: \
`int x;` \
`int x = 10;` \

#### char
char, in short for character is one byte and can store a single character. \
Examples: \
`char ch;` \
`char ch = 'a';` \

### Arrays
You can use arrays for each type, examples: \
```
int[10] myArr;
myArr[0] = 35211;
myArr[1] = 683242;
```

## Printing
There are function from the low standard library to print to the console, explained below. \
### stdout
You can print a string to stdout using the printStr function. \
Usage: \
`printStr("Hello, world!\n");` \
You can also print a single character with the printChar function, example: \
`printChar('a');` \
And for printing numbers use the function printNum, example: \
`printNum(2345);`

### stderr
Currently not supported.

## If statements
is statements are supported, with all basic boolean operators (==, !=, <, >, <=, >=) \
Btw, you can use the "is" keyword instead of the == operator. \
Examples: \
```
if 5 is 5{
    printStr("true\n");
}
```
```
if 5 >= 2{
    printStr("5 is greater that 2!\n");
}
```
### else
You can use else with an if statement, examples: \
```
if 4 is 5{
    printStr("4 is 5\n");
}else{
    printStr("4 is not five\n");
}
```

### else if
You can use else if with an if statement, examples: \
```
if 4 is 5{
    printStr("4 is 5\n");
}else if 10 is 10{
    printStr("10 is 10\n");
}else{
    printStr("None of the above\n");
}
```

## Loops
currently only while loops are supported, there will be more in the future. \
### while
syntax: `while <condition> { code; }` \
Example:
```
int i = 0;
while i < 30{
    printNum(i);
    printChar('\n');
    i++;
}
```

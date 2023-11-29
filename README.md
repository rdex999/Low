# Low
The "Low" programming language. \
I will probably change the name in the future.

# Compiling
compile a .low file with the following command:
```
low main.low -o main
```

# Contributing
As this languege is currently in development, I will not accept pull requests.

# Syntax
Every statement MUST end with a semicolon ( ; ) \
Usualy if you miss a semicolon the compiler will throw an error, but it might not, and can just generate bad code. (will be fixed in the future)

## Variables
there are currently 2 types of variables, int, and char \
You can declare variables with, or without a value. \
Note that when declaring a variable without a value it will have the old value that was used by other programs in that location. \
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
You can print a string to stdout using the printStr function. \
Usage: \
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
Currently only while loops are supported, there will be more in the future.
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
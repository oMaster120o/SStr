# SStr  
SafeStrings library for the C programming language.
  
The SafeStrings library was created for the purpose of not needing to always write safe strings from scratch in every C project.
  
For testing is recommended to use the SStrWithDebugInfo version and then using the version without Debug values, the Debug can be enabled by uncommenting the `//#define SSTR_DEBUG` line on the SStr.c file.
  
# Table of contents.  
[Instalation](#Instalation)  
[Usage](#Usage)  
- [SStr structure](##SStr-structure)
- [Creating a new SStr](##Creating-a-new-SStr)
- [Writting to the data](##Writting-to-the-SStr)
  - [overwrite](###overwrite)
  - [Appending data](###Appending-data)
    - [Appending a char](####Appending-a-char)
    - [Appending a string](####Appending-a-string)
- [Compiling](##Compiling)
  
  
  
  
  
  
# Instalation  
Clone the repository into your project and start using.
  
`git clone https://github.com/oMaster120o/SStr`  
  
The repository comes with both the SStr and SStrWithDebugInfo, each of these have a directory test which can be deleted.
  
# Usage  
  
  
## SStr-structure  
  
The SStr type has the following values:
  
```c
  char*           data;     // a string held by each SStr independently.
  volatile size_t length;   // the length of data minus the null terminator.
  volatile size_t capacity; // the maximum length that data can be before allocating more memory.
```
  
Each of these values cannot be accessed directly to read, write them you need to use a SStrFn **(Safe String Function)**.  
To read any of these valuess you can use the following:
  
```c
SStrFn.get.data(my_sstr);
SStrFn.get.length(my_sstr);
SStrFn.get.capacity(my_sstr);
```
Each returns the a specific value values.
  
  
## Creating-a-new-SStr
Suppose the project structure looks like the following:
  
```
./main.c
./SStr/SStr.h
      /SStr.c
```
  
To create a new SStr and use SStr Functions first import SStr.h header into your your project:
  
`#include "./SStr/SStr.h"`
  
> [!CAUTION]  
> do not include SStr.c directly into the project, but only SStr.h.  
> including SStr.c will likely not compile, to call it's functions use SStrFn.
  
  
  
Create a new SStr variable:
  
```c
#include "./SStr/SStr.h"

int main()
{
      // SStrFn.New() is the first function to be used as it allocates memory and returns a new SStr pointer.
      SStr* my_sstr = SStrFn.New();
      if (SStrFn.Error()) return -1; // if SStrFn.New() fails an error is generated and SStrFn.Error() returns 1.

      return 0;
}
```

## Writting-to-the-SStr
  
  
### overwrite
  
To overwrite the data from a SStr the SStrFn.copy() function is used.

```c
#include "./SStr/SStr.h"

int main()
{
      SStr* my_sstr = SStrFn.New();
      if (SStrFn.Error()) return -1; // if SStrFn.New() fails an error is generated and SStrFn.Error() returns 1.

      SStr* my_second_sstr  = SStrFn.New();
      if (SStrFn.Error()) return -1;



      // with a literal string.
      SStrFn.copy(my_sstr, "Example string.");
      if (SStrFn.Error()) return -1; // if SStrFn.copy() fails an error is generated and SStrFn.Error() returns 1.


      // from a variable.
      char* example_string = "Second string";
      SStrFn.copy(my_second_sstr,  example_string);
      if (SStrFn.Error()) return -1;


      // from another SStr.
      SStrFn.copy(my_second_sstr, SStrFn.get.data(my_sstr));
      if (SStrFn.Error()) return -1;

      return 0;
}
```

### Appending-data
  
  
#### Appending-a-char
  
```c
  SStr* my_sstr = SStrFn.New();
  if (SStrFn.Error()) return -1;



  // appending a char directly.
  SStrFn.append.chr(my_sstr, 'A');
  if (SStrFn.Error()) return -1;


  // appending a char from a variable.
  char my_char = 'A';
  // or
  int  my_char = 'B';

  SStrFn.append.chr(my_sstr, my_char);
  if (SStrFn.Error()) return -1;
```
  
  
#### Appending-a-string
  
```c
  SStr* my_sstr = SStrFn.New();
  if (SStrFn.Error()) return -1; // if SStrFn.New() fails an error is generated and SStrFn.Error() returns 1.



  // appending a literal string.
  SStrFn.append.str(my_sstr, "example literal string.");
  if (SStrFn.Error()) return -1; // if SStrFn.append.str() fails an error is generated and SStrFn.Error() returns 1.


  // appending a string from a variable.
  char* my_string = "Example string.";

  SStrFn.append.str(my_sstr, my_string);
  if (SStrFn.Error()) return -1;


  // appending from another SStr.
  SStrFn.append.str(my_sstr, SStrFn.get.data(SStr_variable));
  if (SStrFn.Error()) return -1;
```
  
  
  
## Compiling
  
to compile a project using the library, first it needs to be compiled as an object file, exactly as from /SStr/test/Makefile does. Must also use -Wnonnull compiler flag:
  
```
CC = gcc
OUTPUT = Test
CFLAGS = -Wnonnull

all: SStr.o tests.o
	@${CC} tests.o SStr.o -o ${OUTPUT} ${CFLAGS}
	@rm ./*.o

tests.o:
	@${CC} -c tests.c

SStr.o:
	@${CC} -c ../SStr.c
```

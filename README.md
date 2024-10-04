# SStr  
SafeStrings library for the C programming language.
  
# Info  
The SafeStrings library was created for the purpose of not needing to always write safe strings from scratch in every C project.
  
For usage is recommended to use the SStrWithDebugInfo version and then using the version without Debug information, the Debug can be enabled by uncommenting the `//#define SSTR_DEBUG` line on the SStr.c file.
  
# Table of contents.  
[Instalation](#Instalation)  
[Usage](#Usage)  
 - [SStr structure](SStr structure)
 - [Creating a new SStr](Creating a new SStr)
  
  
# Instalation  
Clone the repository into your project and start using.
  
`git clone https://github.com/oMaster120o/SStr`  
  The repository comes with both the SStr and SStrWithDebugInfo, each of these have a directory test which can be deleted.
  
# Usage  
  
## SStr structure  
The SStr type has the following information:
  
```c
  char*           data;     // a string held by each SStr independently.
  volatile size_t length;   // the length of data minus the null terminator.
  volatile size_t capacity; // the maximum length that data can be before allocating more memory.
```
  
Each of these values cannot be accessed directly, to modify them you need to use a SStrFn **(Safe String Function)**.
  
## Creating a new SStr
Suppose the project structure looks like the following:
  
```
./main.c
./SStr/SStr.h
      /SStr.c
```
  
To create a new SStr and using SStr Functions first import SStr.h file into your file:
  
`c#include "./SStr/SStr.h"`
  
> [!CAUTION]  
> do not include SStr.c directly into the project, but only SStr.h.  
> including SStr.c will likely not compile, to call it's functions use SStrFn.
  
  
Create a new SStr variable:
  
```c
#include "./SStr/SStr.h"

int main()
{
  SStr* my_sstr = SStrFn.New(); // SStrFn.New() is the first function to be used as it allocates and returns a new SStr pointer.

  return 0;
}
```

## Writting to the data
  
### overwrite
  
To overwrite the data from a SStr the SStrFn.copy() function is used.

```c
#include "./SStr/SStr.h"

int main()
{
  SStr* my_sstr = SStrFn.New();
  SStr* second  = SStrFn.New();

  // with a literal string.
  SStrFn.copy(my_sstr, "Example string.");


  // from a variable.
  char* example_string = "Source string";
  SStrFn.copy(second,  example_string);


  // from another SStr.
  SStrFn.copy(second, SStrFn.get.data(my_sstr));

  return 0;
}
```

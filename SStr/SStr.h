#ifndef SSTR_H
#define SSTR_H
#include <stdlib.h>
#include <stdint.h>

extern char* SSTR_ERROR;

typedef struct SStr SStr;

typedef struct
{
  // Creates a new SStr.
  SStr*    (*New)        (void);

  // If any error occured, prints the Error message and return 1.
  int8_t   (*Error)      (void);

  // Destroy a SStr;
  //   SStr* sstr_string = SStrFn.New();
  //   SStrFn.Destoy(sstr_string);
  void     (*Destroy)    (SStr* sstr_string);

  // Clear the data from SStr:
  //   SStr* sstr_string;
  //   SStrFn.clear_data(sstr_string);
  //   SStrFn.clear_data(sstr_string);
  void     (*clear_data) (SStr* sstr_string);

  // Override the contents of the SStr by string.
  //   SStr* sstr_string = SStrFn.New();
  //   SStrFn.copy(sstr_string, "my string");
  void     (*copy)       (SStr* sstr_string, const char* string);


  struct
  {

    // Returns the data from SStr:
    //   SStr* sstr_string;
    //   SStrFn.get_data(sstr_string);
    char*  (*data)      (SStr* sstr_string);

    // Returns the SStr length.
    //   SStr* sstr_string;
    //   SStrFn.get_length(sstr_string);
    size_t (*length)    (SStr* sstr_string);

    // Returns the SStr capacity.
    //   SStr* sstr_string;
    //   SStrFn.get_capacity(sstr_string);
    size_t (*capacity)  (SStr* sstr_string);

  } get;

  struct
  {

    // Append a single character to the SStr.
    //   SStrFn.append.chr(sstr_string, 'A')
    void (*chr) (SStr* sstr_string, int character);

    // Append a string to the SStr.
    //   SStrFn.append.str(sstr_string, "my string")
    void (*str) (SStr* sstr_string, char* string);

  } append;

} SStrFn_interface;


extern SStrFn_interface SStrFn;
#endif

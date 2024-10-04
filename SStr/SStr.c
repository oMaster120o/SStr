#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_CAPACITY 32
#define SSTR_ERROR_COLOR  "\e[38;2;255;0;0m"


typedef struct
{
  char*            data;     // The actual string.
  volatile size_t  length;   // The string length.
  volatile size_t  capacity; // The string length + extra characters that data can be resized to without realloc.

} SStr;


typedef struct
{
  SStr*   (*New)        (void);
  int8_t  (*Error)      (void);
  void    (*Destroy)    (SStr* sstr_string)                     __attribute__((__nonnull__(1)));

  void    (*clear_data) (SStr* sstr_string)                     __attribute__((__nonnull__(1)));
  void    (*copy)       (SStr* sstr_string, const char* string) __attribute__((__nonnull__(1, 2)));

  struct
  {
    char*  (*data)     (SStr* sstr_string)                      __attribute__((__nonnull__(1)));
    size_t (*length)   (SStr* sstr_string)                      __attribute__((__nonnull__(1)));
    size_t (*capacity) (SStr* sstr_string)                      __attribute__((__nonnull__(1)));
  } get;

  struct
  {
    void (*chr) (SStr* sstr_string, int character)              __attribute__((__nonnull__(1)));
    void (*str) (SStr* sstr_string, char* string)               __attribute__((__nonnull__(1, 2)));
  } append;

} SStrFn_interface;



// Function prototypes.
static SStr*         SStrFn_New          (void);
static int8_t        SStrFn_Error        (void);
static void          SStrFn_Destroy      (SStr* sstr_string)                     __attribute__((__nonnull__(1)));
static void          SStrFn_clear_data   (SStr* sstr_string)                     __attribute__((__nonnull__(1)));
static void          SStrFn_copy         (SStr* sstr_string, const char* string) __attribute__((__nonnull__(1, 2)));
static char*         SStrFn_get_data     (SStr* sstr_string)                     __attribute__((__nonnull__(1)));
static inline size_t SStrFn_get_length   (SStr* sstr_string)                     __attribute__((__nonnull__(1)));
static inline size_t SStrFn_get_capacity (SStr* sstr_string)                     __attribute__((__nonnull__(1)));
static void          SStrFn_append_chr   (SStr* sstr_string, int character)      __attribute__((__nonnull__(1)));
static void          SStrFn_append_str   (SStr* sstr_string, char* string)       __attribute__((__nonnull__(1, 2)));


SStrFn_interface SStrFn = {

  .New          = SStrFn_New,
  .Error        = SStrFn_Error,
  .Destroy      = SStrFn_Destroy,

  .clear_data   = SStrFn_clear_data,
  .copy         = SStrFn_copy,

  .get.data     = SStrFn_get_data,
  .get.length   = SStrFn_get_length,
  .get.capacity = SStrFn_get_capacity,

  .append.chr   = SStrFn_append_chr,
  .append.str   = SStrFn_append_str,

};





static char SSTR_ERROR[250];
static void SET_ERROR(char* error_message)
{
    strncpy(SSTR_ERROR, error_message, sizeof(SSTR_ERROR) - 1);
    SSTR_ERROR[sizeof(SSTR_ERROR) - 1] = '\0';
    return;
}


static int8_t SStrFn_Error(void)
{
    if (SSTR_ERROR[0] != '\0')
    {
      printf(SSTR_ERROR_COLOR "\e[1m" "\nSSTR_ERROR:%s\n" "\e[0m", SSTR_ERROR);
      return 1;
    }
    return 0;
}


static __attribute__((malloc)) SStr* SStrFn_New(void)
{
    SStr* address = (SStr*)malloc(sizeof(SStr));
    if (address == NULL)
    {
        free(address);

        SET_ERROR("Failed to allocate memory SStrFn_New()");
        return address;
    }
    
    address->capacity = DEFAULT_CAPACITY;
    address->length   = 0;
    address->data     = (char*)malloc(address->capacity * sizeof(char));
    address->data[0]  = '\0';
    return address;
};


static char* SStrFn_get_data(SStr* sstr_string)
{
    return sstr_string->data;
}


static void SStrFn_clear_data(SStr* sstr_string)
{
    SStr* REF = sstr_string;

    // replace all the data with '\0'.
    for (register size_t indx = REF->length; indx > 0; indx--)
    {
      REF->data[indx] = '\0';
    }

    REF->capacity = DEFAULT_CAPACITY;
    REF->length   = 0;
    

    char* temp_address = (char*)malloc(REF->capacity * sizeof(char));

    if (!temp_address)
    {
        SET_ERROR("Failed to allocate memory SStrFN_clear_data():");
        return;
    }

    REF->data = temp_address;

    return;
}


static inline size_t SStrFn_get_length(SStr* sstr_string)
{
    return sstr_string->length;
}


static inline size_t SStrFn_get_capacity(SStr* sstr_string)
{
  return sstr_string->capacity;
}


static void SStrFn_copy(SStr* sstr_string, const char* string)
{
    SStr*  REF = sstr_string;

    size_t _string_len = strlen(string); // string minus null terminator

    // if REF capacity is less than length of string + null terminator.
    // reallocate more memory for REF->data making it = (_string_len + null terminator + DEFAULT_CAPACITY) * sizeof(char).
    if ( REF->capacity <= (_string_len + 1) )
    {
        // reallocate memory to a temporary address.                +\0 
        char* temp_address = (char*)realloc(REF->data, (_string_len + 1 + DEFAULT_CAPACITY) * sizeof(char));

        // if realloc fails.
        if (!temp_address)
        {
            REF->capacity = DEFAULT_CAPACITY;
            REF->length   = 0;

            SET_ERROR("Failed to realloc memory SStrFn_copy()");
            return;
        }

        // if it suceeds we copy the temporary address to the REF->data.
        REF->data      =  temp_address;
        REF->length    =  _string_len;
        REF->capacity  =  _string_len + DEFAULT_CAPACITY;
    }


    // copy the contents of the string to the REF->data.
    for (register size_t i = 0; i < _string_len; i++)
    {
        REF->data[i] = string[i];
    }
    REF->data[_string_len] = '\0';

    REF->length = _string_len;

    return;
}



static void SStrFn_append_chr(SStr* sstr_string, int character)
{
    if (character == '\0')
    {
      return;
    }

    SStr* REF = sstr_string;

    if (REF->length == REF->capacity)
    {
      REF->capacity += DEFAULT_CAPACITY;
    }


    // realloc to a temporary address.
    char* temp_address = (char*)realloc(REF->data, REF->capacity * sizeof(char));

    // if realloc fails.
    if (temp_address == NULL)
    {
        REF->capacity = DEFAULT_CAPACITY;
        REF->length   = 0;

        SET_ERROR("Failed to realloc memory SStrFn_append_chr()");
        return;
    }


    // if realloc succeeds, copy the temporary address to REF->data.
    REF->data = temp_address;
    REF->data[REF->length] = character;
    REF->length += 1;

    return;
}


static void SStrFn_Destroy(SStr* sstr_string)
{
    SStr* REF = sstr_string;

    for (register size_t indx = REF->length; indx > 0; indx--)
    {
      REF->data[indx] = '\0';
    }

    REF->length   = 0;
    REF->capacity = 0;

    free(REF->data);
    free(REF);

    REF = NULL;

    return;
}


static void SStrFn_append_str(SStr* sstr_string, char* string)
{
    SStr*  REF        = sstr_string;
    size_t total_size = (REF->length + strlen(string));

    if (total_size >= REF->capacity)
    { //                            double the default capacity here,
      //                            so it doesn't need to realloc memory too often.
      REF->capacity = (total_size + DEFAULT_CAPACITY * 2);

      char* temp_address = (char*)realloc(REF->data, REF->capacity * sizeof(char));
      
      if (!temp_address)
      {
          REF->length   = 0;
          REF->capacity = DEFAULT_CAPACITY;

          SET_ERROR("Failed to realloc memory SStrFn_append_str()");
          return;
      }

      REF->data = temp_address;
    }

    for (register size_t indx = 0; indx < strlen(string); indx++)
    {
      REF->data[REF->length + indx] = string[indx];
    }

    REF->length = (REF->length + strlen(string));

    return;
}

//#define SSTR_DEBUG

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define DEFAULT_CAPACITY 32
#define SSTR_ERROR_COLOR  "\e[38;2;255;0;0m"

#ifdef  SSTR_DEBUG

#include <assert.h>
#define DEBUG_COLOR  "\e[38;2;255;50;0m"
#define RESET_COLOR  "\e[0m"

#endif


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
  void    (*Destroy)    (SStr* sstr_string);

  void    (*clear_data) (SStr* sstr_string);
  void    (*copy)       (SStr* sstr_string, const char* string);

  struct
  {
    char*  (*data)     (SStr* sstr_string);
    size_t (*length)   (SStr* sstr_string);
    size_t (*capacity) (SStr* sstr_string);
  } get;

  struct
  {
    void (*chr) (SStr* sstr_string, int character);
    void (*str) (SStr* sstr_string, char* string);
  } append;

} SStrFn_interface;



// Function prototypes.
static SStr*         SStrFn_New          (void);
static int8_t        SStrFn_Error        (void);
static void          SStrFn_Destroy      (SStr* sstr_string);
static char*         SStrFn_get_data     (SStr* sstr_string);
static void          SStrFn_clear_data   (SStr* sstr_string);
static inline size_t SStrFn_get_length   (SStr* sstr_string);
static inline size_t SStrFn_get_capacity (SStr* sstr_string);
static void          SStrFn_copy         (SStr* sstr_string, const char* string);
static void          SStrFn_append_chr   (SStr* sstr_string, int character);
static void          SStrFn_append_str   (SStr* sstr_string, char* string);


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
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_New()\n" RESET_COLOR);
#endif

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
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_get_data()\n" RESET_COLOR);
    assert(sstr_string != NULL);
#endif

    return sstr_string->data;
}


static void SStrFn_clear_data(SStr* sstr_string)
{
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_clear_data():\n" RESET_COLOR);
    assert(sstr_string != NULL);
#endif

    SStr* REF = sstr_string;

#ifdef SSTR_DEBUG
    puts("erasing data:\n");
    printf("old data:%s\n", REF->data);
#endif

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

#ifdef SSTR_DEBUG
    printf("replaced by:%s\n\n\n", REF->data);
#endif

    return;
}


static inline size_t SStrFn_get_length(SStr* sstr_string)
{
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_get_length()\n" RESET_COLOR);
    assert(sstr_string != NULL);
#endif

    return sstr_string->length;
}


static inline size_t SStrFn_get_capacity(SStr* sstr_string)
{
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_get_capacity()\n" RESET_COLOR);
    assert(sstr_string != NULL);
#endif

  return sstr_string->capacity;
}


static void SStrFn_copy(SStr* sstr_string, const char* string)
{
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_copy():\n" RESET_COLOR);
    assert(sstr_string != NULL);
    assert(string != NULL);
#endif

    SStr*  REF = sstr_string;

    size_t _string_len = strlen(string); // string minus null terminator

#ifdef SSTR_DEBUG
    printf("_string_len:%lu\n",      _string_len);
    printf("old length:%lu\n",       REF->length);
    printf("old capacity:%lu\n\n\n", REF->capacity);
#endif

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

#ifdef SSTR_DEBUG
        puts("reallocated:");
        printf("\tnew length:%lu\n",   REF->length);
        printf("\tnew capacity:%lu\n\n\n", REF->capacity);
        //assert( (REF->length == _string_len) );
#endif
    }

#ifdef SSTR_DEBUG
      printf("copying:\nold data:%s\n", REF->data);
#endif


    // copy the contents of the string to the REF->data.
    for (register size_t i = 0; i < _string_len; i++)
    {
        REF->data[i] = string[i];
    }
    REF->data[_string_len] = '\0';

    REF->length = _string_len;

#ifdef SSTR_DEBUG
    printf("\tnew data:%s\n\n\n",  REF->data);
    puts("returning:");
    assert( (REF->length == _string_len) );
    printf("\tlength:%lu\n",       REF->length);
    printf("\tcapacity:%lu\n\n\n", REF->capacity);
#endif

    return;
}



static void SStrFn_append_chr(SStr* sstr_string, int character)
{
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_append_chr():\n" RESET_COLOR);
    assert(sstr_string != NULL);
#endif

    if (character == '\0')
    {
      return;
    }

    SStr* REF = sstr_string;

    if (REF->length == REF->capacity)
    {
      REF->capacity += DEFAULT_CAPACITY;
    }

#ifdef SSTR_DEBUG
      puts("reallocating memory for data:");
#endif

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


#ifdef SSTR_DEBUG
    puts("\treallocated.");
#endif

    // if realloc succeeds, copy the temporary address to REF->data.
    REF->data = temp_address;
    REF->data[REF->length] = character;
    REF->length += 1;

#ifdef SSTR_DEBUG
    puts("returning:");
    printf("\tdata:%s\n",          REF->data);
    printf("\tlength:%lu\n",       REF->length);
    printf("\tcapacity:%lu\n\n\n", REF->capacity);
#endif

    return;
}


static void SStrFn_Destroy(SStr* sstr_string)
{
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_Destroy()\n" RESET_COLOR);
    assert(sstr_string != NULL);
#endif
  
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
#ifdef SSTR_DEBUG
    puts(DEBUG_COLOR "SStrFn_append_str():\n" RESET_COLOR);
    assert(sstr_string != NULL);
    assert(string != NULL);
    printf("old data:%s\n",          sstr_string->data);
    printf("old length:%lu\n",       sstr_string->length);
    printf("old capacity:%lu\n\n",   sstr_string->capacity);
#endif

    SStr*  REF        = sstr_string;
    size_t total_size = (REF->length + strlen(string));

    if (total_size >= REF->capacity)
    { //                            double the default capacity here,
      //                            so it doesn't need to realloc memory too often.
      REF->capacity = (total_size + DEFAULT_CAPACITY * 2);

      char* temp_address = (char*)realloc(REF->data, REF->capacity * sizeof(char));
#ifdef SSTR_DEBUG
      puts("reallocated.\n");
#endif
      
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

#ifdef SSTR_DEBUG
    puts("returning:");
    printf("\tdata:%s\n",      REF->data);
    printf("\tlength:%lu\n",   REF->length);
    printf("\tcapacity:%lu\n\n\n", REF->capacity);
#endif

    return;
}

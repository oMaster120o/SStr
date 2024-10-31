#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#define DEFAULT_CAPACITY 32
#define SSTR_ERROR_COLOR  "\e[38;2;255;0;0m"


typedef struct
{
  char*            data;     // The actual string.
  volatile size_t  length;   // The string length.
  volatile size_t  capacity; // The string length + extra characters that data can be resized to without realloc.

  pthread_mutex_t  mutex;

} SStr;


typedef struct
{
  SStr*   (*New)        (void);
  int8_t  (*Error)      (void);
  void    (*Destroy)    (SStr** sstr_string);

  void    (*clear_data) (SStr* sstr_string);
  void    (*copy)       (SStr* sstr_string, const char* string) __attribute__((__nonnull__(2)));

  struct
  {
    char*  (*data)     (SStr* sstr_string);
    size_t (*length)   (SStr* sstr_string);
    size_t (*capacity) (SStr* sstr_string);
  } get;

  struct
  {
    void (*chr) (SStr* sstr_string, int character);
    void (*str) (SStr* sstr_string, char* string)               __attribute__((__nonnull__(2)));
  } append;

} SStrFn_interface;



// Function prototypes.
static SStr*         SStrFn_New          (void);
static int8_t        SStrFn_Error        (void);
static void          SStrFn_Destroy      (SStr** sstr_string);
static void          SStrFn_clear_data   (SStr* sstr_string);
static void          SStrFn_copy         (SStr* sstr_string, const char* string) __attribute__((__nonnull__(2)));
static char*         SStrFn_get_data     (SStr* sstr_string);
static inline size_t SStrFn_get_length   (SStr* sstr_string);
static inline size_t SStrFn_get_capacity (SStr* sstr_string);
static void          SStrFn_append_chr   (SStr* sstr_string, int character);
static void          SStrFn_append_str   (SStr* sstr_string, char* string)       __attribute__((__nonnull__(2)));


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
    
    pthread_mutex_init(&address->mutex, NULL);

    pthread_mutex_lock(&address->mutex);
    address->capacity = DEFAULT_CAPACITY;
    address->length   = 0;
    address->data     = (char*)malloc(address->capacity * sizeof(char));
    address->data[0]  = '\0';
    pthread_mutex_unlock(&address->mutex);

    return address;
};




static void SStrFn_clear_data(SStr* sstr_string)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_clear_data: Given argument is NULL.");
      return;
    }

    pthread_mutex_lock(&sstr_string->mutex);


    // replace all the data with '\0'.
    for (register size_t indx = sstr_string->length; indx > 0; indx--)
    {
      sstr_string->data[indx] = '\0';
    }

    sstr_string->capacity = DEFAULT_CAPACITY;
    sstr_string->length   = 0;
    

    char* temp_address = (char*)malloc(sstr_string->capacity * sizeof(char));

    if (!temp_address)
    {
        SET_ERROR("Failed to allocate memory SStrFN_clear_data():");
        pthread_mutex_unlock(&sstr_string->mutex);
        return;
    }

    sstr_string->data = temp_address;


    pthread_mutex_unlock(&sstr_string->mutex);
    return;
}


static char* SStrFn_get_data(SStr* sstr_string)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_get_data: Given argument is NULL.");
      return NULL;
    }

    return sstr_string->data;
}


static inline size_t SStrFn_get_length(SStr* sstr_string)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_get_length: Given argument is NULL.");
      return 0;
    }

    return sstr_string->length;
}


static inline size_t SStrFn_get_capacity(SStr* sstr_string)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_get_capacity: Given argument is NULL.");
      return 0;
    }

    return sstr_string->capacity;
}


static void SStrFn_copy(SStr* sstr_string, const char* string)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_clear_data: Given argument is NULL.");
      return;
    }

    pthread_mutex_lock(&sstr_string->mutex);

    size_t _string_len = strlen(string); // string minus null terminator


    // if sstr_string capacity is less than length of string + null terminator.
    // reallocate more memory for sstr_string->data making it = (_string_len + null terminator + DEFAULT_CAPACITY) * sizeof(char).
    if ( sstr_string->capacity <= (_string_len + 1) )
    {
        // reallocate memory to a temporary address.                +\0 
        char* temp_address = (char*)realloc(sstr_string->data, (_string_len + 1 + DEFAULT_CAPACITY) * sizeof(char));

        // if realloc fails.
        if (!temp_address)
        {
            sstr_string->capacity = DEFAULT_CAPACITY;
            sstr_string->length   = 0;

            SET_ERROR("Failed to realloc memory SStrFn_copy()");
            pthread_mutex_unlock(&sstr_string->mutex);
            return;
        }

        // if it suceeds we copy the temporary address to the sstr_string->data.
        sstr_string->data      =  temp_address;
        sstr_string->length    =  _string_len;
        sstr_string->capacity  =  _string_len + DEFAULT_CAPACITY;
    }



    // copy the contents of the string to the sstr_string->data.
    for (register size_t i = 0; i < _string_len; i++)
    {
        sstr_string->data[i] = string[i];
    }

    // add \0 on all spaces between the string length and the sstr_string->capacity
    for (register size_t i = sstr_string->capacity; i >= _string_len; i--)
    {
        sstr_string->data[i] = '\0';
    }

    sstr_string->length = _string_len;


    pthread_mutex_unlock(&sstr_string->mutex);
    return;
}



static void SStrFn_append_chr(SStr* sstr_string, int character)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_clear_data: Given argument is NULL.");
      return;
    }

    pthread_mutex_lock(&sstr_string->mutex);

    if (character == '\0')
    {
      pthread_mutex_unlock(&sstr_string->mutex);
      return;
    }


    if (sstr_string->length == sstr_string->capacity)
    {
      sstr_string->capacity += DEFAULT_CAPACITY;
    }


    // realloc to a temporary address.
    char* temp_address = (char*)realloc(sstr_string->data, sstr_string->capacity * sizeof(char));

    // if realloc fails.
    if (temp_address == NULL)
    {
        sstr_string->capacity = DEFAULT_CAPACITY;
        sstr_string->length   = 0;

        SET_ERROR("Failed to realloc memory SStrFn_append_chr()");
        pthread_mutex_unlock(&sstr_string->mutex);
        return;
    }


    // if realloc succeeds, copy the temporary address to sstr_string->data.
    sstr_string->data = temp_address;
    sstr_string->data[sstr_string->length] = character;
    sstr_string->length += 1;


    pthread_mutex_unlock(&sstr_string->mutex);
    return;
}


static pthread_mutex_t destroy_mutex = PTHREAD_MUTEX_INITIALIZER;
static void SStrFn_Destroy(SStr** sstr_string)
{
    pthread_mutex_lock(&destroy_mutex);

    if (sstr_string == NULL || *sstr_string == NULL || (*sstr_string)->data == 0 || (*sstr_string)->capacity == 0)
    {
      SET_ERROR("SStrFn_Destroy: Given argument is NULL.");
      pthread_mutex_unlock(&destroy_mutex);
      return;
    }

    SStr* REF = *sstr_string;
    pthread_mutex_lock(&REF->mutex);

    for (register size_t indx = REF->length; indx > 0; indx--)
    {
      REF->data[indx] = '\0';
    }

    free(REF->data);
    REF->data = NULL;
    REF->length   = 0;
    REF->capacity = 0;


    pthread_mutex_destroy(&REF->mutex);
    free(REF);

    pthread_mutex_unlock(&REF->mutex);
    pthread_mutex_unlock(&destroy_mutex);
    return;
}


static void SStrFn_append_str(SStr* sstr_string, char* string)
{
    if (sstr_string == NULL || sstr_string->data == 0 || sstr_string->capacity == 0)
    {
      SET_ERROR("SStrFn_append_str: Given argument is NULL.");
      return;
    }

    pthread_mutex_lock(&sstr_string->mutex);


    size_t total_size = (sstr_string->length + strlen(string));


    if (total_size >= sstr_string->capacity)
    { //                            double the default capacity here,
      //                            so it doesn't need to realloc memory too often.
      sstr_string->capacity = (total_size + DEFAULT_CAPACITY * 2);

      char* temp_address = (char*)realloc(sstr_string->data, sstr_string->capacity * sizeof(char));
      
      if (!temp_address)
      {
          sstr_string->length   = 0;
          sstr_string->capacity = DEFAULT_CAPACITY;

          SET_ERROR("Failed to realloc memory SStrFn_append_str()");
          pthread_mutex_unlock(&sstr_string->mutex);
          return;
      }

      sstr_string->data = temp_address;

    }

    for (register size_t indx = 0; indx < strlen(string); indx++)
    {
      sstr_string->data[sstr_string->length + indx] = string[indx];
    }

    sstr_string->length = (sstr_string->length + strlen(string));


    pthread_mutex_unlock(&sstr_string->mutex);
    return;
}

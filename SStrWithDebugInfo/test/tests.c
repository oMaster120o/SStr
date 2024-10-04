#include <stdio.h>

#include "../SStr.h"

int main()
{

  SStr* my_sstr = SStrFn.New();

  SStrFn.copy(my_sstr, "copy test.");
  if (SStrFn.Error()) return 0;
  printf("data: %s\n",        SStrFn.get.data(my_sstr));
  printf("length: %lu\n",     SStrFn.get.length(my_sstr));
  printf("capacity: %lu\n\n", SStrFn.get.capacity(my_sstr));

  SStrFn.copy(my_sstr, "copy test with looooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong string.");
  if (SStrFn.Error()) return 0;
  printf("data: %s\n",          SStrFn.get.data(my_sstr));
  printf("length: %lu\n",       SStrFn.get.length(my_sstr));
  printf("capacity: %lu\n\n\n", SStrFn.get.capacity(my_sstr));




  SStrFn.copy(my_sstr, "append char test");
  for (register size_t i = 0; i < 50; i++)
  {
      SStrFn.append.chr(my_sstr, 'e'); 
  }
  printf("data: %s\n",          SStrFn.get.data(my_sstr));
  printf("length: %lu\n",       SStrFn.get.length(my_sstr));
  printf("capacity: %lu\n\n\n", SStrFn.get.capacity(my_sstr));

  


  SStrFn.clear_data(my_sstr);
  if (SStrFn.Error()) return 0;

  SStrFn.append.str(my_sstr, "append str ");
  if (SStrFn.Error()) return 0;

  SStrFn.append.str(my_sstr, "was ");
  if (SStrFn.Error()) return 0;

  SStrFn.append.str(my_sstr, "successfull");
  if (SStrFn.Error()) return 0;

  SStrFn.append.str(my_sstr, ":ABCDEFGHIJKLMNOPQRSTUVWXYZ.");
  if (SStrFn.Error()) return 0;

  printf("data: %s\n",          SStrFn.get.data(my_sstr));
  printf("length: %lu\n",       SStrFn.get.length(my_sstr));
  printf("capacity: %lu\n\n\n", SStrFn.get.capacity(my_sstr));



  // Using any SStrFn functions after SStrFn.Destroy() other than SStrFn.New(), will crash the code.
  SStrFn.Destroy(my_sstr);
  if (SStrFn.Error()) return 0;


  // The following Tests are to check if the code crashes.
  my_sstr = SStrFn.New();

  SStrFn.clear_data(my_sstr);
  if (SStrFn.Error()) return 0;

  SStrFn.clear_data(my_sstr);
  if (SStrFn.Error()) return 0;

  for (register size_t indx = 0; indx < 10; indx++)
  {
    SStrFn.append.str(my_sstr, "\n0000\nNNNNNNNN\tTTTTTTTT\rRRRRRRRR0000");
  }

  printf("data: %s\n",          SStrFn.get.data(my_sstr));
  printf("length: %lu\n",       SStrFn.get.length(my_sstr));
  printf("capacity: %lu\n\n\n", SStrFn.get.capacity(my_sstr));



  printf("=====================\n"
         "NO CRASH HAS HAPPENED\n"
         "=====================\n\n");
  return 0;
}



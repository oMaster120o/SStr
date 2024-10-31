#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "../SStr.h"

void* thread_test(void* args);

int main()
{
    SStr* my_sstr = SStrFn.New();


    printf("\033[38;2;255;255;255m");

    SStrFn.copy(my_sstr, "copy test.");
    if (SStrFn.Error()) return 0;
    printf("data: %s\n",        SStrFn.get.data(my_sstr));
    printf("length: %lu\n",     SStrFn.get.length(my_sstr));
    printf("capacity: %lu\n\n", SStrFn.get.capacity(my_sstr));

    SStrFn.copy(my_sstr, "replaced by looooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooong string.");
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
    SStrFn.Destroy(&my_sstr);
    if (SStrFn.Error()) return 0;


    // The following Tests are to check if the code crashes.
    my_sstr = SStrFn.New();

    SStrFn.clear_data(my_sstr);
    if (SStrFn.Error()) return 0;

    SStrFn.clear_data(my_sstr);
    if (SStrFn.Error()) return 0;

    for (register size_t indx = 0; indx < 10; indx++)
    {
      SStrFn.append.str(my_sstr, "\nAAAAAAAAAAAAAAAAAA");
    }

    printf("data: %s\n",          SStrFn.get.data(my_sstr));
    printf("length: %lu\n",       SStrFn.get.length(my_sstr));
    printf("capacity: %lu\n\n\n", SStrFn.get.capacity(my_sstr));

    printf("\033[38;2;0;255;0m"
           "=====================\n"
           "NO CRASH HAS HAPPENED\n"
           "=====================\n\n\n"
          );



    printf("\033[38;2;255;100;0m"
           "MULTI THREAD TEST:\n\n"
           "PARSING SStr* my_sstr as argument for the threaded function:\n\n"
           "\t- pthread_t thread;\n"
           "\t- pthread_create(&thread, NULL, thread_test, my_sstr);\n"
           "\t- pthread_join(thread, NULL);\n"
           "\n\n\n"
          );

    pthread_t thread;
    pthread_create(&thread, NULL, thread_test, my_sstr);
    pthread_join(thread, NULL);



    printf("\033[38;2;255;255;255m" "\n\nPROTECTION FROM USING DESTROYED SStr VARIABLE TEST (SHOULD GIVE ERRORS):\n");

    SStrFn.Destroy(&my_sstr);
    if (SStrFn.Error()){};

    SStrFn.Destroy(&my_sstr);
    if (SStrFn.Error()){};

    SStrFn.clear_data(my_sstr);
    if (SStrFn.Error()){};

    SStrFn.copy(my_sstr, "PROTECTION ");
    if (SStrFn.Error()){};

    SStrFn.append.str(my_sstr, "FAILED.");
    if (SStrFn.Error()){};

    SStrFn.Destroy(&my_sstr);
    if (SStrFn.Error()){};

    printf("data: %s\n",      SStrFn.get.data(my_sstr));
    if (SStrFn.Error()){};
    printf("length: %lu\n",   SStrFn.get.length(my_sstr));
    if (SStrFn.Error()){};
    printf("capacity: %lu\n", SStrFn.get.capacity(my_sstr));
    if (SStrFn.Error()){};

    return 0;
}



void* thread_test(void* args)
{
    SStr* thread_sstr = SStrFn.New();
    if (SStrFn.Error()) return NULL;


    SStrFn.append.str(thread_sstr, "SStr* thread_sstr append str ");
    if (SStrFn.Error()) return 0;

    SStrFn.append.str(thread_sstr, "was ");
    if (SStrFn.Error()) return 0;

    SStrFn.append.str(thread_sstr, "successfull");
    if (SStrFn.Error()) return 0;

    printf("thread_sstr data: %s\n",          SStrFn.get.data(thread_sstr));
    if (SStrFn.Error()) return 0;

    printf("thread_sstr length: %lu\n",       SStrFn.get.length(thread_sstr));
    if (SStrFn.Error()) return 0;

    printf("thread_sstr capacity: %lu\n\n\n", SStrFn.get.capacity(thread_sstr));
    if (SStrFn.Error()) return 0;



    printf("thread my_sstr data: %s\n",       SStrFn.get.data(args));
    if (SStrFn.Error()) return 0;

    printf("thread my_sstr length: %lu\n",    SStrFn.get.length(args));
    if (SStrFn.Error()) return 0;

    printf("thread my_sstr capacity: %lu\n",  SStrFn.get.capacity(args));
    if (SStrFn.Error()) return 0;


    printf("\n\n"
    "\033[38;2;0;255;0m"
    "========================\n"
    "MULTI THREAD TEST PASSED\n"
    "========================\n"
           "\n\n");

    return NULL;
}

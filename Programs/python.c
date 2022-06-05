/* Minimal main program -- everything is loaded from the library */
#include "Python.h"

#ifdef MS_WINDOWS
int
wmain(int argc, wchar_t **argv)
{
    return Py_Main(argc, argv);
}
#else
int
main(int argc, char **argv)
{
    char ** argv_copy = malloc((unsigned long int)(argc+1) * sizeof(char *));

    for(size_t i=(unsigned long int)argc;i>=2;--i){
        argv_copy[i] = argv[i-1];
    }

    argv_copy[1] = "-B";
    argv_copy[0] = argv[0];

    return Py_BytesMain(argc+1, argv_copy);
}
#endif

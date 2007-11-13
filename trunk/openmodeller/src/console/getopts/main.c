#include <stdlib.h>
#include <stdio.h>
#include "getopts.h"

int main(int argc, char **argv)
{
  char *args;
  int c;

  struct options opts[] = 
  {
    { 1, "debug",   "Set the debugging level", "d", 1 },
    { 2, "version", "Display the version info", "V", 0 },
    { 3, NULL,      "Set the config file",      "f", 1 },
    { 4, "test",    "Test getopts_usage",       "t", 0 },
    { 0, NULL,      NULL,                      NULL, 0 }
  };

  while ((c = getopts(argc, argv, opts, &args)) != 0)
    {
      switch(c)
        {
/* Special Case: Recognize options that we didn't set above. */
          case -2: 
            printf("Unknown Option: %s\n", args);
            break;
/* Special Case: getopts() can't allocate memory.  
   Should probably exit() here */
          case -1:
            printf("Unabled to allocate memory from getopts().\n");
            break;
          case 1:
            printf("Setting debug level to: %s\n", args);
/*            setDebugLevel(args); */
            break;
          case 2:
            printf("ProgName 0.4.0\n");
            printf("Written by: Steve Mertz\n");
            exit(0);
            break;
          case 3:
            printf("Setting config file to: %s\n", args);
/*            readConfig(args); */
            break;
          case 4:
            /* This is just to test.  "argv[0]" is the program name */
            getopts_usage(argv[0], opts);
            break;
          default:
            break;
        }
/* This free() is required since getopts() automagically allocates space
   for "args" everytime it's called. */
      free(args);
    }
  return 0;
} 


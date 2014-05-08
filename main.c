// main.c
// Arthur Knapp
// AK146316

// Program 5B:
// This program will implement a basic version of the Unix command tar. The command tar stands for tape archive.
// This command commbines many files into a single binary file. It is assumed that these files are in the current
// working directory. The archive will also be written to the current working directory.

// This program will take in 3 diferent commands from the command line upon execution.
// "-c": create. This command creates the archive. At the command line, "-c" takes in the name of the archive about
// to be created and the file names to be archived.
// "-x": extract. This command extracts all of the file in the archive. At the command line, "-x" takes in the name of the archive to be extracted.
// "-p": prefix. This command will print information to stdout of files starting with the specified prefix. It will print the names and
// sizes of each specified file. "-p" takes in the prefix of the files to be printed and the name of the archive to be read from.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "prototypes.h"
#include "constants.h"

int main(int argc, char *argv[])
{
    if (((strcmp(argv[1], "-c") != 0) && (strcmp(argv[1], "-x") != 0) && (strcmp(argv[1], "-p") != 0)))
    // Checks if the the command entered is a valid command. If not, print error message and close the program
    {
        printf("Error: Incorrect Flag Entered %s", argv[1]);
        exit(1);
    }
    
    if (strcmp(argv[1], "-c") == 0)
    // Checks if the command is "-c" and calls the create function
    {
        create(argc, argv);
    }
    
    else if (strcmp(argv[1], "-x") == 0)
    // Checks if the command is "-x" and calls the extract function
    {
        extract(argv);
    }
    
    else if (strcmp(argv[1], "-p") == 0)
    // Checks if the command is "-p" and calls the findPrefix function
    {
        findPrefix(argv[SECOND_INDEX], argv[THIRD_INDEX]);
    }
    
    return 0;
}


// functions.c
// Arthur Knapp
// AK146316

// Functions:

// create:
// This function creates the file archive. It reads in the name of the archive file and the names of the files to be archived.
// First the, the number of files to be archived are writen to the archive file. Next, the function loops through the files and
// writes the I-Records to the archive file. I-Records contain information for each file such as, the length of the file name,
// the file name and the size of the file. Next, the function loops through the files again and writes the content of each file
// to the archive file.

// extract:
// This function extracts all of the files from the archive file. It first reads through the I-Records and stores the file name
// and the size of the file in an array of structs to be used later. Next, all of the files are extracted. This is done by looping
// through the array of structs and using that information. The file name in the struct is used to create the file. The filesize
// in the struct is used to set the number of that should be writen from the archive file to the new file.

// prefixInfo:
// This function searches for file names with the given prefix and writes the full name and size of the file to stdout. It loops
// through the I-Records and checks each file name to see if it contains the given prefix. This is done by getting the string
// length of the prefix and using strncmp() to compare the prefix to the file name in the I-Record. If no file containing the
// prefix are found, an error message is printed.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "prototypes.h"
#include "externs.h"
#include "constants.h"
#include "structs.h"

void create(int argc, char **argv)
{
    int archiveFile;
    // File descriptor for the archive file
    int inFile;
    // File descriptor the specified input file
    struct stat fileStats;
    // Declare stat struct
    char content[CONTENT_SIZE];
    // Contains the content of a file to be archived
    unsigned char numberOfFiles;
    // Stores the number of files in the archive
    unsigned char fileNameLength;
    // Stores the length of each file name
    unsigned int fileSize;
    int nRead = 1;
    // Stores the number of bytes read
    int i = START_POINT;
    // Counter to write the file information to the archive file
    int j = i;
    // Counter to write the file contents to the archive file
                
    numberOfFiles = (unsigned char)(argc - i);
    // Finds the number of files to be archived by subtracting 3 from the number of arguments. This is done because
    // the first 3 elements of the argument array are not the files to be archived.

    if ((archiveFile = open(argv[ARCHIVE_INDEX], O_WRONLY | O_CREAT, FILE_PERMISSION)) == NOT_OPENED)
    // Creates and opens archive file and checks if successfuly opened. If not, print
    // error message and close program.
    {
        fprintf(stderr, "Error: Could not open archive file.\n");
        exit(1);
    }
    
    write(archiveFile, &numberOfFiles, (size_t)sizeof(numberOfFiles));
    // Write the number of files archived to the archive file
    
    while (i < argc)
    // Loops through the files specified in the command line and writes information about each
    // to the archive file.
    {
        if (stat(argv[i], &fileStats) == NOT_OPENED)
        // Check if stats could be obtained for the current file. If not, print error message
        // and close program.
        {
            fprintf(stderr, "Could not get stats for file: %s\n", argv[i]);
            exit(1);
        }
        
        fileNameLength = (unsigned char)strlen(argv[i]);
        // Get the string length of the name of the file and store in fileLength
        fileSize = (unsigned int)fileStats.st_size;
        // Get the size of the file and store in fileSize
        write(archiveFile, &fileNameLength, sizeof(fileNameLength));
        // Write the file name length to the archive file
        write(archiveFile, argv[i], fileNameLength);
        // Write the file name to the archive file
        write(archiveFile, &fileSize, sizeof(fileSize));
        // Write the file size to the archive file
        i++;
        // Increment counter
    }
    
    while (j < argc)
    // Loops through the files specified in the command line and writes the contents
    // from each file to the archive file.
    {
        if ((inFile = open((argv[j]), O_RDONLY)) == NOT_OPENED)
        // Checks if the specified input file opens. If not, print error message and close program.
        {
            fprintf(stderr, "Could not open file: %s\n", argv[j]);
            exit(1);
        }
        
        while (nRead > 0)
        // Loops through file and wirets it to archive file
        {
            memset(content, '\0', CONTENT_SIZE);
            // Reset countent to '\0'
            
            nRead = (int)read(inFile, content, CONTENT_SIZE);
            // Reads 100 bytes from input file
            
            write(archiveFile, content, nRead);
            // Writes bytes to binary file
        }

        close(inFile);
        // Close input file
        nRead = 1;
        // Reset nRead
        j++;
        // Increment counter
    }
}

void extract(char **argv)
{
    iRecord iNode[MAX_FILES];
    // Array of i record structs
    int archiveFile;
    // Stores the file descriptor for the archive file
    int outFile;
    // Stores the file descriptor for the output file
    int i = 0;
    // Counter for for loops
    char *content;
    // Contains the content of a file to be created
    unsigned char fileNameLength;
    // Stores the length of each file name
    unsigned char numberOfFiles;
    // Stores the number of files archived

    
    if ((archiveFile = open(argv[SECOND_INDEX], O_RDONLY)) == NOT_OPENED)
    // Opens archive file and checks if successfuly opened. If not, print
    // error message and close program.
    {
        fprintf(stderr, "Could not open file: %s\n", argv[SECOND_INDEX]);
        exit(1);
    }
    
    read(archiveFile, &numberOfFiles, 1);
    // Read the number of files. N byte.
   
    for (; i < numberOfFiles; i++)
    // Loops through i records 'numberOfFiles' times
    {
        read(archiveFile, &fileNameLength, 1);
        // Read the file name length. L byte
        
        iNode[i].name = malloc(((int)fileNameLength) + 1);
        // Allocate memory for the name character pointer
        
        read(archiveFile, iNode[i].name, fileNameLength);
        // Read the file name and store in iNode
        iNode[i].name[fileNameLength + 1] = '\0';
        // Insert null character into file name
        
        read(archiveFile, &iNode[i].sizeOfFile, FOUR_BYTES);
        // Read the size of the file and store in iNode
    }
    
    for (i = 0; i < numberOfFiles; i++)
    // Loops through i records 'numberOfFiles' times
    {
        if ((outFile = open(iNode[i].name, O_WRONLY | O_CREAT, FILE_PERMISSION)) == NOT_OPENED)
        // Opens and creates file to be written to from archive file and checks if successfuly opened. If not, print
        // error message and close program.
        {
            fprintf(stderr, "Could not create file: %s\n", iNode[i].name);
            exit(1);
        }
        
        content = malloc(iNode[i].sizeOfFile);
        // Allocate memory for content
        
        read(archiveFile, content, iNode[i].sizeOfFile);
        // Read the contents of file and store in content
        write(outFile, content, iNode[i].sizeOfFile);
        // Writes the contents to the new file
        
        memset(content, '\0', iNode[i].sizeOfFile);
        // Reset content to '\0'
        
        close(outFile);
        // Close outFile
    }
    
    close(archiveFile);
    // Close archiveFile
}

void findPrefix(char *prefix, char *archiveFileName)
{
    int archiveFile;
    // Stores the file descriptor for the archive file
    int nRead;
    // Stores the number of bytes read
    int i = 0;
    // Counter for for loops
    int prefixLength;
    // Stores the length of the prefix. Used to set the character to compare the file name to.
    int fileSize;
    // Stores the size of the file
    int fileNotFoundFlag = 1;
    // Flag signyfing whether or not a file was found with the given prefix.
    // 0 = file found, 1 = file not found
    char *fileName;
    // Stores the name of the current file
    unsigned char fileNameLength;
    // Stores the length of each file name
    unsigned char numberOfFiles;
    // Stores the number of files archived
    
    if ((archiveFile = open(archiveFileName, O_RDONLY)) == NOT_OPENED)
    // Opens archive file and checks if successfuly opened. If not, print
    // error message and close program.
    {
        fprintf(stderr, "Could not open file: %s\n", archiveFileName);
        exit(1);
    }
    
    prefixLength = (int)strlen(prefix);
    // Get the length of the prefix
    read(archiveFile, &numberOfFiles, 1);
    // Read the number of files. N byte.
    
    for (; i < numberOfFiles; i++)
    // Loops through i records 'numberOfFiles' times
    {
        read(archiveFile, &fileNameLength, 1);
        // Read the length of the file
        
        if (prefixLength > fileNameLength)
        // Checks if the prefix length is greater than the file name length. If true, seek ahead to next
        // i record because the names do not match.
        {
            lseek(archiveFile, (off_t)(fileNameLength + FOUR_BYTES), SEEK_CUR);
            // Skip ahead the fileNameLength bytes plus 4 bytes for the file size.
        }
        
        else
        // The prefix length is of valid size
        {
            fileName = malloc(fileNameLength);
            // Allocate memory to fileName
            nRead = (int)read(archiveFile, fileName, fileNameLength);
            // Reads the file name prefixLength bytes
            nRead = (int)read(archiveFile, &fileSize, FOUR_BYTES);
            // Reads the file size for the file.
            
            if (strncmp(fileName, prefix, prefixLength) == 0)
            // Checks if the prefix matches the file name's characters until prefixLength
            {
                printf("%s\t%d\n", fileName, fileSize); fflush(stdout);
                // Print file name and file size
                
                fileNotFoundFlag = 0;
                // Set flag = 0 signifying that a file with the give prefix was found
            }
        }
    }
    
    if (fileNotFoundFlag == 1)
    // Checks if the flag == 1 meaning that no files were found with the given prefix
    {
        printf("No Matching Files Found\n"); fflush(stdout);
    }
    
    close(archiveFile);
    // Close archiveFile
}
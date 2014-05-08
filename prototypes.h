// prototypes.h
// Arthur Knapp
// AK146316

// Functions.c------------------------------------------------------------------

void create(int, char **);
// Creates the archive from the specified files
void extract(char **);
// Extracts files from the archive file
void findPrefix(char *, char *);
// Finds the file with the given prefix and prints the name and size
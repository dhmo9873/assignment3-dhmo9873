/* Assignment 2 
Writer.c file takes in file path and string as an argument input,
and writes the string information into the file.

Author: Dhigvijay Mohan
Date : 25-01-2026
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


int main(int argc , char *argv[])
{
    if (argc < 2){
        printf("Error: Missing arguments.");
        printf("Usage: %s <inputfile> <inputstr>", argv[0]);
        return 1;
    }
    const char *inputfile = argv[1];
    const char *inputstring = argv[2];

    int fd;
    fd = open(inputfile,O_WRONLY | O_CREAT | O_TRUNC, 0644 );

    if (fd == -1){
        printf("\nERROR Opening File\n");
        return 1;
    }

    ssize_t nr;

    nr = write(fd,inputstring,strlen(inputstring));
    if (nr == -1){
        printf("\nError Writing to a file\n");
        return 1;
    }
    

    if (close (fd) == -1){
        printf("\nError CLosing a file\n");
        return 1;
    }
    return 0;
}

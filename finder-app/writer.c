/* Assignment 2 
Writer.c file takes in file path and string as an argument input,
and writes the string information into the file.

Author: Dhigvijay Mohan
Date : 25-01-2026


Acnowledgement : usage of syslog 
https://stackoverflow.com/questions/8485333/syslog-command-in-c-code
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>



int main(int argc , char *argv[])
{
    openlog("writer", LOG_PID | LOG_PERROR, LOG_USER);

    if (argc < 3){
        syslog(LOG_ERR, "Missing arguments or Invalid Arguments");
        printf("Error: Missing arguments.");
        printf("Usage: %s <inputfile> <inputstr>", argv[0]);
        closelog();
        return 1;
    }
    const char *inputfile = argv[1];
    const char *inputstring = argv[2];

    int fd;
    fd = open(inputfile,O_WRONLY | O_CREAT | O_TRUNC, 0644 );

    if (fd == -1){
        perror("Open");
        syslog(LOG_ERR, "Failed to open file %s", inputfile);
        closelog();
        return 1;
    }

    ssize_t nr;

    nr = write(fd,inputstring,strlen(inputstring));
    if (nr == -1){
        perror("Write");
        syslog(LOG_ERR, "Failed to write to file %s", inputfile);
        close(fd);
        closelog();
        return 1;
    }
    
    syslog(LOG_DEBUG, "Writing to file succesfull");

    if (close (fd) == -1){
        perror("Close");
        syslog(LOG_ERR, "Error in Closing the file %s", inputfile);
        closelog();
        return 1;
    }

    closelog();
    return 0;
}

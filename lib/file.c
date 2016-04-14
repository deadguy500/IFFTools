#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/file.h"

int check_filename(char *filename)
{
    const char *dot = strrchr(filename, '.');

    if(strcmp(dot, ".iff"))
    {
        return 1;
    }

    return 0;
}

char *remove_extension(char* filename) 
{
    if (filename == NULL)
    {
        return NULL;
    }

    char *output_filename = malloc(strlen(filename) + 1);

    if (output_filename == NULL)
    {
        return NULL;
    }

    strcpy(output_filename, filename);
    
    char *lastdot = strrchr(output_filename, '.');
    
    if (lastdot != NULL)
    {
        *lastdot = '\0';
    }

    return output_filename;
}

size_t filesize(FILE *file, const char *name)
{
    long current_position;
    long size;

    if ((current_position = ftell(file)) >= 0)
    {
        if (fseek(file, 0, SEEK_END) >= 0)
        {
            if ((size = ftell(file)) >= 0)
            {
                if (fseek(file, current_position, SEEK_SET) >= 0)
                {
                    return (size_t)size;        
                }
            }
        }
    }

    fprintf(stderr, "Cannot determine file size of \"%s\"!\n", name);
  
    return 0;
}
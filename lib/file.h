#ifndef _FILE_H_
#define _FILE_H_

int check_filename(char *filename);

char *remove_extension(char* filename);

size_t filesize(FILE *file, const char *name);

#endif
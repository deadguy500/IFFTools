#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/iff.h"
#include "../lib/file.h"

static void write_bitplane_data(char *filename, char *bitplanes, int length)
{
    char *bitplane_filename = malloc(strlen(filename) + 1);

    if (bitplane_filename == NULL)
    {
        fprintf(stderr, "Out of memory");
    }

    strcpy(bitplane_filename, filename);
    bitplane_filename = strcat(bitplane_filename, ".bin");
    FILE *bitplanes_file = fopen(bitplane_filename, "wb+");

    if(bitplanes_file)
    {
        fwrite(bitplanes, sizeof(char), length, bitplanes_file);
        fclose(bitplanes_file);
    }
    else
    {
        fprintf(stderr, "Could not open \"%s\"!\n", bitplane_filename);
    }

    free(bitplane_filename);
}

static void write_palette_data(char *filename, unsigned short *palette, int length)
{
    char *palette_filename = malloc(strlen(filename) + 1);

    if(palette_filename == NULL)
    {
        fprintf(stderr, "Out of memory");
    }

    strcpy(palette_filename, filename);
    palette_filename = strcat(filename, ".txt");
    FILE *palette_file = fopen(palette_filename, "w+");

    if(palette_file)
    {
        for(int i = 0; i < length; i++)
        {
            fprintf(palette_file, "\tdc.w\t$%.4x\n", palette[i]);
        }

        fclose(palette_file);
    }
    else
    {
        fprintf(stderr, "Could not open \"%s\"!\n", palette_filename);
    }

    free(palette_filename);
}

static void print_info(char *name)
{
    printf("%s - IFF to Bitplane converter\n", name);
    printf("\tUsage:\t\t%s FILENAME.iff\n", name);
    printf("\tOutput:\t\tFILENAME.bin (bitplanes) and FILENAME.txt (palette)\n");
    printf("\tAuthor:\t\tdeadguy@pacific.nu\n");
}

int main(int argc,char *argv[])
{
    if(argc != 2 || check_filename(argv[1]))
    {
        print_info(argv[0]);
        exit(0);        
    }

    char *input_filename = argv[1];
    char *output_filename = remove_extension(argv[1]);

    FILE *file = fopen(input_filename, "rb");

    if (file != 0) 
    {
        size_t file_length = filesize(file, argv[1]);

        if (file_length > 0) 
        {
            char *imagedata = malloc(file_length);
            
            if (imagedata) 
            {
                if (fread(imagedata, 1, file_length, file) == file_length)
                {
                    ChunkFORM *form = get_iff_data(imagedata);

                    unsigned short row_bytes = ((form->bitmap_header->header->width + 15) >> 4) << 1;
                    unsigned int bitplane_size = row_bytes * form->bitmap_header->header->height * 
                    form->bitmap_header->header->bitplanes;

                    char *bitplanes = get_bitplanes(form);
                    write_bitplane_data(output_filename, bitplanes, bitplane_size);

                    unsigned short *palette = get_palette(form->color_map);
                    write_palette_data(output_filename, palette, form->color_map->size / 3);

                    free(form);
                }
                else
                {
                    fprintf(stderr, "Read error (\"%s\")!\n", input_filename);                
                }
            }
            else
            {
                fprintf(stderr, "Out of memory!\n");            
            }

            free(imagedata);
        }

        fclose(file);
    }
    else
    {
        fprintf(stderr, "Cannot open \"%s\"!\n", input_filename);
    }
}
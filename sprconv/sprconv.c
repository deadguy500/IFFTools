#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/iff.h"
#include "../lib/file.h"

#define     SPRITE_BPLS          2
#define     ATTACHED_BPLS        4

static int attached = 0;
static int ctrl_words = 0;

static int write_ctrl_words(char *data)
{
    if(ctrl_words)
    {
        data[0] = 0x00000000;

        return 4;
    }

    return 0;
}

static void write_sprite_data(char *filename, char *data, int width, int height)
{
    unsigned short row_bytes = ((width + 15) >> 4) << 1;
    unsigned short sprites = (row_bytes / 2);

    int length = (SPRITE_BPLS * row_bytes * height) + ctrl_words * 8 * sprites * 2; // ?

    char *sprite_data = malloc(sizeof(unsigned char) * length);

    if(sprite_data)
    {
        char *sprite_filename = malloc(strlen(filename) + 1);

        if(sprite_filename == NULL)
        {
            fprintf(stderr, "Out of memory");
        }
        else
        {
            strcpy(sprite_filename, filename);
            sprite_filename = strcat(sprite_filename, ".bin");
            FILE *file = fopen(sprite_filename, "w+");

            if(file)
            {
                int i = 0;

                for(int byte_offset = 0; byte_offset < sprites; byte_offset++)
                {
                    int bitplane_offset = (height * row_bytes);

                    char *bpl1 = &data[(bitplane_offset * 0) + (byte_offset * 2)];
                    char *bpl2 = &data[(bitplane_offset * 1) + (byte_offset * 2)];

                    i += write_ctrl_words(sprite_data);

                    for(int h = 0; h < height; h++)
                    {  
                        sprite_data[i++] = *bpl1;
                        sprite_data[i++] = *(bpl1 + 1);
                        sprite_data[i++] = *(bpl2);
                        sprite_data[i++] = *(bpl2 + 1);

                        bpl1 += row_bytes;
                        bpl2 += row_bytes;
                    }

                    i += write_ctrl_words(sprite_data);
                }

                fwrite(sprite_data, sizeof(char), length, file);
                free(sprite_data);
                fclose(file);
            }
        }
    }
    else
    {
        fprintf(stderr, "Out of memory!\n");
    }
}

static void write_attached_data(char *filename, char *data, int width, int height)
{
    unsigned short row_bytes = ((width + 15) >> 4) << 1;
    unsigned short sprites = (row_bytes / 2);

    int length = (ATTACHED_BPLS * row_bytes * height) + (ctrl_words * 8 * sprites * 2);  // ?

    char *sprite_data = malloc(sizeof(unsigned char) * length);

    if(sprite_data)
    {
        char *sprite_filename = malloc(strlen(filename) + 1);

        if(sprite_filename == NULL)
        {
            fprintf(stderr, "Out of memory");
        }
        else
        {
            strcpy(sprite_filename, filename);
            sprite_filename = strcat(sprite_filename, ".bin");
            FILE *file = fopen(sprite_filename, "w+");

            if(file)
            {
                int i = 0;

                for(int byte_offset = 0; byte_offset < sprites; byte_offset++)
                {
                    int bitplane_offset = (height * row_bytes);

                    char *bpl1 = &data[(bitplane_offset * 0) + (byte_offset * 2)];
                    char *bpl2 = &data[(bitplane_offset * 1) + (byte_offset * 2)];
                    char *bpl3 = &data[(bitplane_offset * 2) + (byte_offset * 2)];
                    char *bpl4 = &data[(bitplane_offset * 3) + (byte_offset * 2)];

                    i += write_ctrl_words(sprite_data);

                    for(int h = 0; h < height; h++)
                    {  
                        sprite_data[i++] = *bpl1;
                        sprite_data[i++] = *(bpl1 + 1);
                        sprite_data[i++] = *(bpl2);
                        sprite_data[i++] = *(bpl2 + 1);

                        bpl1 += row_bytes;
                        bpl2 += row_bytes;
                    }

                    i += write_ctrl_words(sprite_data);

                    i += write_ctrl_words(sprite_data);

                    for(int h = 0; h < height; h++)
                    {
                        sprite_data[i++] = *bpl3;
                        sprite_data[i++] = *(bpl3 + 1);
                        sprite_data[i++] = *(bpl4);
                        sprite_data[i++] = *(bpl4 + 1);

                        bpl3 += row_bytes;
                        bpl4 += row_bytes;
                    }

                    i += write_ctrl_words(sprite_data);
                }

                fwrite(sprite_data, sizeof(char), length, file);
                free(sprite_data);
                fclose(file);
            }
        }
    }
    else
    {
        fprintf(stderr, "Out of memory!\n");
    }
}

static void write_palette_data(char *filename, unsigned short *palette, int length)
{
    char *palette_filename = malloc(strlen(filename) + 1);

    if(palette_filename == NULL)
    {
        fprintf(stderr, "Out of memory");
    }

    strcpy(palette_filename, filename);
    palette_filename = strcat(palette_filename, ".txt");
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
    printf("%s - IFF to sprite converter\n", name);
    printf("  Usage:\t\t%s [-a] [-c] FILENAME.iff\n", name);
    printf("    -a - Attached sprite\n");
    printf("    -c - With control words\n");
    printf("  Output:\t\tFILENAME.bin (sprite data) and FILENAME.txt (palette)\n");
    printf("  Author:\t\tdeadguy@pacific.nu\n");
}

int main(int argc, char *argv[])
{ 
    char *input_filename;
    char *output_filename;
    
    for(int i = 0; i < argc; i++)
    {
        if(strcmp("-a", argv[i]) == 0)
        {
            attached = 1;
        }
        else if(strcmp("-c", argv[i]) == 0)
        {
            ctrl_words = 1;
        }
        else if(check_filename(argv[i]) == 0)
        {
            input_filename = argv[i];
            output_filename = remove_extension(argv[i]);
        }
    }

    if(argc < 2 || output_filename == NULL)
    {
        print_info(argv[0]);
        exit(0);        
    }

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
                    IFFChunkFORM *form = iff_get_form(imagedata);

                    unsigned short row_bytes = ((form->bmhd->header->width + 15) >> 4) << 1;
                    unsigned int bitplane_size = row_bytes * form->bmhd->header->height * 
                    form->bmhd->header->bitplanes;

                    char *bitplanes = iff_get_bitplanes(form);
                    unsigned short *palette = iff_get_palette(form->cmap);

                    if(attached == 1)
                    {
                        if(form->bmhd->header->bitplanes != 4)
                        {
                            fprintf(stderr, "Must be four bitplanes for attached sprites!\n");
                        }
                        else
                        {
                            write_palette_data(output_filename, palette, form->cmap->size / 3);
                            write_attached_data(output_filename, 
                                                bitplanes, 
                                                form->bmhd->header->width,
                                                form->bmhd->header->height);
                        }                        
                    }
                    else
                    {
                        if(form->bmhd->header->bitplanes != 2)
                        {
                            fprintf(stderr, "Must be two bitplanes for attached sprites!\n");
                        }
                        else
                        {
                            write_palette_data(output_filename, palette, form->cmap->size / 3);
                            write_sprite_data(output_filename, 
                                                bitplanes, 
                                                form->bmhd->header->width,
                                                form->bmhd->header->height);
                        }
                    }

                    free(bitplanes);
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
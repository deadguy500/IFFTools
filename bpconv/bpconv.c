#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lib/iff.h"

static size_t filesize(FILE *file, const char *name)
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

    fprintf(stderr,"Cannot determine file size of \"%s\"!\n", name);
  
    return 0;
}

static void write_palette(ChunkCMAP *cmap, char *name)
{
unsigned short length = cmap->size / 3;

    FILE *palette_file = fopen("testoutput/output_palette.txt", "w+");

    if(palette_file)
    {
        for(int i = 0; i < length; i ++)
        {
            unsigned short red = cmap->colors[i].red;      
            red = red >> 4;
            unsigned short green = cmap->colors[i].green;
            green = green >> 4;
            unsigned short blue = cmap->colors[i].blue;
            blue = blue >> 4;

            unsigned short value = red << 8 | green << 4 | blue;                                
              
            fprintf(palette_file, "\tdc.w\t$%.4x\n", value);
        }

        fclose(palette_file);        
    }
    else
    {
        // FIX ME!
        fprintf(stderr, "Cannot open \"%s\"!\n", name);
    }
}

static char *sort_bitplanes(char *input, unsigned short width, unsigned short height, unsigned short bitplanes)
{
    unsigned short row_bytes = ((width + 15) >> 4) << 1;
    unsigned int data_byte_size = row_bytes * height * bitplanes;

    char *output = malloc(data_byte_size);

    int i = 0;

    for(int w = 0; w < bitplanes; w++)
    {
        for(int y = 0; y < height; y++)
        {
            for(int x = 0; x < row_bytes; x++)
            {   
                int offset = (y * bitplanes * row_bytes) + (w * row_bytes) + x;

                output[i++] = input[offset];
            }
        }
    }

    return output;
}

static void write_bitplanes(ChunkFORM *form, char *name)
{
    FILE *bitplanes_file = fopen("testoutput/output_bitplanes.bin", "wb+");

    if(bitplanes_file)
    {
        unsigned short row_bytes = ((form->bitmap_header->header->width + 15) >> 4) << 1;
        unsigned int data_byte_size = row_bytes * form->bitmap_header->header->height * form->bitmap_header->header->bitplanes;

        char *buffer = malloc(data_byte_size);


        if(form->bitmap_header->header->compress_type == 1)
        {
            unsigned int read_compressed = 0;
            int counter = 0;

            while(read_compressed < form->body->size)
            {
                int byte = (char)form->body->data[read_compressed++];

                if(byte > 0 && byte < 127)
                {
                    for(int i = 0; i < byte + 1; i++)
                    {
                        buffer[counter++] = form->body->data[read_compressed++];
                    }
                }
                else if(byte > -127 && byte < -1)
                {
                    char replicate_byte = form->body->data[read_compressed++];

                    for(int i = 0; i < -byte + 1; i++)
                    {
                        buffer[counter++] = replicate_byte;
                    }
                }
                else
                {
                    printf("XX ");
                    break;
                }
            }

            char *bpls = sort_bitplanes(buffer, 
                form->bitmap_header->header->width, 
                form->bitmap_header->header->height,
                form->bitmap_header->header->bitplanes);

            fwrite(bpls, sizeof(char), data_byte_size, bitplanes_file);
            // free buffer
            //printf("counter: %d\n", counter);
        }
        else
        {

        }
    }
    else
    {
    // FIX ME!
        fprintf(stderr, "Cannot open \"%s\"!\n", name);
    }
}

static void write_data(ChunkFORM *form, char *name)
{
    write_palette(form->color_map, name);
    write_bitplanes(form, name);
}

static void print_image_data(ChunkFORM *form)
{
    /*
    printf("--> %s\n", form->id);
    printf("--> %d\n", form->size);
    printf("--> %s\n", form->type);

    printf("-----------------\n");
    
    printf("id:          %s\n", form->bitmap_header->id);
    printf("size:        %d\n", form->bitmap_header->size);
    printf("width:       %d\n", form->bitmap_header->header->width);
    printf("height:      %d\n", form->bitmap_header->header->height);
    printf("x:           %d\n", form->bitmap_header->header->x_coordinate);
    printf("y:           %d\n", form->bitmap_header->header->y_coordinate);
    printf("bitplanes:   %d\n", form->bitmap_header->header->bitplanes);
    printf("mask:        %d\n", form->bitmap_header->header->mask);
    printf("compress:    %d\n", form->bitmap_header->header->compress_type); 
    printf("padding:     %d\n", form->bitmap_header->header->padding);
    printf("trans:       %d\n", form->bitmap_header->header->transparency);
    printf("x aspect:    %d\n", form->bitmap_header->header->x_aspect_ratio);
    printf("y aspect:    %d\n", form->bitmap_header->header->y_aspect_ratio);
    printf("page width:  %d\n", form->bitmap_header->header->page_width);
    printf("page height: %d\n", form->bitmap_header->header->page_height);
    
    
    printf("-----------------\n");
    
    printf("--> %s\n", form->color_map->id);
    printf("--> %d\n", form->color_map->size);
    
    for(int i = 0; i < form->color_map->size / 3; i++)
    {
        printf("--> %x %x %x\n", 
            form->color_map->colors[i].red, 
            form->color_map->colors[i].green, 
            form->color_map->colors[i].blue);
    }
    
    printf("-----------------\n");
    
    printf("--> %s\n", form->body->id);
    printf("--> %d\n", form->body->size);

    for(int i = 0; i < form->body->size;)
    {
        for (int x = 0; x < 40; x++)
        {
            printf("%x ", form->body->data[i++]);
        }

        printf("\n");
    }
    */
}

int main(int argc,char *argv[])
{

    FILE *file = fopen("testinput/rainbow.iff", "rb");

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

                    print_image_data(form);

                    char *filename = "build/rainbow";

                    write_data(form, filename);
                }
                else
                {
                    fprintf(stderr, "Read error (\"%s\")!\n", argv[1]);                
                }
            }
            else
            {
                fprintf(stderr, "Out of memory!\n");            
            }
        }

        fclose(file);
    }
    else
    {
        fprintf(stderr, "Cannot open \"%s\"!\n",argv[1]);
    }
}
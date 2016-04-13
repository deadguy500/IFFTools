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

static void print_image_data(ChunkFORM *form)
{
    printf("--> %s\n", form->id);
    printf("--> %d\n", form->size);
    printf("--> %s\n", form->type);

    printf("-----------------\n");
    
    printf("--> %s\n", form->bitmap_header->id);
    printf("--> %d\n", form->bitmap_header->size);
    printf("--> %d\n", form->bitmap_header->header->width);
    printf("--> %d\n", form->bitmap_header->header->height);
    printf("--> %d\n", form->bitmap_header->header->x_coordinate);
    printf("--> %d\n", form->bitmap_header->header->y_coordinate);
    printf("--> %d\n", form->bitmap_header->header->bitplanes);
    printf("--> %d\n", form->bitmap_header->header->mask);
    printf("--> %d\n", form->bitmap_header->header->compress_type); 
    printf("--> %d\n", form->bitmap_header->header->padding);
    printf("--> %d\n", form->bitmap_header->header->transparency);
    printf("--> %d\n", form->bitmap_header->header->x_aspect_ratio);
    printf("--> %d\n", form->bitmap_header->header->y_aspect_ratio);
    printf("--> %d\n", form->bitmap_header->header->page_width);
    printf("--> %d\n", form->bitmap_header->header->page_height);
    
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

}

int main(int argc,char *argv[])
{
    FILE *file = fopen("testdata/rainbow.iff", "rb");

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
                    ChunkFORM *chunk_form = get_iff_data(imagedata);

                    print_image_data(chunk_form);
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
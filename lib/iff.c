#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iff.h"

static char *get_id(char *data)
{
    char *id = malloc(4);
    strncpy(id, data, 4);

    return id;
}

static unsigned int get_size(char *data)
{
    char *data_ptr = data + 4;

    unsigned int size = (unsigned char)data_ptr[0] << 24 | 
                        (unsigned char)data_ptr[1] << 16 | 
                        (unsigned char)data_ptr[2] << 8 | 
                        (unsigned char)data_ptr[3];

    return size;
}

static char *get_type(char *data)
{
    char *data_ptr = data + 8;
    char *type = malloc(4);
    strncpy(type, data_ptr, 4);

    return type;
}

static ChunkBMHD *get_bmhd(char *data)
{
    ChunkBMHD *bmhd = malloc(sizeof(ChunkBMHD));
    bmhd->id = get_id(data);
    bmhd->size = get_size(data);

    bmhd->header = malloc(sizeof(BitmapHeader));
    bmhd->header->width = (unsigned char)data[8] << 8 | (unsigned char)data[9]; 
    bmhd->header->height = (unsigned char)data[10] << 8 | (unsigned char)data[11]; 
    bmhd->header->x_coordinate = (unsigned char)data[12] << 8 | (unsigned char)data[13]; 
    bmhd->header->y_coordinate = (unsigned char)data[14] << 8 | (unsigned char)data[15]; 
    bmhd->header->bitplanes = data[16];
    bmhd->header->mask = data[17];
    bmhd->header->compress_type = data[18];
    bmhd->header->padding = data[19];
    bmhd->header->transparency = (unsigned char)data[20] << 8 | (unsigned char)data[21]; 
    bmhd->header->x_aspect_ratio = data[22];
    bmhd->header->y_aspect_ratio = data[23];
    bmhd->header->page_width = (unsigned char)data[24] << 8 | (unsigned char)data[25]; 
    bmhd->header->page_height = (unsigned char)data[26] << 8 | (unsigned char)data[27]; 

    return bmhd;
}

static ChunkCMAP *get_cmap(char *data)
{
    ChunkCMAP *cmap = malloc(sizeof(ChunkCMAP));

    cmap->id = get_id(data);
    cmap->size = get_size(data);

    int num_colors = cmap->size / 3;

    CmapColor *colors = malloc(sizeof(CmapColor) * num_colors);

    int a = 8;

    for(int i = 0; i < num_colors; i++)
    {
        colors[i].red = data[a + 0];
        colors[i].green = data[a + 1];
        colors[i].blue = data[a + 2];

        a += 3;
    }

    cmap->colors = colors;

    return cmap;
}

static ChunkBODY *get_body(char *data)
{
    ChunkBODY *body = malloc(sizeof(ChunkBODY));

    body->id = get_id(data);
    body->size = get_size(data);
    body->data = malloc(body->size);

    for(int i = 0; i < body->size; i++)
    {
        body->data[i] = data[8 + i];
    }

    return body;
}

static ChunkOTHR *get_other(char *data)
{
    ChunkOTHR *other = malloc(sizeof(ChunkOTHR));

    other->id = get_id(data);
    other->size = get_size(data);

    return other;
}

ChunkFORM *get_iff_data(char *data)
{
    if(strncmp(data, "FORM", 4) == 0)
    {
        ChunkFORM *form = malloc(sizeof(ChunkFORM));
        form->id = get_id(data);
        form->size = get_size(data);
        form->type = get_type(data);

        char *data_ptr = data + 12;

        for(int i = 12; i < form->size - 4;)
        {
            if(strncmp(data_ptr, "BMHD", 4) == 0)
            {
                form->bitmap_header = get_bmhd(data_ptr);
                i += form->bitmap_header->size + 8;
                data_ptr = data_ptr + form->bitmap_header->size + 8;
            }
            else if(strncmp(data_ptr, "CMAP", 4) == 0)
            {
                form->color_map = get_cmap(data_ptr);
                i += form->color_map->size + 8;
                data_ptr = data_ptr + form->color_map->size + 8;
            }
            else if(strncmp(data_ptr, "BODY", 4) == 0)
            {
                form->body = get_body(data_ptr);
                i += form->body->size + 8;
                data_ptr = data_ptr + form->body->size + 8;
            }
            else
            {
                ChunkOTHR *other = get_other(data_ptr);
                i += other->size + 8;
                data_ptr = data_ptr + other->size + 8;
            }
        }

        return form;
    }

    return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "iff.h"

#define     ID_SIZE         4   // Number of bytes
#define     TYPE_SIZE       4

#define     FORM_ID_SIZE    4
#define     FORM_SIZE_SIZE  4
#define     TYPE_SIZE       4
#define     BMHD_ID_SIZE    4
#define     BMHD_SIZE_SIZE  4
#define     CMAP_ID_SIZE    4
#define     CMAP_SIZE_SIZE  4
#define     BODY_ID_SIZE    4
#define     BODY_SIZE_SIZE  4
#define     OTHR_ID_SIZE    4
#define     OTHR_SIZE_SIZE  4

#define     COLOR_CHANS     3
#define     RED_CHN         0
#define     GREEN_CHN       1
#define     BLUE_CHN        2

static char *get_id(char *data)
{
    char *id = malloc(ID_SIZE);
    strncpy(id, data, ID_SIZE);

    return id;
}

static unsigned int get_size(char *data)
{
    return (unsigned char)data[0] << 24 | 
            (unsigned char)data[1] << 16 | 
            (unsigned char)data[2] << 8 | 
            (unsigned char)data[3];
}

static char *get_type(char *data)
{
    char *type = malloc(TYPE_SIZE);
    strncpy(type, data, TYPE_SIZE);

    return type;
}

static unsigned short get_header_width(char *data)
{
    return (unsigned char)data[8] << 8 | (unsigned char)data[9]; 
}

static unsigned short get_header_height(char *data)
{
    return (unsigned char)data[10] << 8 | (unsigned char)data[11];   
}

static unsigned short get_header_x_coord(char *data)
{
    return (unsigned char)data[12] << 8 | (unsigned char)data[13]; 
}

static unsigned short get_header_y_coord(char *data)
{
    return (unsigned char)data[14] << 8 | (unsigned char)data[15];
}

static unsigned short get_header_transparency(char *data)
{
    return (unsigned char)data[20] << 8 | (unsigned char)data[21]; 
}

static unsigned short get_header_page_width(char *data)
{
    return (unsigned char)data[24] << 8 | (unsigned char)data[25];
}

static unsigned short get_header_page_height(char *data)
{
    return (unsigned char)data[26] << 8 | (unsigned char)data[27];
}

static IFFChunkBMHD *get_bmhd(char *data)
{
    IFFChunkBMHD *bmhd = malloc(sizeof(IFFChunkBMHD));
    bmhd->id = get_id(data);
    bmhd->size = get_size(&data[BMHD_ID_SIZE]);

    bmhd->header = malloc(sizeof(IFFBitmapHeader));
    bmhd->header->width = get_header_width(data); 
    bmhd->header->height = get_header_height(data);
    bmhd->header->x_coordinate = get_header_x_coord(data);
    bmhd->header->y_coordinate = get_header_y_coord(data);
    bmhd->header->bitplanes = data[16];
    bmhd->header->mask = data[17];
    bmhd->header->compress = data[18];
    bmhd->header->padding = data[19];
    bmhd->header->transparency = get_header_transparency(data);
    bmhd->header->x_aspect_ratio = data[22];
    bmhd->header->y_aspect_ratio = data[23];
    bmhd->header->page_width = get_header_page_width(data);
    bmhd->header->page_height = get_header_page_height(data);

    return bmhd;
}

static IFFChunkCMAP *get_cmap(char *data)
{
    IFFChunkCMAP *cmap = malloc(sizeof(IFFChunkCMAP));

    cmap->id = get_id(data);
    cmap->size = get_size(&data[CMAP_ID_SIZE]);

    IFFCmapColor *colors = malloc(sizeof(IFFCmapColor) * cmap->size / COLOR_CHANS);

    int offset = CMAP_ID_SIZE + CMAP_SIZE_SIZE;

    for(int i = 0; i < cmap->size / COLOR_CHANS; i++)
    {
        colors[i].red = data[offset + RED_CHN];
        colors[i].green = data[offset + GREEN_CHN];
        colors[i].blue = data[offset + BLUE_CHN];

        offset += COLOR_CHANS;
    }

    cmap->colors = colors;

    return cmap;
}

static IFFChunkBODY *get_body(char *data)
{
    IFFChunkBODY *body = malloc(sizeof(IFFChunkBODY));

    body->id = get_id(data);
    body->size = get_size(&data[BODY_ID_SIZE]);
    body->data = malloc(body->size);

    for(int i = 0; i < body->size; i++)
    {
        body->data[i] = data[BODY_ID_SIZE + BODY_SIZE_SIZE + i];
    }

    return body;
}

static IFFChunkOTHR *get_other(char *data)
{
    IFFChunkOTHR *other = malloc(sizeof(IFFChunkOTHR));

    other->id = get_id(data);
    other->size = get_size(&data[OTHR_ID_SIZE]);

    return other;
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

IFFChunkFORM *iff_get_form(char *data)
{
    if(strncmp(data, "FORM", FORM_ID_SIZE) == 0)
    {
        IFFChunkFORM *form = malloc(sizeof(IFFChunkFORM));
        form->id = get_id(data);
        form->size = get_size(&data[FORM_ID_SIZE]);
        form->type = get_type(&data[FORM_ID_SIZE + FORM_SIZE_SIZE]);

        int offset = FORM_ID_SIZE + FORM_SIZE_SIZE + TYPE_SIZE;

        for(int i = offset; i < form->size - 4;)
        {
            if(strncmp(&data[i], "BMHD", BMHD_ID_SIZE) == 0)
            {
                form->bmhd = get_bmhd(&data[i]);
                i += form->bmhd->size + BMHD_ID_SIZE + BMHD_SIZE_SIZE;
            }
            else if(strncmp(&data[i], "CMAP", CMAP_ID_SIZE) == 0)
            {
                form->cmap = get_cmap(&data[i]);
                i += form->cmap->size + CMAP_ID_SIZE + CMAP_SIZE_SIZE;
            }
            else if(strncmp(&data[i], "BODY", BODY_ID_SIZE) == 0)
            {
                form->body = get_body(&data[i]);
                i += form->body->size + BODY_ID_SIZE + BODY_SIZE_SIZE;
            }
            else
            {
                IFFChunkOTHR *other = get_other(&data[i]);
                i += other->size + OTHR_ID_SIZE + OTHR_SIZE_SIZE;

                free(other->id);
                free(other);
            }
        }

        return form;
    }

    return 0;
}

char *iff_get_bitplanes(IFFChunkFORM *form)
{
    unsigned short row_bytes = ((form->bmhd->header->width + 15) >> 4) << 1;
    unsigned int data_byte_size = row_bytes * form->bmhd->header->height * form->bmhd->header->bitplanes;

    char *buffer = malloc(data_byte_size);
    char *bitplanes;

    if(form->bmhd->header->compress == 1)
    {
        unsigned int read_compressed = 0;
        int counter = 0;

        while(read_compressed < form->body->size)
        {
            int byte = (char)form->body->data[read_compressed++];

            if(byte >= 0 && byte < 127)
            {
                for(int i = 0; i < byte + 1; i++)
                {
                    buffer[counter++] = form->body->data[read_compressed++];
                }
            }
            else if(byte > -127 && byte <= -1)
            {
                char replicate_byte = form->body->data[read_compressed++];

                for(int i = 0; i < -byte + 1; i++)
                {
                    buffer[counter++] = replicate_byte;
                }
            }
            else
            {
                fprintf(stderr, "Could not decompress image data!\n");
            }
        }

        bitplanes = sort_bitplanes(buffer, 
            form->bmhd->header->width, 
            form->bmhd->header->height,
            form->bmhd->header->bitplanes);
    }
    else
    {
        bitplanes = sort_bitplanes(buffer, 
            form->bmhd->header->width, 
            form->bmhd->header->height,
            form->bmhd->header->bitplanes);
    }

    return bitplanes;
}

unsigned short *iff_get_palette(IFFChunkCMAP *cmap)
{
    unsigned short length = cmap->size / 3;

    unsigned short *palette = malloc(sizeof(unsigned short) * length);

    for(int i = 0; i < length; i ++)
    {
        palette[i] = (cmap->colors[i].red >> 4) << 8 | 
                    (cmap->colors[i].green >> 4) << 4 |
                    (cmap->colors[i].blue >> 4);
    }

    return palette;
}

void iff_free_form(IFFChunkFORM *form)
{
    free(form->bmhd->header);
    free(form->bmhd->id);
    free(form->bmhd);
    free(form->cmap->colors);
    free(form->cmap->id);
    free(form->cmap);
    free(form->body->id);
    free(form->body->data);
    free(form->body);
    free(form);
}


#ifndef _IFF_H_
#define _IFF_H_

typedef struct _iffbitmapheader
{
    unsigned short width;
    unsigned short height;           
    unsigned short x_coordinate;
    unsigned short y_coordinate;
    unsigned char bitplanes;
    unsigned char mask;
    unsigned char compress;
    unsigned char padding;
    unsigned short transparency;
    unsigned char x_aspect_ratio;
    unsigned char y_aspect_ratio;
    unsigned short page_width;
    unsigned short page_height;
} IFFBitmapHeader;

typedef struct _iffchunkbmhd
{
    char *id;
    unsigned int size;
    IFFBitmapHeader *header;
} IFFChunkBMHD;

typedef struct _iffcmapcolor
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} IFFCmapColor;

typedef struct _iffchunkcmap
{
    char *id;
    unsigned int size;
    IFFCmapColor *colors;
} IFFChunkCMAP;

typedef struct _iffchunkbody
{
    char *id;
    unsigned int size;
    unsigned char *data;  //char?
} IFFChunkBODY;

typedef struct _iffchunkothr
{
    char *id;
    unsigned int size;
} IFFChunkOTHR;

typedef struct _iffchunkform
{
    char *id;
    unsigned int size;
    char *type;
    IFFChunkBMHD *bmhd;
    IFFChunkCMAP *cmap;
    IFFChunkBODY *body;
} IFFChunkFORM;

IFFChunkFORM *iff_get_form(char *data);

char *iff_get_bitplanes(IFFChunkFORM *form);

unsigned short *iff_get_palette(IFFChunkCMAP *cmap);

void iff_free_form(IFFChunkFORM *form);

#endif

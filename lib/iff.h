#ifndef _IFF_H_
#define _IFF_H_

typedef struct _bitmapheader
{
    unsigned short width;
    unsigned short height;           
    unsigned short x_coordinate;
    unsigned short y_coordinate;
    unsigned char bitplanes;
    unsigned char mask;
    unsigned char compress_type;            // rename
    unsigned char padding;
    unsigned short transparency;
    unsigned char x_aspect_ratio;
    unsigned char y_aspect_ratio;
    unsigned short page_width;
    unsigned short page_height;
} BitmapHeader;

typedef struct _chunkbmhd
{
    char *id;
    unsigned int size;
    BitmapHeader *header;
} ChunkBMHD;

typedef struct _cmapcolor
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} CmapColor;

typedef struct _chunkcmap
{
    char *id;
    unsigned int size;
    CmapColor *colors;
} ChunkCMAP;

typedef struct _chunkbody
{
    char *id;
    unsigned int size;
    unsigned char *data;  //char?
} ChunkBODY;

typedef struct _chunkothr
{
    char *id;
    unsigned int size;
} ChunkOTHR;

typedef struct _chunkform
{
    char *id;
    unsigned int size;
    char *type;
    ChunkBMHD *bitmap_header;
    ChunkCMAP *color_map;
    ChunkBODY *body;
} ChunkFORM;

ChunkFORM *get_iff_data(char *data);

char *get_bitplanes(ChunkFORM *form);

unsigned short *get_palette(ChunkCMAP *cmap);


#endif

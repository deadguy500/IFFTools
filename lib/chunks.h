#ifndef _chunks_h_
#define _chunks_h_


typedef struct _IFFChunkFORM
{
	char id;			// FORM
	unsigned int size;		//
	char type;			// ILBM
	IFFChunkBMHD bitmap_header;	//
	IFFChunkCMAP color_map;		//
	IFFChunkBODY body;		//
} IFFChunkFORM;

typedef struct _IFFChunkBMHD
{
	char id;			// BMHD
	unsigned int size;		//
	char data;
} IFFChunkBMHD;

typedef struct _IFFChunkCMAP
{
	char id;			//
	unsigned int size;		//
	char data;			//
} IFFChunkCMAP;

typedef struct _IFFChunkBODY
{
	char id;			//
	unsigned int size;		//
	char data;
} IFFChunkBODY;

#endif

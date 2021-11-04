//
//  saver.h
//  imgloader
//
//  Created by 张子谦 on 2020/11/28.
//

#ifndef saver_h
#define saver_h


#include "gui_base.h"

typedef unsigned char      BYTE;
typedef unsigned short  WORD;
typedef unsigned int      DWORD;
typedef int          LONG;


#define BITS_PER_PIXCEL 24
#define FORMAT_RGBA 4
#define FORMAT_RGB  3

/** must needed. pack */
#pragma pack(1)

typedef struct
{
   WORD    bfType;
   DWORD   bfSize;
   WORD    bfReserved1;
   WORD    bfReserved2;
   DWORD   bfOffBits;
} BMP_FILE_HEADER;

typedef struct{
   DWORD      biSize;
   LONG       biWidth;
   LONG       biHeight;
   WORD       biPlanes;
   WORD       biBitCount;
   DWORD      biCompression;
   DWORD      biSizeImage;
   LONG       biXPelsPerMeter;
   LONG       biYPelsPerMeter;
   DWORD      biClrUsed;
   DWORD      biClrImportant;
} BMP_INFO_HEADER;

#pragma pack()
int rgbToBmpFile(char *pFileName, PBitmap* bmp, const int format);
int rgbToJpgFile(char *pFileName, PBitmap* bmp);
int rgbToPngFile(char *pFileName, PBitmap* bmp);
#endif /* saver_h */

#ifndef LOADER_H
#define LOADER_H

#include "gui_base.h"

enum imgType{
    JPG,
    BMP,
    PNG,
    NONE,
};

int type(char* filename);
PBitmap LoadBmp(char* filename);
PBitmap LoadJpeg(char* filename);
PBitmap LoadPng(char* filename);
PBitmap LoadImg(char* filename);


#endif

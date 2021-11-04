#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H
#include "gui_base.h"

// image_utils.c
int picScale(struct PBitmap * src, struct PBitmap * dst);
int mixColor(struct PBitmap * src, float fx, float fy, struct RGB * dst);
struct RGB * getColor(struct PBitmap * pic, int x, int y, int* isInPic);
int picTurn(struct PBitmap * src, struct PBitmap * dst, float angle);
int getTurnSize(int* width, int* height, float angle);
int picRollingOver(struct PBitmap * src, struct PBitmap * dst);
int picTurnAround(struct PBitmap * src, struct PBitmap * dst);
int setColor(struct RGB * src, struct RGB * dst);

#endif

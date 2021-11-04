#ifndef GUI_BASE_H
#define GUI_BASE_H

#define GUI_BUF 0x9000

#ifndef __ASSEMBLER__

#include "types.h"

ushort SCREEN_WIDTH;
ushort SCREEN_HEIGHT;
int screen_size; 

#define UTITLE_HEIGHT 30
#define USCREEN_HEIGHT 600
#define USCREEN_WIDTH 800

// 24 bit RGB. used in GUI Utility
typedef struct RGB {
    unsigned char B;
    unsigned char G;
    unsigned char R;
} RGB;

// 32 bit RGBA. used above GUI Utility
typedef struct RGBA {
    unsigned char B;
    unsigned char G;
    unsigned char R;
    unsigned char A;
} RGBA;


typedef struct Rect {
    int x;
    int y;
    int h;
    int w;
} Rect;

typedef struct Point {
    int x;
    int y;
} Point;

typedef struct Size {
    int h;
    int w;
} Size;

typedef struct Window {
    int hwnd;
    Point pos;
    Size size;
    char* title;
    struct RGB* content;
    struct RGB* wholeContent;
} Window;

typedef struct Image
{
	char* image_name;
    char* image_type;
    int image_size;
    int gif_img_num;
    int is_onshow;
    struct RGB* data;
    struct RGB* gif_preview;
    int scale_needed;
    struct RGB* scale_data;
    int h;
    int scale_h;
    int w;
    int scale_w;
    int save_time;
	struct Image* prev;
	struct Image* next;
}Image;
typedef struct GIF {
    long frame_num;
    long width, height;
    RGB * data;
} GIF;
typedef struct ImageList
{
	struct Image* head;
	struct Image* tail;
}ImageList;

typedef struct PBitmap
{
    int width;
    int height;
    struct RGB* data;
} PBitmap;

#pragma pack(2)
typedef struct PBitmapFileHeader{
    unsigned short bfType;
    unsigned int bfSize;
    unsigned short bfReserved1;
    unsigned short bfReserved2;
    unsigned int bfOffBits;
}PBitmapFileHeader;

typedef struct PBitmapInfoHeader{
    unsigned int biSize;
    int biWidth;
    int biHeight;
    unsigned short biPlanes;
    unsigned short biBitCount;
    unsigned int biCompression;
    unsigned int biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    unsigned int biClrUsed;
    unsigned int biClrImportant;
} PBitmapInfoHeader;

typedef struct APNGFrame{
    PBitmap* bmp;
    unsigned char dispose_op;
    unsigned char blend_op;
}APNGFrame;

typedef struct APNG{
    APNGFrame* frames;
}APNG;

// gui_base.h
void drawPoint(struct RGB* , struct RGB);
void drawPointAlpha(struct RGB* , struct RGBA);
void drawCharacter(struct RGB *, struct Point, struct Size, char, struct RGBA);
void drawString(struct RGB *, struct Point, struct Size, char *, struct RGBA);
void drawRect(struct RGB *, struct Point, struct Size, struct RGB, struct Size);
void drawBorder(RGB* buf, Point p, Size s, RGB color, Size rect_size, int border);
void drawBitmap(struct RGB* , struct RGB* , struct Point, struct Point, struct Size, struct Size, struct Size);
void colorShift(RGB* buf, Point p, Size s, Size rect_size, int shift);
void drawTransparentBitmap(struct RGB*, struct RGB*, struct Point, struct Point, struct Size, struct Size, struct Size);
void copyContent(struct RGB*, struct RGB*, struct Point, struct Size, struct Size);
void drawMouse(struct RGB *, int, int, int);

#endif
#endif

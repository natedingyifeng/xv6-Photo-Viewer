#ifndef __ASSEMBLER__

typedef struct BITMAP_FILE_HEADER {
    ushort bfType;
    uint bfSize;
    ushort bfReserved1;
    ushort bfReserved2;
    uint btOffBits;
} __attribute__((packed)) BITMAP_FILE_HEADER;

typedef struct BITMAP_INFO_HEADER {
    uint biSize;
    int biWidth;
    int biHeight;
    ushort biPlanes;
    ushort biBitCount;
    uint biCompression;
    uint biSizeImage;
    int biXPelsPerMeter;
    int biYPelsPerMeter;
    uint biCirUserd;
    uint biCirImportant;
} __attribute__((packed)) BITMAP_INFO_HEADER;

// bitmap.c
int readBitmapFile(char *, struct RGBA *, int *, int *);
int read24BitmapFile(char *, struct RGB *, int *, int *);
int write24BitmapFile(char *, struct RGB *, int, int);

#endif

#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "gui_base.h"
#include "bitmap.h"

void readBitmapHeader(int bmpFile, BITMAP_FILE_HEADER *bmpFileHeader, BITMAP_INFO_HEADER *bmpInfoHeader) {
    // Read Bitmap file header
    read(bmpFile, bmpFileHeader, sizeof(BITMAP_FILE_HEADER));
    // Read Bitmap info header
    read(bmpFile, bmpInfoHeader, sizeof(BITMAP_INFO_HEADER));
}

int readBitmapFile(char *fileName, RGBA *result, int *height, int *width) {
    int i;
    int bmpFile = open(fileName, 0);
    if (bmpFile < 0) {
        return -1;
    }

    BITMAP_FILE_HEADER bmpFileHeader;
    BITMAP_INFO_HEADER bmpInfoHeader;

    readBitmapHeader(bmpFile, &bmpFileHeader, &bmpInfoHeader);
    char headerbuf[100];
    read(bmpFile, headerbuf, bmpFileHeader.btOffBits - 54); // read out the extra header
    *width = bmpInfoHeader.biWidth;
    *height = bmpInfoHeader.biHeight;
    int column = bmpInfoHeader.biWidth;
    int row = bmpInfoHeader.biHeight;
    int bits = bmpInfoHeader.biBitCount;
    char tmpBytes[3];
    int rowBytes = column * bits / 8;
    char *buf = (char *) result;
    for (i = row - 1; i >= 0; i--) {
        if (bits == 32) {
            read(bmpFile, buf + i * rowBytes, rowBytes);
        } else {
            int j = 0;
            for (j = 0; j < column; j++) {
                read(bmpFile, buf + i * column * 4 + j * sizeof(RGBA), 3);
                *(buf + i * column * 4 + j * sizeof(RGBA) + 3) = 255;
            }
        }
        if (rowBytes % 4 > 0) {
            read(bmpFile, tmpBytes, 4 - (rowBytes % 4));
        }
    }

    close(bmpFile);
    return 0;
}

int read24BitmapFile(char *fileName, RGB *result, int *height, int *width) {
    int i;
    int bmpFile = open(fileName, 0);
    if (bmpFile < 0) {
        return -1;
    }

    BITMAP_FILE_HEADER bmpFileHeader;
    BITMAP_INFO_HEADER bmpInfoHeader;

    readBitmapHeader(bmpFile, &bmpFileHeader, &bmpInfoHeader);
    char headerbuf[100];
    read(bmpFile, headerbuf, bmpFileHeader.btOffBits - 54); // read out the extra header
    *width = bmpInfoHeader.biWidth;
    *height = bmpInfoHeader.biHeight;
    int column = bmpInfoHeader.biWidth;
    int row = bmpInfoHeader.biHeight;
    int bits = bmpInfoHeader.biBitCount;
    char tmpBytes[3];
    int rowBytes = column * 3;
    char *buf = (char *) result;
    for (i = row - 1; i >= 0; i--) {
        if (bits == 24) {
            read(bmpFile, buf + i * rowBytes, rowBytes);
        } else {
            int j = 0;
            for (j = 0; j < column; j++) {
                read(bmpFile, buf + i * column * 3 + j * sizeof(RGB), 3);
                read(bmpFile, tmpBytes, 1);
            }
        }

        if (rowBytes % 4 > 0) {
            read(bmpFile, tmpBytes, 4 - (rowBytes % 4));
        }
    }

    close(bmpFile);
    return 0;
}

void write24BitmapFileHeader(int bmpFile, int height, int width) {
    int rowSize = (24 * width + 31) / 32 * 4;  // zero padding
    BITMAP_FILE_HEADER bmpFileHeader;
    bmpFileHeader.bfType = 0x4D42;  // "BM"
    bmpFileHeader.bfSize = rowSize * height + 54;
    bmpFileHeader.bfReserved1 = 0;
    bmpFileHeader.bfReserved2 = 0;
    bmpFileHeader.btOffBits = 54;  // size of header

    BITMAP_INFO_HEADER bmpInfoHeader;
    bmpInfoHeader.biSize = 40;  // header size
    bmpInfoHeader.biWidth = width;
    bmpInfoHeader.biHeight = height;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biBitCount = 24;
    bmpInfoHeader.biCompression = 0;               // No Compression
    bmpInfoHeader.biSizeImage = rowSize * height;  // TODO:
    bmpInfoHeader.biXPelsPerMeter = 100;             //TODO:
    bmpInfoHeader.biYPelsPerMeter = 100;             //TODO:
    bmpInfoHeader.biCirUserd = 0;
    bmpInfoHeader.biCirImportant = 0;

    write(bmpFile, &bmpFileHeader, sizeof(BITMAP_FILE_HEADER));
    write(bmpFile, &bmpInfoHeader, sizeof(BITMAP_INFO_HEADER));
}

int write24BitmapFile(char *filename, RGB *img, int height, int width) {
    int bmpFile = open(filename, O_CREATE | O_RDWR);
    int rowBytes = width * 3;
    char tmpBytes[3] = {0, 0, 0};

    write24BitmapFileHeader(bmpFile, height, width);
    for (int i = height - 1; i >= 0; i--) {
        write(bmpFile, img + i * width, rowBytes);
        if (rowBytes % 4 > 0) {
            write(bmpFile, tmpBytes, 4 - (rowBytes % 4));
        }
    }

    close(bmpFile);
    return 0;
}
#include "loader.h"
#include "fcntl.h"
#include "loadjpeg.h"
#include "loadpng.h"
#include "user.h"

int type(char* filename){
    int len=strlen(filename);
    if((filename[len-1]=='g'&&filename[len-2]=='p'&&filename[len-3]=='j')||(filename[len-1]=='g'&&filename[len-2]=='e'&&filename[len-3]=='p'&&filename[len-4]=='j')) return JPG;
    if(filename[len-1]=='p'&&filename[len-2]=='m'&&filename[len-3]=='b') return BMP;
    if(filename[len-1]=='g'&&filename[len-2]=='n'&&filename[len-3]=='p') return PNG;
    else return NONE;
}

PBitmap LoadBmp(char* filename){
    PBitmap bmp = {0, 0, 0};
    int fd;
    if((fd = open(filename, O_RDONLY)) < 0){
        printf(2, "Can't open %s\n", filename);
        return bmp;
    }
    PBitmapFileHeader fileHeader;
    read(fd, (char*)&fileHeader, sizeof(fileHeader));
    
    PBitmapInfoHeader infoHeader;
    read(fd, (char*)&infoHeader, sizeof(infoHeader));
    
    bmp.width = infoHeader.biWidth;
    bmp.height = infoHeader.biHeight;
    bmp.data = (RGB*)malloc(bmp.width * bmp.height * sizeof(RGB));
    
    int count = infoHeader.biBitCount;
    int length = (((bmp.width * count) + 31) >> 5) << 2;
    int size = length * bmp.height;
    
    int wastedLen = fileHeader.bfOffBits - sizeof(fileHeader) - sizeof(infoHeader);
    uchar* waste  = (uchar*)malloc(sizeof(uchar) * wastedLen);
    read(fd, (char*)waste, wastedLen);

    uchar* data = (uchar*)malloc(sizeof(uchar) * fileHeader.bfSize);
    read(fd, (char*)data, sizeof(uchar) * fileHeader.bfSize);

    int bits = infoHeader.biBitCount / 8;
    for(int j=0; j<bmp.height; ++j){
        int offset = (bmp.height - j - 1) * bmp.width;
        int dataOffset = j * length;
        for(int i=0; i<bmp.width; ++i){
            int specOffset = dataOffset + bits * (i + 1);
            bmp.data[offset+i].R = (int)data[specOffset - 1];
            bmp.data[offset+i].G = (int)data[specOffset - 2];
            bmp.data[offset+i].B = (int)data[specOffset - 3];
        }
    }
    close(fd);
    return bmp;
}

PBitmap LoadJpeg(char* filename){
    char ZZ[64] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18,
        11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35,
        42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45,
        38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63 };
    Context* ctx=malloc(sizeof(Context));
    memset(ctx, 0, sizeof(Context));

    int fd;
    fd = open(filename, O_RDONLY);
    uchar* buf = (uchar*)malloc(MAX_JPEG_SIZE);

    read(fd, buf, MAX_JPEG_SIZE);
    
    close(fd);

    _DecodeJPEG(ctx, ZZ, buf, MAX_JPEG_SIZE);

   
    PBitmap bmp;
    bmp.height=0;
    bmp.width=0;
    bmp.data=0;
    
    int imgsize = GetImageSize(ctx);
    uchar* c = GetImage(ctx);
    bmp.width = GetWidth(ctx);
    bmp.height = GetHeight(ctx);
    int n = bmp.width * bmp.height;
    bmp.data = (RGB*)malloc(n * sizeof(RGB));
    for(int i=0; i<imgsize; i+=3){
        bmp.data[i/3].R = c[i];
        bmp.data[i/3].G = c[i+1];
        bmp.data[i/3].B = c[i+2];
    }
    return bmp;
}

PBitmap LoadPng(char* filename){
    unsigned char* image = 0;
    unsigned width, height;
    lodepng_decode24_file(&image, &width, &height, filename);
    PBitmap bmp;
    bmp.height=height;
    bmp.width=width;
    bmp.data=0;

    // int imgsize = width * height;
    int n = bmp.width * bmp.height;
    bmp.data = (RGB *)malloc(sizeof(RGB) * n);
    //这里是用decode24,所以是3个3个的读取
    for (int i = 0; i < n; i += 1)
    {
        unsigned char R = image[i*3];
        unsigned char G = image[i*3 + 1];
        unsigned char B = image[i*3 + 2];
        bmp.data[i].R = R;
        bmp.data[i].G = G;
        bmp.data[i].B = B;

    }
    free(image);
    return bmp;

}

PBitmap LoadImg(char* filename){
    int t = type(filename);
    PBitmap bmp;
    switch(t){
        case JPG: return LoadJpeg(filename);
        case BMP: return LoadBmp(filename);
        case PNG: return LoadPng(filename);

        default: return bmp;
    }
}

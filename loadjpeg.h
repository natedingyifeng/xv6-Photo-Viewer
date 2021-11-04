#ifndef LOADJPEG_H
#define LOADJPEG_H

#define MAX_JPEG_SIZE 1000000
#define ERROR_RETURN(err) do{ctx->result = err; return;}while(0);

#include "types.h"

enum  DecodeResult{
    OK,             //normally decode
    NotJPEG,        //not JPEG
    Unsupported,    //unsupported JPEG format
    OutOfMemory,    //out of memory
    InternalError,  //internal error
    SyntaxError,    //Syntax error
    InternalFinished,//mark ending
};

typedef struct VlcCode{
    uchar bits, code;
}VlcCode;

typedef struct Component{
    int cid,
    ssx, ssy,
    width, height,
    stride,
    qtselect,
    actabsel, dctabsel,
    dcpred;
    uchar* pixels;
}Component;


typedef struct Context{
    int result;                     //DecodeResult, save errortype
    const uchar* pos;               //reading positiion
    int size,                       //file size
    length,                         //segment length
    width, height,            //height and width in pixel
    MCUwidth, MCUheight,
    MCUsizex, MCUsizey,
    compNum;                          //number of color components
    Component comp[3];
    int qtused, qtavail;
    uchar qtable[4][64];
    VlcCode vlctable[4][65536];
    int buffer, bufferBits;
    int block[64];
    int rstInterval;
    uchar* rgb;
}Context;

#endif

uchar* GetImage(Context* ctx);
int GetWidth(Context* ctx);
int GetHeight(Context* ctx);
uint GetImageSize(Context* ctx);

//internal methods
uchar _Clip(const int x);
void _Skip(Context* ctx, int c);
void _DecodeLength(Context* ctx);
int _Decode2Bytes(const uchar* pos);
int _DecodeJPEG(Context* ctx, char* ZZ, const uchar* jpeg, const int size);
void _DecodeSOF(Context* ctx);
void _DecodeDHT(Context* ctx);
void _DecodeDQT(Context* ctx);
void _DecodeDRI(Context* ctx);
void _DecodeSOS(Context* ctx, char* ZZ);
void _SkipMarker(Context* ctx);
void _Convert(Context* ctx);
void _DecodeBlock(Context* ctx, char * ZZ, Component* c, uchar* out);
int _GetVLC(Context* ctx, VlcCode* vlc, uchar* code);
int _ShowBits(Context* ctx, int bits);
void _SkipBits(Context* ctx, int bits);
int _GetBits(Context* ctx, int bits);
void _RowIDCT(int* blk);
void _ColIDCT(const int* blk, uchar* out, int stride);
void _ByteAlign(Context* ctx);
void _UpsampleH(Context* ctx, Component* c);
void _UpsampleV(Context* ctx, Component* c);
uchar CF(const int x);

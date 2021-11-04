#include "loadjpeg.h"

uchar* GetImage(Context* ctx){
    return (ctx->compNum == 1) ? ctx->comp[0].pixels : ctx->rgb;

}

int GetWidth(Context* ctx){
    return ctx->width;
}

int GetHeight(Context* ctx){
    return ctx->height;
}

uint GetImageSize(Context* ctx){
    return ctx->width * ctx->height * ctx->compNum;
}

uchar _Clip(const int x){
    return x < 0 ? 0: ((x > 0xFF) ? 0xFF : (uchar)x);
}

void _Skip(Context* ctx, int c){
    ctx->pos += c;
    ctx->size -= c;
    ctx->length -= c;
    if(ctx->size < 0) ctx->result = SyntaxError;
}

void _DecodeLength(Context* ctx){
    if(ctx->size < 2) ERROR_RETURN(SyntaxError);
    ctx->length = _Decode2Bytes(ctx->pos);
    if(ctx->length > ctx->size) ERROR_RETURN(SyntaxError);
    _Skip(ctx, 2);
}

int _Decode2Bytes(const uchar* pos){
    return (pos[0] << 8) | pos[1];
}

int _DecodeJPEG(Context* ctx, char* ZZ, const uchar* jpeg, const int size){
    ctx->pos = jpeg;
    ctx->size = size & 0x7FFFFFFF;
    if( (ctx->size < 2) || ((ctx->pos[0] != 0xFF) || (ctx->pos[1] != 0xD8)) ) return NotJPEG;
    _Skip(ctx, 2);
    while(!ctx->result){
        if(ctx->size < 0 || ctx->pos[0] != 0xFF) return SyntaxError;
        _Skip(ctx, 2);
        switch(ctx->pos[-1]){
            case 0xC0: _DecodeSOF(ctx); break;
            case 0xC4: _DecodeDHT(ctx); break;
            case 0xDB: _DecodeDQT(ctx); break;
            case 0xDD: _DecodeDRI(ctx); break;
            case 0xDA: _DecodeSOS(ctx, ZZ); break;
            case 0xFE: _SkipMarker(ctx); break;
            default:
                if( (ctx->pos[-1] & 0xF0) == 0xE0) _SkipMarker(ctx);
                else return Unsupported;
        }
    }
    if(ctx->result != InternalFinished) return ctx->result;
    ctx->result = OK;
    _Convert(ctx);
    return ctx->result;
 }

void _DecodeSOF(Context* ctx){
    _DecodeLength(ctx);
    if(ctx->result)return;
    if(ctx->length < 9) ERROR_RETURN(SyntaxError);
    if(ctx->pos[0] != 8) ERROR_RETURN(Unsupported);
    ctx->height = _Decode2Bytes(ctx->pos + 1);
    ctx->width = _Decode2Bytes(ctx->pos + 3);
    ctx->compNum = ctx->pos[5];
    _Skip(ctx, 6);
    switch(ctx->compNum){
        case 1:
        case 3:break;
        default: ERROR_RETURN(Unsupported);
    }
    if(ctx->length < (ctx->compNum * 3)) ERROR_RETURN(SyntaxError);
    int i, Vmax=0, Hmax=0;
    Component* c;
    for(i=0, c=ctx->comp; i<ctx->compNum; ++i, ++c){
        c->cid = ctx->pos[0];
        if(!(c->ssx = ctx->pos[1] >> 4)) ERROR_RETURN(SyntaxError);
        if(!(c->ssy = ctx->pos[1] & 0xF)) ERROR_RETURN(SyntaxError);
        //ssx, ssy must be 1, 2, 4, 8...
        if(c->ssx & (c->ssx - 1)) ERROR_RETURN(Unsupported);
        if(c->ssy & (c->ssy - 1)) ERROR_RETURN(Unsupported);
        if((c->qtselect = ctx->pos[2]) & 0xFC) ERROR_RETURN(SyntaxError);
        _Skip(ctx, 3);
        ctx->qtused |= 1 << c->qtselect;
        if(c->ssx > Hmax) Hmax =c->ssx;
        if(c->ssy > Vmax) Vmax =c->ssy;
    }
    ctx->MCUsizex = Hmax << 3;
    ctx->MCUsizey = Vmax << 3;
    ctx->MCUwidth = (ctx->width + ctx->MCUsizex - 1) / ctx->MCUsizex;
    ctx->MCUheight = (ctx->height + ctx->MCUsizey - 1) / ctx->MCUsizey;
    for(i=0, c=ctx->comp; i< ctx->compNum; ++i, ++c){
        c->width = (ctx->width * c->ssx + Hmax - 1) / Hmax;
        c->height = (ctx->height * c->ssy + Vmax - 1) / Vmax;
        c->stride = ctx->MCUwidth * ctx->MCUsizex * c->ssx / Hmax;
        if(((c->width < 3) && (c->ssx != Hmax)) || (c->height < 3) && (c->ssy != Vmax)) ERROR_RETURN(Unsupported);
        if(!(c->pixels = (uchar*)malloc(c->stride * (ctx->MCUheight * ctx->MCUsizey * c->ssy / Vmax)))) ERROR_RETURN(OutOfMemory);
    }
    if(ctx->compNum == 3){
        ctx->rgb = (uchar*)malloc(ctx->width * ctx->height * ctx->compNum);
        if(!ctx->rgb) ERROR_RETURN(OutOfMemory);
    }
    _Skip(ctx, ctx->length);
}

void _DecodeDHT(Context* ctx){
    int i, codelen, remain, spread, curcount;
    uchar counts[16];
    _DecodeLength(ctx);
    if(ctx->result) return;
    while(ctx->length >= 17){
        i = ctx->pos[0];
        if(i & 0xEC) ERROR_RETURN(SyntaxError);
        if(i & 0x02) ERROR_RETURN(Unsupported);
        i =(i | (i >> 3)) & 3;
        for(codelen = 1; codelen <= 16; ++codelen){
            counts[codelen - 1] = ctx->pos[codelen];
        }
        _Skip(ctx, 17);
        VlcCode* vlc = &ctx->vlctable[i][0];
        remain = spread = 65536;
        for(codelen = 1; codelen <= 16; ++codelen){
            spread >>= 1;
            curcount = counts[codelen - 1];
            if(!curcount) continue;
            if(ctx->length < curcount) ERROR_RETURN(SyntaxError);
            remain -= curcount << (16 - codelen);
            if(remain < 0) ERROR_RETURN(SyntaxError);
            for(i=0; i<curcount; ++i){
                register uchar code = ctx->pos[i];
                register int j = spread;
                while(j--){
                    vlc->bits = (uchar)codelen;
                    vlc->code = code;
                    ++vlc;
                }
            }
            _Skip(ctx, curcount);
        }
        while(remain--){
            vlc->bits = 0;
            ++vlc;
        }
    }
    if(ctx->length) ERROR_RETURN(SyntaxError);
}

void _DecodeDQT(Context* ctx){
    int i;
    uchar* t;
    _DecodeLength(ctx);
    if(ctx->result) return;
    while(ctx->length >= 65){
        i = ctx->pos[0];
        if(i & 0xEC) ERROR_RETURN(SyntaxError);
        if(i & 0x10) ERROR_RETURN(Unsupported);
        ctx->qtavail |= 1 << i;
        t =&ctx->qtable[i][0];
        for(i=0; i<64; ++i) t[i] = ctx->pos[i + 1];
        _Skip(ctx, 65);
    }
    if(ctx->length) ERROR_RETURN(SyntaxError);
}

void _DecodeDRI(Context* ctx){
    _DecodeLength(ctx);
    if(ctx->result) return;
    if(ctx->length < 2) ERROR_RETURN(SyntaxError);
    ctx->rstInterval = _Decode2Bytes(ctx->pos);
    _Skip(ctx, ctx->length);
}

void _DecodeSOS(Context* ctx, char* ZZ){
    int i, MCUx, MCUy, sbx, sby;
    int rstcount = ctx->rstInterval, nextrst = 0;
    Component* c;
    _DecodeLength(ctx);
    if(ctx->result) return;
    if((ctx->length < (4 + 2 * ctx->compNum)) || (ctx->pos[0] != ctx->compNum)) ERROR_RETURN(SyntaxError);
    _Skip(ctx, 1);
    for(i=0, c=ctx->comp; i<ctx->compNum; ++i, ++c){
        if(ctx->pos[0] != c->cid) ERROR_RETURN(SyntaxError);
        if(ctx->pos[1] & 0xEC) ERROR_RETURN(SyntaxError);
        if(ctx->pos[1] & 0x02) ERROR_RETURN(Unsupported);
        c->dctabsel = ctx->pos[1] >> 4;
        c->actabsel = (ctx->pos[1] & 1) | 2;
        _Skip(ctx, 2);
    }
    if(ctx->pos[0] || (ctx->pos[1] != 0x3F)) ERROR_RETURN(SyntaxError);
    if(ctx->pos[2]) ERROR_RETURN(Unsupported);
    _Skip(ctx, ctx->length);
    for(MCUy=0; MCUy<ctx->MCUheight; ++MCUy)
        for(MCUx=0; MCUx<ctx->MCUwidth; ++MCUx){
            for(i=0, c=ctx->comp; i<ctx->compNum; ++i,++c)
                for(sby=0; sby<c->ssy; ++sby)
                    for(sbx=0; sbx<c->ssx; ++sbx){
                        _DecodeBlock(ctx, ZZ, c, &c->pixels[((MCUy * c->ssy + sby) * c->stride + MCUx * c->ssx + sbx) << 3]);
                        if(ctx->result) return;
                    }
            if(ctx->rstInterval && !(--rstcount)){
                _ByteAlign(ctx);
                i = _GetBits(ctx, 16);
                if(((i & 0xFFF8) != 0xFFFD0) || ((i & 7) != nextrst)) ERROR_RETURN(SyntaxError);
                nextrst = (nextrst + 1) & 7;
                rstcount = ctx->rstInterval;
                for(i=0; i<3; ++i) ctx->comp[i].dcpred = 0;
            }
        }
    ctx->result = InternalFinished;
}

void _DecodeBlock(Context* ctx, char * ZZ, Component* c, uchar* out){
    uchar code;
    int value, coef=0;
    memset(ctx->block, 0, sizeof(ctx->block));
    c->dcpred += _GetVLC(ctx, &ctx->vlctable[c->dctabsel][0], 0);
    ctx->block[0] = (c->dcpred) * ctx->qtable[c->qtselect][0];
    do{
        value = _GetVLC(ctx, &ctx->vlctable[c->actabsel][0], &code);
        if(!code) break;   // EOB
        if(!(code & 0x0F) && (code != 0xF0)) ERROR_RETURN(SyntaxError);
        coef += (code >> 4) + 1;
        if(coef>63) ERROR_RETURN(SyntaxError);
        ctx->block[(int)ZZ[coef]] = value * ctx->qtable[c->qtselect][coef];
    }while(coef < 63);
    for(coef=0; coef<64; coef+=8) _RowIDCT(&ctx->block[coef]);
    for(coef=0; coef<8; ++coef) _ColIDCT(&ctx->block[coef], &out[coef], c->stride);
}

int _GetVLC(Context* ctx, VlcCode* vlc, uchar* code){
    int value = _ShowBits(ctx, 16);
    int bits = vlc[value].bits;
    if(!bits){ ctx->result = SyntaxError; return 0; }
    _SkipBits(ctx, bits);
    value = vlc[value].code;
    if(code) *code = (uchar)value;
    bits = value & 15;
    if(!bits) return 0;
    value = _GetBits(ctx, bits);
    if(value < (1 << (bits - 1)))
        value += ((-1) << bits) + 1;
    return value;
}

int _ShowBits(Context* ctx, int bits){
    uchar nextbyte;
    if(!bits) return 0;
    while(ctx->bufferBits < bits){
        if(ctx->size <= 0){
            ctx->buffer = (ctx->buffer << 8) | 0xFF;
            ctx->bufferBits += 8;
            continue;
        }
        nextbyte = *ctx->pos++;
        --ctx->size;
        ctx->bufferBits += 8;
        ctx->buffer = (ctx->buffer << 8) | nextbyte;
        if(nextbyte == 0xFF){
            if(ctx->size){
                uchar marker = *ctx->pos++;
                --ctx->size;
                switch(marker){
                    case 0: break;
                    case 0xD9: ctx->size = 0; break;
                    default:
                        if((marker & 0xF8) != 0xD0)
                            ctx->result = SyntaxError;
                        else {
                            ctx->buffer = (ctx->buffer << 8) | marker;
                            ctx->bufferBits += 8;
                        }
                }
            }
            else ctx->result = SyntaxError;
        }
    }
    return (ctx->buffer >> (ctx->bufferBits - bits)) & ((1 << bits) - 1);
}

void _SkipBits(Context* ctx, int bits){
    if(ctx->bufferBits < bits) _ShowBits(ctx, bits);
    ctx->bufferBits -= bits;
}

int _GetBits(Context* ctx, int bits){
    int res = _ShowBits(ctx, bits);
    _SkipBits(ctx, bits);
    return res;
}

enum {
    W1 = 2841,
    W2 = 2676,
    W3 = 2408,
    W5 = 1609,
    W6 = 1108,
    W7 = 565,
};

void _RowIDCT(int* blk){
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;
    if(!((x1 = blk[4] << 11)
        | (x2 = blk[6])
        | (x3 = blk[2])
        | (x4 = blk[1])
        | (x5 = blk[7])
        | (x6 = blk[5])
        | (x7 = blk[3])))
    {
        blk[0] = blk[1] = blk[2] = blk[3] = blk[4] = blk[5] = blk[6] = blk[7] = blk[0] << 3;
        return;
    }
    x0 = (blk[0] << 11) + 128;
    x8 = W7 * (x4 + x5);
    x4 = x8 + (W1 - W7) * x4;
    x5 = x8 - (W1 + W7) * x5;
    x8 = W3 * (x6 + x7);
    x6 = x8 - (W3 - W5) * x6;
    x7 = x8 - (W3 + W5) * x7;
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6 * (x3 + x2);
    x2 = x1 - (W2 + W6) * x2;
    x3 = x1 + (W2 - W6) * x3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181 * (x4 + x5) + 128) >> 8;
    x4 = (181 * (x4 - x5) + 128) >> 8;
    blk[0] = (x7 + x1) >> 8;
    blk[1] = (x3 + x2) >> 8;
    blk[2] = (x0 + x4) >> 8;
    blk[3] = (x8 + x6) >> 8;
    blk[4] = (x8 - x6) >> 8;
    blk[5] = (x0 - x4) >> 8;
    blk[6] = (x3 - x2) >> 8;
    blk[7] = (x7 - x1) >> 8;
}

void _ColIDCT(const int* blk, uchar* out, int stride){
    int x0, x1, x2, x3, x4, x5, x6, x7, x8;
    if(!((x1 = blk[8 * 4] << 8)
        | (x2 = blk[8 * 6])
        | (x3 = blk[8 * 2])
        | (x4 = blk[8 * 1])
        | (x5 = blk[8 * 7])
        | (x6 = blk[8 * 5])
        | (x7 = blk[8 * 3])))
    {
        x1 = _Clip(((blk[0] + 32) >> 6) + 128);
        for (x0 = 8; x0; --x0) {
            *out = (unsigned char)x1;
            out += stride;
        }
        return;
    }
    x0 = (blk[0] << 8) + 8192;
    x8 = W7 * (x4 + x5) + 4;
    x4 = (x8 + (W1 - W7) * x4) >> 3;
    x5 = (x8 - (W1 + W7) * x5) >> 3;
    x8 = W3 * (x6 + x7) + 4;
    x6 = (x8 - (W3 - W5) * x6) >> 3;
    x7 = (x8 - (W3 + W5) * x7) >> 3;
    x8 = x0 + x1;
    x0 -= x1;
    x1 = W6 * (x3 + x2) + 4;
    x2 = (x1 - (W2 + W6) * x2) >> 3;
    x3 = (x1 + (W2 - W6) * x3) >> 3;
    x1 = x4 + x6;
    x4 -= x6;
    x6 = x5 + x7;
    x5 -= x7;
    x7 = x8 + x3;
    x8 -= x3;
    x3 = x0 + x2;
    x0 -= x2;
    x2 = (181 * (x4 + x5) + 128) >> 8;
    x4 = (181 * (x4 - x5) + 128) >> 8;
    *out = _Clip(((x7 + x1) >> 14) + 128);  out += stride;
    *out = _Clip(((x3 + x2) >> 14) + 128);  out += stride;
    *out = _Clip(((x0 + x4) >> 14) + 128);  out += stride;
    *out = _Clip(((x8 + x6) >> 14) + 128);  out += stride;
    *out = _Clip(((x8 - x6) >> 14) + 128);  out += stride;
    *out = _Clip(((x0 - x4) >> 14) + 128);  out += stride;
    *out = _Clip(((x3 - x2) >> 14) + 128);  out += stride;
    *out = _Clip(((x7 - x1) >> 14) + 128);
}

void _ByteAlign(Context* ctx){
    ctx->bufferBits &= 0xF8;
}

void _SkipMarker(Context* ctx){
    _DecodeLength(ctx);
    _Skip(ctx, ctx->length);
}

void _Convert(Context* ctx){
    int i;
    Component* c;
    for(i=0, c=ctx->comp; i<ctx->compNum; ++i, ++c){
        while((c->width < ctx->width) || (c->height < ctx->height)){
            if(c->width < ctx->width) _UpsampleH(ctx, c);
            if(ctx->result) return;
            if(c->height < ctx->height) _UpsampleV(ctx, c);
            if(ctx->result) return;
        }
        if((c->width < ctx->width) || (c->height < ctx->height)) ERROR_RETURN(InternalError);
    }
    if(ctx->compNum == 3){
        // convert to RGB
        int x, yy;
        uchar *prgb = ctx->rgb;
        const uchar *py = ctx->comp[0].pixels;
        const uchar *pcb = ctx->comp[1].pixels;
        const uchar *pcr = ctx->comp[2].pixels;
        for(yy=ctx->height; yy; --yy){
            for(x=0; x<ctx->width; ++x){
                register int y = py[x] << 8;
                register int cb = pcb[x] - 128;
                register int cr = pcr[x] - 128;
                *prgb++ = _Clip((y + 359 * cr + 128) >> 8);
                *prgb++ = _Clip((y - 88 * cb - 183 * cr + 128) >> 8);
                *prgb++ = _Clip((y + 454 * cb + 128) >> 8);
            }
            py += ctx->comp[0].stride;
            pcb += ctx->comp[1].stride;
            pcr += ctx->comp[2].stride;
        }
    }
    else if(ctx->comp[0].width != ctx->comp[0].stride){
        // grayscale -> only remove stride
        uchar *pin = &ctx->comp[0].pixels[ctx->comp[0].stride];
        uchar *pout = &ctx->comp[0].pixels[ctx->comp[0].width];
        int y;
        for(y = ctx->comp[0].height - 1; y; --y){
            memmove(pout, pin, ctx->comp[0].width);
            pin += ctx->comp[0].stride;
            pout += ctx->comp[0].width;
        }
        ctx->comp[0].stride = ctx->comp[0].width;
    }
}

enum {
    CF4A = (-9),
    CF4B = (111),
    CF4C = (29),
    CF4D = (-3),
    CF3A = (28),
    CF3B = (109),
    CF3C = (-9),
    CF3X = (104),
    CF3Y = (27),
    CF3Z = (-3),
    CF2A = (139),
    CF2B = (-11),
};

void _UpsampleH(Context* ctx, Component* c){
    const int xmax = c->width - 3;
    uchar *out, *lin, *lout;
    int x, y;
    out = (uchar*)malloc((c->width * c->height) << 1);
    if(!out) ERROR_RETURN(OutOfMemory);
    lin = c->pixels;
    lout = out;
    y =c->height;
   while(y--){
        lout[0] = CF(CF2A * lin[0] + CF2B * lin[1]);
        lout[1] = CF(CF3X * lin[0] + CF3Y * lin[1] + CF3Z * lin[2]);
        lout[2] = CF(CF3A * lin[0] + CF3B * lin[1] + CF3C * lin[2]);
        for (x = 0; x < xmax; ++x) {
            lout[(x << 1) + 3] = CF(CF4A * lin[x] + CF4B * lin[x + 1] + CF4C * lin[x + 2] + CF4D * lin[x + 3]);
            lout[(x << 1) + 4] = CF(CF4D * lin[x] + CF4C * lin[x + 1] + CF4B * lin[x + 2] + CF4A * lin[x + 3]);
        }
        lin += c->stride;
        lout += c->width << 1;
        lout[-3] = CF(CF3A * lin[-1] + CF3B * lin[-2] + CF3C * lin[-3]);
        lout[-2] = CF(CF3X * lin[-1] + CF3Y * lin[-2] + CF3Z * lin[-3]);
        lout[-1] = CF(CF2A * lin[-1] + CF2B * lin[-2]);
    }
    c->width <<= 1;
    c->stride = c->width;
    free(c->pixels);
    c->pixels = out;
}
void _UpsampleV(Context* ctx, Component* c){
    const int w = c->width, s1 = c->stride, s2 = s1 + s1;
    uchar *out, *cin, *cout;
    int x, y;
    out = (uchar*)malloc((c->width * c->height) << 1);
    if(!out) ERROR_RETURN(OutOfMemory);
    for(x = 0; x<w; ++x){
        cin = &c->pixels[x];
        cout = &out[x];
        *cout = CF(CF2A * cin[0] + CF2B * cin[s1]);  cout += w;
        *cout = CF(CF3X * cin[0] + CF3Y * cin[s1] + CF3Z * cin[s2]);  cout += w;
        *cout = CF(CF3A * cin[0] + CF3B * cin[s1] + CF3C * cin[s2]);  cout += w;
        cin += s1;
        for(y=c->height-3; y; --y) {
            *cout = CF(CF4A * cin[-s1] + CF4B * cin[0] + CF4C * cin[s1] + CF4D * cin[s2]);  cout += w;
            *cout = CF(CF4D * cin[-s1] + CF4C * cin[0] + CF4B * cin[s1] + CF4A * cin[s2]);  cout += w;
            cin += s1;
        }
        cin += s1;
        *cout = CF(CF3A * cin[0] + CF3B * cin[-s1] + CF3C * cin[-s2]);  cout += w;
        *cout = CF(CF3X * cin[0] + CF3Y * cin[-s1] + CF3Z * cin[-s2]);  cout += w;
        *cout = CF(CF2A * cin[0] + CF2B * cin[-s1]);
    }
    c->height <<= 1;
    c->stride = c->width;
    free(c->pixels);
    c->pixels = out;
}

uchar CF(const int x){
    return _Clip((x + 64) >> 7);
}

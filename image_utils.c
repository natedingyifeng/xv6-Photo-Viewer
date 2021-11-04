#include "msg.h"
#include "image_utils.h"
#include "types.h"
#include "fcntl.h"
#include "fs.h"
#include "stat.h"
#include "math.h"

#define PI 3.1415926536

RGB whiteRGB = {255, 255, 255};
RGB blackRGB = {0, 0, 0};

// 排除边界，获取对应的RGB，对于错误的x，y，将isInPic置位0
struct RGB *getColor(PBitmap *pic, int y, int x, int *isInPic)
{
    if (y < 0 || y > pic->height || x < 0 || x > pic->width)
    {
        *isInPic = 1;
        return (&whiteRGB);
    }

    if (y == pic->height)
        y--;
    if (x == pic->width)
        x--;

    *isInPic = 1;
    return (pic->data + y * pic->width + x);
}

int setColor(RGB *src, RGB *dst)
{
    dst->R = src->R;
    dst->G = src->G;
    dst->B = src->B;

    return 1;
}

// 获取经过二次插值之后的RGB
int mixColor(PBitmap *src, float fy, float fx, RGB *dst)
{
    int x = (int)fx;
    int y = (int)fy;
    if (x > fx)
        x--;
    if (y > fy)
        y--;

    int flag[4];
    RGB *Color0 = getColor(src, y, x, &flag[0]);
    RGB *Color1 = getColor(src, y + 1, x, &flag[1]);
    RGB *Color2 = getColor(src, y, x + 1, &flag[2]);
    RGB *Color3 = getColor(src, y + 1, x + 1, &flag[3]);

    float u = fx - x;
    float v = fy - y;

    float pm3 = u * v;
    float pm2 = u * (1 - v);
    float pm1 = (1 - u) * v;
    float pm0 = (1 - u) * (1 - v);

    dst->R = (int)(Color0->R * pm0 * flag[0] + Color1->R * pm1 * flag[1] + Color2->R * pm2 * flag[2] + Color3->R * pm3 * flag[3]);
    dst->G = (int)(Color0->G * pm0 * flag[0] + Color1->G * pm1 * flag[1] + Color2->G * pm2 * flag[2] + Color3->G * pm3 * flag[3]);
    dst->B = (int)(Color0->B * pm0 * flag[0] + Color1->B * pm1 * flag[1] + Color2->B * pm2 * flag[2] + Color3->B * pm3 * flag[3]);

    return 1;
}

// 将float改为int计算，加快速度
int mixColorInt(PBitmap *src, const int y_16, const int x_16, RGB *dst)
{
    int x = x_16 >> 16;
    int y = y_16 >> 16;

    int flag[4];
    RGB *Color0 = getColor(src, y, x, &flag[0]);
    RGB *Color1 = getColor(src, y + 1, x, &flag[1]);
    RGB *Color2 = getColor(src, y, x + 1, &flag[2]);
    RGB *Color3 = getColor(src, y + 1, x + 1, &flag[3]);

    unsigned int u_8 = (x_16 & 0xFFFF) >> 8;
    unsigned int v_8 = (y_16 & 0xFFFF) >> 8;

    unsigned int pm3_16 = (u_8 * v_8);
    unsigned int pm2_16 = (u_8 * (255 - v_8));
    unsigned int pm1_16 = ((255 - u_8) * v_8);
    unsigned int pm0_16 = ((255 - u_8) * (255 - v_8));

    dst->R = ((pm0_16 * Color0->R * flag[0] + pm1_16 * Color1->R * flag[1] + pm2_16 * Color2->R * flag[2] + pm3_16 * Color3->R * flag[3]) >> 16);
    dst->G = ((pm0_16 * Color0->G * flag[0] + pm1_16 * Color1->G * flag[1] + pm2_16 * Color2->G * flag[2] + pm3_16 * Color3->G * flag[3]) >> 16);
    dst->B = ((pm0_16 * Color0->B * flag[0] + pm1_16 * Color1->B * flag[1] + pm2_16 * Color2->B * flag[2] + pm3_16 * Color3->B * flag[3]) >> 16);
}

int picScale(PBitmap *src, PBitmap *dst)
{
    // float xScale = (float)src->width / (float)dst->width;
    // float yScale = (float)src->height / (float)dst->height;

    int xrIntFloat_16 = ((src->width)<<16)/dst->width+1;
    int yrIntFloat_16 = ((src->height)<<16)/dst->height+1;
    const int csdErrorX = -(1<<15)+(xrIntFloat_16>>1);
    const int csdErrorY = -(1<<15)+(yrIntFloat_16>>1);

    int srcy_16=csdErrorY;
    for (int y = 0; y < dst->height; y++)
    {
        int srcx_16=csdErrorX;
        for (int x = 0; x < dst->width; x++)
        {
            // mixColor(src, (float)(y + 0.49999) * yScale - 0.5, (float)(x + 0.49999) * xScale - 0.5, (dst->data + y * dst->width + x));
            mixColorInt(src, srcy_16, srcx_16, dst->data + y * dst->width + x);
            srcx_16+=xrIntFloat_16;
        }
        srcy_16+=yrIntFloat_16;
    }

    return 1;
}

int picFastScale(PBitmap *src, PBitmap *dst)
{
    float xScale = (float)src->width / (float)dst->width;
    float yScale = (float)src->height / (float)dst->height;

    for (int y = 0; y < dst->height; y++)
    {
        for (int x = 0; x < dst->width; x++)
        {
            // mixColor(src, (float)(y + 0.49999) * yScale - 0.5, (float)(x + 0.49999) * xScale - 0.5, (dst->data + y * dst->width + x));
            dst = getColor(src, (int)(y*yScale), (int)(x*xScale), 0);
        }
    }
    return 1;
}

int getTurnSize(int *width, int *height, float angle)
{
    int w = (int)(*width * abs(cos(angle)) + *height * abs(sin(angle)));
    int h = (int)(*width * abs(sin(angle)) + *height * abs(cos(angle)));

    *height = h;
    *width = w;
    return 1;
}

int picTurn(PBitmap *src, PBitmap *dst, float angle)
{

    // 以中心为旋转中心，要找到目标图中的旋转中心
    const float my = (float)src->height / 2;
    const float mx = (float)src->width / 2;

    const float dy = (float)(dst->height - src->height) / 2;
    const float dx = (float)(dst->width - src->width) / 2;

    const float COSX = cos(-angle);
    const float SINX = sin(-angle);

    float srcI = 0;
    float srcJ = 0;

    for (int i = 0; i < dst->height; i++)
    {
        for (int j = 0; j < dst->width; j++)
        {
            srcI = ((float)i - dy - my) * COSX + ((float)j - dx - mx) * SINX + my;
            srcJ = ((float)j - dx - mx) * COSX - ((float)i - dy - my) * SINX + mx;
            if (!(srcI >= 0 && srcI < src->height && srcJ >= 0 && srcJ < src->width))
            {
                setColor(&whiteRGB, (dst->data + i * dst->width + j));
            }
            mixColor(src, srcI, srcJ, (dst->data + i * dst->width + j));
        }
    }

    return 1;
}

// （上下）翻转
int picRollingOver(PBitmap *src, PBitmap *dst)
{
    // 如果原图与目标图尺寸不一样，则返回 0；
    if (src->width != dst->width || src->height != dst->height)
        return 0;

    int h = src->height;
    int w = src->width;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            setColor((src->data + i * w + j), (dst->data + (h - i - 1) * w + j));
        }
    }

    return 1;
}

int picTurnAround(PBitmap *src, PBitmap *dst)
{
    // 如果原图与目标图尺寸不一样，则返回 0；
    if (src->width != dst->width || src->height != dst->height)
        return 0;

    int h = src->height;
    int w = src->width;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            setColor((src->data + i * w + j), (dst->data + i * w + (w - j - 1)));
        }
    }

    return 1;
}


#include "types.h"
#include "user.h"
#include "gui_base.h"
#include "msg.h"
#include "character.h"

int api_createwindow(Window* wnd)
{
    Size size = wnd->size;
    wnd->wholeContent = malloc(size.w * (size.h + UTITLE_HEIGHT) * 3);
    wnd->content = wnd->wholeContent + size.w * UTITLE_HEIGHT;
    // set all content to zero
    memset(wnd->content, 255, size.w * size.h * 3);
    wnd->hwnd = createwindow(wnd->pos.x, wnd->pos.y,
            wnd->size.w, wnd->size.h, wnd->title, wnd->wholeContent);
    return wnd->hwnd;
}

int api_paint24Bitmap(Window* wnd, struct RGB* img, Point p, Size s)
{
    drawBitmap(wnd->content, img, p, (Point){0, 0}, wnd->size, s, s);
    return 0;
}

int api_paint24BitmapToContent(Window* wnd, struct RGB* img, Point pWnd, Point pBmp, Size sBmp, Size s)
{
    drawBitmap(wnd->content, img, pWnd, pBmp, wnd->size, sBmp, s);
    return 0;
}

int api_paint24BitmapToContentTransparent(Window* wnd, struct RGB* img, Point pWnd, Point pBmp, Size sBmp, Size s)
{
    drawTransparentBitmap(wnd->content, img, pWnd, pBmp, wnd->size, sBmp, s);
    return 0;
}

int api_repaint(Window* wnd)
{
    repaintwindow(wnd->hwnd);
    return 0;
}

int api_update(Window* wnd, Rect rect)
{
    updatewindow(wnd->hwnd, rect.x, rect.y, rect.h, rect.w);
    return 0;
}

int api_exec(Window* wnd, ProcFun pf)
{
    message msg;
    int r = 0;
    while(1)
    {
        r = getmessage(wnd->hwnd, &msg);
        if(r)
        {
            pf(&msg);
        }
    }
    return -1;
}

int api_settimer(Window* wnd, int interval)
{
    settimer(wnd->hwnd, interval);
    return 0;
}

int api_drawRect(Window *wnd, Point p, Size s, RGB color)
{
    drawRect(wnd->content, p, wnd->size, color, s);
    return 0;
}

int api_drawCharacter(Window *wnd, int x, int y, char ch, RGBA color) {
    drawCharacter(wnd->content, (Point){x, y}, wnd->size, ch, color);
    return 0;
}


int api_drawString(Window *wnd, int x, int y, char *str, RGBA color) {
    drawString(wnd->content, (Point){x, y}, wnd->size, str, color);
    return 0;
}

int api_drawButton(Window *wnd, Point p, Size s, char * str)
{
    RGB bColor;
    bColor.R = 9;
    bColor.G = 163;
    bColor.B = 220;
    api_drawRect(wnd, p, s, bColor);
    api_drawString(wnd, p.x + 10, p.y + 10, str, (RGBA){255,255,255,255});
    return 0;
}


int api_drawImgButton(Window* wnd, struct RGB* img, Point pWnd, Size s, int borderWidth, RGB borderColor, int shift) {
    drawBitmap(wnd->content, img, pWnd, (Point){0, 0}, wnd->size, s, s);
    colorShift(wnd->content, pWnd, wnd->size, s, shift);
    drawBorder(wnd->content, pWnd, wnd->size, borderColor, s, borderWidth);
    return 0;
}


int api_destroywindow(Window *wnd) {
    destroywindow(wnd->hwnd);
    return 0;
}
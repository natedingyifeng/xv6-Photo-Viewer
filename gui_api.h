#ifndef GUI_API_H
#define GUI_API_H

int api_createwindow(struct Window *);
int api_paint24Bitmap(struct Window *, struct RGB*, struct Point, struct Size);
int api_paint24BitmapToContent(struct Window*, struct RGB*,struct Point,struct Point,struct Size, struct Size);
int api_paint24BitmapToContentTransparent(struct Window*, struct RGB*,struct Point,struct Point,struct Size, struct Size);
int api_paintContentToContent(struct Window*, struct RGB*,struct Point,struct Point,struct Size, struct Size);
int api_repaint(struct Window *);
int api_update(struct Window *, struct Rect);
int api_settimer(struct Window *, int);
int api_exec(struct Window *, ProcFun);
int api_fastrand();
int api_drawPointAlpha(struct RGB*, struct RGBA);
int api_drawCharacter(struct Window *, int, int, char, struct RGBA);
int api_drawString(struct Window *, int, int, char *, struct RGBA);
int api_destroywindow(struct Window *wnd);
int api_drawRect(struct Window *, struct Point, struct Size, struct RGB);
int api_drawButton(struct Window *, struct Point, struct Size, char *);
int api_drawImgButton(Window* wnd, struct RGB* img, Point pWnd, Size s, int borderWidth, RGB borderColor, int colorShift);

#endif
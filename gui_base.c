#include "types.h"
#include "defs.h"
#include "gui_base.h"
#include "mouse_shape.h"
#include "character.h"


void drawPoint(RGB* color, RGB origin) {
    color->R = origin.R;
    color->G = origin.G;
    color->B = origin.B;
}

void drawPointAlpha(RGB* color, RGBA origin) {
    float alpha;
    if (origin.A == 255) {
        color->R = origin.R;
        color->G = origin.G;
        color->B = origin.B;
        return;
    }
    if (origin.A == 0) {
        return;
    }
    alpha = (float)origin.A / 255;
    color->R = color->R * (1 - alpha) + origin.R * alpha;
    color->G = color->G * (1 - alpha) + origin.G * alpha;
    color->B = color->B * (1 - alpha) + origin.B * alpha;
}

void drawCharacter(RGB *buf, Point p, Size s, char ch, RGBA color) {
    int i, j;
    RGB *t;
    int ord = ch - 0x20; // omit control ASCII code.
    if (ord < 0 || ord >= (CHARACTER_NUMBER - 1)) {
        return;
    }

    for (i = 0; i < CHARACTER_HEIGHT; i++) {
        if (p.y + i > s.h || p.y + i < 0) {
            break;
        }
        for (j = 0; j < CHARACTER_WIDTH; j++) {
            if (character[ord][i][j] == 1) {
                if (p.x + j > s.w || p.x + j < 0) {
                    break;
                }
                t = buf + (p.y + i) * s.w + p.x + j;
                drawPointAlpha(t, color);
            }
        }
    }
}

void drawString(RGB *buf, Point p, Size s, char *str, RGBA color) {
    int offset_x = 0;

    while (*str != '\0') {
        drawCharacter(buf, (Point){p.x+offset_x, p.y}, s, *str, color);
        offset_x += CHARACTER_WIDTH;
        str++;
    }
}

void drawRect(RGB *buf, Point p, Size s, RGB color, Size rect_size) {
    struct RGB * t;
    int draw_h = rect_size.h;
    int draw_w = rect_size.w;

    if (draw_h > s.h - p.y) {
        draw_h = s.h - p.y;
    }

    if (draw_w > s.w - p.x) {
        draw_w = s.w - p.x;
    }

    for (int i = 0; i < draw_h; i++) {
        for(int j = 0; j < draw_w; j++) {
            t = buf + (p.y + i) * s.w + p.x + j;
            *t = color;
        }
    }
}

void drawBorder(RGB* buf, Point p, Size s, RGB color, Size rect_size, int border) {
    drawRect(buf, p, s, color, (Size){border, rect_size.w});
    drawRect(buf, (Point){p.x, p.y + rect_size.h - border}, s, color, (Size){border, rect_size.w});

    drawRect(buf, (Point){p.x, p.y + border}, s, color, (Size){rect_size.h - 2 * border, border});
    drawRect(buf, (Point){p.x + rect_size.w - border, p.y + border}, s, color, (Size){rect_size.h - 2 * border, border});
}


// pt: start point of target
// pc: start point of content
// st: size of target
// sc: size of content
// s: size to be drawed
void drawBitmap(struct RGB* tgt, struct RGB* cont, Point pt, Point pc, Size st, Size sc, Size s) {
    struct RGB *t;
    struct RGB *o;
    int draw_h = s.h;
    int draw_w = s.w;

    if (draw_h > st.h - pt.y) {
        draw_h = st.h - pt.y;
    }
    if (draw_h > sc.h - pc.y) {
        draw_h = sc.h - pc.y;
    }

    if (draw_w > st.w - pt.x) {
        draw_w = st.w - pt.x;
    }
    if (draw_w > sc.w - pc.x) {
        draw_w = sc.w - pc.x;
    }

    for (int i = 0; i < draw_h; i++) {
        if (pt.y + i < 0 || pt.y + i >= st.h) {
            continue;
        }
        t = tgt + (pt.y + i) * st.w + pt.x;
        o = cont + (pc.y + i) * sc.w + pc.x;
        memmove(t, o, draw_w * 3);
    }
}

void drawTransparentBitmap(struct RGB* tgt, struct RGB* cont, Point pt, Point pc, Size st, Size sc, Size s) {
    struct RGB *t;
    struct RGB *o;
    int draw_h = s.h;
    int draw_w = s.w;

    if (draw_h > st.h - pt.y) {
        draw_h = st.h - pt.y;
    }
    if (draw_h > sc.h - pc.y) {
        draw_h = sc.h - pc.y;
    }

    if (draw_w > st.w - pt.x) {
        draw_w = st.w - pt.x;
    }
    if (draw_w > sc.w - pc.x) {
        draw_w = sc.w - pc.x;
    }

    for (int i = 0; i < s.h; ++i) {
        for(int j = 0; j < s.w; ++j)
        {
            t = tgt + (pt.y + i) * st.w + pt.x + j;
            o = cont + (pc.y + i) * sc.w + pc.x + j;

            if(o->R==255 && o->G == 255 && o->B==255)
                continue;
            t->R = o->R;
            t->G = o->G;
            t->B = o->B;
        }
    }
}

void copyContent(RGB* tgt, RGB* src, Point p, Size s, Size copy_size) {
    drawBitmap(tgt, src, p, p, s, s, copy_size);
}

void colorShift(RGB* buf, Point p, Size s, Size rect_size, int shift) {
    struct RGB * t;
    int draw_h = rect_size.h;
    int draw_w = rect_size.w;

    if (draw_h > s.h - p.y) {
        draw_h = s.h - p.y;
    }

    if (draw_w > s.w - p.x) {
        draw_w = s.w - p.x;
    }

    for (int i = 0; i < draw_h; i++) {
        for(int j = 0; j < draw_w; j++) {
            t = buf + (p.y + i) * s.w + p.x + j;
            if (t->R > 200 && t->G > 200 && t->B > 200) {
                t->R = (t->R + shift + 256) % 256;
                t->G = (t->G + shift + 256) % 256;
                t->B = (t->B + shift + 256) % 256;
            }
        }
    }
}

void drawMouse(RGB *buf, int mode, int x, int y) {
    int i, j;
    RGB *t;
    for (i = 0; i < MOUSE_HEIGHT; i++) {
        if (y + i > SCREEN_HEIGHT || y + i < 0) {
            break;
        }
        for (j = 0; j < MOUSE_WIDTH; j++) {
            if (x + j > SCREEN_WIDTH || x + j < 0) {
                break;
            }
            uchar temp = mouse_pointer[mode][i][j];
            if (temp) {
                t = buf + (y + i) * SCREEN_WIDTH + x + j;
                drawPoint(t, mouse_color[temp - 1]);
            }
        }
    }
}


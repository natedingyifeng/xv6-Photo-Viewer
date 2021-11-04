#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "spinlock.h"
#include "gui_base.h"
#include "mouse_shape.h"

struct spinlock screen_lock;
struct spinlock screen_wo_focus_lock;
struct spinlock screen_buf_lock;

RGB *screen = 0;
RGB *screen_wo_focus = 0; // this is the screen without focused window
RGB *screen_buf = 0;

void initGUI() {
    uint GraphicMem = KERNBASE + 0x1028;
    uint baseAdd = *((uint*)GraphicMem);
    screen = (RGB*)baseAdd;
    SCREEN_WIDTH = *((ushort*)(KERNBASE + 0x1012));
    SCREEN_HEIGHT = *((ushort*)(KERNBASE + 0x1014));
    screen_size = (SCREEN_WIDTH * SCREEN_HEIGHT) * 3;
    screen_wo_focus = (RGB*)(baseAdd + screen_size);
    screen_buf = (RGB*)(baseAdd + screen_size * 2);
    initlock(&screen_lock, "screen");
    initlock(&screen_wo_focus_lock, "screen_wo_focus");
    initlock(&screen_buf_lock, "screen_buf");

    mouse_color[0].G = 0;
    mouse_color[0].B = 0;
    mouse_color[0].R = 0;
    mouse_color[1].G = 200;
    mouse_color[1].B = 200;
    mouse_color[1].R = 200;

    cprintf("@Screen Width:   %d\n", SCREEN_WIDTH);
    cprintf("@Screen Height:  %d\n", SCREEN_HEIGHT);
    cprintf("@Bits per pixel: %d\n",*((uchar*)(KERNBASE+0x1019)));
    cprintf("@Video card drivers initialized successfully.\n");
}

// seems like we don't need GUI lock
void acquireGUILock(RGB *buf) {
    return;
    if (buf == screen) {
        acquire(&screen_lock);
    } else if (buf == screen_wo_focus) {
        acquire(&screen_wo_focus_lock);
    } else if (buf == screen_buf) {
        acquire(&screen_buf_lock);
    }
}

void releaseGUILock(RGB *buf) {
    return;
    if (buf == screen) {
        release(&screen_lock);
    } else if (buf == screen_wo_focus) {
        release(&screen_wo_focus_lock);
    } else if (buf == screen_buf) {
        release(&screen_buf_lock);
    }
}

void drawCharacterToScreen(RGB *buf, Point p, Size s, char ch, RGBA color) {
    acquireGUILock(buf);
    drawCharacter(buf, p, s, ch, color);
    releaseGUILock(buf);
}

void drawStringToScreen(RGB *buf, Point p, Size s, char *str, RGBA color) {
    acquireGUILock(buf);
    drawString(buf, p, s, str, color);
    releaseGUILock(buf);
}

void drawBitmapToScreen(RGB *buf, RGB *img, Point p, Size s) {
    acquireGUILock(buf);
    drawBitmap(buf, img, p, (Point){0, 0}, (Size){USCREEN_HEIGHT, USCREEN_WIDTH}, s, s);
    releaseGUILock(buf);
}

void drawPartBitmapToScreen(RGB *buf, RGB *img, Point screen_p, Point img_p, Size img_s, Size draw_s) {
    acquireGUILock(buf);
    drawBitmap(buf, img, screen_p, img_p, (Size){USCREEN_HEIGHT, USCREEN_WIDTH}, img_s, draw_s);
    releaseGUILock(buf);
}

void drawTransparentBitmapToScreen(RGB *buf, RGB *img, Point p, Size s) {
    acquireGUILock(buf);
    drawTransparentBitmap(buf, img, p, (Point){0, 0}, (Size){USCREEN_HEIGHT, USCREEN_WIDTH}, s, s);
    releaseGUILock(buf);
}

void drawMouseToScreen(RGB *buf, int mode, int x, int y) {
    acquireGUILock(buf);
    drawMouse(buf, mode, x, y);
    releaseGUILock(buf);
}

void drawScreenToScreen(RGB* buf, RGB* img) {
    acquireGUILock(buf);
    memmove(buf, img, SCREEN_WIDTH * SCREEN_HEIGHT * 3);
    releaseGUILock(buf);
}

void clearMouse(RGB *buf, RGB *no_mouse_buf, int x, int y) {
    acquireGUILock(buf);
    drawBitmap(buf, no_mouse_buf, (Point){x, y}, (Point){x, y}, (Size){USCREEN_HEIGHT, USCREEN_WIDTH}, (Size){USCREEN_HEIGHT, USCREEN_WIDTH}, (Size){MOUSE_HEIGHT, MOUSE_WIDTH});
    releaseGUILock(buf);
}

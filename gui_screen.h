#ifndef GUI_SCREEN_H
#define GUI_SCREEN_H

#ifndef __ASSEMBLER__

extern struct RGB* screen;
extern struct RGB* screen_wo_focus;
extern struct RGB* screen_buf;
void initGUI(void);
void drawCharacterToScreen(struct RGB *, struct Point, struct Size, char, struct RGBA);
void drawStringToScreen(struct RGB *, struct Point, struct Size, char *, struct RGBA);
void drawBitmapToScreen(struct RGB *, struct RGB *, struct Point, struct Size);
void drawPartBitmapToScreen(struct RGB *, struct RGB *, struct Point, struct Point, struct Size, struct Size);
void drawTransparentBitmapToScreen(struct RGB *, struct RGB *, struct Point, struct Size);
void drawMouseToScreen(struct RGB *, int, int, int);
void drawScreenToScreen(struct RGB*, struct RGB*);
void clearMouse(struct RGB *, struct RGB *, int, int);

#endif
#endif
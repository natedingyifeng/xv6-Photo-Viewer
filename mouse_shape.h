#ifndef MOUSE_SHAPE_H
#define MOUSE_SHAPE_H

#ifndef __ASSEMBLER__

#define MOUSE_MODE 2
#define MOUSE_HEIGHT 18
#define MOUSE_WIDTH 15

struct RGB;

RGB mouse_color[2];

extern const uchar mouse_pointer[MOUSE_MODE][MOUSE_HEIGHT][MOUSE_WIDTH];

#endif
#endif
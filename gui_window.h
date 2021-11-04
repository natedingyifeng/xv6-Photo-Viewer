
#ifndef GUI_KERNEL_H
#define GUI_KERNEL_H

#ifndef __ASSEMBLER__

#define MAX_MSG_COUNT 50
#define MAX_WINDOW_COUNT 10

#define MOUSE_SPEED_X 0.6f
#define MOUSE_SPEED_Y -0.6f

struct Rect;
struct Point;
struct Size;
struct message;
struct proc;
struct WndInfo;
struct TimerInfo; 
struct spinlock;

enum mouseInWinPos {
    NOT_IN, CONTENT, BAR, CLOSE_BTN
};

typedef struct MsgQueue {
    int head;
    int tail;
    int length;
    struct message msgList[MAX_MSG_COUNT];
} MsgQueue;

typedef struct WndInfo {
    int hwnd;
    Rect wndTitleBar;
    Rect wndBody;
    struct proc * procPtr;
    struct RGB * content;
    struct RGB * wholeContent;
    char * title;
    MsgQueue msgQ;
} WndInfo;

extern struct WndInfo wndInfoList[MAX_WINDOW_COUNT];
extern int wndCount;

typedef struct Focus {
    int x;
    int y;
    int ID;
} Focus;

typedef struct MousePos {
    int x;
    int y;
} MousePos;

typedef struct TimerInfo {
    int ticks;
    int intervalList[MAX_WINDOW_COUNT];
    int countList[MAX_WINDOW_COUNT];
} TimerInfo;

// void setRect(struct Rect *rect, int x, int y, int w, int h);

// int drawWndTitleBar(int hwnd);
// int repaintAllWindow(int hwnd);
// int focusOnWindow(int hwnd);
// int updateWindow(int hwnd, int x, int y, int w, int h);

// void  initMsgQueue(MsgQueue * msgQ);
// int isQueueEmpty(MsgQueue *msgQ);
// int isQueueFull(MsgQueue *msgQ);
// int addMsgToQueue(MsgQueue *msgQ, message *msg);
// int dispatchMessage(int hwnd, message *msg);
// int getMessageFromQueue(MsgQueue *msgQ, message * msg);

#endif
#endif
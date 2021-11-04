struct stat;
struct rtcdate;
struct RGBA;
struct RGB;
struct Window;
struct Point;
struct Size;
struct message;
struct Rect;
struct PBitmap;

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int exec(char*, char**);
int open(char*, int);
int mknod(char*, short, short);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkdir(char*);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int lseek(int, int, int);

typedef void (* ProcFun)(struct message *);
// system calls for gui
int createwindow(int, int, int, int, char *, struct RGB*);
int repaintwindow(int);
int updatewindow(int, int, int, int, int);
int getmessage(int,struct message *);
int settimer(int , int);
int destroywindow(int);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
void printf(int, char*, ...);
char* gets(char*, int max);
unsigned int strlen(char*);
void* memset(void*, int, unsigned int);
void* malloc(uint);
void free(void*);
int atoi(const char*);


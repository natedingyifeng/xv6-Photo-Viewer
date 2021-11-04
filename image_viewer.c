//Desktop

#include "msg.h"
#include "user.h"
#include "types.h"
#include "user.h"
#include "fcntl.h"
#include "fs.h"
#include "stat.h"
#include "image_utils.h"
#include "bitmap.h"

#define PI 3.1415926536
#define MOUSE_SPEED_X 0.6f
#define MOUSE_SPEED_Y -0.6f

//define the xv6 param
#include "gui_base.h"
#include "gui_api.h"

#include "loader.h"
#include "saver.h"
#include "loadgif.h"

// #include "loader.h"
// #include "saver.h"


Window wnd;

RGB* save_icon;
RGB* delete_icon;
RGB* cut_icon;
RGB* pen_icon;
RGB* rubber_icon;
RGB* red_icon;
RGB* green_icon;
RGB* blue_icon;
RGB* purple_icon;
RGB* zoomin_icon;
RGB* zoomout_icon;
RGB* rotate_left_90_icon;
RGB* rotate_left_30_icon;
RGB* rotate_right_30_icon;
RGB* rotate_right_90_icon;
RGB* image_list_up_icon;
RGB* image_list_down_icon;
RGB* rollover_icon;
RGB* turnaround_icon;
RGB* cut_confirm_icon;
RGB* cut_cancel_icon;
RGB* brightness_up_icon;
RGB* brightness_down_icon;
RGB* edit_img;
RGB* edit_img_test;
RGB* edit_img_origin;
RGB* cut_img_save;
RGB* cut_img_result;
RGB* image_list_origin;
RGB* image_title_origin;
RGB* image_in_content;
RGB* image_in_preview;
RGB* image_in_preview_scaled;
RGB* image_in_content_scaled;
RGB* image_in_content_turned;
RGB* image_in_content_rolledover;
RGB* image_in_content_turnedaround;
RGB* image_in_content_torollover;
RGB* image_in_content_toturnaround;
PBitmap* image_origin;
PBitmap* image_origin_preview;
PBitmap* image_origin_mirror;
PBitmap* image_scale;
PBitmap* image_scale_preview;
PBitmap* image_turn;
PBitmap* image_rollover;
PBitmap* image_turnaround;
char save_filename[] = "save-w5.jpeg";
char delete_filename[] = "delete-w3.jpeg";
char cut_filename[] = "cut-w3.jpeg";
char pen_filename[] = "pen-w3.jpeg";
char rubber_filename[] = "rubber-w3.jpeg";
char red_filename[] = "red-circle-w6.jpeg";
char green_filename[] = "green-w2.jpeg";
char blue_filename[] = "blue-w2.jpeg";
char purple_filename[] = "purple-w2.jpeg";
char rotate_left_90_filename[] = "90-left-w3.jpeg";
char rotate_left_30_filename[] = "30-left-w3.jpeg";
char rotate_right_30_filename[] = "30-right-w3.jpeg";
char rotate_right_90_filename[] = "90-right-w4.jpeg";
char zoomin_filename[] = "zoomin-w3.jpeg";
char zoomout_filename[] = "zoomout-w2.jpeg";
char image_list_up_filename[] = "up-w3.jpeg";
char image_list_down_filename[] = "down-w3.jpeg";
char rollover_filename[] = "rollover-w11.jpeg";
char turnaround_filename[] = "turnaround-w5.jpeg";
char cut_confirm_filename[] = "ok-w3.jpeg";
char cut_cancel_filename[] = "no-w3.jpeg";
char brightness_up_filename[] = "brightness_up_w4.jpeg";
char brightness_down_filename[] = "brightness_down_w4.jpeg";
char filename[] = "testtest.bmp";

RGB pencil_color={0,0,255};
RGB cut_box_color={9,9,10};
Size edit_img_size={410,500};
Point edit_img_pos={140,30};
Size image_list_size={465,140};
Point image_list_pos={0,80};
Size image_title_size={18,200};
Point image_title_pos={140,6};
Size content_size={410,500};
Size cutbox_size={410,500};
Point content_pos={140,30};
Point cutbox_pos={140,30};
Point lastMousePos = {0, 0};
Point mousePos = {0, 0};
const int pointSize = 5;
const int cut_box_dash_width = 6;
const int cut_box_dash_corner_width = 10;
const int cut_box_dash_height = 2;
const int cut_box_dash_corner_height = 5;
const int cut_box_button_width = 20;
const int cut_box_button_height = 20;
int mouse_down = 0;
int is_pencil = 0;
int is_rubber = 0;
int is_cut = 0;
int is_cut_first_move = 0;
int is_file = 0;
int has_content = 0;
int image_item = 0;
int gif_frame = 1;
int current_gif_img_num = 0;
int color_pick = -1;
int has_img_content = 0;
Image* current_gif_img;
float scale_degree = 1;
float brightness_degree=1;
float turn_degree = 0;

ImageList *image_list;
Image** image_show;

int border1 = 1;
int border2 = 2;
RGB borderColor = (RGB){50, 50, 50};
int normal_shift = 0;
int hover_shift = -30;
int pressed_shift = -50;

int max(int a, int b, int c) {
    if(a>b && a>c)
    {
        return a;
    }
    else if(b>c && b>a)
    {
        return b;
    }
    else if(c>b && c>a)
    {
        return c;
    }
    else
    {
        return a;
    }
}

int min(int a, int b, int c) {
    if(a<b && a<c)
    {
        return a;
    }
    else if(b<c && b<a)
    {
        return b;
    }
    else if(c<b && c<a)
    {
        return c;
    }
    else
    {
        return a;
    }
}

void ImageListInit(ImageList *image_list)
{
	image_list->head=0;
	image_list->tail=0;
}

void ImageListAppend(char *filename, int size, int filename_len, ImageList *image_list, char* type, int type_len)
{
    int len = 0;
    char* image_type_tem = (char*)malloc(sizeof(char)*(type_len+1));
    for (int i = 0; i < type_len; i++)
    {
        if(type[i]!=' ')
        {
            image_type_tem[i]=type[i];
            len++;
        }
    }
    image_type_tem[len]='\0';
    if(strcmp(image_type_tem, "bmp")!=0 && strcmp(image_type_tem, "png")!=0 && strcmp(image_type_tem, "jpeg")!=0 && strcmp(image_type_tem, "gif")!=0)
    {
        return;
    }
    else
    {
        free(image_type_tem);
        Image *append_image=(Image*)malloc(sizeof(Image));
        append_image->image_name=(char*)malloc(sizeof(char)*filename_len);
        len=0;
        for (int i = 0; i < filename_len; i++)
        {
            if(filename[i]!=' ')
            {
                append_image->image_name[i]=filename[i];
                len++;
            }
        }
        append_image->image_name[len]='\0';
        len = 0;
        append_image->image_type=(char*)malloc(sizeof(char)*(type_len+1));
        for (int i = 0; i < type_len; i++)
        {
            if(type[i]!=' ')
            {
                append_image->image_type[i]=type[i];
                len++;
            }
        }
        append_image->image_type[len]='\0';
        append_image->image_size=size;
        printf(1, "Image: name: %s, type: %s, size: %d\n", append_image->image_name, append_image->image_type, append_image->image_size);
        int h,w;
        if(strcmp(append_image->image_type, "bmp")==0)
        {
            append_image->data=(RGB*)malloc(sizeof(RGB)*append_image->image_size);
            read24BitmapFile(append_image->image_name, append_image->data, &h, &w);
        }
        else if(strcmp(append_image->image_type, "gif")==0)
        {
            GIF gif = read_gif(append_image->image_name);
            append_image->data=(RGB*)malloc(sizeof(RGB)*gif.height*gif.width*gif.frame_num);
            memmove(append_image->data, gif.data, gif.height*gif.width*gif.frame_num*3);
            h=gif.height*gif.frame_num;
            w=gif.width;
            append_image->gif_img_num = gif.frame_num;
        }
        else if(strcmp(append_image->image_type, "jpeg")==0 || strcmp(append_image->image_type, "png")==0)
        {
            PBitmap img_png_or_jpeg = LoadImg(append_image->image_name);
            append_image->data=(RGB*)malloc(sizeof(RGB)*img_png_or_jpeg.width*img_png_or_jpeg.height);
            memmove(append_image->data, img_png_or_jpeg.data, img_png_or_jpeg.width*img_png_or_jpeg.height*3);
            h=img_png_or_jpeg.height;
            w=img_png_or_jpeg.width;
        }
        append_image->h=h;
        append_image->w=w;
        append_image->is_onshow = 0;
        len=0;
        append_image->scale_needed=0;
        if(strcmp(append_image->image_type, "gif")!=0)
        {
            append_image->gif_img_num = 1;
        }
        int is_large = 0;
        PBitmap* large_image_origin;
        PBitmap* image_origin_scale;
        if(strcmp(append_image->image_type, "gif")!=0)
        {
            if(h>410 || w>500)
            {
                float scale_tem_origin=1;
                if(h>=w)
                {
                    scale_tem_origin=(float)410/h;
                }
                else
                {
                    scale_tem_origin=(float)500/w;
                }
                append_image->h=append_image->h*scale_tem_origin;
                append_image->w=append_image->w*scale_tem_origin;
                image_origin_scale->data=(RGB*)malloc(sizeof(RGB)*append_image->h*append_image->w);
                image_origin_scale->height=append_image->h;
                image_origin_scale->width=append_image->w;
                large_image_origin->height=h;
                large_image_origin->width=w;
                large_image_origin->data=append_image->data;
                picScale(large_image_origin, image_origin_scale);
                // free(append_image->data);
                // append_image->data = image_origin_scale->data;
                is_large = 1;
            }
        }
        if(strcmp(append_image->image_type, "gif")==0)
        {
            image_origin_preview->height=h/append_image->gif_img_num;
            image_origin_preview->width=w;
            append_image->gif_preview = (RGB*)malloc(sizeof(RGB)*image_origin_preview->height*image_origin_preview->width);
            struct RGB *t;
            struct RGB *o;
            int max_line = image_origin_preview->height;
            for (int i = 0; i < image_origin_preview->height; i++) {
                o = append_image->data + i * image_origin_preview->height;
                t = append_image->gif_preview + i * image_origin_preview->height;
                memmove(t, o, max_line * 3);
            }
            image_origin_preview->data=append_image->gif_preview;
            if(h>80 || w>80)
            {
                append_image->scale_needed=1;
                float scale_tem=1;
                if(image_origin_preview->height>=image_origin_preview->width)
                {
                    scale_tem=(float)80/image_origin_preview->height;
                }
                else
                {
                    scale_tem=(float)80/image_origin_preview->width;
                }
                image_scale_preview->height=image_origin_preview->height*scale_tem;
                image_scale_preview->width=image_origin_preview->width*scale_tem;
                append_image->scale_data=(RGB*)malloc(sizeof(RGB)*image_scale_preview->height*image_scale_preview->width);
                image_scale_preview->data=append_image->scale_data;
                picScale(image_origin_preview, image_scale_preview);
                append_image->scale_h=image_scale_preview->height;
                append_image->scale_w=image_scale_preview->width;
            }
        }
        else
        {
            image_origin_preview->height=h;
            image_origin_preview->width=w;
            image_origin_preview->data=append_image->data;
            if(h>80 || w>80)
            {
                append_image->scale_needed=1;
                float scale_tem=1;
                if(h>=w)
                {
                    scale_tem=(float)80/h;
                }
                else
                {
                    scale_tem=(float)80/w;
                }
                image_scale_preview->height=image_origin_preview->height*scale_tem;
                image_scale_preview->width=image_origin_preview->width*scale_tem;
                append_image->scale_data=(RGB*)malloc(sizeof(RGB)*image_scale_preview->height*image_scale_preview->width);
                image_scale_preview->data=append_image->scale_data;
                picScale(image_origin_preview, image_scale_preview);
                append_image->scale_h=image_scale_preview->height;
                append_image->scale_w=image_scale_preview->width;
            }
        }
        if(is_large==1)
        {
            append_image->data = image_origin_scale->data;
        }
        append_image->save_time=0;
        if(image_list->tail==0)
        {
            image_list->head=append_image;
            image_list->tail=append_image;
            image_list->head->next=0;
            image_list->head->prev=0;
        }
        else{
            Image* ptr=image_list->tail;
            image_list->tail->next=append_image;
            image_list->tail=append_image;
            image_list->tail->prev=ptr;
            image_list->tail->next=0;
        }
    }
}

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;
  
  // Find first character after last slash.
  for(p=path+strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  
  // Return blank-padded name.
  if(strlen(p) >= DIRSIZ)
    return p;
  memmove(buf, p, strlen(p));
  memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
  return buf;
}

void
ls_new(char *path)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  
  if((fd = open(path, 0)) < 0){
    printf(2, "ls: cannot open %s\n", path);
    return;
  }
  
  if(fstat(fd, &st) < 0){
    printf(2, "ls: cannot stat %s\n", path);
    close(fd);
    return;
  }
  
  switch(st.type){
  case T_FILE:
    printf(1, "%s %d %d %d %s\n", fmtname(path), st.type, st.ino, st.size);
    break;
  
  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      printf(1, "ls: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf+strlen(buf);
    *p++ = '/';
    printf(1, "Image loading...\n");
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf(1, "ls: cannot stat %s\n", buf);
        continue;
      }
      is_file = 0;
      char *filetype;
      char *filename;
      filename=fmtname(buf);
      for(filetype=filename+strlen(filename); filetype >= filename && *filetype != '.'; filetype--)
          ;
      if(*filetype != '.' || filename[0]=='.') is_file=0;
      else if(*filetype == '.') is_file=1;
      if(is_file == 1)
      {
          filetype++;
          char tem[DIRSIZ+1];
          memmove(tem, filetype, strlen(filetype));
          if(filename[strlen(filename)-strlen(filetype)-2]=='t' && filename[strlen(filename)-strlen(filetype)-3]=='_')
          {
              ImageListAppend(filename, st.size, strlen(filename), image_list, tem, strlen(tem));
          }
      }
    }
    break;
  }
  close(fd);
}

int isMouseInCutBoxLeft(int x, int y) {
   if (cutbox_pos.x-cut_box_dash_corner_height <= x && x <= cutbox_pos.x 
        && cutbox_pos.y + cut_box_dash_corner_width <= y && y <= cutbox_pos.y + cutbox_size.h - cut_box_dash_corner_width ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxUpLeft(int x, int y) {
   if ( ( cutbox_pos.x-cut_box_dash_corner_height <= x && x <= cutbox_pos.x 
            && cutbox_pos.y - cut_box_dash_corner_height <= y && y < cutbox_pos.y + cut_box_dash_corner_width ) 
        || ( cutbox_pos.x-cut_box_dash_corner_height <= x && x < cutbox_pos.x + cut_box_dash_corner_width
            && cutbox_pos.y - cut_box_dash_corner_height <= y && y <= cutbox_pos.y ) ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxDownLeft(int x, int y) {
   if ( ( cutbox_pos.x-cut_box_dash_corner_height <= x && x <= cutbox_pos.x 
            && cutbox_pos.y + cutbox_size.h - cut_box_dash_corner_width < y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ) 
        || ( cutbox_pos.x-cut_box_dash_corner_height <= x && x < cutbox_pos.x + cut_box_dash_corner_width
            && cutbox_pos.y + cutbox_size.h <= y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ) ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxRight(int x, int y) {
   if (cutbox_pos.x + cutbox_size.w <= x && x < cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height 
        && cutbox_pos.y + cut_box_dash_corner_width <= y && y <= cutbox_pos.y + cutbox_size.h - cut_box_dash_corner_width ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxUpRight(int x, int y) {
   if ( ( cutbox_pos.x + cutbox_size.w -cut_box_dash_corner_width < x && x <= cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height
            && cutbox_pos.y - cut_box_dash_corner_height <= y && y <= cutbox_pos.y ) 
        || ( cutbox_pos.x + cutbox_size.w <= x && x <= cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height
            && cutbox_pos.y - cut_box_dash_corner_height <= y && y <= cutbox_pos.y + cut_box_dash_corner_width ) ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxDownRight(int x, int y) {
   if ( ( cutbox_pos.x + cutbox_size.w -cut_box_dash_corner_width < x && x < cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height
            && cutbox_pos.y + cutbox_size.h <= y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ) 
        || ( cutbox_pos.x + cutbox_size.w <= x && x < cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height
            && cutbox_pos.y + cutbox_size.h - cut_box_dash_corner_width <= y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ) ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxUp(int x, int y) {
   if (cutbox_pos.x + cut_box_dash_corner_width <= x && x <= cutbox_pos.x + cutbox_size.w - cut_box_dash_corner_width
        && cutbox_pos.y - cut_box_dash_corner_height <= y && y <= cutbox_pos.y ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxDown(int x, int y) {
   if (cutbox_pos.x + cut_box_dash_corner_width <= x && x <= cutbox_pos.x + cutbox_size.w - cut_box_dash_corner_width
        && cutbox_pos.y + cutbox_size.h <= y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxConfirmButton(int x, int y) {
   if (cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height <= x && x < cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height + cut_box_button_width
        && cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height - cut_box_button_width <= y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInCutBoxCancelButton(int x, int y) {
   if (cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height + cut_box_button_width <= x && x <= cutbox_pos.x + cutbox_size.w + cut_box_dash_corner_height + 2*cut_box_button_width
        && cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height - cut_box_button_width <= y && y <= cutbox_pos.y + cutbox_size.h + cut_box_dash_corner_height ){
        return 1;
   }
   else {
        return 0;
   }
}

int isCutBoxInContent(int x, int y, int width, int height) {
   if ( content_pos.x <= x && x + width <= content_pos.x + content_size.w
        && content_pos.y <= y && y + height <= content_pos.y + content_size.h ){
        return 1;
   }
   else {
        return 0;
   }
}

int isCutBoxSmallest(int width, int height) {
   if ( width <= 2*cut_box_dash_corner_width || height <= 2*cut_box_dash_corner_height ){
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInContent(int x, int y) {
   if (140 + (500-content_size.w)/2 < x && x < 640-(500-content_size.w)/2 && 30 + (410-content_size.h)/2 < y && y < 440 - (410-content_size.h)/2){
       return 1;
   }
   else {
        return 0;
   }
}

void setColorItem() {
    if (color_pick == -1)
    {
        api_drawImgButton(&wnd, red_icon, (Point){495,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, green_icon, (Point){465,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, blue_icon, (Point){435,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, purple_icon, (Point){405,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_update(&wnd, (Rect){495, 5, 20, 20});
        api_update(&wnd, (Rect){465, 5, 20, 20});
        api_update(&wnd, (Rect){435, 5, 20, 20});
        api_update(&wnd, (Rect){405, 5, 20, 20});
    }
    else if (color_pick == 0)
    {
        api_drawImgButton(&wnd, red_icon, (Point){495,5}, (Size){20,20}, border1, borderColor, normal_shift);
        api_drawImgButton(&wnd, green_icon, (Point){465,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, blue_icon, (Point){435,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, purple_icon, (Point){405,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_update(&wnd, (Rect){495, 5, 20, 20});
        api_update(&wnd, (Rect){465, 5, 20, 20});
        api_update(&wnd, (Rect){435, 5, 20, 20});
        api_update(&wnd, (Rect){405, 5, 20, 20});
    }
    else if (color_pick == 1)
    {
        api_drawImgButton(&wnd, red_icon, (Point){495,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, green_icon, (Point){465,5}, (Size){20,20}, border1, borderColor, normal_shift);
        api_drawImgButton(&wnd, blue_icon, (Point){435,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, purple_icon, (Point){405,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_update(&wnd, (Rect){495, 5, 20, 20});
        api_update(&wnd, (Rect){465, 5, 20, 20});
        api_update(&wnd, (Rect){435, 5, 20, 20});
        api_update(&wnd, (Rect){405, 5, 20, 20});
    }
    else if (color_pick == 2)
    {
        api_drawImgButton(&wnd, red_icon, (Point){495,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, green_icon, (Point){465,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, blue_icon, (Point){435,5}, (Size){20,20}, border1, borderColor, normal_shift);
        api_drawImgButton(&wnd, purple_icon, (Point){405,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_update(&wnd, (Rect){495, 5, 20, 20});
        api_update(&wnd, (Rect){465, 5, 20, 20});
        api_update(&wnd, (Rect){435, 5, 20, 20});
        api_update(&wnd, (Rect){405, 5, 20, 20});
    }
    else if (color_pick == 3)
    {
        api_drawImgButton(&wnd, red_icon, (Point){495,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, green_icon, (Point){465,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, blue_icon, (Point){435,5}, (Size){20,20}, 0, borderColor, normal_shift);
        api_drawImgButton(&wnd, purple_icon, (Point){405,5}, (Size){20,20}, border1, borderColor, normal_shift);
        api_update(&wnd, (Rect){495, 5, 20, 20});
        api_update(&wnd, (Rect){465, 5, 20, 20});
        api_update(&wnd, (Rect){435, 5, 20, 20});
        api_update(&wnd, (Rect){405, 5, 20, 20});
    }
}

int isMouseInPencilColorButton(int x, int y) {
    if (495 <= x && x <= 515 && 5 <= y && y <= 25){
        pencil_color = (RGB){0,0,255};
        color_pick = 0;
        setColorItem();
        return 1;
    }
    else if(465 <= x && x <= 485 && 5 <= y && y <= 25)
    {
        pencil_color = (RGB){0,255,0};
        color_pick = 1;
        setColorItem();
        return 1;
    }
    else if(435 <= x && x <= 455 && 5 <= y && y <= 25)
    {
        pencil_color = (RGB){255,0,0};
        color_pick = 2;
        setColorItem();
        return 1;
    }
    else if(405 <= x && x <= 425 && 5 <= y && y <= 25)
    {
        pencil_color = (RGB){128,0,128};
        color_pick = 3;
        setColorItem();
        return 1;
    }
    else {
        return 0;
    }
}

int isMouseInRubberButton(int x, int y) {
   if (550 < x && x <= 580 && 0 <= y && y <= 30){
        pencil_color = (RGB){255,255,255};
        if(is_rubber == 0) 
        {
            is_rubber = 1;
            is_pencil = 0;
            color_pick = -1;
            api_drawImgButton(&wnd, rubber_icon, (Point){550,0}, (Size){30,30}, border1, borderColor, pressed_shift);
            api_update(&wnd, (Rect){550, 0, 30, 30});
            api_drawImgButton(&wnd, pen_icon, (Point){520,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){520, 0, 30, 30});
            setColorItem();
        }
        else if(is_rubber == 1) 
        {
            is_rubber = 0;
            api_drawImgButton(&wnd, rubber_icon, (Point){550,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){550, 0, 30, 30});
        }
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInPencilButton(int x, int y) {
   if (520 <= x && x <= 550 && 0 <= y && y <= 30){
        pencil_color = (RGB){0,0,255};
        if(is_pencil == 0) 
        {
            is_pencil = 1;
            is_rubber = 0;
            if(color_pick == -1) color_pick = 0;
            api_drawImgButton(&wnd, pen_icon, (Point){520,0}, (Size){30,30}, border1, borderColor, pressed_shift);
            api_update(&wnd, (Rect){520, 0, 30, 30});
            api_drawImgButton(&wnd, rubber_icon, (Point){550,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){550, 0, 30, 30});
            setColorItem();
        }
        else if(is_pencil == 1) 
        {
            color_pick = -1;
            api_drawImgButton(&wnd, pen_icon, (Point){520,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){520, 0, 30, 30});
            setColorItem();
            is_pencil = 0;
        }
        return 1;
   }
   else {
        return 0;
   }
}

void image_brightness_adjust_process(float degree)
{
    if(has_content == 1 && current_gif_img->gif_img_num > 1 && current_gif_img->is_onshow == 1)
    {
        has_content = 0;
        brightness_degree = brightness_degree * degree;
        edit_img = malloc(content_size.h*current_gif_img->gif_img_num*content_size.w*3);
        memmove(edit_img, current_gif_img->data, content_size.h*content_size.w*current_gif_img->gif_img_num*3);
        for(int i=0;i< content_size.h*content_size.w*current_gif_img->gif_img_num;i++)
        {
            edit_img[i].R = (unsigned char)(int)min(edit_img[i].R*brightness_degree, 255, 1000);
            edit_img[i].G = (unsigned char)(int)min(edit_img[i].G*brightness_degree, 255, 1000);
            edit_img[i].B = (unsigned char)(int)min(edit_img[i].B*brightness_degree, 255, 1000);
        }
        current_gif_img->data = edit_img;
        int pos_y=235-(current_gif_img->h/current_gif_img->gif_img_num)/2;
        int pos_x=390-current_gif_img->w/2;
        api_paint24BitmapToContent(&wnd, current_gif_img->data, (Point){pos_x,pos_y}, (Point){0,(current_gif_img->h/current_gif_img->gif_img_num)*(gif_frame-1)},(Size){current_gif_img->h,current_gif_img->w},(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w});
        gif_frame = (gif_frame % current_gif_img->gif_img_num ) + 1;
        api_repaint(&wnd);
        has_content = 1;
    }
    else
    {
        brightness_degree = brightness_degree * degree;
        edit_img = malloc(content_size.h*content_size.w*3);
        if(scale_degree == 1 && turn_degree == 0)
        {
            memmove(edit_img, image_origin->data, content_size.h*content_size.w*3);
        }
        else if(turn_degree != 0)
        {
            memmove(edit_img, image_turn->data, content_size.h*content_size.w*3);
        }
        else if(scale_degree != 1)
        {
            memmove(edit_img, image_scale->data, content_size.h*content_size.w*3);
        }
        for(int i=0;i< content_size.h*content_size.w;i++)
        {
            edit_img[i].R = (unsigned char)(int)min(edit_img[i].R*brightness_degree, 255, 1000);
            edit_img[i].G = (unsigned char)(int)min(edit_img[i].G*brightness_degree, 255, 1000);
            edit_img[i].B = (unsigned char)(int)min(edit_img[i].B*brightness_degree, 255, 1000);
        }
        api_paint24BitmapToContent(&wnd, edit_img, (Point){content_pos.x,content_pos.y}, (Point){0,0},(Size){content_size.h,content_size.w},(Size){content_size.h,content_size.w});
        api_repaint(&wnd);
    }
}

int isMouseInBrightnessUpButton(int x, int y) {
   if (580 < x && x <= 610 && 0 <= y && y <= 30){
        api_drawImgButton(&wnd, brightness_up_icon, (Point){580,0}, (Size){30,30}, border1, borderColor, pressed_shift);
        api_update(&wnd, (Rect){580, 0, 30, 30});
        image_brightness_adjust_process(1.1);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInBrightnessDownButton(int x, int y) {
   if (610 < x && x <= 640 && 0 <= y && y <= 30){
        api_drawImgButton(&wnd, brightness_down_icon, (Point){610,0}, (Size){30,30}, border1, borderColor, pressed_shift);
        api_update(&wnd, (Rect){610, 0, 30, 30});
        image_brightness_adjust_process(0.9);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInSaveButton(int x, int y) {
   if (0 <= x && x <= 30 && 0 <= y && y <= 30){
        api_drawImgButton(&wnd, save_icon, (Point){0, 0}, (Size){30, 30}, border1, borderColor, pressed_shift);
        api_update(&wnd, (Rect){0, 0, 30, 30});
        struct RGB *t;
        struct RGB *o;
        printf(1, "Save begin.\n");
        edit_img = malloc(content_size.h*content_size.w*3);
        int max_line = content_size.w;
        for (int i = 0; i < content_size.h; i++) {
            o = wnd.content + (content_pos.y + i) * wnd.size.w + content_pos.x;
            t = edit_img + i * content_size.w;
            memmove(t, o, max_line * 3);
        }
        image_show[image_item-1]->save_time=image_show[image_item-1]->save_time+1;
        int filename_len = strlen(image_show[image_item-1]->image_name);
        int filetype_len = strlen(image_show[image_item-1]->image_type);
        char* save_filename;
        save_filename=malloc(sizeof(char)*(filename_len+10));
        for(int i=0;i<filename_len-filetype_len-1;i++)
        {
            save_filename[i]=image_show[image_item-1]->image_name[i];
        }
        save_filename[filename_len-filetype_len-1]='-';
        save_filename[filename_len-filetype_len]='v';
        char s[100];
        int k=0;
        int save_time_tem=image_show[image_item-1]->save_time;
        do{
           s[k++]=save_time_tem%10+'0';
        }
        while ((save_time_tem/=10)>0);
        s[k]='.';
        int index = filename_len-filetype_len+1;
        for(int j=k-1;j>=0;j--)
        {
            save_filename[index] = s[j];
            index++;
        }
        save_filename[index]='.';
        index++;
        for(int i=index;i<index+filetype_len;i++)
        {
            save_filename[i]=image_show[image_item-1]->image_type[i-index];
        }
        save_filename[index+filetype_len]='\0';
        write24BitmapFile(save_filename, edit_img, content_size.h, content_size.w);
        ImageListAppend(save_filename, content_size.h*content_size.w*3, strlen(save_filename), image_list, "bmp", 3);
        printf(1, "Save succeed.\n");
        free(edit_img);
        free(save_filename);
        api_repaint(&wnd);
        return 1;
   }
   else {
        return 0;
   }
}

void drawCutBoxWidth(int y_corner, int y_dash, int width) {
    api_drawRect(&wnd, (Point) {cutbox_pos.x, y_corner}, (Size) {cut_box_dash_corner_height, cut_box_dash_corner_width}, cut_box_color);
    api_update(&wnd, (Rect){cutbox_pos.x, y_corner, cut_box_dash_corner_width, cut_box_dash_corner_height});
    int image_width = width - cut_box_dash_corner_width*2 - cut_box_dash_width*2;
    if(image_width > 0)
    {
        int cut_box_width = image_width / cut_box_dash_width;
        for(int i=0;i<cut_box_width;i++)
        {
            if(i%2==0)
            {
                api_drawRect(&wnd, (Point) {cutbox_pos.x+cut_box_dash_corner_width+cut_box_dash_width+i*cut_box_dash_width, y_dash}, (Size) {cut_box_dash_height, cut_box_dash_width}, cut_box_color);
                api_update(&wnd, (Rect){cutbox_pos.x+cut_box_dash_corner_width+cut_box_dash_width+i*cut_box_dash_width, y_dash, cut_box_dash_width, cut_box_dash_height});
            }
        }
        if(cut_box_width%2==0)
        {
            api_drawRect(&wnd, (Point) {cutbox_pos.x +cut_box_dash_corner_width+cut_box_dash_width+ cut_box_width*cut_box_dash_width, y_dash}, (Size) {cut_box_dash_height, image_width - cut_box_width*cut_box_dash_width}, cut_box_color);
            api_update(&wnd, (Rect){cutbox_pos.x+cut_box_dash_corner_width+cut_box_dash_width+cut_box_width*cut_box_dash_width, y_dash, image_width - cut_box_width*cut_box_dash_width, cut_box_dash_height});
        }
    }
    api_drawRect(&wnd, (Point) {cutbox_pos.x+image_width+cut_box_dash_width*2+cut_box_dash_corner_width, y_corner}, (Size) {cut_box_dash_corner_height, cut_box_dash_corner_width}, cut_box_color);
    api_update(&wnd, (Rect){cutbox_pos.x+image_width+cut_box_dash_width*2+cut_box_dash_corner_width, y_corner, cut_box_dash_corner_width, cut_box_dash_corner_height});
}

void drawCutBoxHeight(int x_corner, int x_dash, int height) {
    api_drawRect(&wnd, (Point) {x_corner, cutbox_pos.y}, (Size) {cut_box_dash_corner_width, cut_box_dash_corner_height}, cut_box_color);
    api_update(&wnd, (Rect){x_corner, cutbox_pos.y, cut_box_dash_corner_height, cut_box_dash_corner_width});
    int image_height = height - cut_box_dash_corner_width*2 - cut_box_dash_width*2;
    if(image_height > 0)
    {
        int cut_box_width = image_height / cut_box_dash_width;
        for(int i=0;i<cut_box_width;i++)
        {
            if(i%2==0)
            {
                api_drawRect(&wnd, (Point) {x_dash, cutbox_pos.y+cut_box_dash_corner_width+cut_box_dash_width+i*cut_box_dash_width}, (Size) {cut_box_dash_width, cut_box_dash_height}, cut_box_color);
                api_update(&wnd, (Rect){x_dash, cutbox_pos.y+cut_box_dash_corner_width+cut_box_dash_width+i*cut_box_dash_width, cut_box_dash_height, cut_box_dash_width});
            }
        }
        if(cut_box_width%2==0)
        {
            api_drawRect(&wnd, (Point) {x_dash, cutbox_pos.y+cut_box_dash_corner_width+cut_box_dash_width+cut_box_width*cut_box_dash_width}, (Size) {image_height - cut_box_width*cut_box_dash_width, cut_box_dash_height}, cut_box_color);
            api_update(&wnd, (Rect){x_dash, cutbox_pos.y+cut_box_dash_corner_width+cut_box_dash_width+cut_box_width*cut_box_dash_width, cut_box_dash_height, image_height - cut_box_width*cut_box_dash_width});
        }
    }
    api_drawRect(&wnd, (Point) {x_corner, cutbox_pos.y+image_height+cut_box_dash_width*2+cut_box_dash_corner_width}, (Size) {cut_box_dash_corner_width, cut_box_dash_corner_height}, cut_box_color);
    api_update(&wnd, (Rect){x_corner, cutbox_pos.y+image_height+cut_box_dash_width*2+cut_box_dash_corner_width, cut_box_dash_corner_height, cut_box_dash_corner_width});
}

void drawCutBoxCorner(Point pos) {
    api_drawRect(&wnd, pos, (Size) {cut_box_dash_corner_height, cut_box_dash_corner_height}, cut_box_color);
    api_update(&wnd, (Rect){pos.x, pos.y, cut_box_dash_corner_height, cut_box_dash_corner_height});
}

void drawCutBoxButton(Point pos) {
    api_paint24BitmapToContentTransparent(&wnd, cut_confirm_icon, (Point){pos.x,pos.y}, (Point){0,0}, (Size){cut_box_button_width,cut_box_button_height},(Size){cut_box_button_width,cut_box_button_height});
    api_update(&wnd, (Rect){pos.x, pos.y, cut_box_button_height, cut_box_button_height});
    api_paint24BitmapToContentTransparent(&wnd, cut_cancel_icon, (Point){pos.x+cut_box_button_width,pos.y}, (Point){0,0}, (Size){cut_box_button_width,cut_box_button_height},(Size){cut_box_button_width,cut_box_button_height});
    api_update(&wnd, (Rect){pos.x+cut_box_button_width, pos.y, cut_box_button_height, cut_box_button_height});
}

void drawCutBox(Point pos, int width, int height) {
    api_paint24BitmapToContent(&wnd, cut_img_save, (Point){content_pos.x - cut_box_dash_corner_height,content_pos.y - cut_box_dash_corner_height}, (Point){0,0}, (Size){content_size.h+2*cut_box_dash_corner_height, content_size.w+2*cut_box_dash_corner_height+2*cut_box_button_width},(Size){content_size.h+2*cut_box_dash_corner_height,content_size.w+2*cut_box_dash_corner_height+2*cut_box_button_width});
    drawCutBoxWidth(pos.y - cut_box_dash_corner_height, pos.y - cut_box_dash_height, width);
    drawCutBoxWidth(pos.y + height, pos.y + height, width);
    drawCutBoxHeight(pos.x - cut_box_dash_corner_height, pos.x - cut_box_dash_height, height);
    drawCutBoxHeight(pos.x + width, pos.x + width, height);
    drawCutBoxCorner((Point){pos.x - cut_box_dash_corner_height, pos.y - cut_box_dash_corner_height});
    drawCutBoxCorner((Point){pos.x + width, pos.y - cut_box_dash_corner_height});
    drawCutBoxCorner((Point){pos.x - cut_box_dash_corner_height, pos.y + height});
    drawCutBoxCorner((Point){pos.x + width, pos.y + height});
    drawCutBoxButton((Point){pos.x + width + cut_box_dash_corner_height, pos.y + height + cut_box_dash_corner_height - cut_box_button_height});
}

int isMouseInCutButton(int x, int y) {
    if (60 < x && x <= 90 && 0 <= y && y <= 30){
        if(is_cut == 0) 
        {
            is_cut = 1;
            is_cut_first_move = 0;
            api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, pressed_shift);
            api_update(&wnd, (Rect){60, 0, 30, 30});
            cutbox_pos.x = content_pos.x;
            cutbox_pos.y = content_pos.y;
            cutbox_size.w = content_size.w;
            cutbox_size.h = content_size.h;
            struct RGB *t;
            struct RGB *o;
            cut_img_save = malloc((cutbox_size.w+cut_box_dash_corner_height*2)*(cutbox_size.h+cut_box_dash_corner_height*2+cut_box_button_width*2)*3);
            int max_line = cutbox_size.w+cut_box_dash_corner_height*2+cut_box_button_width*2;
            for (int i = 0; i < cutbox_size.h+cut_box_dash_corner_height*2; i++) {
                o = wnd.content + (cutbox_pos.y - cut_box_dash_corner_height + i) * wnd.size.w + cutbox_pos.x - cut_box_dash_corner_height;
                t = cut_img_save + i * (cutbox_size.w+cut_box_dash_corner_height*2+cut_box_button_width*2);
                memmove(t, o, max_line * 3);
            }
            drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
        }
        else if(is_cut == 1)
        {
            is_cut = 0;
            is_cut_first_move = 0;
            api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){60, 0, 30, 30});
            api_paint24BitmapToContent(&wnd, cut_img_save, (Point){content_pos.x - cut_box_dash_corner_height,content_pos.y - cut_box_dash_corner_height}, (Point){0,0}, (Size){content_size.h+2*cut_box_dash_corner_height, content_size.w+2*cut_box_dash_corner_height+2*cut_box_button_width},(Size){content_size.h+2*cut_box_dash_corner_height,content_size.w+2*cut_box_dash_corner_height+2*cut_box_button_width});
            api_repaint(&wnd);
        }
        return 1;
    }
    else {
        return 0;
    }
}

void setImageList()
{
    RGBA image_name_color = {0,0,0,255};
    struct RGB *t;
    struct RGB *o;
    int max_line = image_list_size.w;
    for (int i = 0; i < image_list_size.h; i++) {
        o = wnd.content + (image_list_pos.y + i) * wnd.size.w + image_list_pos.x;
        t = image_list_origin + i * image_list_size.w;
        memmove(o, t, max_line * 3);
    }
    for(int i=0;i<3;i++)
    {
        if(image_show[i]->scale_needed==1)
        {
            int pos_y=120+130*i-image_show[i]->scale_h/2;
            int pos_x=70-image_show[i]->scale_w/2;
            api_paint24BitmapToContent(&wnd, image_show[i]->scale_data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_show[i]->scale_h,image_show[i]->scale_w},(Size){image_show[i]->scale_h,image_show[i]->scale_w});
        }
        else
        {
            int pos_y=120+130*i-image_show[i]->h/2;
            int pos_x=70-image_show[i]->w/2;
            api_paint24BitmapToContent(&wnd, image_show[i]->data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_show[i]->h,image_show[i]->w},(Size){image_show[i]->h,image_show[i]->w});
        }
        int name_len=strlen(image_show[i]->image_name);
        int offset_x;
        if(name_len>15)
        {
            char* image_name_slug;
            image_name_slug=(char*)malloc(sizeof(char)*16);
            for(int j=0;j<6;j++) 
            {
                image_name_slug[j]=image_show[i]->image_name[j];
                image_name_slug[14-j]=image_show[i]->image_name[name_len-1-j];
            }
            image_name_slug[6]='.';
            image_name_slug[7]='.';
            image_name_slug[8]='.';
            name_len=15;
            image_name_slug[15] = '\0';
            offset_x=(140-name_len*9)/2;
            api_drawString(&wnd, offset_x, 80+130*i+90, image_name_slug, image_name_color);
        }
        else if(name_len<=15 && name_len>=0)
        {
            offset_x=(140-name_len*9)/2;
            api_drawString(&wnd, offset_x, 80+130*i+90, image_show[i]->image_name, image_name_color);
        }
        api_repaint(&wnd);
    }
}

int isMouseInListUpButton(int x, int y) {
   if (47 <= x && x <= 82 && 45 <= y && y <= 65){
        api_drawImgButton(&wnd, image_list_up_icon, (Point){47,45}, (Size){20,35}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){47, 45, 35, 20});
        if(image_show[0]->prev==0) return 1;
        else
        {
            image_show[2]=image_show[1];
            image_show[1]=image_show[0];
            image_show[0]=image_show[0]->prev;
            setImageList();
            api_repaint(&wnd);
        }
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInListDownButton(int x, int y) {
   if (47 <= x && x <= 82 && 465 <= y && y <= 485){
        api_drawImgButton(&wnd, image_list_down_icon, (Point){47,465}, (Size){20,35}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){47, 465, 35, 20});
        if(image_show[2]->next==0) return 1;
        else
        {
            image_show[0]=image_show[1];
            image_show[1]=image_show[2];
            image_show[2]=image_show[2]->next;
            setImageList();
            api_repaint(&wnd);
        }
        return 1;
   }
   else {
        return 0;
   }
}

void showImageInContent()
{
    RGBA image_name_color = {0,0,0,255};
    struct RGB *t;
    struct RGB *o;
    int max_line = edit_img_size.w;
    for (int i = 0; i < edit_img_size.h; i++) {
        o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
        t = edit_img_origin + i * edit_img_size.w;
        memmove(o, t, max_line * 3);
    }
    max_line = image_title_size.w;
    for (int i = 0; i < image_title_size.h; i++) {
        o = wnd.content + (image_title_pos.y + i) * wnd.size.w + image_title_pos.x;
        t = image_title_origin + i * image_title_size.w;
        memmove(o, t, max_line * 3);
    }
    if(image_show[image_item-1]->gif_img_num>1)
    {
        free(current_gif_img);
        int pos_y=235-(image_show[image_item-1]->h/image_show[image_item-1]->gif_img_num)/2;
        int pos_x=390-image_show[image_item-1]->w/2;
        api_paint24BitmapToContent(&wnd, image_show[image_item-1]->data, (Point){pos_x,pos_y}, (Point){0,(image_show[image_item-1]->h/image_show[image_item-1]->gif_img_num)*(gif_frame-1)},(Size){image_show[image_item-1]->h,image_show[image_item-1]->w},(Size){(image_show[image_item-1]->h/image_show[image_item-1]->gif_img_num),image_show[image_item-1]->w});
        api_drawString(&wnd, 140, 6, image_show[image_item-1]->image_name, image_name_color);
        content_size=(Size){(image_show[image_item-1]->h/image_show[image_item-1]->gif_img_num),image_show[image_item-1]->w};
        content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
        gif_frame = (gif_frame % image_show[image_item-1]->gif_img_num ) + 1;
        current_gif_img = (Image*)malloc(sizeof(Image));
        current_gif_img->data = image_show[image_item-1]->data;
        current_gif_img->h = image_show[image_item-1]->h;
        current_gif_img->w = image_show[image_item-1]->w;
        current_gif_img->gif_img_num = image_show[image_item-1]->gif_img_num;
        current_gif_img->is_onshow = 1;
        has_content = 1;
        scale_degree=1;
        brightness_degree=1;
        turn_degree=0;
        image_origin->data=image_show[image_item-1]->data;
        image_origin->height=image_show[image_item-1]->h;
        image_origin->width=image_show[image_item-1]->w;
        image_origin_mirror->height=image_show[image_item-1]->h;
        image_origin_mirror->width=image_show[image_item-1]->w;
        image_in_content_torollover=(RGB*)malloc(sizeof(RGB)*image_origin->height*image_origin->width);
        memmove(image_in_content_torollover, image_show[image_item-1]->data, image_origin->height*image_origin->width*3);
        image_in_content_toturnaround=(RGB*)malloc(sizeof(RGB)*image_origin->height*image_origin->width);
        memmove(image_in_content_toturnaround, image_show[image_item-1]->data, image_origin->height*image_origin->width*3);
    }
    else
    {
        int pos_y=235-image_show[image_item-1]->h/2;
        int pos_x=390-image_show[image_item-1]->w/2;
        api_paint24BitmapToContent(&wnd, image_show[image_item-1]->data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_show[image_item-1]->h,image_show[image_item-1]->w},(Size){image_show[image_item-1]->h,image_show[image_item-1]->w});
        api_drawString(&wnd, 140, 6, image_show[image_item-1]->image_name, image_name_color);
        content_size=(Size){image_show[image_item-1]->h,image_show[image_item-1]->w};
        content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
        scale_degree=1;
        brightness_degree=1;
        turn_degree=0;
        image_origin->data=image_show[image_item-1]->data;
        image_origin->height=content_size.h;
        image_origin->width=content_size.w;
        image_origin_mirror->height=content_size.h;
        image_origin_mirror->width=content_size.w;
        image_in_content_torollover=(RGB*)malloc(sizeof(RGB)*image_origin->height*image_origin->width);
        memmove(image_in_content_torollover, image_show[image_item-1]->data, image_origin->height*image_origin->width*3);
        image_in_content_toturnaround=(RGB*)malloc(sizeof(RGB)*image_origin->height*image_origin->width);
        memmove(image_in_content_toturnaround, image_show[image_item-1]->data, image_origin->height*image_origin->width*3);
    }
    current_gif_img_num = image_show[image_item-1]->gif_img_num;
    api_repaint(&wnd);
}

int isMouseInListItem(int x, int y) {
   if (30 <= x && x <= 110 && 80 <= y && y <= 188){
        has_content = 0;
        has_img_content = 1;
        is_cut = 0;
        api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
        api_update(&wnd, (Rect){60, 0, 30, 30});
        image_item = 1;
        showImageInContent();
        return 1;
   }
   else if (30 <= x && x <= 110 && 210 <= y && y <= 318)
   {
        has_content = 0;
        has_img_content = 1;
        is_cut = 0;
        api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
        api_update(&wnd, (Rect){60, 0, 30, 30});
        image_item = 2;
        showImageInContent();
        return 1;
   }
   else if (30 <= x && x <= 110 && 340 <= y && y <= 448)
   {
        has_content = 0;
        has_img_content = 1;
        is_cut = 0;
        api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
        api_update(&wnd, (Rect){60, 0, 30, 30});
        image_item = 3;
        showImageInContent();
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInDeleteButton(int x, int y) {
   if (30 < x && x <= 60 && 0 <= y && y <= 30){
        api_drawImgButton(&wnd, delete_icon, (Point){30,0},(Size){30,30}, border1, borderColor, pressed_shift);
        api_update(&wnd, (Rect){30, 0, 30, 30});
        struct RGB *t;
        struct RGB *o;
        int max_line = edit_img_size.w;
        for (int i = 0; i < edit_img_size.h; i++) {
            o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
            t = edit_img_origin + i * edit_img_size.w;
            memmove(o, t, max_line * 3);
        }
        max_line = image_title_size.w;
        for (int i = 0; i < image_title_size.h; i++) {
            o = wnd.content + (image_title_pos.y + i) * wnd.size.w + image_title_pos.x;
            t = image_title_origin + i * image_title_size.w;
            memmove(o, t, max_line * 3);
        }
        image_item = 0;
        content_size=(Size){410,380};
        content_pos=(Point){140,30};
        api_repaint(&wnd);
        has_content = 0;
        current_gif_img->is_onshow = 0;
        has_img_content = 0;
        return 1;
   }
   else {
        return 0;
   }
}

void image_scale_process(float zoom_degree)
{
    if(has_content == 1 && current_gif_img->gif_img_num > 1 && current_gif_img->is_onshow == 1)
    {
        if((int)(image_origin->height/current_gif_img->gif_img_num)*scale_degree*zoom_degree >= edit_img_size.h || (int)image_origin->width*scale_degree*zoom_degree >= edit_img_size.w)
        {
            return;
        }
        scale_degree=scale_degree*zoom_degree;
        has_content = 0;
        image_scale->height=image_origin->height*scale_degree;
        image_scale->width=image_origin->width*scale_degree;
        image_in_content_scaled=(RGB*)malloc(sizeof(RGB)*image_scale->height*image_scale->width);
        image_scale->data=image_in_content_scaled;
        struct RGB *t;
        struct RGB *o;
        int max_line = edit_img_size.w;
        for (int i = 0; i < edit_img_size.h; i++) {
            o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
            t = edit_img_origin + i * edit_img_size.w;
            memmove(o, t, max_line * 3);
        }
        picScale(image_origin, image_scale);
        current_gif_img->w = image_scale->width;
        current_gif_img->h = image_scale->height;
        current_gif_img->data = image_scale->data;
        int pos_y=235-(current_gif_img->h/current_gif_img->gif_img_num)/2;
        int pos_x=390-current_gif_img->w/2;
        api_paint24BitmapToContent(&wnd, current_gif_img->data, (Point){pos_x,pos_y}, (Point){0,(current_gif_img->h/current_gif_img->gif_img_num)*(gif_frame-1)},(Size){current_gif_img->h,current_gif_img->w},(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w});
        gif_frame = (gif_frame % current_gif_img->gif_img_num ) + 1;
        content_size=(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w};
        content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
        free(image_in_content_scaled);
        api_repaint(&wnd);
        has_content = 1;
    }
    else
    {
        if((int)image_origin->height*scale_degree*zoom_degree > edit_img_size.h || (int)image_origin->width*scale_degree*zoom_degree > edit_img_size.w)
        {
            return;
        }
        scale_degree=scale_degree*zoom_degree;
        image_scale->height=image_origin->height*scale_degree;
        image_scale->width=image_origin->width*scale_degree;
        image_in_content_scaled=(RGB*)malloc(sizeof(RGB)*image_scale->height*image_scale->width);
        image_scale->data=image_in_content_scaled;
        struct RGB *t;
        struct RGB *o;
        int max_line = edit_img_size.w;
        for (int i = 0; i < edit_img_size.h; i++) {
            o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
            t = edit_img_origin + i * edit_img_size.w;
            memmove(o, t, max_line * 3);
        }
        picScale(image_origin, image_scale);
        int pos_y=235-image_scale->height/2;
        int pos_x=390-image_scale->width/2;
        api_paint24BitmapToContent(&wnd, image_scale->data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_scale->height,image_scale->width},(Size){image_scale->height,image_scale->width});
        content_size=(Size){image_scale->height,image_scale->width};
        content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
        free(image_in_content_scaled);
        api_repaint(&wnd);
    }
}

int isMouseInZoominButton(int x, int y) {
   if (142 < x && x <= 200 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, zoomin_icon, (Point){142, 440}, (Size){60, 60}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){142, 440, 60, 60});
        image_scale_process(2);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInZoomoutButton(int x, int y) {
   if (202 < x && x <= 260 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, zoomout_icon, (Point){202,440}, (Size){60,60}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){202, 440, 60, 60});
        image_scale_process(0.5);
        return 1;
   }
   else {
        return 0;
   }
}

void image_turn_process(float rotate_degree)
{
    if((int)image_origin->height > edit_img_size.w || (int)image_origin->width > edit_img_size.h)
    {
        return;
    }
    turn_degree=turn_degree+rotate_degree;
    image_turn->height=image_origin->height;
    image_turn->width=image_origin->width;
    getTurnSize(&(image_turn->width), &(image_turn->height), turn_degree);
    image_in_content_turned=(RGB*)malloc(sizeof(RGB)*image_turn->height*image_turn->width);
    image_turn->data=image_in_content_turned;
    struct RGB *t;
    struct RGB *o;
    int max_line = edit_img_size.w;
    for (int i = 0; i < edit_img_size.h; i++) {
        o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
        t = edit_img_origin + i * edit_img_size.w;
        memmove(o, t, max_line * 3);
    }
    picTurn(image_origin, image_turn, turn_degree);
    int pos_y=235-image_turn->height/2;
    int pos_x=390-image_turn->width/2;
    api_paint24BitmapToContent(&wnd, image_turn->data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_turn->height,image_turn->width},(Size){image_turn->height,image_turn->width});
    content_size=(Size){image_turn->height,image_turn->width};
    content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
    free(image_in_content_turned);
    api_repaint(&wnd);
}

int isMouseInRotateLeftNinetyButton(int x, int y) {
   if (262 < x && x <= 325 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, rotate_left_90_icon, (Point){262,440}, (Size){60,63}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){262, 440, 63, 60});
        image_turn_process(-PI/2);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInRotateLeftThirtyButton(int x, int y) {
   if (325 < x && x <= 390 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, rotate_left_30_icon, (Point){325,440}, (Size){60,65}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){325, 440, 65, 60});
        image_turn_process(-PI/6);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInRotateRightThirtyButton(int x, int y) {
   if (390 < x && x <= 455 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, rotate_right_30_icon, (Point){390,440}, (Size){60,65}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){390, 440, 65, 60});
        image_turn_process(PI/6);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInRotateRightNinetyButton(int x, int y) {
   if (455 < x && x <= 520 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, rotate_right_90_icon, (Point){455,440}, (Size){60,65}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){455, 440, 65, 60});
        image_turn_process(PI/2);
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInRolloverButton(int x, int y) {
   if (520 < x && x <= 580 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, rollover_icon, (Point){520,440}, (Size){60,60}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){520, 440, 65, 60});
        if(has_content == 1 && current_gif_img->gif_img_num > 1 && current_gif_img->is_onshow == 1)
        {
            has_content = 0;
            image_origin_mirror->data=image_in_content_torollover;
            image_rollover->height=image_origin_mirror->height;
            image_rollover->width=image_origin_mirror->width;
            image_in_content_rolledover=(RGB*)malloc(sizeof(RGB)*image_rollover->height*image_rollover->width);
            image_rollover->data=image_in_content_rolledover;
            struct RGB *t;
            struct RGB *o;
            int max_line = edit_img_size.w;
            for (int i = 0; i < edit_img_size.h; i++) {
                o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
                t = edit_img_origin + i * edit_img_size.w;
                memmove(o, t, max_line * 3);
            }
            picRollingOver(image_origin_mirror, image_rollover);
            current_gif_img->w = image_rollover->width;
            current_gif_img->h = image_rollover->height;
            current_gif_img->data = image_rollover->data;
            int pos_y=235-(current_gif_img->h/current_gif_img->gif_img_num)/2;
            int pos_x=390-current_gif_img->w/2;
            api_paint24BitmapToContent(&wnd, current_gif_img->data, (Point){pos_x,pos_y}, (Point){0,(current_gif_img->h/current_gif_img->gif_img_num)*(gif_frame-1)},(Size){current_gif_img->h,current_gif_img->w},(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w});
            gif_frame = (gif_frame % current_gif_img->gif_img_num ) + 1;
            content_size=(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w};
            content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
            free(image_in_content_torollover);
            image_in_content_torollover=(RGB*)malloc(sizeof(RGB)*image_rollover->height*image_rollover->width);
            memmove(image_in_content_torollover, image_in_content_rolledover, image_rollover->height*image_rollover->width*3);
            free(image_in_content_rolledover);
            api_repaint(&wnd);
            has_content = 1;
        }
        else
        {
            image_origin_mirror->data=image_in_content_torollover;
            image_rollover->height=image_origin_mirror->height;
            image_rollover->width=image_origin_mirror->width;
            image_in_content_rolledover=(RGB*)malloc(sizeof(RGB)*image_rollover->height*image_rollover->width);
            image_rollover->data=image_in_content_rolledover;
            struct RGB *t;
            struct RGB *o;
            int max_line = edit_img_size.w;
            for (int i = 0; i < edit_img_size.h; i++) {
                o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
                t = edit_img_origin + i * edit_img_size.w;
                memmove(o, t, max_line * 3);
            }
            picRollingOver(image_origin_mirror, image_rollover);
            int pos_y=235-image_rollover->height/2;
            int pos_x=390-image_rollover->width/2;
            api_paint24BitmapToContent(&wnd, image_rollover->data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_rollover->height,image_rollover->width},(Size){image_rollover->height,image_rollover->width});
            content_size=(Size){image_rollover->height,image_rollover->width};
            content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
            free(image_in_content_torollover);
            image_in_content_torollover=(RGB*)malloc(sizeof(RGB)*image_rollover->height*image_rollover->width);
            memmove(image_in_content_torollover, image_in_content_rolledover, image_rollover->height*image_rollover->width*3);
            free(image_in_content_rolledover);
            api_repaint(&wnd);
        }
        return 1;
   }
   else {
        return 0;
   }
}

int isMouseInTurnaroundButton(int x, int y) {
   if (580 < x && x <= 640 && 440 <= y && y <= 500){
        api_drawImgButton(&wnd, turnaround_icon, (Point){580,440}, (Size){60,60}, border2, borderColor, pressed_shift);
        api_update(&wnd, (Rect){580, 440, 65, 60});
        if(has_content == 1 && current_gif_img->gif_img_num > 1 && current_gif_img->is_onshow == 1)
        {
            has_content = 0;
            image_origin_mirror->data=image_in_content_toturnaround;
            image_turnaround->height=image_origin_mirror->height;
            image_turnaround->width=image_origin_mirror->width;
            image_in_content_turnedaround=(RGB*)malloc(sizeof(RGB)*image_turnaround->height*image_turnaround->width);
            image_turnaround->data=image_in_content_turnedaround;
            struct RGB *t;
            struct RGB *o;
            int max_line = edit_img_size.w;
            for (int i = 0; i < edit_img_size.h; i++) {
                o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
                t = edit_img_origin + i * edit_img_size.w;
                memmove(o, t, max_line * 3);
            }
            picTurnAround(image_origin_mirror, image_turnaround);
            current_gif_img->w = image_turnaround->width;
            current_gif_img->h = image_turnaround->height;
            current_gif_img->data = image_turnaround->data;
            int pos_y=235-(current_gif_img->h/current_gif_img->gif_img_num)/2;
            int pos_x=390-current_gif_img->w/2;
            api_paint24BitmapToContent(&wnd, current_gif_img->data, (Point){pos_x,pos_y}, (Point){0,(current_gif_img->h/current_gif_img->gif_img_num)*(gif_frame-1)},(Size){current_gif_img->h,current_gif_img->w},(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w});
            gif_frame = (gif_frame % current_gif_img->gif_img_num ) + 1;
            content_size=(Size){(current_gif_img->h/current_gif_img->gif_img_num),current_gif_img->w};
            content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
            free(image_in_content_toturnaround);
            image_in_content_toturnaround=(RGB*)malloc(sizeof(RGB)*image_turnaround->height*image_turnaround->width);
            memmove(image_in_content_toturnaround, image_in_content_turnedaround, image_turnaround->height*image_turnaround->width*3);
            free(image_in_content_turnedaround);
            api_repaint(&wnd);
            has_content = 1;
        }
        else
        {
            image_origin_mirror->data=image_in_content_toturnaround;
            image_turnaround->height=image_origin_mirror->height;
            image_turnaround->width=image_origin_mirror->width;
            image_in_content_turnedaround=(RGB*)malloc(sizeof(RGB)*image_turnaround->height*image_turnaround->width);
            image_turnaround->data=image_in_content_turnedaround;
            struct RGB *t;
            struct RGB *o;
            int max_line = edit_img_size.w;
            for (int i = 0; i < edit_img_size.h; i++) {
                o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
                t = edit_img_origin + i * edit_img_size.w;
                memmove(o, t, max_line * 3);
            }
            picTurnAround(image_origin_mirror, image_turnaround);
            int pos_y=235-image_turnaround->height/2;
            int pos_x=390-image_turnaround->width/2;
            api_paint24BitmapToContent(&wnd, image_turnaround->data, (Point){pos_x,pos_y}, (Point){0,0}, (Size){image_turnaround->height,image_turnaround->width},(Size){image_turnaround->height,image_turnaround->width});
            content_size=(Size){image_turnaround->height,image_turnaround->width};
            content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
            free(image_in_content_toturnaround);
            image_in_content_toturnaround=(RGB*)malloc(sizeof(RGB)*image_turnaround->height*image_turnaround->width);
            memmove(image_in_content_toturnaround, image_in_content_turnedaround, image_turnaround->height*image_turnaround->width*3);
            free(image_in_content_turnedaround);
            api_repaint(&wnd);
        }
        return 1;
   }
   else {
        return 0;
   }
}

void MsgProc(struct message * msg)
{
    switch (msg->msg_type)
    {
    case M_MOUSE_DOWN:
        if( has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInPencilColorButton(msg->params[0], msg->params[1]))
        {
            break;
        }
        if( has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInPencilButton(msg->params[0], msg->params[1]))
        {
            break;
        }
        if( has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInRubberButton(msg->params[0], msg->params[1]))
        {
            break;
        }
        if( has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInSaveButton(msg->params[0], msg->params[1]))
        {
            api_drawImgButton(&wnd, save_icon, (Point){0, 0}, (Size){30, 30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){0, 0, 30, 30});
            break;
        }
        if(has_img_content == 1 && isMouseInBrightnessUpButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, brightness_up_icon, (Point){580,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){580, 0, 30, 30});
            break;
        }
        if(has_img_content == 1 && isMouseInBrightnessDownButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, brightness_down_icon, (Point){610,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){610, 0, 30, 30});
            break;
        }
        if(has_img_content == 1 && isMouseInDeleteButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, delete_icon, (Point){30,0},(Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){30, 0, 30, 30});
            break;
        }
        if(isMouseInListUpButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, image_list_up_icon, (Point){47,45}, (Size){20,35}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){47, 45, 35, 20});
            break;
        }
        if(isMouseInListDownButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, image_list_down_icon, (Point){47,465}, (Size){20,35}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){47, 465, 35, 20});
            break;
        }
        if(isMouseInListItem(msg->params[0], msg->params[1]))
        {
            break;
        }
        if(has_img_content == 1 && isMouseInZoominButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, zoomin_icon, (Point){142, 440}, (Size){60, 60}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){142, 440, 60, 60});
            break;
        }
        if(has_img_content == 1 && isMouseInZoomoutButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, zoomout_icon, (Point){202,440}, (Size){60,60}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){202, 440, 60, 60});
            break;
        }
        if(has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInRotateLeftNinetyButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, rotate_left_90_icon, (Point){262,440}, (Size){60,63}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){262, 440, 63, 60});
            break;
        }
        if(has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInRotateLeftThirtyButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, rotate_left_30_icon, (Point){325,440}, (Size){60,65}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){325, 440, 65, 60});
            break;
        }
        if(has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInRotateRightNinetyButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, rotate_right_90_icon, (Point){455,440}, (Size){60,65}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){455, 440, 65, 60});
            break;
        }
        if(has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInRotateRightThirtyButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, rotate_right_30_icon, (Point){390,440}, (Size){60,65}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){390, 440, 65, 60});
            break;
        }
        if(has_img_content == 1 && isMouseInRolloverButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, rollover_icon, (Point){520,440}, (Size){60,60}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){520, 440, 65, 60});
            break;
        }
        if(has_img_content == 1 && isMouseInTurnaroundButton(msg->params[0], msg->params[1]))
        {
            sleep(3);
            api_drawImgButton(&wnd, turnaround_icon, (Point){580,440}, (Size){60,60}, border2, borderColor, normal_shift);
            api_update(&wnd, (Rect){580, 440, 65, 60});
            break;
        }
        if(has_img_content == 1 && (has_content != 1 || current_gif_img->gif_img_num == 1 || current_gif_img->is_onshow != 1) && isMouseInCutButton(msg->params[0], msg->params[1]))
        {
            break;
        }
        if(mouse_down == 0 && (is_pencil==1 ||  is_rubber == 1) && isMouseInContent(msg->params[0], msg->params[1]))
        {
            mouse_down = 1;
            api_drawRect(&wnd, (Point) {msg->params[0] - pointSize / 2, msg->params[1] - pointSize / 2},
                                 (Size) {pointSize, pointSize}, pencil_color);
            api_update(&wnd, (Rect){msg->params[0] - pointSize / 2, msg->params[1] - pointSize /2, pointSize, pointSize});
            is_pencil=1;
        }
        if(mouse_down == 0 && is_cut==1)
        {
            mouse_down = 1;
            mousePos.x = msg->params[0];
            mousePos.y = msg->params[1];
        }
        if(has_img_content == 1 && isMouseInCutBoxConfirmButton(msg->params[0], msg->params[1]))
        {
            struct RGB *t;
            struct RGB *o;
            cut_img_result = malloc(cutbox_size.w*cutbox_size.h*3);
            int max_line = cutbox_size.w;
            for (int i = 0; i < cutbox_size.h; i++) {
                o = wnd.content + (cutbox_pos.y + i) * wnd.size.w + cutbox_pos.x;
                t = cut_img_result + i * cutbox_size.w;
                memmove(t, o, max_line * 3);
            }
            max_line = edit_img_size.w;
            for (int i = 0; i < edit_img_size.h; i++) {
                o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
                t = edit_img_origin + i * edit_img_size.w;
                memmove(o, t, max_line * 3);
            }
            content_size=(Size){cutbox_size.h,cutbox_size.w};
            content_pos=(Point){140 + (500-content_size.w)/2,30 + (410-content_size.h)/2};
            int pos_y=235-content_size.h/2;
            int pos_x=390-content_size.w/2;
            api_paint24BitmapToContent(&wnd, cut_img_result, (Point){pos_x,pos_y}, (Point){0,0}, (Size){content_size.h,content_size.w},(Size){content_size.h,content_size.w});
            api_repaint(&wnd);
            api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){60, 0, 30, 30});
            is_cut_first_move = 0;
            is_cut = 0;
        }
        if(has_img_content == 1 && isMouseInCutBoxCancelButton(msg->params[0], msg->params[1]))
        {
            is_cut = 0;
            api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
            api_update(&wnd, (Rect){60, 0, 30, 30});
            api_paint24BitmapToContent(&wnd, cut_img_save, (Point){content_pos.x - cut_box_dash_corner_height,content_pos.y - cut_box_dash_corner_height}, (Point){0,0}, (Size){content_size.h+2*cut_box_dash_corner_height, content_size.w+2*cut_box_dash_corner_height+2*cut_box_button_width},(Size){content_size.h+2*cut_box_dash_corner_height,content_size.w+2*cut_box_dash_corner_height+2*cut_box_button_width});
            api_repaint(&wnd);
            is_cut_first_move = 0;
        }
        break;
    case M_MOUSE_UP:
        mouse_down = 0;
        break;
    case M_MOUSE_MOVE:
        if(mouse_down == 1 && is_cut==1 && isCutBoxInContent(cutbox_pos.x, cutbox_pos.y, cutbox_size.w, cutbox_size.h) && !isCutBoxSmallest(cutbox_size.w, cutbox_size.h)) {
            if(is_cut_first_move == 0)
            {
                is_cut_first_move = 1;
                mousePos.x = msg->params[0];
                mousePos.y = msg->params[1];
            }
            lastMousePos.x = mousePos.x;
            lastMousePos.y = mousePos.y;
            mousePos.x = msg->params[0];
            mousePos.y = msg->params[1];
            int dx = mousePos.x - lastMousePos.x;
            int dy = mousePos.y - lastMousePos.y;
            if(dx < 20 && dx > -20 && dy < 20 && dy > -20)
            {
                if(isMouseInCutBoxLeft(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x + dx, cutbox_pos.y, cutbox_size.w - dx, cutbox_size.h) && !isCutBoxSmallest(cutbox_size.w - dx, cutbox_size.h))
                    {
                        cutbox_pos.x = cutbox_pos.x + dx;
                        cutbox_size.w = cutbox_size.w - dx;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxRight(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x, cutbox_pos.y, cutbox_size.w + dx, cutbox_size.h) && !isCutBoxSmallest(cutbox_size.w + dx, cutbox_size.h))
                    {
                        cutbox_size.w = cutbox_size.w + dx;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxUp(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x, cutbox_pos.y + dy, cutbox_size.w, cutbox_size.h - dy) && !isCutBoxSmallest(cutbox_size.w, cutbox_size.h - dy))
                    {
                        cutbox_pos.y = cutbox_pos.y + dy;
                        cutbox_size.h = cutbox_size.h - dy;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxDown(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x, cutbox_pos.y, cutbox_size.w, cutbox_size.h + dy) && !isCutBoxSmallest(cutbox_size.w, cutbox_size.h + dy))
                    {
                        cutbox_size.h = cutbox_size.h + dy;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxUpLeft(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x + dx, cutbox_pos.y + dy, cutbox_size.w - dx, cutbox_size.h - dy) && !isCutBoxSmallest(cutbox_size.w - dx, cutbox_size.h - dy))
                    {
                        cutbox_pos.x = cutbox_pos.x + dx;
                        cutbox_pos.y = cutbox_pos.y + dy;
                        cutbox_size.w = cutbox_size.w - dx;
                        cutbox_size.h = cutbox_size.h - dy;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxUpRight(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x, cutbox_pos.y + dy, cutbox_size.w + dx, cutbox_size.h - dy) && !isCutBoxSmallest(cutbox_size.w + dx, cutbox_size.h - dy))
                    {
                        cutbox_pos.y = cutbox_pos.y + dy;
                        cutbox_size.w = cutbox_size.w + dx;
                        cutbox_size.h = cutbox_size.h - dy;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxDownLeft(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x + dx, cutbox_pos.y, cutbox_size.w - dx, cutbox_size.h + dy) && !isCutBoxSmallest(cutbox_size.w - dx, cutbox_size.h + dy))
                    {
                        cutbox_pos.x = cutbox_pos.x + dx;
                        cutbox_size.w = cutbox_size.w - dx;
                        cutbox_size.h = cutbox_size.h + dy;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
                else if(isMouseInCutBoxDownRight(lastMousePos.x, lastMousePos.y))
                {
                    if(isCutBoxInContent(cutbox_pos.x, cutbox_pos.y, cutbox_size.w + dx, cutbox_size.h + dy) && !isCutBoxSmallest(cutbox_size.w + dx, cutbox_size.h + dy))
                    {
                        cutbox_size.w = cutbox_size.w + dx;
                        cutbox_size.h = cutbox_size.h + dy;
                        drawCutBox(cutbox_pos, cutbox_size.w, cutbox_size.h);
                        api_repaint(&wnd);
                    }
                }
            }
        }
        if(mouse_down == 1 && (is_pencil==1 ||  is_rubber == 1) && isMouseInContent(msg->params[0], msg->params[1]))
        {
            api_drawRect(&wnd, (Point) {msg->params[0] - pointSize / 2, msg->params[1] - pointSize / 2},
                                 (Size) {pointSize, pointSize}, pencil_color);
            api_update(&wnd, (Rect){msg->params[0] - pointSize / 2, msg->params[1] - pointSize /2, pointSize, pointSize});
        }
        break;
    case M_TIMER:
        if(has_content == 1 && current_gif_img->gif_img_num > 1 && current_gif_img->is_onshow == 1)
        {
            struct RGB *t;
            struct RGB *o;
            int max_line = edit_img_size.w;
            for (int i = 0; i < edit_img_size.h; i++) {
                o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
                t = edit_img_origin + i * edit_img_size.w;
                memmove(o, t, max_line * 3);
            }
            int pos_y=235-(current_gif_img->h/current_gif_img->gif_img_num)/2;
            int pos_x=390-current_gif_img->w/2;
            api_paint24BitmapToContent(&wnd, current_gif_img->data, (Point){pos_x,pos_y}, (Point){0,(current_gif_img->h / current_gif_img->gif_img_num)*(gif_frame-1)},(Size){current_gif_img->h,current_gif_img->w},(Size){(current_gif_img->h / current_gif_img->gif_img_num),current_gif_img->w});
            gif_frame = (gif_frame % current_gif_img->gif_img_num ) + 1;
            // api_repaint(&wnd);
            api_update(&wnd, (Rect){pos_x, pos_y, (current_gif_img->h / current_gif_img->gif_img_num), current_gif_img->w});
        }
        break;
    case M_CLOSE_WINDOW:
        printf(1, "USER_CLOSE\n");
        free(save_icon);
        free(delete_icon);
        free(cut_icon);
        free(pen_icon);
        free(rubber_icon);
        free(red_icon);
        free(green_icon);
        free(blue_icon);
        free(purple_icon);
        free(zoomin_icon);
        free(zoomout_icon);
        free(rotate_left_90_icon);
        free(rotate_left_30_icon);
        free(rotate_right_30_icon);
        free(rotate_right_90_icon);
        free(image_list_up_icon);
        free(image_list_down_icon);
        free(edit_img_origin);
        free(image_title_origin);
        free(image_list_origin);
        free(cut_confirm_icon);
        free(cut_cancel_icon);
        api_destroywindow(&wnd);
        break;
    }
}
int
main(int argc, char *argv[])
{
    //int pra = argv[1][0] - '0';
    wnd.pos.x = 100;
    wnd.pos.y = 60;
    //wnd.pos.x = 160;
    //wnd.pos.y = 30;
    wnd.size.w = 640;
    wnd.size.h = 500;
    wnd.title = "PhotoViewer";
    
    save_icon = malloc(30*30*3);
    delete_icon = malloc(30*30*3);
    cut_icon = malloc(30*30*3);
    pen_icon = malloc(30*30*3);
    rubber_icon = malloc(30*30*3);
    red_icon = malloc(20*20*3);
    green_icon = malloc(20*20*3);
    blue_icon = malloc(20*20*3);
    purple_icon = malloc(20*20*3);
    zoomin_icon = malloc(60*60*3);
    zoomout_icon = malloc(60*60*3);
    rotate_left_90_icon = malloc(60*63*3);
    rotate_left_30_icon = malloc(60*65*3);
    rotate_right_30_icon = malloc(60*65*3);
    rotate_right_90_icon = malloc(60*65*3);
    image_list_up_icon = malloc(35*20*3);
    image_list_down_icon = malloc(35*20*3);
    rollover_icon = malloc(60*60*3);
    turnaround_icon = malloc(60*60*3);
    cut_confirm_icon = malloc(cut_box_button_width*cut_box_button_height*3);
    cut_cancel_icon = malloc(cut_box_button_width*cut_box_button_height*3);
    brightness_up_icon = malloc(30*30*3);
    brightness_down_icon = malloc(30*30*3);
    edit_img_test = malloc(edit_img_size.w*edit_img_size.h*3);
    
    api_createwindow(&wnd);

    // save_icon = LoadImg(save_filename);

    
    // PBitmap png = LoadImg("icon1.png");
    PBitmap save = LoadImg(save_filename);
    memmove(save_icon, save.data, 30*30*3);
    free(save.data);
    PBitmap delete = LoadImg(delete_filename);
    memmove(delete_icon, delete.data, 30*30*3);
    free(delete.data);
    PBitmap cut = LoadImg(cut_filename);
    memmove(cut_icon, cut.data, 30*30*3);
    free(cut.data);
    PBitmap pen = LoadImg(pen_filename);
    memmove(pen_icon, pen.data, 30*30*3);
    free(pen.data);
    PBitmap rubber = LoadImg(rubber_filename);
    memmove(rubber_icon, rubber.data, 30*30*3);
    free(rubber.data);
    PBitmap red = LoadImg(red_filename);
    memmove(red_icon, red.data, 20*20*3);
    free(red.data);
    PBitmap green = LoadImg(green_filename);
    memmove(green_icon, green.data, 20*20*3);
    free(green.data);
    PBitmap blue = LoadImg(blue_filename);
    memmove(blue_icon, blue.data, 20*20*3);
    free(blue.data);
    PBitmap purple = LoadImg(purple_filename);
    memmove(purple_icon, purple.data, 20*20*3);
    free(purple.data);
    PBitmap zoomin = LoadImg(zoomin_filename);
    memmove(zoomin_icon, zoomin.data, 60*60*3);
    free(zoomin.data);
    PBitmap zoomout = LoadImg(zoomout_filename);
    memmove(zoomout_icon, zoomout.data, 60*60*3);
    free(zoomout.data);
    PBitmap rotate_left_90 = LoadImg(rotate_left_90_filename);
    memmove(rotate_left_90_icon, rotate_left_90.data, 60*63*3);
    free(rotate_left_90.data);
    PBitmap rotate_left_30 = LoadImg(rotate_left_30_filename);
    memmove(rotate_left_30_icon, rotate_left_30.data, 60*65*3);
    free(rotate_left_30.data);
    PBitmap rotate_right_30 = LoadImg(rotate_right_30_filename);
    memmove(rotate_right_30_icon, rotate_right_30.data, 60*65*3);
    free(rotate_right_30.data);
    PBitmap rotate_right_90 = LoadImg(rotate_right_90_filename);
    memmove(rotate_right_90_icon, rotate_right_90.data, 60*65*3);
    free(rotate_right_90.data);
    PBitmap image_list_down = LoadImg(image_list_down_filename);
    memmove(image_list_down_icon, image_list_down.data, 35*20*3);
    free(image_list_down.data);
    PBitmap image_list_up = LoadImg(image_list_up_filename);
    memmove(image_list_up_icon, image_list_up.data, 35*20*3);
    free(image_list_up.data);
    PBitmap rollover = LoadImg(rollover_filename);
    memmove(rollover_icon, rollover.data, 60*60*3);
    free(rollover.data);
    PBitmap turnaround = LoadImg(turnaround_filename);
    memmove(turnaround_icon, turnaround.data, 60*60*3);
    free(turnaround.data);
    PBitmap cut_confirm = LoadImg(cut_confirm_filename);
    memmove(cut_confirm_icon, cut_confirm.data, cut_box_button_width*cut_box_button_height*3);
    free(cut_confirm.data);
    PBitmap cut_cancel = LoadImg(cut_cancel_filename);
    memmove(cut_cancel_icon, cut_cancel.data, cut_box_button_width*cut_box_button_height*3);
    free(cut_cancel.data);
    PBitmap brightness_up = LoadImg(brightness_up_filename);
    memmove(brightness_up_icon, brightness_up.data, 30*30*3);
    free(brightness_up.data);
    PBitmap brightness_down = LoadImg(brightness_down_filename);
    memmove(brightness_down_icon, brightness_down.data, 30*30*3);
    free(brightness_down.data);
    
    // memset(wnd.content, pra * 50, wnd.size.w * wnd.size.h * 3);

    api_drawImgButton(&wnd, save_icon, (Point){0, 0}, (Size){30, 30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, delete_icon, (Point){30,0},(Size){30,30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, pen_icon, (Point){520,0}, (Size){30,30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, rubber_icon, (Point){550,0}, (Size){30,30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, brightness_up_icon, (Point){580,0}, (Size){30,30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, brightness_down_icon, (Point){610,0}, (Size){30,30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, red_icon, (Point){495,5}, (Size){20,20}, 0, borderColor, normal_shift);
    api_drawImgButton(&wnd, green_icon, (Point){465,5}, (Size){20,20}, 0, borderColor, normal_shift);
    api_drawImgButton(&wnd, blue_icon, (Point){435,5}, (Size){20,20}, 0, borderColor, normal_shift);
    api_drawImgButton(&wnd, purple_icon, (Point){405,5}, (Size){20,20}, 0, borderColor, normal_shift);
    api_drawImgButton(&wnd, cut_icon, (Point){60,0}, (Size){30,30}, border1, borderColor, normal_shift);
    api_drawImgButton(&wnd, zoomin_icon, (Point){142, 440}, (Size){60, 60}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, zoomout_icon, (Point){202,440}, (Size){60,60}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, rotate_left_90_icon, (Point){262,440}, (Size){60,63}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, rotate_left_30_icon, (Point){325,440}, (Size){60,65}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, rotate_right_30_icon, (Point){390,440}, (Size){60,65}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, rotate_right_90_icon, (Point){455,440}, (Size){60,65}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, rollover_icon, (Point){520,440}, (Size){60,60}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, turnaround_icon, (Point){580,440}, (Size){60,60}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, image_list_up_icon, (Point){47,45}, (Size){20,35}, border2, borderColor, normal_shift);
    api_drawImgButton(&wnd, image_list_down_icon, (Point){47,465}, (Size){20,35}, border2, borderColor, normal_shift);
    api_drawRect(&wnd, (Point){142, 30}, (Size){600, border2}, borderColor);
    api_drawRect(&wnd, (Point){0, 30}, (Size){border2, 800}, borderColor);

    struct RGB *t;
    struct RGB *o;
    edit_img_origin = malloc(edit_img_size.h*edit_img_size.w*3);
    int max_line = edit_img_size.w;
    for (int i = 0; i < edit_img_size.h; i++) {
        o = wnd.content + (edit_img_pos.y + i) * wnd.size.w + edit_img_pos.x;
        t = edit_img_origin + i * edit_img_size.w;
        memmove(t, o, max_line * 3);
    }

    image_list_origin = malloc(image_list_size.h*image_list_size.w*3);
    max_line = image_list_size.w;
    for (int i = 0; i < image_list_size.h; i++) {
        o = wnd.content + (image_list_pos.y + i) * wnd.size.w + image_list_pos.x;
        t = image_list_origin + i * image_list_size.w;
        memmove(t, o, max_line * 3);
    }

    image_title_origin = malloc(image_title_size.h*image_title_size.w*3);
    max_line = image_title_size.w;
    for (int i = 0; i < image_title_size.h; i++) {
        o = wnd.content + (image_title_pos.y + i) * wnd.size.w + image_title_pos.x;
        t = image_title_origin + i * image_title_size.w;
        memmove(t, o, max_line * 3);
    }

    image_origin=(PBitmap*)malloc(sizeof(PBitmap));
    image_origin_preview=(PBitmap*)malloc(sizeof(PBitmap));
    image_origin_mirror=(PBitmap*)malloc(sizeof(PBitmap));
    image_scale=(PBitmap*)malloc(sizeof(PBitmap));
    image_scale_preview=(PBitmap*)malloc(sizeof(PBitmap));
    image_turn=(PBitmap*)malloc(sizeof(PBitmap));
    image_turnaround=(PBitmap*)malloc(sizeof(PBitmap));
    image_rollover=(PBitmap*)malloc(sizeof(PBitmap));
    image_in_content = malloc(edit_img_size.w*edit_img_size.h*3);

    image_list=(ImageList* )malloc(sizeof(ImageList));
	ImageListInit(image_list);
    ls_new(".");
    Image *header;
    header = image_list->head;
    image_show=(Image**)malloc(sizeof(Image*)*3);
    image_show[0] = header;
    image_show[1] = header->next;
    image_show[2] = header->next->next;
    setImageList();
    current_gif_img = (Image*)malloc(sizeof(Image));

    api_repaint(&wnd);
    api_settimer(&wnd, 50);
    api_exec(&wnd, &MsgProc);
    return 0;
}

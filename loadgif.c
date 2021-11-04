#include "fcntl.h"
#include "loadgif.h"
#include "gui_base.h"

#ifndef O_BINARY
    #define O_BINARY 0
#endif

#pragma pack(push, 1)
typedef struct {
    void *data, *pict, *prev;
    unsigned long size, last;
    int uuid;
    unsigned long xdim, ydim, nfrm;
    void *results;
} STAT; /** #pragma avoids -Wpadded on 64-bit machines **/
#pragma pack(pop)


void Frame(void*, struct GIF_WHDR*); /** keeps -Wmissing-prototypes happy **/
void Frame(void *data, struct GIF_WHDR *whdr) {
    uint32_t *pict, *prev, x, y, yoff, iter, ifin, dsrc, ddst;
    uint8_t head[18] = {0};
    STAT *stat = (STAT*)data;

    #define BGRA(i) ((whdr->bptr[i] == whdr->tran)? 0 : \
          ((uint32_t)(whdr->cpal[whdr->bptr[i]].R << ((GIF_BIGE)? 8 : 16)) \
         | (uint32_t)(whdr->cpal[whdr->bptr[i]].G << ((GIF_BIGE)? 16 : 8)) \
         | (uint32_t)(whdr->cpal[whdr->bptr[i]].B << ((GIF_BIGE)? 24 : 0)) \
         | ((GIF_BIGE)? 0xFF : 0xFF000000)))

    if (!whdr->ifrm) {
        stat->xdim = whdr->xdim;
        stat->ydim = whdr->ydim;
        stat->nfrm = whdr->nfrm;

        stat->pict = malloc(sizeof(uint32_t) * whdr->xdim * whdr->ydim);
        stat->prev = malloc(sizeof(uint32_t) * whdr->xdim * whdr->ydim);
        stat->results = malloc(sizeof(uint32_t) * whdr->xdim * whdr->ydim * whdr->nfrm);
    }

    pict = (uint32_t*)stat->pict;
    ddst = (uint32_t)(whdr->xdim * whdr->fryo + whdr->frxo);
    ifin = (!(iter = (whdr->intr)? 0 : 4))? 4 : 5; /** interlacing support **/
    for (dsrc = (uint32_t)-1; iter < ifin; iter++)
        for (yoff = 16U >> ((iter > 1)? iter : 1), y = (8 >> iter) & 7;
             y < (uint32_t)whdr->fryd; y += yoff)
            for (x = 0; x < (uint32_t)whdr->frxd; x++)
                if (whdr->tran != (long)whdr->bptr[++dsrc])
                    pict[(uint32_t)whdr->xdim * y + x + ddst] = BGRA(dsrc);
    
    RGB* target = (RGB*)stat->results + whdr->ifrm * whdr->xdim * whdr->ydim;
    RGBA* source = pict; 
    for (int i = 0; i < whdr->ydim; i++) {
        for (int j = 0; j < whdr->xdim; j++) {
            RGB* t = target + i * whdr->xdim + j;
            RGBA* o = source + i * whdr->xdim + j;
            t->R = o->R;
            t->G = o->G;
            t->B = o->B;
        }
    }
    // memcpy(stat->results + whdr->ifrm * sizeof(uint32_t) * whdr->xdim * whdr->ydim, pict, sizeof(uint32_t) * whdr->xdim * whdr->ydim);

    if ((whdr->mode == GIF_PREV) && !stat->last) {
        whdr->frxd = whdr->xdim;
        whdr->fryd = whdr->ydim;
        whdr->mode = GIF_BKGD;
        ddst = 0;
    }
    else {
        stat->last = (whdr->mode == GIF_PREV)?
                      stat->last : (unsigned long)(whdr->ifrm + 1);
        pict = (uint32_t*)((whdr->mode == GIF_PREV)? stat->pict : stat->prev);
        prev = (uint32_t*)((whdr->mode == GIF_PREV)? stat->prev : stat->pict);
        for (x = (uint32_t)(whdr->xdim * whdr->ydim); --x;
             pict[x - 1] = prev[x - 1]);
    }
    if (whdr->mode == GIF_BKGD) /** cutting a hole for the next frame **/
        for (whdr->bptr[0] = (uint8_t)((whdr->tran >= 0)?
                                        whdr->tran : whdr->bkgd), y = 0,
             pict = (uint32_t*)stat->pict; y < (uint32_t)whdr->fryd; y++)
            for (x = 0; x < (uint32_t)whdr->frxd; x++)
                pict[(uint32_t)whdr->xdim * y + x + ddst] = BGRA(0);
    #undef BGRA
}

GIF read_gif(char * filename) {
    GIF gif;
    STAT stat = {0};
    stat.uuid = 2;
    if ((stat.uuid = open(filename, O_RDONLY | O_BINARY)) <= 0)
        return gif;
    stat.size = (unsigned long)lseek(stat.uuid, 0UL, 2 /** SEEK_END **/);
    int current_pos = lseek(stat.uuid, 0UL, 0 /** SEEK_SET **/);
    stat.data = malloc(stat.size);
    read(stat.uuid, stat.data, stat.size);
    close(stat.uuid);
    // unlink(argv[argc - 1]);
    GIF_Load(stat.data, (long)stat.size, Frame, 0, (void*)&stat, 0L);
    free(stat.pict);
    free(stat.prev);
    free(stat.data);

    gif.data = stat.results;
    gif.frame_num = stat.nfrm;
    gif.height = stat.ydim;
    gif.width = stat.xdim;
    return gif;

}
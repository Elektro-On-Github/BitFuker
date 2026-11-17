#ifndef IMGFX_H
#define IMGFX_H

void vline(unsigned char *framebuffer, int dimFB, int width);
void xorfx(unsigned char *framebuffer, int dimFB);
void saturate(unsigned char *framebuffer, int dimFB);
void bug(unsigned char *framebuffer, int dimFB, int width);
void nintendo3ds_fx(unsigned char *framebuffer, int dimFB);
void soft_ghostfx(unsigned char *framebuffer, int dimFB);
void hard_ghostfx(unsigned char *framebuffer, int dimFB);
void granuly(unsigned char *framebuffer, int dimFB);
void blacknoise(unsigned char *framebuffer, int dimFB);
void hugedistortion(unsigned char *framebuffer, int dimFB);

#endif
#ifndef IMGFX_H
#define IMGFX_H

void vline(unsigned char *framebuffer, int dimFB, int width);
void xorfx(unsigned char *framebuffer, int dimFB, int width);
void saturate(unsigned char *framebuffer, int dimFB, int width);
void bug(unsigned char *framebuffer, int dimFB, int width);
void nintendo3ds_fx(unsigned char *framebuffer, int dimFB, int width);
void soft_ghostfx(unsigned char *framebuffer, int dimFB, int width);
void hard_ghostfx(unsigned char *framebuffer, int dimFB, int width);
void granuly(unsigned char *framebuffer, int dimFB, int width);
void blacknoise(unsigned char *framebuffer, int dimFB, int width);
void hugedistortion(unsigned char *framebuffer, int dimFB, int width);
void CRTglitch(unsigned char *framebuffer, int dimFB, int width);
void nofx(unsigned char *framebuffer, int dimFB, int width);

#endif
/** @file
	gd.h: declarations file for the gifdraw module.

	Written by Tom Boutell, 5/94.
	Copyright 1994, Cold Spring Harbor Labs.
	Permission granted to use this code in any fashion provided
	that this notice is retained and any alterations are
	labeled as such. It is requested, but not required, that
	you share extensions to this module with us so that we
	can incorporate them into new versions. */

#ifndef GIF_H
#define GIF_H 1

#include <stdio.h>
#include <string.h>
#define gdMaxColors 0x100

/* Image type. See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for 
	read-only purposes. */

typedef struct gdImageStruct {
	unsigned char ** pixels;
	int sx;
	int sy;
	int colorsTotal;
	int red[gdMaxColors];
	int green[gdMaxColors];
	int blue[gdMaxColors]; 
	int open[gdMaxColors];
	int transparent;
	int *polyInts;
	int polyAllocated;
	int styleWidth;
	int interlace;
} gdImage;

typedef gdImage * gdImagePtr;

/* For backwards compatibility only. Use gdImageSetStyle() for MUCH more flexible line drawing. Also see gdImageSetBrush(). */

#define gdDashSize 4

/* Special colors. */

/* Functions to manipulate images. */

gdImagePtr gdImageCreate(int sx, int sy);
gdImagePtr gdImageCreateFromGif(FILE *fd);
gdImagePtr gdImageCreateFromGd(FILE *in);
gdImagePtr gdImageCreateFromXbm(FILE *fd);
void gdImageDestroy(gdImagePtr im);
void gdImageSetPixel(gdImagePtr im, int x, int y, int color);
int gdImageGetPixel(gdImagePtr im, int x, int y);
void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);
void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color);

/* Point type for use in polygon drawing. */

typedef struct {
	int x, y;
} gdPoint, *gdPointPtr;

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c);
void gdImageFilledPolygonReplaceColor(gdImagePtr im, gdPointPtr p, int n, int a, int b);

int gdImageColorAllocate(gdImagePtr im, int r, int g, int b);
int gdImageColorClosest(gdImagePtr im, int r, int g, int b);
int gdImageColorExact(gdImagePtr im, int r, int g, int b);
int gdImageColorRGB(gdImagePtr im, int r, int g, int b);
int gdImageColor(gdImagePtr im, unsigned int rgb);
void gdImageColorDeallocate(gdImagePtr im, int color);
void gdImageColorTransparent(gdImagePtr im, int color);
void gdImageGif(gdImagePtr im, FILE *out);
void gdImageGd(gdImagePtr im, FILE *out);
void gdImageArc(gdImagePtr im, int cx, int cy, int w, int h, int s, int e, int color);
void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color);
void gdImageFill(gdImagePtr im, int x, int y, int color);
void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h);
void gdImageCopyResized(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH);
void gdImageStyle(gdImagePtr im, int width);
void gdImageInterlace(gdImagePtr im, int interlaceArg); /* On or off (1 or 0) */

/* Macros to access information about images. READ ONLY. Changing
	these values will NOT have the desired result. */
#define gdImageSX(im) ((im)->sx)
#define gdImageSY(im) ((im)->sy)
#define gdImageColorsTotal(im) ((im)->colorsTotal)
#define gdImageRed(im, c) ((im)->red[(c)])
#define gdImageGreen(im, c) ((im)->green[(c)])
#define gdImageBlue(im, c) ((im)->blue[(c)])
#define gdImageGetTransparent(im) ((im)->transparent)
#define gdImageGetInterlaced(im) ((im)->interlace)

#endif

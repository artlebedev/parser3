#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "gif.h"

static void gdImageBrushApply(gdImagePtr im, int x, int y);
static void gdImageTileApply(gdImagePtr im, int x, int y);

gdImagePtr gdImageCreate(int sx, int sy)
{
	int i;
	gdImagePtr im;
	im = (gdImage *) malloc(sizeof(gdImage));
	im->pixels = (unsigned char **) malloc(sizeof(unsigned char *) * sx);
	im->polyInts = 0;
	im->polyAllocated = 0;
	im->styleWidth = 1;
	for (i=0; (i<sx); i++) {
		im->pixels[i] = (unsigned char *) calloc(
			sy, sizeof(unsigned char));
	}	
	im->sx = sx;
	im->sy = sy;
	im->colorsTotal = 0;
	im->transparent = (-1);
	im->interlace = 0;
	return im;
}

void gdImageDestroy(gdImagePtr im)
{
	int i;
	for (i=0; (i<im->sx); i++) {
		free(im->pixels[i]);
	}	
	free(im->pixels);
	if (im->polyInts) {
			free(im->polyInts);
	}
	free(im);
}

int gdImageColorClosest(gdImagePtr im, int r, int g, int b)
{
	int i;
	long rd, gd, bd;
	int ct = (-1);
	long mindist = 0;
	for (i=0; (i<(im->colorsTotal)); i++) {
		long dist;
		if (im->open[i]) {
			continue;
		}
		rd = (im->red[i] - r);	
		gd = (im->green[i] - g);
		bd = (im->blue[i] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if ((i == 0) || (dist < mindist)) {
			mindist = dist;	
			ct = i;
		}
	}
	return ct;
}

int gdImageColorExact(gdImagePtr im, int r, int g, int b)
{
	int i;
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			continue;
		}
		if ((im->red[i] == r) && 
			(im->green[i] == g) &&
			(im->blue[i] == b)) {
			return i;
		}
	}
	return -1;
}

int gdImageColorAllocate(gdImagePtr im, int r, int g, int b)
{
	int i;
	int ct = (-1);
	for (i=0; (i<(im->colorsTotal)); i++) {
		if (im->open[i]) {
			ct = i;
			break;
		}
	}	
	if (ct == (-1)) {
		ct = im->colorsTotal;
		if (ct == gdMaxColors) {
			return -1;
		}
		im->colorsTotal++;
	}
	im->red[ct] = r;
	im->green[ct] = g;
	im->blue[ct] = b;
	im->open[ct] = 0;
	return ct;
}

int gdImageColorRGB(gdImagePtr im, int r, int g, int b){
    int idx=gdImageColorExact(im,r,g,b);
    return idx<0 ? gdImageColorAllocate(im,r,g,b) : idx;
}

int gdImageColor(gdImagePtr im, unsigned int rgb){
    unsigned int b=rgb, g=b>>8, r=g>>8;
    return gdImageColorRGB(im,r & 0xFF,g & 0xFF,b & 0xFF);
}

void gdImageColorDeallocate(gdImagePtr im, int color)
{
	/* Mark it open. */
	im->open[color] = 1;
}

void gdImageColorTransparent(gdImagePtr im, int color)
{
	im->transparent = color;
}


inline int gdImageBoundsSafe(gdImagePtr im, int x, int y){
    return (!(((y < 0) || (y >= im->sy)) || ((x < 0) || (x >= im->sx))));
}

inline /*paf int*/void gdImageDoSetPixel(gdImagePtr im, int x, int y, int color){
    if (gdImageBoundsSafe(im, x, y)) im->pixels[x][y] = color;
}

void gdImageSetPixel(gdImagePtr im, int x, int y, int color)
{
//paf	int p;

	switch (im->styleWidth){
	    case 1: {
		gdImageDoSetPixel(im, x, y,color);
		return;
	    }
	    case 2: {
		gdImageDoSetPixel(im, x, y-1,color);
		gdImageDoSetPixel(im, x-1, y,color);
		gdImageDoSetPixel(im, x, y,color);
		gdImageDoSetPixel(im, x+1, y,color);
		gdImageDoSetPixel(im, x, y+1,color);
		return;
	    }
	    default:{
		int i,j;
		for (i=-1;i<=1;i++) gdImageDoSetPixel(im, x+i, y-2,color);
		for (j=-1;j<=1;j++) for (i=-2;i<=2;i++) gdImageDoSetPixel(im, x+i, y+j,color);
		for (i=-1;i<=1;i++) gdImageDoSetPixel(im, x+i, y+2,color);
		return;
	    }
	}
}

int gdImageGetPixel(gdImagePtr im, int x, int y)
{
	return gdImageBoundsSafe(im, x, y) ? im->pixels[x][y]:0;
}

/* Bresenham as presented in Foley & Van Dam */

void gdImageLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (dy <= dx) {
		d = 2*dy - dx;
		incr1 = 2*dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}
		gdImageSetPixel(im, x, y, color);
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y--;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		}		
	} else {
		d = 2*dx - dy;
		incr1 = 2*dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}
		gdImageSetPixel(im, x, y, color);
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		} else {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x--;
					d+=incr2;
				}
				gdImageSetPixel(im, x, y, color);
			}
		}
	}
}

/* As above, plus dashing */

#define dashedSet \
	{ \
		dashStep++; \
		if (dashStep == gdDashSize) { \
			dashStep = 0; \
			on = !on; \
		} \
		if (on) { \
			gdImageSetPixel(im, x, y, color); \
		} \
	}

void gdImageDashedLine(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int dashStep = 0;
	int on = 1;
	dx = abs(x2-x1);
	dy = abs(y2-y1);
	if (dy <= dx) {
		d = 2*dy - dx;
		incr1 = 2*dy;
		incr2 = 2 * (dy - dx);
		if (x1 > x2) {
			x = x2;
			y = y2;
			ydirflag = (-1);
			xend = x1;
		} else {
			x = x1;
			y = y1;
			ydirflag = 1;
			xend = x2;
		}
		dashedSet;
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
				dashedSet;
			}
		} else {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y--;
					d+=incr2;
				}
				dashedSet;
			}
		}		
	} else {
		d = 2*dx - dy;
		incr1 = 2*dx;
		incr2 = 2 * (dx - dy);
		if (y1 > y2) {
			y = y2;
			x = x2;
			yend = y1;
			xdirflag = (-1);
		} else {
			y = y1;
			x = x1;
			yend = y2;
			xdirflag = 1;
		}
		dashedSet;
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
				dashedSet;
			}
		} else {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x--;
					d+=incr2;
				}
				dashedSet;
			}
		}
	}
}

/* s and e are integers modulo 360 (degrees), with 0 degrees
  being the rightmost extreme and degrees changing clockwise.
  cx and cy are the center in pixels; w and h are the horizontal 
  and vertical diameter in pixels. Nice interface, but slow, since
  I don't yet use Bresenham (I'm using an inefficient but
  simple solution with too much work going on in it; generalizing
  Bresenham to ellipses and partial arcs of ellipses is non-trivial,
  at least for me) and there are other inefficiencies (small circles
  do far too much work). */

void gdImageFillToBorder(gdImagePtr im, int x, int y, int border, int color)
{
	int lastBorder;
	/* Seek left */
	int leftLimit, rightLimit;
	int i;
	leftLimit = (-1);
	if (border < 0) {
		/* Refuse to fill to a non-solid border */
		return;
	}
	for (i = x; (i >= 0); i--) {
		if (gdImageGetPixel(im, i, y) == border) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		leftLimit = i;
	}
	if (leftLimit == (-1)) {
		return;
	}
	/* Seek right */
	rightLimit = x;
	for (i = (x+1); (i < im->sx); i++) {	
		if (gdImageGetPixel(im, i, y) == border) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		rightLimit = i;
	}
	/* Look at lines above and below and start paints */
	/* Above */
	if (y > 0) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y-1);
			if (lastBorder) {
				if ((c != border) && (c != color)) {	
					gdImageFillToBorder(im, i, y-1, 
						border, color);		
					lastBorder = 0;
				}
			} else if ((c == border) || (c == color)) {
				lastBorder = 1;
			}
		}
	}
	/* Below */
	if (y < ((im->sy) - 1)) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y+1);
			if (lastBorder) {
				if ((c != border) && (c != color)) {	
					gdImageFillToBorder(im, i, y+1, 
						border, color);		
					lastBorder = 0;
				}
			} else if ((c == border) || (c == color)) {
				lastBorder = 1;
			}
		}
	}
}

void gdImageFill(gdImagePtr im, int x, int y, int color)
{
	int lastBorder;
	int old;
	int leftLimit, rightLimit;
	int i;
	old = gdImageGetPixel(im, x, y);
	if (old == color) {
		/* Nothing to be done */
		return;
	}
	/* Seek left */
	leftLimit = (-1);
	for (i = x; (i >= 0); i--) {
		if (gdImageGetPixel(im, i, y) != old) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		leftLimit = i;
	}
	if (leftLimit == (-1)) {
		return;
	}
	/* Seek right */
	rightLimit = x;
	for (i = (x+1); (i < im->sx); i++) {	
		if (gdImageGetPixel(im, i, y) != old) {
			break;
		}
		gdImageSetPixel(im, i, y, color);
		rightLimit = i;
	}
	/* Look at lines above and below and start paints */
	/* Above */
	if (y > 0) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y-1);
			if (lastBorder) {
				if (c == old) {	
					gdImageFill(im, i, y-1, color);		
					lastBorder = 0;
				}
			} else if (c != old) {
				lastBorder = 1;
			}
		}
	}
	/* Below */
	if (y < ((im->sy) - 1)) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = gdImageGetPixel(im, i, y+1);
			if (lastBorder) {
				if (c == old) {
					gdImageFill(im, i, y+1, color);		
					lastBorder = 0;
				}
			} else if (c != old) {
				lastBorder = 1;
			}
		}
	}
}
	
#ifdef TEST_CODE
void gdImageDump(gdImagePtr im)
{
	int i, j;
	for (i=0; (i < im->sy); i++) {
		for (j=0; (j < im->sx); j++) {
			printf("%d", im->pixels[j][i]);
		}
		printf("\n");
	}
}
#endif

void gdImageRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	gdImageLine(im, x1, y1, x2, y1, color);		
	gdImageLine(im, x1, y2, x2, y2, color);		
	gdImageLine(im, x1, y1, x1, y2, color);
	gdImageLine(im, x2, y1, x2, y2, color);
}

void gdImageFilledRectangle(gdImagePtr im, int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	for (y=y1; (y<=y2); y++) {
		for (x=x1; (x<=x2); x++) {
			gdImageSetPixel(im, x, y, color);
		}
	}
}

void gdImageCopy(gdImagePtr dst, gdImagePtr src, int dstX, int dstY, int srcX, int srcY, int w, int h)
{
	int c;
	int x, y;
	int tox, toy;
	int i;
	int colorMap[gdMaxColors];
	for (i=0; (i<gdMaxColors); i++) {
		colorMap[i] = (-1);
	}
	toy = dstY;
	for (y=srcY; (y < (srcY + h)); y++) {
		tox = dstX;
		for (x=srcX; (x < (srcX + w)); x++) {
			int nc;
			c = gdImageGetPixel(src, x, y);
			/* Added 7/24/95: support transparent copies */
			if (gdImageGetTransparent(src) == c) {
				tox++;
				continue;
			}
			/* Have we established a mapping for this color? */
			if (colorMap[c] == (-1)) {
				/* If it's the same image, mapping is trivial */
				if (dst == src) {
					nc = c;
				} else { 
					/* First look for an exact match */
					nc = gdImageColorExact(dst,
						src->red[c], src->green[c],
						src->blue[c]);
				}	
				if (nc == (-1)) {
					/* No, so try to allocate it */
					nc = gdImageColorAllocate(dst,
						src->red[c], src->green[c],
						src->blue[c]);
					/* If we're out of colors, go for the
						closest color */
					if (nc == (-1)) {
						nc = gdImageColorClosest(dst,
							src->red[c], src->green[c],
							src->blue[c]);
					}
				}
				colorMap[c] = nc;
			}
			gdImageSetPixel(dst, tox, toy, colorMap[c]);
			tox++;
		}
		toy++;
	}
}			

int gdGetWord(int *result, FILE *in)
{
	int r;
	r = getc(in);
	if (r == EOF) {
		return 0;
	}
	*result = r << 8;
	r = getc(in);	
	if (r == EOF) {
		return 0;
	}
	*result += r;
	return 1;
}

void gdPutWord(int w, FILE *out)
{
	putc((unsigned char)(w >> 8), out);
	putc((unsigned char)(w & 0xFF), out);
}

int gdGetByte(int *result, FILE *in)
{
	int r;
	r = getc(in);
	if (r == EOF) {
		return 0;
	}
	*result = r;
	return 1;
}

gdImagePtr gdImageCreateFromGd(FILE *in)
{
	int sx, sy;
	int x, y;
	int i;
	gdImagePtr im;
	if (!gdGetWord(&sx, in)) {
		goto fail1;
	}
	if (!gdGetWord(&sy, in)) {
		goto fail1;
	}
	im = gdImageCreate(sx, sy);
	if (!gdGetByte(&im->colorsTotal, in)) {
		goto fail2;
	}
	if (!gdGetWord(&im->transparent, in)) {
		goto fail2;
	}
	if (im->transparent == 257) {
		im->transparent = (-1);
	}
	for (i=0; (i<gdMaxColors); i++) {
		if (!gdGetByte(&im->red[i], in)) {
			goto fail2;
		}
		if (!gdGetByte(&im->green[i], in)) {
			goto fail2;
		}
		if (!gdGetByte(&im->blue[i], in)) {
			goto fail2;
		}
	}	
	for (y=0; (y<sy); y++) {
		for (x=0; (x<sx); x++) {	
			int ch;
			ch = getc(in);
			if (ch == EOF) {
				gdImageDestroy(im);
				return 0;
			}
			im->pixels[x][y] = ch;
		}
	}
	return im;
fail2:
	gdImageDestroy(im);
fail1:
	return 0;
}
	
void gdImageGd(gdImagePtr im, FILE *out)
{
	int x, y;
	int i;
	int trans;
	gdPutWord(im->sx, out);
	gdPutWord(im->sy, out);
	putc((unsigned char)im->colorsTotal, out);
	trans = im->transparent;
	if (trans == (-1)) {
		trans = 257;
	}	
	gdPutWord(trans, out);
	for (i=0; (i<gdMaxColors); i++) {
		putc((unsigned char)im->red[i], out);
		putc((unsigned char)im->green[i], out);	
		putc((unsigned char)im->blue[i], out);	
	}
	for (y=0; (y < im->sy); y++) {	
		for (x=0; (x < im->sx); x++) {	
			putc((unsigned char)im->pixels[x][y], out);
		}
	}
}

gdImagePtr
gdImageCreateFromXbm(FILE *fd)
{
	gdImagePtr im;	
	int bit;
	int w, h;
	int bytes;
	int ch;
	int i, x, y;
	char *sp;
	char s[161];
	if (!fgets(s, 160, fd)) {
		return 0;
	}
	sp = &s[0];
	/* Skip #define */
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Skip width label */
	sp++;
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Get width */
	w = atoi(sp + 1);
	if (!w) {
		return 0;
	}
	if (!fgets(s, 160, fd)) {
		return 0;
	}
	sp = s;
	/* Skip #define */
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Skip height label */
	sp++;
	sp = strchr(sp, ' ');
	if (!sp) {
		return 0;
	}
	/* Get height */
	h = atoi(sp + 1);
	if (!h) {
		return 0;
	}
	/* Skip declaration line */
	if (!fgets(s, 160, fd)) {
		return 0;
	}
	bytes = (w * h / 8) + 1;
	im = gdImageCreate(w, h);
	gdImageColorAllocate(im, 255, 255, 255);
	gdImageColorAllocate(im, 0, 0, 0);
	x = 0;
	y = 0;
	for (i=0; (i < bytes); i++) {
		char h[3];
		int b;
		/* Skip spaces, commas, CRs, 0x */
		while(1) {
			ch = getc(fd);
			if (ch == EOF) {
				goto fail;
			}
			if (ch == 'x') {
				break;
			}	
		}
		/* Get hex value */
		ch = getc(fd);
		if (ch == EOF) {
			goto fail;
		}
		h[0] = ch;
		ch = getc(fd);
		if (ch == EOF) {
			goto fail;
		}
		h[1] = ch;
		h[2] = '\0';
		sscanf(h, "%x", &b);		
		for (bit = 1; (bit <= 128); (bit = bit << 1)) {
			gdImageSetPixel(im, x++, y, (b & bit) ? 1 : 0);	
			if (x == im->sx) {
				x = 0;
				y++;
				if (y == im->sy) {
					return im;
				}
				/* Fix 8/8/95 */
				break;
			}
		}
	}
	/* Shouldn't happen */
	fprintf(stderr, "Error: bug in gdImageCreateFromXbm!\n");
	return 0;
fail:
	gdImageDestroy(im);
	return 0;
}

void gdImagePolygon(gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int lx, ly;
	if (!n) {
		return;
	}
	lx = p->x;
	ly = p->y;
	gdImageLine(im, lx, ly, p[n-1].x, p[n-1].y, c);
	for (i=1; (i < n); i++) {
		p++;
		gdImageLine(im, lx, ly, p->x, p->y, c);
		lx = p->x;
		ly = p->y;
	}
}	
	
int gdCompareInt(const void *a, const void *b);
	
void gdImageFilledPolygon(gdImagePtr im, gdPointPtr p, int n, int c)
{
	int i;
	int y;
	int y1, y2;
	int ints;
	if (!n) {
		return;
	}
	if (!im->polyAllocated) {
		im->polyInts = (int *) malloc(sizeof(int) * n);
		im->polyAllocated = n;
	}		
	if (im->polyAllocated < n) {
		while (im->polyAllocated < n) {
			im->polyAllocated *= 2;
		}	
		im->polyInts = (int *) realloc(im->polyInts,
			sizeof(int) * im->polyAllocated);
	}
	y1 = p[0].y;
	y2 = p[0].y;
	for (i=1; (i < n); i++) {
		if (p[i].y < y1) {
			y1 = p[i].y;
		}
		if (p[i].y > y2) {
			y2 = p[i].y;
		}
	}
	for (y=y1; (y <= y2); y++) {
		int interLast = 0;
		int dirLast = 0;
		int interFirst = 1;
		ints = 0;
		for (i=0; (i <= n); i++) {
			int x1, x2;
			int y1, y2;
			int dir;
			int ind1, ind2;
			int lastInd1 = 0;
			if ((i == n) || (!i)) {
				ind1 = n-1;
				ind2 = 0;
			} else {
				ind1 = i-1;
				ind2 = i;
			}
			y1 = p[ind1].y;
			y2 = p[ind2].y;
			if (y1 < y2) {
				y1 = p[ind1].y;
				y2 = p[ind2].y;
				x1 = p[ind1].x;
				x2 = p[ind2].x;
				dir = -1;
			} else if (y1 > y2) {
				y2 = p[ind1].y;
				y1 = p[ind2].y;
				x2 = p[ind1].x;
				x1 = p[ind2].x;
				dir = 1;
			} else {
				/* Horizontal; just draw it */
				gdImageLine(im, 
					p[ind1].x, y1, 
					p[ind2].x, y1,
					c);
				continue;
			}
			if ((y >= y1) && (y <= y2)) {
				int inter = 
					(y-y1) * (x2-x1) / (y2-y1) + x1;
				/* Only count intersections once
					except at maxima and minima. Also, 
					if two consecutive intersections are
					endpoints of the same horizontal line
					that is not at a maxima or minima,	
					discard the leftmost of the two. */
				if (!interFirst) {
					if ((p[ind1].y == p[lastInd1].y) &&
						(p[ind1].x != p[lastInd1].x)) {
						if (dir == dirLast) {
							if (inter > interLast) {
								/* Replace the old one */
								im->polyInts[ints] = inter;
							} else {
								/* Discard this one */
							}	
							continue;
						}
					}
					if (inter == interLast) {
						if (dir == dirLast) {
							continue;
						}
					}
				} 
				if (i > 0) {
					im->polyInts[ints++] = inter;
				}
				lastInd1 = i;
				dirLast = dir;
				interLast = inter;
				interFirst = 0;
			}
		}
		qsort(im->polyInts, ints, sizeof(int), gdCompareInt);
		for (i=0; (i < (ints-1)); i+=2) {
			gdImageLine(im, im->polyInts[i], y,
				im->polyInts[i+1], y, c);
		}
	}
}

//001005paf левый хак, только для прямых линий в gdImageFilledPolygonReplaceColor
void gdImageLineReplaceColor(gdImagePtr im, int x1, int y1, int x2, int y2, int a, int b) {
	if(y1!=y2)
		return;

	for(int x=x1; x<=x2; x++) {
		unsigned char *pixel=&im->pixels[x][y1];
		if(*pixel==a)
			*pixel=b;
	}
}

void gdImageFilledPolygonReplaceColor(gdImagePtr im, gdPointPtr p, int n, int a, int b)
{
	int i;
	int y;
	int y1, y2;
	int ints;
	if (!n) {
		return;
	}
	if (!im->polyAllocated) {
		im->polyInts = (int *) malloc(sizeof(int) * n);
		im->polyAllocated = n;
	}		
	if (im->polyAllocated < n) {
		while (im->polyAllocated < n) {
			im->polyAllocated *= 2;
		}	
		im->polyInts = (int *) realloc(im->polyInts,
			sizeof(int) * im->polyAllocated);
	}
	y1 = p[0].y;
	y2 = p[0].y;
	for (i=1; (i < n); i++) {
		if (p[i].y < y1) {
			y1 = p[i].y;
		}
		if (p[i].y > y2) {
			y2 = p[i].y;
		}
	}
	for (y=y1; (y <= y2); y++) {
		int interLast = 0;
		int dirLast = 0;
		int interFirst = 1;
		ints = 0;
		for (i=0; (i <= n); i++) {
			int x1, x2;
			int y1, y2;
			int dir;
			int ind1, ind2;
			int lastInd1 = 0;
			if ((i == n) || (!i)) {
				ind1 = n-1;
				ind2 = 0;
			} else {
				ind1 = i-1;
				ind2 = i;
			}
			y1 = p[ind1].y;
			y2 = p[ind2].y;
			if (y1 < y2) {
				y1 = p[ind1].y;
				y2 = p[ind2].y;
				x1 = p[ind1].x;
				x2 = p[ind2].x;
				dir = -1;
			} else if (y1 > y2) {
				y2 = p[ind1].y;
				y1 = p[ind2].y;
				x2 = p[ind1].x;
				x1 = p[ind2].x;
				dir = 1;
			} else {
				/* Horizontal; just draw it */
				gdImageLineReplaceColor(im, 
					p[ind1].x, y1, 
					p[ind2].x, y1,
					a,b);
				continue;
			}
			if ((y >= y1) && (y <= y2)) {
				int inter = 
					(y-y1) * (x2-x1) / (y2-y1) + x1;
				/* Only count intersections once
					except at maxima and minima. Also, 
					if two consecutive intersections are
					endpoints of the same horizontal line
					that is not at a maxima or minima,	
					discard the leftmost of the two. */
				if (!interFirst) {
					if ((p[ind1].y == p[lastInd1].y) &&
						(p[ind1].x != p[lastInd1].x)) {
						if (dir == dirLast) {
							if (inter > interLast) {
								/* Replace the old one */
								im->polyInts[ints] = inter;
							} else {
								/* Discard this one */
							}	
							continue;
						}
					}
					if (inter == interLast) {
						if (dir == dirLast) {
							continue;
						}
					}
				} 
				if (i > 0) {
					im->polyInts[ints++] = inter;
				}
				lastInd1 = i;
				dirLast = dir;
				interLast = inter;
				interFirst = 0;
			}
		}
		qsort(im->polyInts, ints, sizeof(int), gdCompareInt);
		for (i=0; (i < (ints-1)); i+=2) {
			gdImageLineReplaceColor(im, im->polyInts[i], y,
				im->polyInts[i+1], y, a,b);
		}
	}
}

int gdCompareInt(const void *a, const void *b)
{
	return (*(const int *)a) - (*(const int *)b);
}

void gdImageInterlace(gdImagePtr im, int interlaceArg)
{
	im->interlace = interlaceArg;
}

void gdImageStyle(gdImagePtr im, int width)
{
	im->styleWidth=width;
}


/** @file
	Parser: image manipulations impl1.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: gif.C,v 1.9 2001/09/26 10:32:25 parser Exp $

	based on: gd

	Written by Tom Boutell, 5/94.
	Copyright 1994, Cold Spring Harbor Labs.
	Permission granted to use this code in any fashion provided
	that this notice is retained and any alterations are
	labeled as such. It is requested, but not required, that
	you share extensions to this module with us so that we
	can incorporate them into new versions. 
*/

#include "gif.h"

//static void BrushApply(int x, int y);
//static void TileApply(int x, int y);

void gdImage::Create(int asx, int asy) {
	sx = asx;
	sy = asy;

	int i;
	pixels = (unsigned char **) malloc(sizeof(unsigned char *) * sx);
	polyInts = 0;
	polyAllocated = 0;
	lineWidth = 1;  lineStyle=0;
	for (i=0; (i<asx); i++)
		pixels[i] = (unsigned char *) calloc(asy);
	colorsTotal = 0;
	transparent = (-1);
	interlace = 0;
}

int gdImage::ColorClosest(int r, int g, int b)
{
	int i;
	long rd, gd, bd;
	int ct = (-1);
	long mindist = 0;
	for (i=0; (i<(colorsTotal)); i++) {
		long dist;
		if (open[i]) {
			continue;
		}
		rd = (red[i] - r);	
		gd = (green[i] - g);
		bd = (blue[i] - b);
		dist = rd * rd + gd * gd + bd * bd;
		if ((i == 0) || (dist < mindist)) {
			mindist = dist;	
			ct = i;
		}
	}
	return ct;
}

int gdImage::ColorExact(int r, int g, int b)
{
	int i;
	for (i=0; (i<(colorsTotal)); i++) {
		if (open[i]) {
			continue;
		}
		if ((red[i] == r) && 
			(green[i] == g) &&
			(blue[i] == b)) {
			return i;
		}
	}
	return -1;
}

int gdImage::ColorAllocate(int r, int g, int b)
{
	int i;
	int ct = (-1);
	for (i=0; (i<(colorsTotal)); i++) {
		if (open[i]) {
			ct = i;
			break;
		}
	}	
	if (ct == (-1)) {
		ct = colorsTotal;
		if (ct == gdMaxColors) {
			return -1;
		}
		colorsTotal++;
	}
	red[ct] = r;
	green[ct] = g;
	blue[ct] = b;
	open[ct] = 0;
	return ct;
}

int gdImage::ColorRGB(int r, int g, int b){
    int idx=ColorExact(r,g,b);
    return idx<0 ? ColorAllocate(r,g,b) : idx;
}

int gdImage::Color(unsigned int rgb){
    unsigned int b=rgb, g=b>>8, r=g>>8;
    return ColorRGB(r & 0xFF,g & 0xFF,b & 0xFF);
}

void gdImage::ColorDeallocate(int color)
{
	/* Mark it open. */
	open[color] = 1;
}

void gdImage::SetColorTransparent(int color)
{
	transparent = color;
}


void gdImage::SetPixel(int x, int y, int color)
{
//paf	int p;

	switch (lineWidth){
	    case 1: {
		DoSetPixel(x, y,color);
		return;
	    }
	    case 2: {
		DoSetPixel(x, y-1,color);
		DoSetPixel(x-1, y,color);
		DoSetPixel(x, y,color);
		DoSetPixel(x+1, y,color);
		DoSetPixel(x, y+1,color);
		return;
	    }
	    default:{
		int i,j;
		for (i=-1;i<=1;i++) DoSetPixel(x+i, y-2,color);
		for (j=-1;j<=1;j++) for (i=-2;i<=2;i++) DoSetPixel(x+i, y+j,color);
		for (i=-1;i<=1;i++) DoSetPixel(x+i, y+2,color);
		return;
	    }
	}
}

int gdImage::GetPixel(int x, int y)
{
	return BoundsSafe(x, y) ? pixels[x][y]:0;
}

/* Bresenham as presented in Foley & Van Dam */

/* As above, plus dashing */

#define styledSet \
	{ \
		if (lineStyle) { \
			if(!lineStyle[styleStep]) \
				styleStep = 0; \
			on=lineStyle[styleStep++]!=' '; \
		} \
		if (on) { \
			SetPixel(x, y, color); \
		} \
	}

void gdImage::Line(int x1, int y1, int x2, int y2, int color)
{
	int dx, dy, incr1, incr2, d, x, y, xend, yend, xdirflag, ydirflag;
	int styleStep = 0;
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
		styledSet;
		if (((y2 - y1) * ydirflag) > 0) {
			while (x < xend) {
				x++;
				if (d <0) {
					d+=incr1;
				} else {
					y++;
					d+=incr2;
				}
				styledSet;
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
				styledSet;
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
		styledSet;
		if (((x2 - x1) * xdirflag) > 0) {
			while (y < yend) {
				y++;
				if (d <0) {
					d+=incr1;
				} else {
					x++;
					d+=incr2;
				}
				styledSet;
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
				styledSet;
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

void gdImage::FillToBorder(int x, int y, int border, int color)
{
	if(!BoundsSafe(x, y)) //PAF
		return;

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
		if (GetPixel(i, y) == border) {
			break;
		}
		SetPixel(i, y, color);
		leftLimit = i;
	}
	if (leftLimit == (-1)) {
		return;
	}
	/* Seek right */
	rightLimit = x;
	for (i = (x+1); (i < sx); i++) {	
		if (GetPixel(i, y) == border) {
			break;
		}
		SetPixel(i, y, color);
		rightLimit = i;
	}
	/* Look at lines above and below and start paints */
	/* Above */
	if (y > 0) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = GetPixel(i, y-1);
			if (lastBorder) {
				if ((c != border) && (c != color)) {	
					FillToBorder(i, y-1, 
						border, color);		
					lastBorder = 0;
				}
			} else if ((c == border) || (c == color)) {
				lastBorder = 1;
			}
		}
	}
	/* Below */
	if (y < ((sy) - 1)) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = GetPixel(i, y+1);
			if (lastBorder) {
				if ((c != border) && (c != color)) {	
					FillToBorder(i, y+1, 
						border, color);		
					lastBorder = 0;
				}
			} else if ((c == border) || (c == color)) {
				lastBorder = 1;
			}
		}
	}
}

void gdImage::Fill(int x, int y, int color)
{
	if(!BoundsSafe(x, y)) //PAF
		return;

	int lastBorder;
	int old;
	int leftLimit, rightLimit;
	int i;
	old = GetPixel(x, y);
	if (old == color) {
		/* Nothing to be done */
		return;
	}
	/* Seek left */
	leftLimit = (-1);
	for (i = x; (i >= 0); i--) {
		if (GetPixel(i, y) != old) {
			break;
		}
		SetPixel(i, y, color);
		leftLimit = i;
	}
	if (leftLimit == (-1)) {
		return;
	}
	/* Seek right */
	rightLimit = x;
	for (i = (x+1); (i < sx); i++) {	
		if (GetPixel(i, y) != old) {
			break;
		}
		SetPixel(i, y, color);
		rightLimit = i;
	}
	/* Look at lines above and below and start paints */
	/* Above */
	if (y > 0) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = GetPixel(i, y-1);
			if (lastBorder) {
				if (c == old) {	
					Fill(i, y-1, color);		
					lastBorder = 0;
				}
			} else if (c != old) {
				lastBorder = 1;
			}
		}
	}
	/* Below */
	if (y < ((sy) - 1)) {
		lastBorder = 1;
		for (i = leftLimit; (i <= rightLimit); i++) {
			int c;
			c = GetPixel(i, y+1);
			if (lastBorder) {
				if (c == old) {
					Fill(i, y+1, color);		
					lastBorder = 0;
				}
			} else if (c != old) {
				lastBorder = 1;
			}
		}
	}
}
	
void gdImage::Rectangle(int x1, int y1, int x2, int y2, int color)
{
	Line(x1, y1, x2, y1, color);		
	Line(x1, y2, x2, y2, color);		
	Line(x1, y1, x1, y2, color);
	Line(x2, y1, x2, y2, color);
}

void gdImage::FilledRectangle(int x1, int y1, int x2, int y2, int color)
{
	int x, y;
	for (y=y1; (y<=y2); y++)
		for (x=x1; (x<=x2); x++)
			SetPixel(x, y, color);
}

void gdImage::Copy(gdImage& dst, int dstX, int dstY, int srcX, int srcY, int w, int h)
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
			c = GetPixel(x, y);
			/* Added 7/24/95: support transparent copies */
			if (GetTransparent() == c) {
				tox++;
				continue;
			}
			/* Have we established a mapping for this color? */
			if (colorMap[c] == (-1)) {
				/* If it's the same image, mapping is trivial */
				if (&dst == this) {
					nc = c;
				} else { 
					/* First look for an exact match */
					nc = dst.ColorExact(
						red[c], green[c],
						blue[c]);
				}	
				if (nc == (-1)) {
					/* No, so try to allocate it */
					nc = dst.ColorAllocate(
						red[c], green[c],
						blue[c]);
					/* If we're out of colors, go for the
						closest color */
					if (nc == (-1)) {
						nc = dst.ColorClosest(
							red[c], green[c],
							blue[c]);
					}
				}
				colorMap[c] = nc;
			}
			dst.SetPixel(tox, toy, colorMap[c]);
			tox++;
		}
		toy++;
	}
}			

static int gdGetWord(int *result, FILE *in)
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

static void gdPutWord(int w, FILE *out)
{
	putc((unsigned char)(w >> 8), out);
	putc((unsigned char)(w & 0xFF), out);
}

static int gdGetByte(int *result, FILE *in)
{
	int r;
	r = getc(in);
	if (r == EOF) {
		return 0;
	}
	*result = r;
	return 1;
}

void gdImage::Polygon(Point *p, int n, int c, bool closed)
{
	int i;
	int lx, ly;
	if (!n) {
		return;
	}
	lx = p->x;
	ly = p->y;
	if(closed)
		Line(lx, ly, p[n-1].x, p[n-1].y, c);
	for (i=1; (i < n); i++) {
		p++;
		Line(lx, ly, p->x, p->y, c);
		lx = p->x;
		ly = p->y;
	}
}	
	
static int gdCompareInt(const void *a, const void *b)
{
	return (*(const int *)a) - (*(const int *)b);
}


	
void gdImage::FilledPolygon(Point *p, int n, int c)
{
	int i;
	int y;
	int y1, y2;
	int ints;
	if (!n) {
		return;
	}
	if (!polyAllocated) {
		polyInts = (int *) malloc(sizeof(int) * n);
		polyAllocated = n;
	}		
	if (polyAllocated < n) {
		while (polyAllocated < n) {
			polyAllocated *= 2;
		}	
		polyInts = (int *) realloc(polyInts,
			sizeof(int) * polyAllocated);
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
				Line( 
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
								polyInts[ints] = inter;
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
					polyInts[ints++] = inter;
				}
				lastInd1 = i;
				dirLast = dir;
				interLast = inter;
				interFirst = 0;
			}
		}
		qsort(polyInts, ints, sizeof(int), gdCompareInt);
		for (i=0; (i < (ints-1)); i+=2)
			Line(polyInts[i], y, polyInts[i+1], y, c);
	}
}

//001005paf this used in drawing straight lines in gdImage::FilledPolygonReplaceColor
void gdImage::LineReplaceColor(int x1, int y1, int x2, int y2, int a, int b) {
	if(y1!=y2)
		return;

	for(int x=x1; x<=x2; x++) {
		unsigned char *pixel=&pixels[x][y1];
		if(*pixel==a)
			*pixel=b;
	}
}

void gdImage::FilledPolygonReplaceColor(Point *p, int n, int a, int b)
{
	int i;
	int y;
	int y1, y2;
	int ints;
	if (!n) {
		return;
	}
	if (!polyAllocated) {
		polyInts = (int *) malloc(sizeof(int) * n);
		polyAllocated = n;
	}		
	if (polyAllocated < n) {
		while (polyAllocated < n) {
			polyAllocated *= 2;
		}	
		polyInts = (int *) realloc(polyInts,
			sizeof(int) * polyAllocated);
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
				LineReplaceColor( 
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
								polyInts[ints] = inter;
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
					polyInts[ints++] = inter;
				}
				lastInd1 = i;
				dirLast = dir;
				interLast = inter;
				interFirst = 0;
			}
		}
		qsort(polyInts, ints, sizeof(int), gdCompareInt);
		for (i=0; (i < (ints-1)); i+=2) {
			LineReplaceColor(polyInts[i], y,
				polyInts[i+1], y, a,b);
		}
	}
}

void gdImage::SetInterlace(int interlaceArg)
{
	interlace = interlaceArg;
}

void gdImage::SetLineWidth(int width)
{
	lineWidth=width;
}

void gdImage::SetLineStyle(const char *alineStyle)
{
	lineStyle=alineStyle;
}


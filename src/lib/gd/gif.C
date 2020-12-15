/** @file
	Parser: image manipulations impl1.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
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
#include "mtables.h"

volatile const char * IDENT_GIF_C="$Id: gif.C,v 1.11 2020/12/15 17:10:33 moko Exp $" IDENT_GIF_H;

//static void BrushApply(int x, int y);
//static void TileApply(int x, int y);

void gdImage::Create(int asx, int asy) {
	sx = asx;
	sy = asy;

	int i;
	pixels = (unsigned char **) pa_malloc(sizeof(unsigned char *) * sx);
	polyInts = 0;
	polyAllocated = 0;
	lineWidth = 1;
	for (i=0; (i<asx); i++)
		pixels[i] = (unsigned char *) pa_malloc_atomic(asy);
	colorsTotal = 0;
	transparent = (-1);
	interlace = 0;
}

int gdImage::ColorClosest(int r, int g, int b, int tolerance)
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
		if ((i == 0) || (dist < mindist+tolerance)) {
			mindist = dist;	
			ct = i;
		}
	}
	return mindist<tolerance?ct:-1;
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
unsigned int gdImage::DecodeColor(int color) {
	if(color<0)
		return color;
	return (((red[color]<<8) + green[color])<<8)+blue[color];
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
	return BoundsSafe(x, y) ? pixels[x][y]:-1;
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

void gdImage::Arc(int cx, int cy, int w, int h, int s, int e, int color)
{
#if 0
	if(s==0 && e==360) { // full 
		if(w==h) { // circle?
			/* Bresenham octant code, which I should use eventually */
			int x, y, d;
			x = 0;
			y = w/2;
			d = 3-w;
			while (x <= y) {
				SetPixel(cx+x, cy+y, color);
				SetPixel(cx+x, cy-y, color);
				SetPixel(cx-x, cy+y, color);
				SetPixel(cx-x, cy-y, color);
				SetPixel(cx+y, cy+x, color);
				SetPixel(cx+y, cy-x, color);
				SetPixel(cx-y, cy+x, color);
				SetPixel(cx-y, cy-x, color);
				if (d < 0) {
					d += 4 * x + 6;
				} else {
					d += 4 * (x - y) + 10;
					y--;
				}
				x++;
			}
		} else { // full ellipse
			w/=2;
			h/=2;
			int elx, ely;
			long aa, aa2, bb, bb2, d, dx, dy;
			
			elx = 0; ely = h; aa = (long)w * w; aa2 = 2 * aa;
			bb = (long)h * h; bb2 = 2 * bb;
			d = bb - aa * h + aa/4; dx = 0; dy = aa2 * h;
			SetPixel(cx, cy - ely, color); SetPixel(cx, cy + ely, color);
			SetPixel(cx - w, cy, color); SetPixel(cx + w, cy, color);
			
			while (dx < dy)
			{
				if (d > 0) { ely--; dy-=aa2; d-=dy;}
				elx++; dx+=bb2; d+=bb+dx;
				SetPixel(cx + elx, cy + ely, color);
				SetPixel(cx - elx, cy + ely, color);
				SetPixel(cx + elx, cy - ely, color);
				SetPixel(cx - elx, cy - ely, color);
			};
			d+=(3 * (aa - bb)/2 - (dx + dy))/2;
			while (ely > 0)
			{
				if (d < 0) {elx++; dx+=bb2; d+=bb + dx;}
				ely--; dy-=aa2; d+=aa - dy;
				SetPixel(cx + elx, cy + ely, color);
				SetPixel(cx - elx, cy + ely, color);
				SetPixel(cx + elx, cy - ely, color);
				SetPixel(cx - elx, cy - ely, color);
			};
		}
	} else {
#endif
		int i;
		int lx = 0, ly = 0;
		int w2, h2;
		w2 = w/2;
		h2 = h/2;
		while (e < s) e += 360;
		// paf
		while(s<0) s+=360;
		while(s>360) s-=360;
		while(e<0) e+=360;
		while(e>360) e-=360;
		for (i=s; (i <= e); i++) {
			int x, y;
			x = ((long)cost[i] * (long)w2 / costScale) + cx; 
			y = ((long)sint[i] * (long)h2 / sintScale) + cy;
			if (i != s) {
				Line(lx, ly, x, y, color);	
			}
			lx = x;
			ly = y;
		}
#if 0
	}
#endif
}

/*

// http://firststeps.narod.ru/cgi/18.html

int CGIScreen::Ellipse(int exc, int eyc, int ea, int eb , unsigned char Color)
{
  int elx, ely;
  long aa, aa2, bb, bb2, d, dx, dy;

  elx = 0; ely = eb; aa = (long)ea * ea; aa2 = 2 * aa;
  bb = (long)eb * eb; bb2 = 2 * bb;
  d = bb - aa * eb + aa/4; dx = 0; dy = aa2 * eb;
  PutPixel(exc, eyc - ely, Color); PutPixel(exc, eyc + ely, Color);
  PutPixel(exc - ea, eyc, Color); PutPixel(exc + ea, eyc, Color);

  while (dx < dy)
  {
    if (d > 0) { ely--; dy-=aa2; d-=dy;}
    elx++; dx+=bb2; d+=bb+dx;
    PutPixel(exc + elx, eyc + ely, Color);
    PutPixel(exc - elx, eyc + ely, Color);
    PutPixel(exc + elx, eyc - ely, Color);
    PutPixel(exc - elx, eyc - ely, Color);
  };
  d+=(3 * (aa - bb)/2 - (dx + dy))/2;
  while (ely > 0)
  {
    if (d < 0) {elx++; dx+=bb2; d+=bb + dx;}
    ely--; dy-=aa2; d+=aa - dy;
    PutPixel(exc + elx, eyc + ely, Color);
    PutPixel(exc - elx, eyc + ely, Color);
    PutPixel(exc + elx, eyc - ely, Color);
    PutPixel(exc - elx, eyc - ely, Color);
  };
  return 0;
};

*/


void gdImage::Sector(int cx, int cy, int w, int h, int s, int e, int color)
{
	int i;
	int lx = 0, ly = 0;
	int w2, h2;
	w2 = w/2;
	h2 = h/2;
	while (e < s) e += 360;
	// paf
	while(s<0) s+=360;
	while(s>360) s-=360;
	while(e<0) e+=360;
	while(e>360) e-=360;
	for (i=s; (i <= e); i++) {
		int x, y;
		x = ((long)cost[i] * (long)w2 / costScale) + cx; 
		y = ((long)sint[i] * (long)h2 / sintScale) + cy;
		if(i==s || i==e)
			Line(cx, cy, x, y, color);
		if (i != s) {
			Line(lx, ly, x, y, color);	
		}
		lx = x;
		ly = y;
	}
}



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
		DoSetPixel(i, y, color); // PAF, was SetPixel
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
		DoSetPixel(i, y, color); // PAF, was SetPixel
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
	if(x1>x2) {
		int t=x1;
		x1=x2;
		x2=t;
	}
	if(y1>y2) {
		int t=y1;
		y1=y2;
		y2=t;
	}
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

void gdImage::CopyResampled(gdImage& dst,
		      int dstX, int dstY,
		      int /*srcX*/, int /*srcY*/,
		      int dstW, int dstH,
			  int srcW, int srcH,
			  int tolerance)
{
	gdImage& src=*this;
	int x, y;
	int srcTransparent=src.GetTransparent();
	int dstTransparent=dst.GetTransparent();
	for (y = dstY; (y < dstY + dstH); y++) {
		for (x = dstX; (x < dstX + dstW); x++) {
			int pd = dst.GetPixel (x, y);
			/* Added 7/24/95: support transparent copies */
			/* fixed by paf 20030116, another fix below */
			if (pd == dstTransparent)
				continue;
			
			double sy1, sy2, sx1, sx2;
			double sx, sy;
			double spixels = 0;
			double red = 0.0, green = 0.0, blue = 0.0;
			bool transparent=true;
			sy1 = ((double) y - (double) dstY) * (double) srcH /
				(double) dstH;
			sy2 = ((double) (y + 1) - (double) dstY) * (double) srcH /
				(double) dstH;
			sy = sy1;
			do
			{
				double yportion;
				if (floor (sy) == floor (sy1))
				{
					yportion = 1.0 - (sy - floor (sy));
					if (yportion > sy2 - sy1)
					{
						yportion = sy2 - sy1;
					}
					sy = floor (sy);
				}
				else if (sy == floor (sy2))
				{
					yportion = sy2 - floor (sy2);
				}
				else
				{
					yportion = 1.0;
				}
				sx1 = ((double) x - (double) dstX) * (double) srcW /
					dstW;
				sx2 = ((double) (x + 1) - (double) dstX) * (double) srcW /
					dstW;
				sx = sx1;
				do
				{
					double xportion;
					double pcontribution;
					int p;
					if (floor (sx) == floor (sx1))
					{
						xportion = 1.0 - (sx - floor (sx));
						if (xportion > sx2 - sx1)
						{
							xportion = sx2 - sx1;
						}
						sx = floor (sx);
					}
					else if (sx == floor (sx2))
					{
						xportion = sx2 - floor (sx2);
					}
					else
					{
						xportion = 1.0;
					}
					pcontribution = xportion * yportion;
					p = src.GetPixel (
						(int) sx,
						(int) sy);
					// fix added 20020116 by paf to support transparent src
					if (p!=srcTransparent) {
						transparent = false;
						red += Red (p) * pcontribution;
						green += Green (p) * pcontribution;
						blue += Blue (p) * pcontribution;
					}
					spixels += xportion * yportion;
					sx += 1.0;
				} while (sx < sx2);
				sy += 1.0;
			} while (sy < sy2);

			if(transparent)
				continue;

			if (spixels != 0.0) {
				red /= spixels;
				green /= spixels;
				blue /= spixels;
			}
			/* Clamping to allow for rounding errors above */
			if (red > 255.0)
				red = 255.0;
			if (green > 255.0)
				green = 255.0;
			if (blue > 255.0)
				blue = 255.0;
			
			red=round(red);
			green=round(green);
			blue=round(blue);
			/* First look for an exact match */
			int nc = dst.ColorExact((int)red, (int)green, (int)blue);
			if (nc == (-1)) {
				/* No, so go for the closest color with high tolerance */
				nc = dst.ColorClosest((int)red, (int)green, (int)blue,  tolerance);
				if (nc == (-1)) {
					/* Not found with even high tolerance, so try to allocate it */
					nc = dst.ColorAllocate((int)red, (int)green, (int)blue);

					/* If we're out of colors, go for the closest color */
					if (nc == (-1))
						nc = dst.ColorClosest((int)red, (int)green, (int)blue);
				}
			}
			dst.SetPixel(x, y, nc);
		}
    }
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
		polyInts = (int *) pa_malloc(sizeof(int) * n);
		polyAllocated = n;
	}		
	if (polyAllocated < n) {
		while (polyAllocated < n) {
			polyAllocated *= 2;
		}	
		polyInts = (int *) pa_realloc(polyInts,
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

	for(int x=x1; x<=x2; x++) 
		if(BoundsSafe(x, y1)) {
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
		polyInts = (int *) pa_malloc(sizeof(int) * n);
		polyAllocated = n;
	}		
	if (polyAllocated < n) {
		while (polyAllocated < n) {
			polyAllocated *= 2;
		}	
		polyInts = (int *) pa_realloc(polyInts,
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

void gdImage::SetLineStyle(const char* alineStyle)
{
	lineStyle=alineStyle;
}


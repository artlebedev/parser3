/** @file
	Parser: image manipulations decls.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: gif.h,v 1.19 2002/02/08 07:27:42 paf Exp $

	based on:
	gd.h: declarations file for the gifdraw module.

	Written by Tom Boutell, 5/94.
	Copyright 1994, Cold Spring Harbor Labs.
	Permission granted to use this code in any fashion provided
	that this notice is retained and any alterations are
	labeled as such. It is requested, but not required, that
	you share extensions to this module with us so that we
	can incorporate them into new versions. 
*/

#ifndef GIF_H
#define GIF_H 1

#include "pa_config_includes.h"

#include "pa_pool.h"
#include "pa_string.h"

#define gdMaxColors 0x100
#define HSIZE  5003            /* 80% occupancy */

/** Image type. 
	See functions below; you will not need to change
	the elements directly. Use the provided macros to
	access sx, sy, the color table, and colorsTotal for 
	read-only purposes. 
*/
class gdImage : public Pooled {

public: 
	
	//@{
	/// @name Functions to manipulate images
	gdImage(Pool& pool) : Pooled(pool) {}
	void Create(int asx, int asy);
	bool CreateFromGif(FILE *fd);
	void SetPixel(int x, int y, int color);
	int GetPixel(int x, int y);
	void Line(int x1, int y1, int x2, int y2, int color);
	void StyledLine(int x1, int y1, int x2, int y2, int color, const char *lineStyle);
	void Rectangle(int x1, int y1, int x2, int y2, int color);
	void LineReplaceColor(int x1, int y1, int x2, int y2, int a, int b);
	void FilledRectangle(int x1, int y1, int x2, int y2, int color);
	//@}

	/// Point type for use in polygon drawing.
	struct Point {
		int x, y;
	};
	
	void Polygon(Point *p, int n, int c, bool closed=true);
	void FilledPolygon(Point *p, int n, int c);
	void FilledPolygonReplaceColor(Point *p, int n, int a, int b);

	int ColorAllocate(int r, int g, int b);
	int ColorClosest(int r, int g, int b, int tolerance=0);
	int ColorExact(int r, int g, int b);
	int ColorRGB(int r, int g, int b);
	int Color(unsigned int rgb);
	void ColorDeallocate(int color);
	void SetColorTransparent(int color);
	
	int BoundsSafe(int x, int y);
	void DoSetPixel(int x, int y, int color);
	
	void Gif(String& out);
	void Arc(int cx, int cy, int w, int h, int s, int e, int color);
	void Sector(int cx, int cy, int w, int h, int s, int e, int color);
	void FillToBorder(int x, int y, int border, int color);
	void Fill(int x, int y, int color);
	void Copy(gdImage& dst, int dstX, int dstY, int srcX, int srcY, int w, int h);
	void CopyResampled(gdImage& dst, int dstX, int dstY, int srcX, int srcY, int dstW, int dstH, int srcW, int srcH, int tolerance);
	void SetLineWidth(int width);
	void SetLineStyle(const char *aLineStyle);
	void SetInterlace(int interlaceArg); /* On or off(1 or 0) */
	
public: 

	//@{
	/// @name information about image. READ ONLY
	int SX() { return sx; }
	int SY() { return sy; }
	int ColorsTotal() { return colorsTotal; }
	int Red(int c) { return red[c]; }
	int Green(int c) { return green[c]; }
	int Blue(int c) { return blue[c]; }
	int GetTransparent() { return transparent; }
	int GetInterlaced() { return interlace; }
	//@}

private:

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
	int lineWidth; const char *lineStyle;
	int interlace;

private: // read gif

	int GetDataBlock(FILE *fd, unsigned char *buf);
	int LWZReadByte(FILE *fd, int flag, int input_code_size);
	void ReadImage(FILE *fd, int len, int height, unsigned char(*cmap)[256], int interlace, int ignore);
	int DoExtension(FILE *fd, int label, int *Transparent);
	int GetCode(FILE *fd, int code_size, int flag);

private: // read gif

	int ZeroDataBlock;
	
};

///	used by gdImage::Gif to produce buffer with bytes in GIF format
class gdGifEncoder : public Pooled {
public:

	gdGifEncoder(Pool& pool, gdImage& aim, String& afp);

	void encode( 
		int GWidth, int GHeight, 
		int GInterlace, int Background, 
		int Transparent, int BitsPerPixel, 
		int *Red, int *Green, int *Blue);

private:

	/// a code_int must be able to hold 2**GIFBITS values of type int, and also -1
	typedef int             code_int;
#ifdef SIGNED_COMPARE_SLOW
	typedef unsigned long int count_int;
	typedef unsigned short int count_short;
#else
	typedef long int          count_int;
#endif

private:

	void Putbyte(int c);
	void Putword(int w);
	void Write(void *buf, size_t size);

	void prepare_encoder(void);
	void BumpPixel(void);
	int GIFNextPixel();
	void compress(int init_bits);
	void output(code_int code);
	void cl_block(void);
	void cl_hash(count_int hsize);
	void char_init(void);
	void char_out(int c);
	void flush_char(void);

private:
	
	gdImage& im;
	String& fp;

	int Width, Height;
	int curx, cury;
	long CountDown;
	int Pass;
	int Interlace;

	int g_init_bits;

	int ClearCode;
	int EOFCode;

	int n_bits;                        /* number of bits/code */
	int maxbits;                /* user settable max # bits/code */
	
	code_int maxcode;                  /* maximum code, given n_bits */
	code_int maxmaxcode; /* should NEVER generate this code */

	count_int htab [HSIZE];
	unsigned short codetab [HSIZE];
	code_int hsize;                 /* for dynamic table sizing */
	
	code_int free_ent;                  /* first unused entry */
	
	/*
	* block compression parameters -- after all codes are used up,
	* and compression rate changes, start over.
	*/
	int clear_flg;
	
	int offset;
	long int in_count;            /* length of input */
	long int out_count;           /* # of codes output(for debugging) */
	
	unsigned long cur_accum;
	int cur_bits;

	/*
	 * Number of characters so far in this 'packet'
	 */
	int a_count;

	/*
	 * Define the storage for the packet accumulator
	 */
	char accum[ 256 ];

};

inline int gdImage::BoundsSafe(int x, int y){
    return(!(((y < 0) ||(y >= sy)) ||((x < 0) ||(x >= sx))));
}

inline /*paf int*/void gdImage::DoSetPixel(int x, int y, int color){
    if(BoundsSafe(x, y)) pixels[x][y] = color;
}

#endif

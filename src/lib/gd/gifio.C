/** @file
	Parser: image manipulations impl2.

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

	based on:
 **
 ** Based on GIFENCOD by David Rowley <mgardi@watdscu.waterloo.edu>. A
 ** Lempel-Zim compression based on "compress".
 **
 ** Modified by Marcel Wijkstra <wijkstra@fwi.uva.nl>
 **
 ** Copyright(C) 1989 by Jef Poskanzer.
 **
 ** Permission to use, copy, modify, and distribute this software and its
 ** documentation for any purpose and without fee is hereby granted, provided
 ** that the above copyright notice appear in all copies and that both that
 ** copyright notice and this permission notice appear in supporting
 ** documentation.  This software is provided "as is" without express or
 ** implied warranty.
 **
 ** The Graphics Interchange Format(c) is the Copyright property of
 ** CompuServe Incorporated.  GIF(sm) is a Service Mark property of
 ** CompuServe Incorporated.
*/

#include "gif.h"

volatile const char * IDENT_GIFIO_C="$Id: gifio.C,v 1.8 2020/12/15 17:10:33 moko Exp $";

static int colorstobpp(int colors);

gdBuf gdImage::Gif()
{
	int BitsPerPixel = colorstobpp(colorsTotal);
	/* Clear any old values in statics strewn through the GIF code */
	gdGifEncoder encoder(*this);
	/* All set, let's do it. */
	return encoder.encode(
		sx, sy, interlace, 0, transparent, BitsPerPixel,
		red, green, blue);
}

static int
colorstobpp(int colors)
{
    int bpp = 0;
	
    if( colors <= 2 )
        bpp = 1;
    else if( colors <= 4 )
        bpp = 2;
    else if( colors <= 8 )
        bpp = 3;
    else if( colors <= 16 )
        bpp = 4;
    else if( colors <= 32 )
        bpp = 5;
    else if( colors <= 64 )
        bpp = 6;
    else if( colors <= 128 )
        bpp = 7;
    else if( colors <= 256 )
        bpp = 8;
    return bpp;
}

/*****************************************************************************
*
* GIFENCODE.C    - GIF Image compression interface
*
* GIFEncode( FName, GHeight, GWidth, GInterlace, Background, Transparent,
*            BitsPerPixel, Red, Green, Blue, gdGifEncoder::Ptr )
*
*****************************************************************************/

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/*
* Bump the 'curx' and 'cury' to point to the next pixel
*/
void
gdGifEncoder::BumpPixel(void)
{
/*
* Bump the current X position
	*/
	++curx;
	
	/*
	* If we are at the end of a scan line, set curx back to the beginning
	* If we are interlaced, bump the cury to the appropriate spot,
	* otherwise, just increment it.
	*/
	if( curx == Width ) {
		curx = 0;
		
		if( !Interlace )
			++cury;
		else {
			switch( Pass ) {
				
			case 0:
				cury += 8;
				if( cury >= Height ) {
					++Pass;
					cury = 4;
				}
				break;
				
			case 1:
				cury += 8;
				if( cury >= Height ) {
					++Pass;
					cury = 2;
				}
				break;
				
			case 2:
				cury += 4;
				if( cury >= Height ) {
					++Pass;
					cury = 1;
				}
				break;
				
			case 3:
				cury += 2;
				break;
			}
		}
	}
}

/*
* Return the next pixel from the image
*/
int
gdGifEncoder::GIFNextPixel()
{
	int r;
	
	if( CountDown == 0 )
		return EOF;
	
	--CountDown;
	
	r = im.GetPixel(curx, cury);
	
	BumpPixel();
	
	return r;
}

/* public */

gdBuf
gdGifEncoder::encode(int GWidth, int GHeight, 
					 int GInterlace, int Background, int Transparent, int BitsPerPixel, 
					 int *Red, int *Green, int *Blue)
{
	int B;
	int RWidth, RHeight;
	int LeftOfs, TopOfs;
	int Resolution;
	int ColorMapSize;
	int InitCodeSize;
	int i;
	
	Interlace = GInterlace;
	
	ColorMapSize = 1 << BitsPerPixel;
	
	RWidth = Width = GWidth;
	RHeight = Height = GHeight;
	LeftOfs = TopOfs = 0;
	
	Resolution = BitsPerPixel;
	
	/*
	* Calculate number of bits we are expecting
	*/
	CountDown =(long)Width *(long)Height;
	
	/*
	* Indicate which pass we are on(if interlace)
	*/
	Pass = 0;
	
	/*
	* The initial code size
	*/
	if( BitsPerPixel <= 1 )
		InitCodeSize = 2;
	else
		InitCodeSize = BitsPerPixel;
	
        /*
		* Set up the current x and y position
	*/
	curx = cury = 0;
	
	/*
	* Write the Magic header
	*/
	Putbyte('G');Putbyte('I');Putbyte('F');
	Putbyte('8');Putbyte(Transparent < 0?'7':'9');Putbyte('a');
	
	/*
	* Write out the screen width and height
	*/
	Putword( RWidth);
	Putword( RHeight);
	
	/*
	* Indicate that there is a global colour map
	*/
	B = 0x80;       /* Yes, there is a color map */
	
					/*
					* OR in the resolution
	*/
	B |=(Resolution - 1) << 5;
	
	/*
	* OR in the Bits per Pixel
	*/
	B |=(BitsPerPixel - 1);
	
	/*
	* Write it out
	*/
	Putbyte(B);
	
	/*
	* Write out the Background colour
	*/
	Putbyte(Background);
	
	/*
	* Byte of 0's(future expansion)
	*/
	Putbyte(0);
	
	/*
	* Write out the Global Colour Map
	*/
	for( i=0; i<ColorMapSize; ++i ) {
		Putbyte( Red[i]);
		Putbyte( Green[i]);
		Putbyte( Blue[i]);
	}
	
	/*
	* Write out extension for transparent colour index, if necessary.
	*/
	if( Transparent >= 0 ) {
		Putbyte( '!');
		Putbyte( 0xf9);
		Putbyte( 4);
		Putbyte( 1);
		Putbyte( 0);
		Putbyte( 0);
		Putbyte((unsigned char) Transparent);
		Putbyte( 0);
	}
	
	/*
	* Write an Image separator
	*/
	Putbyte( ',');
	
	/*
	* Write the Image header
	*/
	
	Putword( LeftOfs);
	Putword( TopOfs);
	Putword( Width);
	Putword( Height);
	
	/*
	* Write out whether or not the image is interlaced
	*/
	if( Interlace )
		Putbyte( 0x40);
	else
		Putbyte( 0x00);
	
        /*
		* Write out the initial code size
	*/
	Putbyte( InitCodeSize);
	
	/*
	* Go and actually compress the data
	*/
	compress( InitCodeSize+1 );
	
	/*
	* Write out a Zero-length packet(to end the series)
	*/
	Putbyte( 0);
	
	/*
	* Write the GIF file terminator
	*/
	Putbyte( ';');

	return buf;
}

/*
* Write out a byte to the GIF file
*/
void 
gdGifEncoder::Putbyte(unsigned char c) {
	buf.append(&c, 1);
}
/*
* Write out a word to the GIF file
*/
void
gdGifEncoder::Putword(int w)
{
	unsigned char b0=w & 0xff;
	unsigned char b1=w >> 8;
	buf.append(&b0, 1);
	buf.append(&b1, 1);
}

void gdGifEncoder::Write(void *abuf, size_t size) {
	buf.append((unsigned char*)abuf, size);
}

/***************************************************************************
*
*  GIFCOMPR.C       - GIF Image compression routines
*
*  Lempel-Ziv compression based on 'compress'.  GIF modifications by
*  David Rowley(mgardi@watdcsu.waterloo.edu)
*
***************************************************************************/

/*
* General DEFINEs
*/

#define GIFBITS    12

#ifdef NO_UCHAR
typedef char   char_type;
#else
typedef        unsigned char   char_type;
#endif

/*
*
* GIF Image compression - modified 'compress'
*
* Based on: compress.c - File compression ala IEEE Computer, June 1984.
*
* By Authors:  Spencer W. Thomas     (decvax!harpo!utah-cs!utah-gr!thomas)
*              Jim McKie             (decvax!mcvax!jim)
*              Steve Davies          (decvax!vax135!petsd!peora!srd)
*              Ken Turkowski         (decvax!decwrl!turtlevax!ken)
*              James A. Woods        (decvax!ihnp4!ames!jaw)
*              Joe Orost             (decvax!vax135!petsd!joe)
*
*/
#ifdef COMPATIBLE               /* But wrong! */
# define MAXCODE(n_bits)      ((code_int) 1 <<(n_bits) - 1)
#else
# define MAXCODE(n_bits)      (((code_int) 1 <<(n_bits)) - 1)
#endif

#define HashTabOf(i)       htab[i]
#define CodeTabOf(i)    codetab[i]
/*
* To save much memory, we overlay the table used by compress() with those
* used by decompress().  The tab_prefix table is the same size and type
* as the codetab.  The tab_suffix table needs 2**GIFBITS characters.  We
* get this from the beginning of htab.  The output stack uses the rest
* of htab, and contains characters.  There is plenty of room for any
* possible stack(stack used to be 8000 characters).
*/

#define tab_prefixof(i) CodeTabOf(i)
#define tab_suffixof(i)      ((char_type*)(htab))[i]
#define de_stack             ((char_type*)&tab_suffixof((code_int)1<<GIFBITS))

/*
* compress stdin to stdout
*
* Algorithm:  use open addressing double hashing(no chaining) on the
* prefix code / next character combination.  We do a variant of Knuth's
* algorithm D(vol. 3, sec. 6.4) along with G. Knott's relatively-prime
* secondary probe.  Here, the modular division first probe is gives way
* to a faster exclusive-or manipulation.  Also do block compression with
* an adaptive reset, whereby the code table is cleared when the compression
* ratio decreases, but after the table fills.  The variable-length output
* codes are re-sized at this point, and a special CLEAR code is generated
* for the decompressor.  Late addition:  construct the table according to
* file size for noticeable speed improvement on small files.  Please direct
* questions about this implementation to ames!jaw.
*/

void
gdGifEncoder::compress(int init_bits)
{
    register long fcode;
    register code_int i /* = 0 */;
    register int c;
    register code_int ent;
    register code_int disp;
    register code_int hsize_reg;
    register int hshift;
	
    /*
	* Set up the globals:  g_init_bits - initial number of bits
	*                      g_outfile   - pointer to output file
	*/
    g_init_bits = init_bits;
	
    /*
	* Set up the necessary values
	*/
    offset = 0;
    out_count = 0;
    clear_flg = 0;
    in_count = 1;
    maxcode = MAXCODE(n_bits = g_init_bits);
	
    ClearCode =(1 <<(init_bits - 1));
    EOFCode = ClearCode + 1;
    free_ent = ClearCode + 2;
	
    char_init();
	
    ent = GIFNextPixel( );
	
    hshift = 0;
    for( fcode =(long) hsize;  fcode < 65536L; fcode *= 2L )
        ++hshift;
    hshift = 8 - hshift;                /* set hash code range bound */
	
    hsize_reg = hsize;
    cl_hash((count_int) hsize_reg);            /* clear hash table */
	
    output((code_int)ClearCode );
	
#ifdef SIGNED_COMPARE_SLOW
    while((c = GIFNextPixel( )) !=(unsigned) EOF ) {
#else
	while((c = GIFNextPixel( )) != EOF ) {  /* } */
#endif
		
	++in_count;
	
	fcode =(long)(((long) c << maxbits) + ent);
	i =(((code_int)c << hshift) ^ ent);    /* xor hashing */
	
	if( HashTabOf(i) == fcode ) {
		ent = CodeTabOf(i);
		continue;
	} else if((long)HashTabOf(i) < 0 )      /* empty slot */
		goto nomatch;
	disp = hsize_reg - i;           /* secondary hash(after G. Knott) */
	if( i == 0 )
		disp = 1;
probe:
	if((i -= disp) < 0 )
		i += hsize_reg;
	
	if( HashTabOf(i) == fcode ) {
		ent = CodeTabOf(i);
		continue;
	}
	if((long)HashTabOf(i) > 0 )
		goto probe;
nomatch:
	output((code_int) ent );
	++out_count;
	ent = c;
#ifdef SIGNED_COMPARE_SLOW
	if((unsigned) free_ent <(unsigned) maxmaxcode) {
#else
		if( free_ent < maxmaxcode ) {  /* } */
#endif
			CodeTabOf(i) = free_ent++; /* code -> hashtable */
			HashTabOf(i) = fcode;
		} else
			cl_block();
	}
	/*
	* Put out the final code.
	*/
	output((code_int)ent );
	++out_count;
	output((code_int) EOFCode );
}

/*****************************************************************
* TAG( output )
*
* Output the given code.
* Inputs:
*      code:   A n_bits-bit integer.  If == -1, then EOF.  This assumes
*              that n_bits =<(long)wordsize - 1.
* Outputs:
*      Outputs code to the file.
* Assumptions:
*      Chars are 8 bits long.
* Algorithm:
*      Maintain a GIFBITS character long buffer(so that 8 codes will
* fit in it exactly).  Use the VAX insv instruction to insert each
* code in turn.  When the buffer fills up empty it and start over.
*/

static unsigned long masks[] = { 0x0000, 0x0001, 0x0003, 0x0007, 0x000F,
	0x001F, 0x003F, 0x007F, 0x00FF,
	0x01FF, 0x03FF, 0x07FF, 0x0FFF,
	0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF };

void
	gdGifEncoder::output(code_int code)
{
	cur_accum &= masks[ cur_bits ];
	
	if( cur_bits > 0 )
		cur_accum |=((long)code << cur_bits);
	else
		cur_accum = code;
	
	cur_bits += n_bits;
	
	while( cur_bits >= 8 ) {
		char_out((unsigned int)(cur_accum & 0xff) );
		cur_accum >>= 8;
		cur_bits -= 8;
	}
	
	/*
	* If the next entry is going to be too big for the code size,
	* then increase it, if possible.
	*/
	if( free_ent > maxcode || clear_flg ) {
		
		if( clear_flg ) {
			
			maxcode = MAXCODE(n_bits = g_init_bits);
			clear_flg = 0;
			
		} else {
			
			++n_bits;
			if( n_bits == maxbits )
				maxcode = maxmaxcode;
			else
				maxcode = MAXCODE(n_bits);
		}
	}
	
	if( code == EOFCode ) {
	/*
	* At EOF, write the rest of the buffer.
		*/
		while( cur_bits > 0 ) {
			char_out((unsigned int)(cur_accum & 0xff) );
			cur_accum >>= 8;
			cur_bits -= 8;
		}
		
		flush_char();
			}
}

/*
* Clear out the hash table
*/
void
	gdGifEncoder::cl_block(void)             /* table clear for block compress */
{
	
	cl_hash((count_int) hsize );
	free_ent = ClearCode + 2;
	clear_flg = 1;
	
	output((code_int)ClearCode );
}

void
	gdGifEncoder::cl_hash(count_int hsize)          /* reset code table */
	
{
	
	register count_int *htab_p = htab+hsize;
	
	register long i;
	register long m1 = -1;
	
	i = hsize - 16;
	do {                            /* might use Sys V memset(3) here */
        *(htab_p-16) = m1;
        *(htab_p-15) = m1;
        *(htab_p-14) = m1;
        *(htab_p-13) = m1;
        *(htab_p-12) = m1;
        *(htab_p-11) = m1;
        *(htab_p-10) = m1;
        *(htab_p-9) = m1;
        *(htab_p-8) = m1;
        *(htab_p-7) = m1;
        *(htab_p-6) = m1;
        *(htab_p-5) = m1;
        *(htab_p-4) = m1;
        *(htab_p-3) = m1;
        *(htab_p-2) = m1;
        *(htab_p-1) = m1;
        htab_p -= 16;
	} while((i -= 16) >= 0);
	
	for( i += 16; i > 0; --i )
        *--htab_p = m1;
}

/******************************************************************************
*
* GIF Specific routines
*
******************************************************************************/

/*
* Set up the 'byte output' routine
*/
void
	gdGifEncoder::char_init(void)
{
	a_count = 0;
}

/*
* Add a character to the end of the current packet, and if it is 254
* characters, flush the packet to disk.
*/
void
	gdGifEncoder::char_out(int c)
{
	accum[ a_count++ ] = c;
	if( a_count >= 254 )
        flush_char();
}

/*
* Flush the packet to disk, and reset the accumulator
*/
void
	gdGifEncoder::flush_char(void)
{
	if( a_count > 0 ) {
        Putbyte( a_count );
        Write( accum, a_count);
        a_count = 0;
	}
}

gdGifEncoder::gdGifEncoder(gdImage& aim): 
	im(aim) {
	/* Some of these are properly initialized later. What I'm doing
	here is making sure code that depends on C's initialization
	of statics doesn't break when the code gets called more
	than once. */
	Width = 0;
	Height = 0;
	curx = 0;
	cury = 0;
	CountDown = 0;
	Pass = 0;
	Interlace = 0;
	a_count = 0;
	cur_accum = 0;
	cur_bits = 0;
	g_init_bits = 0;
	ClearCode = 0;
	EOFCode = 0;
	free_ent = 0;
	clear_flg = 0;
	offset = 0;
	in_count = 1;
	out_count = 0;	
	hsize = HSIZE;
	n_bits = 0;
	maxbits = GIFBITS;
	maxcode = 0;
	maxmaxcode =(code_int)1 << GIFBITS;
}


/* +-------------------------------------------------------------------+ */
/* | Copyright 1990, 1991, 1993, David Koblas.(koblas@netcom.com)    | */
/* |   Permission to use, copy, modify, and distribute this software   | */
/* |   and its documentation for any purpose and without fee is hereby | */
/* |   granted, provided that the above copyright notice appear in all | */
/* |   copies and that both that copyright notice and this permission  | */
/* |   notice appear in supporting documentation.  This software is    | */
/* |   provided "as is" without express or implied warranty.           | */
/* +-------------------------------------------------------------------+ */


#define        MAXCOLORMAPSIZE         256
	
#ifndef TRUE
#define        TRUE    1
#endif
#ifndef FALSE
#define        FALSE   0
#endif
	
#define CM_RED         0
#define CM_GREEN       1
#define CM_BLUE                2
	
#define        MAX_LWZ_BITS            12
	
#define INTERLACE              0x40
#define LOCALCOLORMAP  0x80
#define BitSet(byte, bit)    (((byte) &(bit)) ==(bit))
	
#define        ReadOK(file,buffer,len)(fread(buffer, len, 1, file) != 0)
	
#define LM_to_uint(a,b)                      (((b)<<8)|(a))
	
	/* We may eventually want to use this information, but def it out for now */
#if 0
	struct GifScreen {
		unsigned int    Width;
		unsigned int    Height;
		unsigned char   ColorMap[3][MAXCOLORMAPSIZE];
		unsigned int    BitPixel;
		unsigned int    ColorResolution;
		unsigned int    Background;
		unsigned int    AspectRatio;
	};
#endif

/// Graphic Control Extension struct
#ifndef DOXYGEN
struct Gif89 {
	int     transparent;
	int     delayTime;
	int     inputFlag;
	int     disposal;
};
#endif
static int ReadColorMap(FILE *fd, int number, unsigned char(*buffer)[256]);

bool gdImage::CreateFromGif(FILE *fd)
{
	int imageNumber;
	int BitPixel;
	int Transparent =(-1);
	unsigned char   buf[16];
	unsigned char   c;
	unsigned char   ColorMap[3][MAXCOLORMAPSIZE];
	unsigned char   localColorMap[3][MAXCOLORMAPSIZE];
	int             imw, imh;
	int             useGlobalColormap;
	int             bitPixel;
	int             imageCount = 0;
	char            version[4];
	ZeroDataBlock = FALSE;
	
	imageNumber = 1;
	if(! ReadOK(fd,buf,6)) {
		return false;
	}
	if(strncmp((char *)buf,"GIF",3) != 0) {
		return false;
	}
	strncpy(version,(char *)buf + 3, 3);
	version[3] = '\0';
	
	if((strcmp(version, "87a") != 0) &&(strcmp(version, "89a") != 0)) {
		return false;
	}
	if(! ReadOK(fd,buf,7)) {
		return false;
	}
	BitPixel        = 2<<(buf[4]&0x07);
	
	if(BitSet(buf[4], LOCALCOLORMAP)) {    /* Global Colormap */
		if(ReadColorMap(fd, BitPixel, ColorMap)) {
			return false;
		}
	}
	for(;;) {
		if(! ReadOK(fd,&c,1)) {
			return false;
		}
		if(c == ';') {         /* GIF terminator */
			int i;
			if(imageCount < imageNumber) {
				return false;
			}
			/* Check for open colors at the end, so
			we can reduce colorsTotal and ultimately
			BitsPerPixel */
			for(i=((colorsTotal-1));(i>=0); i--) {
				if(open[i]) {
					colorsTotal--;
				} else {
					break;
				}
			} 
			return true;
		}
		
		if(c == '!') {         /* Extension */
			if(! ReadOK(fd,&c,1)) {
				return false;
			}
			DoExtension(fd, c, &Transparent);
			continue;
		}
		
		if(c != ',') {         /* Not a valid start character */
			continue;
		}
		
		++imageCount;
		
		if(! ReadOK(fd,buf,9)) {
			return false;
		}
		
		useGlobalColormap = ! BitSet(buf[8], LOCALCOLORMAP);
		
		bitPixel = 1<<((buf[8]&0x07)+1);
		
		imw = LM_to_uint(buf[4],buf[5]);
		imh = LM_to_uint(buf[6],buf[7]);
		Create(imw, imh);
		interlace = BitSet(buf[8], INTERLACE);
		if(! useGlobalColormap) {
			if(ReadColorMap(fd, bitPixel, localColorMap)) { 
				return false;
			}
			ReadImage(fd, imw, imh, localColorMap, 
				BitSet(buf[8], INTERLACE), 
				imageCount != imageNumber);
		} else {
			ReadImage(fd, imw, imh,
				ColorMap, 
				BitSet(buf[8], INTERLACE), 
				imageCount != imageNumber);
		}
		if(Transparent !=(-1)) {
			SetColorTransparent(Transparent);
		}	   
	}
}

static int
ReadColorMap(FILE *fd, int number, unsigned char(*buffer)[256])
{
	int             i;
	unsigned char   rgb[3];
	
	
	for(i = 0; i < number; ++i) {
		if(! ReadOK(fd, rgb, sizeof(rgb))) {
			return TRUE;
		}
		buffer[CM_RED][i] = rgb[0] ;
		buffer[CM_GREEN][i] = rgb[1] ;
		buffer[CM_BLUE][i] = rgb[2] ;
	}
	
	
	return FALSE;
}

int gdImage::DoExtension(FILE *fd, int label, int *Transparent)
{
	static unsigned char     buf[256];
	
	switch(label) {
	case 0xf9: {             /* Graphic Control Extension */
		(void) GetDataBlock(fd,(unsigned char*) buf);
		if((buf[0] & 0x1) != 0)
			*Transparent = buf[3];
		
		while(GetDataBlock(fd,(unsigned char*) buf) != 0)
			;
		return FALSE;
		}
	default:
		break;
	}
	while(GetDataBlock(fd,(unsigned char*) buf) != 0)
		;
	
	return FALSE;
}

int
gdImage::GetDataBlock(FILE *fd, unsigned char *buf)
{
	unsigned char   count;
	
	if(! ReadOK(fd,&count,1)) {
		return -1;
	}
	
	ZeroDataBlock = count == 0;
	
	if((count != 0) &&(! ReadOK(fd, buf, count))) {
		return -1;
	}
	
	return count;
}

int gdImage::GetCode(FILE *fd, int code_size, int flag)
{
	static unsigned char    buf[280];
	static int              curbit, lastbit, done, last_byte;
	int                     i, j, ret;
	unsigned char           count;
	
	if(flag) {
		curbit = 0;
		lastbit = 0;
		done = FALSE;
		return 0;
	}
	
	if((curbit+code_size) >= lastbit) {
		if(done) {
			if(curbit >= lastbit) {
				/* Oh well */
			}                        
			return -1;
		}
		buf[0] = buf[last_byte-2];
		buf[1] = buf[last_byte-1];
		
		if((count = GetDataBlock(fd, &buf[2])) == 0)
			done = TRUE;
		
		last_byte = 2 + count;
		curbit =(curbit - lastbit) + 16;
		lastbit =(2+count)*8 ;
	}
	
	ret = 0;
	for(i = curbit, j = 0; j < code_size; ++i, ++j)
		ret |=((buf[ i / 8 ] &(1 <<(i % 8))) != 0) << j;
	
	curbit += code_size;
	
	return ret;
}

int gdImage::LWZReadByte(FILE *fd, int flag, int input_code_size)
{
	static int      fresh = FALSE;
	int             code, incode;
	static int      code_size, set_code_size;
	static int      max_code, max_code_size;
	static int      firstcode, oldcode;
	static int      clear_code, end_code;
	static int      table[2][(1<< MAX_LWZ_BITS)];
	static int      stack[(1<<(MAX_LWZ_BITS))*2], *sp;
	register int    i;
	
	if(flag) {
		set_code_size = input_code_size;
		code_size = set_code_size+1;
		clear_code = 1 << set_code_size ;
		end_code = clear_code + 1;
		max_code_size = 2*clear_code;
		max_code = clear_code+2;
		
		GetCode(fd, 0, TRUE);
		
		fresh = TRUE;
		
		for(i = 0; i < clear_code; ++i) {
			table[0][i] = 0;
			table[1][i] = i;
		}
		for(; i <(1<<MAX_LWZ_BITS); ++i)
			table[0][i] = table[1][0] = 0;
		
		sp = stack;
		
		return 0;
	} else if(fresh) {
		fresh = FALSE;
		do {
			firstcode = oldcode =
				GetCode(fd, code_size, FALSE);
		} while(firstcode == clear_code);
		return firstcode;
	}
	
	if(sp > stack)
		return *--sp;
	
	while((code = GetCode(fd, code_size, FALSE)) >= 0) {
		if(code == clear_code) {
			for(i = 0; i < clear_code; ++i) {
				table[0][i] = 0;
				table[1][i] = i;
			}
			for(; i <(1<<MAX_LWZ_BITS); ++i)
				table[0][i] = table[1][i] = 0;
			code_size = set_code_size+1;
			max_code_size = 2*clear_code;
			max_code = clear_code+2;
			sp = stack;
			firstcode = oldcode =
				GetCode(fd, code_size, FALSE);
			return firstcode;
		} else if(code == end_code) {
			int             count;
			unsigned char   buf[260];
			
			if(ZeroDataBlock)
				return -2;
			
			while((count = GetDataBlock(fd, buf)) > 0)
				;
			
			if(count != 0)
				return -2;
		}
		
		incode = code;
		
		if(code >= max_code) {
			*sp++ = firstcode;
			code = oldcode;
		}
		
		while(code >= clear_code) {
			*sp++ = table[1][code];
			if(code == table[0][code]) {
				/* Oh well */
			}
			code = table[0][code];
		}
		
		*sp++ = firstcode = table[1][code];
		
		if((code = max_code) <(1<<MAX_LWZ_BITS)) {
			table[0][code] = oldcode;
			table[1][code] = firstcode;
			++max_code;
			if((max_code >= max_code_size) &&
				(max_code_size <(1<<MAX_LWZ_BITS))) {
				max_code_size *= 2;
				++code_size;
			}
		}
		
		oldcode = incode;
		
		if(sp > stack)
			return *--sp;
	}
	return code;
}

void gdImage::ReadImage(FILE *fd, int len, int height, unsigned char(*cmap)[256], int interlace, int ignore)
{
	unsigned char   c;      
	int             v;
	int             xpos = 0, ypos = 0, pass = 0;
	int i;
	/* Stash the color map into the image */
	for(i=0;(i<gdMaxColors); i++) {
		red[i] = cmap[CM_RED][i];	
		green[i] = cmap[CM_GREEN][i];	
		blue[i] = cmap[CM_BLUE][i];	
		open[i] = 1;
	}
	/* Many(perhaps most) of these colors will remain marked open. */
	colorsTotal = gdMaxColors;
	/*
	**  Initialize the Compression routines
	*/
	if(! ReadOK(fd,&c,1)) {
		return; 
	}
	if(LWZReadByte(fd, TRUE, c) < 0) {
		return;
	}
	
	/*
	**  If this is an "uninteresting picture" ignore it.
	*/
	if(ignore) {
		while(LWZReadByte(fd, FALSE, c) >= 0)
			;
		return;
	}
	
	while((v = LWZReadByte(fd,FALSE,c)) >= 0 ) {
		/* This how we recognize which colors are actually used. */
		if(open[v]) {
			open[v] = 0;
		}
		SetPixel(xpos, ypos, v);
		++xpos;
		if(xpos == len) {
			xpos = 0;
			if(interlace) {
				switch(pass) {
				case 0:
				case 1:
					ypos += 8; break;
				case 2:
					ypos += 4; break;
				case 3:
					ypos += 2; break;
				}
				
				if(ypos >= height) {
					++pass;
					switch(pass) {
					case 1:
						ypos = 4; break;
					case 2:
						ypos = 2; break;
					case 3:
						ypos = 1; break;
					default:
						goto fini;
					}
				}
			} else {
				++ypos;
			}
		}
		if(ypos >= height)
			break;
	}
	
fini:
	if(LWZReadByte(fd,FALSE,c)>=0) {
		/* Ignore extra */
	}
}


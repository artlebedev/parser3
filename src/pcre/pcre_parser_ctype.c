/** @file
	Parser: @b $MAIN:CTYPE character types for Perl-Compatible Regular Expressions

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pcre_parser_ctype.c,v 1.4 2001/08/20 13:22:05 parser Exp $


Perl-Compatible Regular ExpressionsThis is a support program to generate part of auto.p, containing
character tables of various kinds. They are built according to specified
locale and used as the default tables by PCRE. Now that pcre_maketables is
a function visible to the outside world, we make use of its code from here in
order to be consistent.
 

PCRE is a library of functions to support regular expressions whose syntax
and semantics are as close as possible to those of the Perl 5 language.

 Written by: Philip Hazel <ph10@cam.ac.uk>
 
  Copyright (c) 1997-1999 University of Cambridge
  
   -----------------------------------------------------------------------------
   Permission is granted to anyone to use this software for any purpose on any
   computer system, and to redistribute it freely, subject to the following
   restrictions:
   
	1. This software is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
	
	 2. The origin of this software must not be misrepresented, either by
	 explicit claim or by omission.
	 
	  3. Altered versions must be plainly marked as such, and must not be
	  misrepresented as being the original software.
	  
	   4. If PCRE is embedded in any software that is released under the GNU
	   General Purpose Licence (GPL), then the terms of that licence shall
	   supersede any condition above with which it is incompatible.
	   -----------------------------------------------------------------------------
	   
		See the file Tech.Notes for some information on the internals.
*/

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#define DFTABLES
#include "internal.h"
#include "maketables.c"

#define TABLE_WRAP_COL 62

size_t out_c(int c) {
	return printf(
		isprint(c)&&
		!isspace(c) &&
		!(c>=128&&c<192)?
		strchr("^$;()[]{}\"", c)==0?"%c":"^%c":"^#%02X", c);
}

void ctype_out(unsigned const char *bit_table, 
			   unsigned char bit, 
			   const char *name) {
	int c;
	int printed=0;

	printf("    ");
	printf("$.%s[", name);
	for(c=0; c<0x100; c++)
		if(bit_table[c] & bit) {
			if(!printed || printed > TABLE_WRAP_COL) {
				printf("\n        "); printed=0;
			}
			printed+=out_c(c);
		}
	printf("]\n");
}

void case_out(unsigned const char *case_table, const char *name) {
	int c;
	int printed=0;
	printf("    ");
	printf("$.%s[", name);
	for(c=0; c<0x100; c++)
		if(case_table[c] != c) {
			if(!printed || printed > TABLE_WRAP_COL) {
				printf("\n        "); printed=0;
			}
			printed+=out_c(c);
			printed+=out_c(case_table[c]);
		}
	printf("]\n");
}

int main(int argc, char *argv[])
{
	unsigned const char *tables, *ctypes_table, *case_table;

	if(argc<2) {
		printf("Usage: %s Russian_Russia.1251   (locale-name)\n", argv[0]?argv[0]:"pcre_parser_ctype");
		return 1;
	}

	// Russian_Russia.1251
	if(!setlocale(LC_CTYPE, argv[1])) {
		printf("locale '%s' is invalid", argv[1]);
		return 2;
	}
	tables = pcre_maketables();
	ctypes_table=tables+ctypes_offset;
	case_table=tables+lcc_offset;
	
	printf("$CTYPE[\n");
	
	ctype_out(ctypes_table, ctype_space, "white-space");
	ctype_out(ctypes_table, ctype_digit, "digit");
	ctype_out(ctypes_table, ctype_xdigit, "hex-digit");
	ctype_out(ctypes_table, ctype_letter, "letter");
	ctype_out(ctypes_table, ctype_word, "word");
	printf("\n");
	case_out(case_table, "lowercase");

	printf("]\n\n");
	return 0;
}

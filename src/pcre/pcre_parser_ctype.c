/** @file
	Parser: @b $MAIN:CTYPE character types for Perl-Compatible Regular Expressions

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)


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
static const char *RCSId="$Id: pcre_parser_ctype.c,v 1.8 2002/02/08 07:27:49 paf Exp $"; 

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#define DFTABLES
#include "internal.h"
#include "maketables.c"

#define TABLE_WRAP_COL 62

const char *out_c(char *buf, int c) {
	sprintf(buf,
		isprint(c)&&
		!isspace(c) &&
		c!='#' &&
		c!='"' &&
		c<128?
		"%c":"0x%02X", c);
	return buf;
}

#define MAX_UCM 2
unsigned int ucm[0x100][MAX_UCM];

int read_ucm(const char *file) {
	unsigned int unicode;
	unsigned int ch;
	char buf[0x400];
	int i;
	int ok;

	FILE *f=fopen(file, "rt");
	
	if(!f)
		return 1;

	memset(ucm, 0, sizeof(ucm));

	while(!feof(f)) {
		fgets(buf, sizeof(buf), f);
		if(sscanf(buf, "<U%04X> \\x%02X", &unicode, &ch)) {
			ok=0;
			for(i=0; i<MAX_UCM; i++)
				if(ucm[ch][i]==0) {
					ucm[ch][i]=unicode;
					ok=1;
					break;
				}
			if(!ok)
				return 2;
		}
	}

	fclose(f);
	return 0;
}

int main(int argc, char *argv[])
{
	unsigned const char *tables, *ctypes_table, *case_table;
	unsigned int c;
	char buf_c[0x100];
	char buf_lc[0x100];
	int error;
	int i, printed;

	if(argc<3) {
		printf("Usage: %s Russian_Russia.1251 ibm-1251.ucm  (locale-name)\n", argv[0]?argv[0]:"pcre_parser_ctype");
		return 1;
	}

	// Russian_Russia.1251
	if(!setlocale(LC_CTYPE, argv[1])) {
		printf("locale '%s' is invalid", argv[1]);
		return 2;
	}
	// ibm-1251.ucm
	error=read_ucm(argv[2]);
	if(error) {
		switch(error) {
		case 1:
			printf("could not read '%s'", argv[2]);
			break;
		case 2:
			printf("could store all unicode variants of  '%s'", argv[2]);
			break;
		default: 
			printf("problem with '%s'", argv[2]);
			break;
		}
		return 3;
	}

	tables = pcre_maketables();
	ctypes_table=tables+ctypes_offset;
	case_table=tables+lcc_offset;
	
	printf(
		"char\t"
		"white-space\t"
		"digit\t"
		"hex-digit\t"
		"letter\t"
		"word\t"
		"lowercase\t"
		"unicode1\t"
		"unicode2\t"
		"\n");
	
	for(c=0; c<0x100; c++)
		if(
			ctypes_table[c]&(ctype_space|ctype_digit|ctype_xdigit|ctype_letter|ctype_word) || 
			c!=case_table[c] || 
			!(c==ucm[c][0] && ucm[c][1]==0)) {
			printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t", 
				out_c(buf_c, c),
				ctypes_table[c]&ctype_space?"x":"",
				ctypes_table[c]&ctype_digit?"x":"",
				ctypes_table[c]&ctype_xdigit?"x":"",
				ctypes_table[c]&ctype_letter?"x":"",
				ctypes_table[c]&ctype_word?"x":"",
				case_table[c]&&c!=case_table[c]?out_c(buf_lc, case_table[c]):""				
			);
			if(!(c==ucm[c][0] && ucm[c][1]==0)) {
				printed=0;
				for(i=0; i<MAX_UCM; i++) {
					if(ucm[c][i]) {
						if(printed!=0)
							printf("\t");
						printf("0x%04X", ucm[c][i]);
						printed++;
					}
				}
			}
			printf("\n");
		}

	return 0;
}

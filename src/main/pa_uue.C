/** @file
	Parser: uuencoding impl.

	Copyright(c) 2000,2001-2004 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	@todo setrlimit
*/

static const char * const IDENT_UUE_C="$Date: 2004/02/17 10:37:20 $";

#include "pa_config_includes.h"

#include "pa_uue.h"

static unsigned char uue_table[64] = {
  '`', '!', '"', '#', '$', '%', '&', '\'',
  '(', ')', '*', '+', ',', '-', '.', '/',
  '0', '1', '2', '3', '4', '5', '6', '7',
  '8', '9', ':', ';', '<', '=', '>', '?',
  '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
  'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
  'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
  'X', 'Y', 'Z', '[', '\\',']', '^', '_'
};
void pa_uuencode(String& result, const String& file_name, const VFile& vfile) {
	//header
	result << "content-transfer-encoding: x-uuencode\n" << "\n";
	result << "begin 644 " << file_name << "\n";

	//body
	const unsigned char *in=(const unsigned char *)vfile.value_ptr();
	size_t in_length=vfile.value_size();

	int count=45;
	for(const unsigned char *itemp=in; itemp<(in+in_length); itemp+=count) {
		int index;	

		if((itemp+count)>(in+in_length)) 
			count=in_length-(itemp-in);

		char *buf=new(PointerFreeGC) char[MAX_STRING];
		char *optr=buf;
		
		/*
		* for UU and XX, encode the number of bytes as first character
		*/
		*optr++ = uue_table[count];
		
		for (index=0; index<=count-3; index+=3) {
			*optr++ = uue_table[itemp[index] >> 2];
			*optr++ = uue_table[((itemp[index  ] & 0x03) << 4) | (itemp[index+1] >> 4)];
			*optr++ = uue_table[((itemp[index+1] & 0x0f) << 2) | (itemp[index+2] >> 6)];
			*optr++ = uue_table[  itemp[index+2] & 0x3f];
		}
		
		/*
		* Special handlitempg for itempcomplete litempes
		*/
		if (index != count) {
			if (count - index == 2) {
				*optr++ = uue_table[itemp[index] >> 2];
				*optr++ = uue_table[((itemp[index  ] & 0x03) << 4) | 
					( itemp[index+1] >> 4)];
				*optr++ = uue_table[((itemp[index+1] & 0x0f) << 2)];
				*optr++ = uue_table[0];
			}
			else if (count - index == 1) {
				*optr++ = uue_table[ itemp[index] >> 2];
				*optr++ = uue_table[(itemp[index] & 0x03) << 4];
				*optr++ = uue_table[0];
				*optr++ = uue_table[0];
			}
		}
		/*
		* end of line
		*/
		*optr++ = '\n';	
		*optr = 0;
		result << buf;
	}
	
	//footer
	result<< "`\n"
		"end\n";
}

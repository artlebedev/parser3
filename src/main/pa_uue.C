/** @file
	Parser: uuencoding impl.

	Copyright(c) 2000-2009 ArtLebedev Group(http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)

	@todo setrlimit
*/

static const char * const IDENT_UUE_C="$Date: 2009/09/28 08:59:29 $";

#include "pa_config_includes.h"

#include "pa_uue.h"
#include "pa_memory.h"

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

const char* pa_uuencode(const unsigned char* in, size_t in_size, const char* file_name) {
	size_t new_size=((in_size / 3 + 1) * 4);
	new_size += 2 * new_size / 60/*chars in line + new lines*/ + 2;
	new_size += strlen(file_name) + 11/*header*/ + 6/*footer*/ + 1/*zero terminator*/;

	const char* result=new(PointerFreeGC) char[new_size];
	char* optr=(char*)result;

	//header
	optr += sprintf(optr, "begin 644 %s\n", file_name);

	//body
	int count=45;
	for(const unsigned char *itemp=in; itemp<(in+in_size); itemp+=count) {
		int index;	

		if((itemp+count)>(in+in_size)) 
			count=in_size-(itemp-in);

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
	}
	
	//footer
	optr += sprintf(optr, "`\nend\n");

	*optr = 0;

	//throw Exception(PARSER_RUNTIME, 0, "%d %d %d", in_size, new_size, (size_t)(optr-result));
	assert((size_t)(optr-result) < new_size);

	return result;
}

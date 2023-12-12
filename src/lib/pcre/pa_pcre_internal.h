/* Some internal stuff from PCRE library */
/* Author: Sergey B Kirpichev <skirpichev@gmail.com> */

#include "pa_config_includes.h"

/* Bit definitions for entries in the pcre_ctypes table. */

#define ctype_space   0x01
#define ctype_letter  0x02
#define ctype_digit   0x04
#define ctype_xdigit  0x08
#define ctype_word    0x10   /* alphanumeric or '_' */
#define ctype_meta    0x80   /* regexp meta char or zero (end pattern) */

/* Offsets for the bitmap tables in pcre_cbits. Each table contains a set
   of bits for a class map. Some classes are built by combining these tables. */

#define cbit_space     0      /* [:space:] or \s */
#define cbit_xdigit   32      /* [:xdigit:] */
#define cbit_digit    64      /* [:digit:] or \d */
#define cbit_upper    96      /* [:upper:] */
#define cbit_lower   128      /* [:lower:] */
#define cbit_word    160      /* [:word:] or \w */
#define cbit_graph   192      /* [:graph:] */
#define cbit_print   224      /* [:print:] */
#define cbit_punct   256      /* [:punct:] */
#define cbit_cntrl   288      /* [:cntrl:] */
#define cbit_length  320      /* Length of the cbits table */

/* Offsets of the various tables from the base tables pointer, and total length. */

#define lcc_offset      0
#define fcc_offset    256
#define cbits_offset  512
#define ctypes_offset (cbits_offset + cbit_length)
#define tables_length (ctypes_offset + 256)

/* Internal shared data tables. The data for these tables is in the
pa_pcre_chartables.c module. */

#ifdef __cplusplus
	extern "C" const unsigned char pa_pcre_default_tables[];
#else
	extern const unsigned char pa_pcre_default_tables[];
#endif

/* Internal function for validating UTF-8 character strings. The code
for this function is in the pcre_valid_utf8.c module. */

#ifdef __cplusplus
    extern "C" int pa_pcre_valid_utf(unsigned char *string, int length, int *erroroffset);
#else
    extern int pa_pcre_valid_utf(unsigned char *string, int length, int *erroroffset);
#endif

#ifdef HAVE_PCRE2
#define PCRE_UTF8_ERR0 0
#define PCRE_UTF8_ERR6 PCRE2_ERROR_UTF8_ERR6
#define PCRE_UTF8_ERR7 PCRE2_ERROR_UTF8_ERR7
#define PCRE_UTF8_ERR8 PCRE2_ERROR_UTF8_ERR8
#define PCRE_UTF8_ERR9 PCRE2_ERROR_UTF8_ERR9
#define PCRE_UTF8_ERR10 PCRE2_ERROR_UTF8_ERR10
#define PCRE_UTF8_ERR11 PCRE2_ERROR_UTF8_ERR11
#define PCRE_UTF8_ERR12 PCRE2_ERROR_UTF8_ERR12
#define PCRE_UTF8_ERR13 PCRE2_ERROR_UTF8_ERR13
#define PCRE_UTF8_ERR14 PCRE2_ERROR_UTF8_ERR14
#define PCRE_UTF8_ERR15 PCRE2_ERROR_UTF8_ERR15
#define PCRE_UTF8_ERR16 PCRE2_ERROR_UTF8_ERR16
#define PCRE_UTF8_ERR17 PCRE2_ERROR_UTF8_ERR17
#define PCRE_UTF8_ERR18 PCRE2_ERROR_UTF8_ERR18
#define PCRE_UTF8_ERR19 PCRE2_ERROR_UTF8_ERR19
#define PCRE_UTF8_ERR20 PCRE2_ERROR_UTF8_ERR20
#define PCRE_UTF8_ERR21 PCRE2_ERROR_UTF8_ERR21

#define PCRE_ERROR_BADUTF8_OFFSET PCRE2_ERROR_BADUTFOFFSET

#define PCRE_UTF8 PCRE2_UTF
#define PCRE_UCP PCRE2_UCP
#define PCRE_EXTENDED PCRE2_EXTENDED
#define PCRE_ERROR_NOMATCH PCRE2_ERROR_NOMATCH

#define PCRE_INFO_SIZE PCRE2_INFO_SIZE

#define PCRE_CASELESS PCRE2_CASELESS
#define PCRE_DOTALL PCRE2_DOTALL
#define PCRE_MULTILINE PCRE2_MULTILINE
#define PCRE_UNGREEDY PCRE2_UNGREEDY
#define PCRE_DOLLAR_ENDONLY PCRE2_DOLLAR_ENDONLY
#define PCRE_EXTRA 0

#define pcre_fullinfo(fcode, fextra, type, result) pcre2_pattern_info(fcode, type, result)

#endif

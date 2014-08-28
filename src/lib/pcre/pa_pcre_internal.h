/* Some internal stuff from PCRE library */
/* Author: Sergey B Kirpichev <skirpichev@gmail.com> */

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


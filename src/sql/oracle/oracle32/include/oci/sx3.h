/*
 * $Header: /home/cvsroot/parser3project/parser3/src/sql/oracle/oracle32/include/oci/Attic/sx3.h,v 1.1 2001/08/22 14:02:20 parser Exp $
 */

/* Copyright (c) Oracle Corporation 1997, 1998, 1999. All Rights Reserved. */ 
 
/* 
NAME 
  sx3.h - System dependent eXternal definitions for disk data structures.  
           (Solaris version).

DESCRIPTION
  System dependent definitions and declarations used in all Oracle disk 
  data structures. 

RELATED DOCUMENTS

INSPECTION STATUS
  Inspection date:
  Inspection status:
  Estimated increasing cost defects per page:
  Rule sets:

ACCEPTANCE REVIEW STATUS
  Review date:
  Review status:
  Reviewers:

PUBLIC FUNCTIONS
  None

PRIVATE FUNCTIONS
  None

EXAMPLES
  N/A

NOTES  

  This file contains system dependent definitions and declarations of
  data types used in on-disk structures. On-disk structures should be
  defined in separate header files and can only use data types defined
  in this file.  On-disk structures should not use any native types or
  any other type not defined in this file (but defined in sx.h).

  Separating the definitions of on-disk data types allows us to keep 
  them compatible across different versions. The key purpose of this 
  file is to restrict the data types which can go on disk and to make sure
  that their sizes remain the same across different versions. 
 
  The -MAXVAL and -MINVAL constants represent the numerical range of 
  the relevant data type on the current port, i.e. the value of such 
  symbol may change from one port to another.  The MIN-MAXVAL and 
  MAX-MINVAL constants represent the numerical range of the relevant 
  data type across all ports, i.e. the value of these should never be 
  changed by porters.  Note that MAXSB1MINVAL is -127, instead of 
  -128, to support one's ** complement arithmetic architectures.  


  DEVELOPERS: This file has been created to separate all the valid
  data types that can go on on-disk structures. All the types in
  this file are guaranteed to be of the same size across different
  versions, specifically while migrating from 32 bit to 64 bit. 
  This is very important to maintain the consistency of data on 
  the disk. All on-disk structures should be defined in separate
  header files (*3.h) and *3.h can only include other *3.h header
  files. New data types d*word, dptr_t, dboolean and dsize_t have been
  defined to be used in place of *word, ptr_t, boolean and size_t. All
  the existing references to *word, size_t, boolean and ptr_t in on-disk
  structures have to be replaced with d*word, dsize_t, dboolean and 
  dptr_t as word, size_t, boolean  and ptr_t could be of different sizes 
  in different versions. d*word, dsize_t, dboolean and dptr_t are currently 
  defined to have the same size as the existing type and are guaranteed to 
  remain the same size in all future versions, like in 64 bit versions.  

  The d*word, dsize_t, dboolean and dptr_t data types should never be used in
  any new data structures. Also dptr_t is defined as a non pointer type
  to preserve the size. So it should never be used to store a pointer. 
  It is provided as a place holder to preserve the size of the structure.  

  RELATED NOTE: Normal alignment macros like slalsu should never be used
  for on-disk data types. You should use a new set of alignment macros 
  defined in sl.h. Instead of slalsu, slalsd, slalpu and slalpd you should 
  use sldalsu, sldalsd, sldalpu and  sldalpd respectively.


  PRODUCT LINE DEVELOPERS: This file is solaris specific and
  product lines have to modify this file to reflect their platform. In
  this file the datatypes eb1, ub1, sb1, eb2, ub2, sb2, eb4, ub4, sb4
  must be defined so that they are always of same size for a
  particular platform across different versions. For eg: In solaris
  ub1, ub2, ub4 occupy 1, 2 and 4 bytes respectively. While porting to
  different versions for solaris (say to 64 bit), porters have to make
  sure that they always occupy 1,2 and 4 bytes. If the native data
  type associated with these types are changed to preserve the sizes,
  then literals MIN & MAXVALS have to be changed accordingly. While
  porting to 64 bit, only the definitions of *b1,*b2 and *b4 can be
  changed to preserve their sizes. All other datatypes must not be
  changed.

  dword, dsize_t, dboolean and dptr_t are defined to provide backward
  compatibility with word, size_t, boolean  and ptr_t. Their definitions 
  must be set such that their size is the same as existing word, size_t, 
  boolean and ptr_t and should never be changed in the future. For example
  in solaris ptr_t (defined as void *) has the same size as unsigned long
  which is 4 bytes. So dptr_t is defined as ub4 and as ub4's size will
  be constant in the future so will be dsize_t's size. The definition
  of d* should never be changed once they have been defined for a 
  particular product line.

   MODIFIED   (MM/DD/YY)
    tsaulys    02/10/99 - move obsolete types and macros
    dcolello   08/25/98 - WIN32COMMON: port
   kkarun     08/14/98 -  Rollback 667803
   tsaulys     07/31/98 - do not use text for C++ (when LUSEMFC defined)
   skabraha    05/08/98 - no more 1's complement support
   skabraha    04/17/98 - changing xb4 to int
   skabraha    01/27/98 - adding M_IDEN and SLMXFNMLEN
   skabraha    10/28/97 - Defining some datatypes to garbage
   skabraha    10/16/97 - Creation


*/


#ifndef  SX3_ORACLE
#define  SX3_ORACLE

#ifndef ORALIMITS
# include <limits.h>
# define ORALIMITS
#endif /* !ORALIMITS */

/*
** Note: this is the WIN32COMMON version of sx3.h.  Always make sure that
**   WIN32COMMON is defined.  It is not defined yet if this file is included
**   first for whatever reason.
*/
#ifndef WIN32COMMON
#define WIN32COMMON
#endif

#ifdef WIN32COMMON
/* make sure stdlib.h is included to avoid warning with max()/min() macros */
#ifndef ORASTDLIB
# include <stdlib.h>
# define ORASTDLIB
#endif
#endif /* WIN32COMMON */


/* All references to size_t has to be replaced with dsize_t. size_t should not
 * be used in any disk structures. To enforce this size_t is defined to
 * an invalid type here. It is redefined to a valid type in sx.h from the
 * included file stddef.h.
 */

#ifdef size_t 
# undef size_t 
#endif /* size_t */


#define size_t size_t_may_not_be_used_here

/* As olint accepts most of the datatypes on its own here we are #defining
 * the unwanted data types to garbage to catch its usage. These will be
 * #undefed in the beginning of sx.h.
 */

#ifndef SX3_GARBAGE_DEFS
# define SX3_GARBAGE_DEFS

# define     uword      uword_may_not_be_used_here
# define     sword      sword_may_not_be_used_here
# define  ubig_ora   ubig_ora_may_not_be_used_here
# define  sbig_ora   sbig_ora_may_not_be_used_here
# define       ub8        ub8_may_not_be_used_here
# define       sb8        sb8_may_not_be_used_here
# ifndef WIN32COMMON
#  define   boolean    boolean_may_not_be_used_here
# else /* WIN32COMMON */
#  undef boolean
# endif /* WIN32COMMON */
# define     ptr_t      ptr_t_may_not_be_used_here

#endif /* SX3_GARBAGE_DEFS */
  
/**-------------------------------------------------------------------------**/
/**                         DATA TYPE DEFINITIONS                           **/
/**-------------------------------------------------------------------------**/

/* We #define while linting as lint complains while casting between 
 * two datatypes which have been typedefed to compatible types.             
 */

/* xb1 definitions. xb1 will be exactly one byte - at least 8 bits,
 * possibly more. sizeof(ub1) is guaranteed to be 1
 */

#ifndef lint 
typedef unsigned char  ub1;                  /* use where unsigned important */ 
typedef   signed char  sb1;                  /* use where   signed important */ 
#else 
#define ub1 unsigned char 
#define sb1 signed char 
#endif /* LINT */ 
 
#define UB1MAXVAL ((ub1)UCHAR_MAX) 
#define UB1MINVAL ((ub1)        0) 
#define SB1MAXVAL ((sb1)SCHAR_MAX) 
#define SB1MINVAL ((sb1)SCHAR_MIN) 
#define MINUB1MAXVAL ((ub1)  255) 
#define MAXUB1MINVAL ((ub1)    0) 
#define MINSB1MAXVAL ((sb1)  127) 
#define MAXSB1MINVAL ((sb1) -127) 
 
 
/* xb2 definitions. xb2 will be atleast 2 bytes, possible more */ 
 
#ifndef lint 
typedef unsigned short    ub2;               /* use where unsigned important */ 
typedef   signed short    sb2;               /* use where   signed important */ 
#else 
#define ub2  unsigned short 
#define sb2  signed short 
#endif /* LINT */ 

#define UB2MAXVAL ((ub2)USHRT_MAX) 
#define UB2MINVAL ((ub2)        0) 
#define SB2MAXVAL ((sb2) SHRT_MAX) 
#define SB2MINVAL ((sb2) SHRT_MIN) 
#define MINUB2MAXVAL ((ub2) 65535) 
#define MAXUB2MINVAL ((ub2)     0) 
#define MINSB2MAXVAL ((sb2) 32767) 
#define MAXSB2MINVAL ((sb2)-32767) 
  

/* xb4 definitions. xb4 will be atleast 4 bytes, possibly more */

#ifndef lint 
typedef unsigned int  ub4;                  /* use where unsigned important */ 
typedef   signed int  sb4;                  /* use where   signed important */ 
#else 
#define ub4 unsigned int 
#define sb4 signed int 
#endif /* LINT */ 
 
#define UB4MAXVAL ((ub4)UINT_MAX) 
#define UB4MINVAL ((ub4)        0) 
#define SB4MAXVAL ((sb4) INT_MAX) 
#define SB4MINVAL ((sb4) INT_MIN) 
#define MINUB4MAXVAL ((ub4) 4294967295) 
#define MAXUB4MINVAL ((ub4)          0) 
#define MINSB4MAXVAL ((sb4) 2147483647) 
#define MAXSB4MINVAL ((sb4)-2147483647) 


/* number of bits in a byte */
#define UB1BITS          CHAR_BIT
#define UB1MASK          ((1 << ((uword)CHAR_BIT)) - 1)

/* This definition has been moved from s.h as it is used by on-disk 
 * data structures. The macros for bitvec are in s.h. 
 */
typedef ub1   bitvec;  
   
/* BITVEC - to size bitvec arrays */
#define BITVEC(n) (((n)+(UB1BITS-1))>>3) 
                               
/* human readable (printable) characters. oratext should always be one byte */
/*
 * Update for C++ design: change text to oratext
 * typedef  unsigned char text;
 */

#ifdef lint
# define oratext unsigned char
#else
  typedef  unsigned char oratext;
#endif

/* macros defining min & max */

#ifdef WIN32COMMON
#ifndef max
#define max(x, y)     (((x) < (y)) ?  (y) : (x))
#endif
#else
#define max(x, y)     (((x) < (y)) ?  (y) : (x))
#endif
#define min(x, y)     (((x) < (y)) ?  (x) : (y))
 
/**-------------------------------------------------------------------------**/
/** ALTERNATE DATA TYPES FOR THOSE OBSOLETED FOR ON_DISK DATA STRUCTURES    **/
/**-------------------------------------------------------------------------**/

/* Developers should replace all existing references to *word, ptr_t, boolean 
 * and size_t in disk data structures with d*word, dptr_t, dboolean and 
 * dsize_t respectively. These types are guaranteed to be of the same size   
 * across different versions on a platform.  
 *
 * d*word, dsize_t, dboolean and dptr_t should never be used in new data structures.
 * They should only be used to preserve the sizes of existing data
 * structures.
 *
 * WARNING: dptr_t has been defined to be a non-pointer datatype so that we 
 * can guarantee that the size remains same!!!  
 */ 


/* PRODUCT LINE DEVELOPERS: Change this typedef to a type which has the 
 * same size as the current size of *word in your platform. Once the size 
 * of d*word has been defined, it should always be of the same size in 
 * all future versions (irrespective of the size of *word in later versions)
 *
 * For example in Solaris uword is defined as having 4 bytes. Hence d*word is
 * defined as *b4. This definition will NOT be changed in the future, thus
 * preserving the size.
 */

/* deword is defined as eb4. As ebx is defined later in the obselete section
 * the definition of deword was moved there.
 */

#ifndef lint 
typedef         ub4      duword;                   
typedef         sb4      dsword;                 
/* word is an obselete datatype. But it still needs to be changed to dword */ 
typedef         dsword   dword;

#else 
#define duword ub4 
#define dsword sb4 
#define dword  dsword
#endif /*LINT */ 

#define  DUWORDMAXVAL       UB4MAXVAL 
#define  DUWORDMINVAL       UB4MINVAL 
#define  DSWORDMAXVAL       SB4MAXVAL 
#define  DSWORDMINVAL       SB4MINVAL 
#define  MINDUWORDMAXVAL    MINUB4MAXVAL 
#define  MAXDUWORDMINVAL    MAXUB4MINVAL 
#define  MINDSWORDMAXVAL    MINSB4MAXVAL 
#define  MAXDSWORDMINVAL    MAXSB4MINVAL 
#define  DWORDMAXVAL        DSWORDMAXVAL 
#define  DWORDMINVAL        DSWORDMINVAL 
   


/* PRODUCT LINE DEVELOPERS: Change this typedef to a type which has the 
 * same size as the current size of size_t in your platform. Once the size 
 * of dsize_t has been defined, it should always be of the same size in all
 * future versions (irrespective of the size of size_t in later versions) 
 *
 * For example in Solaris size_t is defined as unsigned int which is
 * the same as ub4. Hence it is defined as ub4.
 */

#ifndef lint  
typedef ub4 dsize_t;  
# else 
# define dsize_t ub4 
#endif /*LINT */  
 
# define DSIZE_TMAXVAL UB4MAXVAL           
# define MINDSIZE_TMAXVAL (dsize_t)65535 
 

/* PRODUCT LINE DEVELOPERS: Change this typedef to a type which has the 
 * same size as the current size of boolean in your platform. Once the size 
 * of dboolean has been defined, it should always be of the same size in all
 * future versions (irrespective of the size of boolean in later versions) 
 *
 * For example in Solaris boolean is defined as int which is 4 bytes. Hence 
 * dboolean is defined as sb4.
 */

#ifndef lint  
typedef sb4 dboolean;  
# else 
# define dboolean sb4 
#endif /*LINT */  
 
 
/*
 * PRODUCT LINE DEVELOPERS: Change this typedef to a type which has the 
 * same size as the size of ptr_t in the existing version. Once the size of 
 * dptr_t has been defined it should always be of the same size in all future
 * versions, irrrespective of the size of ptr_t in later versions.  
 * 
 * For example in Solaris the pointer is a 4 byte quantity and hence dptr_t
 * is defined as ub4.
 * 
 * WARNING! dptr_t has been defined to be a non-pointer datatype so that 
 * we can  guarantee that the size remains same! So it should never be used
 * to store a pointer. It should only be used as a place holder to preserve
 * the size of the structure. 
 */


#ifndef lint 
typedef ub4 dptr_t; 
#else 
#define dptr_t ub4 
#endif /* lint */ 


/* SLMXFNMLEN: Maximum file name length */			
#ifdef AIXRIOS
# define SLMXFNMLEN 256                  
#else
# define SLMXFNMLEN 512                
#endif /* AIXRIOS */


/**-------------------------------------------------------------------------**/
/**                      OBSOLETE DATA TYPES                                **/
/**-------------------------------------------------------------------------**/

/* OBSOLETE: The following data types have been obsoleted. They should
 * not be used in any new code. Types word, b1, b2, & b4 were
 * considered to be synonyms of eword, eb1, eb2 & eb4, but which
 * actually had to be implemented as sword, sb1, sb2, & sb4 for
 * backward compatibility of old code. 
 */

/* ebx definitions */

#ifndef lint 
typedef          char     eb1;
typedef          short    eb2;               
typedef          int      eb4;               
typedef          eb4      deword;     
#else
# define         eb1      char
# define         eb2      short
# define         eb4      int
# define         deword   eb4
#endif

#define EB1MAXVAL      ((eb1)SCHAR_MAX) 
#define EB1MINVAL      ((eb1)        0) 
#define MINEB1MAXVAL   ((eb1)  127) 
#define MAXEB1MINVAL   ((eb1)    0) 
#define EB2MAXVAL      ((eb2) SHRT_MAX) 
#define EB2MINVAL      ((eb2)        0) 
#define MINEB2MAXVAL   ((eb2) 32767) 
#define MAXEB2MINVAL   ((eb2)     0) 
#define EB4MAXVAL      ((eb4) INT_MAX) 
#define EB4MINVAL      ((eb4)        0) 
#define MINEB4MAXVAL   ((eb4) 2147483647) 
#define MAXEB4MINVAL   ((eb4)          0) 
#define DEWORDMAXVAL       EB4MAXVAL 
#define DEWORDMINVAL       EB4MINVAL 
#define MINDEWORDMAXVAL    MINEB4MAXVAL 
#define MAXDEWORDMINVAL    MAXEB4MINVAL 


/* bx definitions */

#ifndef lint 
typedef         sb1  b1;                   
#else 
#define         b1 sb1 
#endif  /* lint */ 
#define  B1MAXVAL  SB1MAXVAL 
#define  B1MINVAL  SB1MINVAL 
 
#ifndef lint 
typedef         sb2      b2;              
#else 
#define         b2 sb2 
#endif  /* lint */ 
#define  B2MAXVAL  SB2MAXVAL 
#define  B2MINVAL  SB2MINVAL 
 
#ifndef lint 
typedef         sb4    b4;                
#else 
#define         b4 sb4 
#endif  /* lint */ 
# define  B4MAXVAL  SB4MAXVAL 
# define  B4MINVAL  SB4MINVAL 
 

#ifndef uiXT
/* Since the use of BITS16 and BITS32 conflict with X Windows definitions, */
/* they need to disappear when Xt-based toolkits are being compiled.       */

/* OBSOLETE: BITS8/16/32 are synonyms of ub1/2/4, which are preferred */
typedef   ub1       BITS8;                               /* Can hold 8 bits  */
typedef   ub2       BITS16;                              /* Can hold 16 bits */
typedef   ub4       BITS32;                              /* Can hold 32 bits */
#endif /* uiXT */
 
/* OBSOLETE:  For C++ users who are using Microsoft Foundation Classes, the 
 * obsolete Oracle type text conflicts with a reserved keyword.  If LUSEMFC is
 * defined, the Oracle definition of text is not used. 
 */ 
#if !defined(LUSEMFC)
# ifdef lint
#  define text unsigned char
#  define OraText unsigned char
# else
   typedef oratext text;
   typedef oratext OraText; 
# endif /* lint */
#endif /* !LUSEMFC */

/* OBSOLETE:
** M_IDEN       - maximum identifier size
**
** This constant is generic, and should NEVER be changed.  
*/
#define  M_IDEN    30


#endif /* SX3_ORACLE */ 

  

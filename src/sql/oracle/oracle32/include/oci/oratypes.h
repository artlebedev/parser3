/*
ORACLE, Copyright (c) 1982, 1983, 1986, 1990, 1995, 1998, 1999 ORACLE Corporation
ORACLE Utilities, Copyright (c) 1981, 1982, 1983, 1986, 1990, 1991, 1995, 1998, 1999 ORACLE
Corp
 
Restricted Rights
This program is an unpublished work under the Copyright Act of the
United States and is subject to the terms and conditions stated in
your  license  agreement  with  ORACORP  including  retrictions on
use, duplication, and disclosure.
 
Certain uncopyrighted ideas and concepts are also contained herein.
These are trade secrets of ORACORP and cannot be  used  except  in
accordance with the written permission of ORACLE Corporation.
*/

/*
 * $Header: /home/cvsroot/parser3project/parser3/src/sql/oracle/oracle32/include/oci/Attic/oratypes.h,v 1.1 2001/08/22 14:02:20 parser Exp $
 */
 

 
#ifndef ORATYPES
# define ORATYPES
# define SX_ORACLE
# define SX3_ORACLE 


#ifndef ORASTDDEF
# include <stddef.h>
# define ORASTDDEF
#endif

#ifndef ORALIMITS
# include <limits.h>
# define ORALIMITS
#endif


#ifndef TRUE
# define TRUE  1
# define FALSE 0
#endif


#ifdef lint
# ifndef mips
#  define signed
# endif 
#endif 

#ifdef ENCORE_88K
# ifndef signed
#  define signed
# endif 
#endif 

#if defined(SYSV_386) || defined(SUN_OS)
# ifdef signed
#  undef signed
# endif 
# define signed
#endif 





#ifndef lint 
typedef unsigned char  ub1;                   
typedef   signed char  sb1;                  
#else 
#define ub1 unsigned char 
#define sb1 signed char 
#endif 
 
#define UB1MAXVAL ((ub1)UCHAR_MAX) 
#define UB1MINVAL ((ub1)        0) 
#define SB1MAXVAL ((sb1)SCHAR_MAX) 
#define SB1MINVAL ((sb1)SCHAR_MIN) 
#define MINUB1MAXVAL ((ub1)  255) 
#define MAXUB1MINVAL ((ub1)    0) 
#define MINSB1MAXVAL ((sb1)  127) 
#define MAXSB1MINVAL ((sb1) -127) 
 
 

 
#ifndef lint 
typedef unsigned short    ub2;                
typedef   signed short    sb2;               
#else 
#define ub2  unsigned short 
#define sb2  signed short 
#endif

#define UB2MAXVAL ((ub2)USHRT_MAX) 
#define UB2MINVAL ((ub2)        0) 
#define SB2MAXVAL ((sb2) SHRT_MAX) 
#define SB2MINVAL ((sb2) SHRT_MIN) 
#define MINUB2MAXVAL ((ub2) 65535) 
#define MAXUB2MINVAL ((ub2)     0) 
#define MINSB2MAXVAL ((sb2) 32767) 
#define MAXSB2MINVAL ((sb2)-32767) 
  



#ifndef lint 
typedef unsigned int  ub4;                   
typedef   signed int  sb4;                   
#else 
#define eb4 int 
#define ub4 unsigned int 
#define sb4 signed int 
#endif 
 
#define UB4MAXVAL ((ub4)UINT_MAX) 
#define UB4MINVAL ((ub4)        0) 
#define SB4MAXVAL ((sb4) INT_MAX) 
#define SB4MINVAL ((sb4) INT_MIN) 
#define MINUB4MAXVAL ((ub4) 4294967295) 
#define MAXUB4MINVAL ((ub4)          0) 
#define MINSB4MAXVAL ((sb4) 2147483647) 
#define MAXSB4MINVAL ((sb4)-2147483647) 



#define UB1BITS          CHAR_BIT
#define UB1MASK          ((1 << ((uword)CHAR_BIT)) - 1)



typedef ub1   bitvec;    
#define BITVEC(n) (((n)+(UB1BITS-1))>>3) 
                               


#ifdef lint
# define oratext unsigned char
#else
  typedef  unsigned char oratext;
#endif


#ifndef max
#define max(x, y)     (((x) < (y)) ?  (y) : (x))
#endif


#ifndef min
#define min(x, y)     (((x) < (y)) ?  (x) : (y))
#endif
 

#ifndef lint 
typedef         ub4      duword;                   
typedef         sb4      dsword;                 
typedef         dsword   dword;

#else 
#define duword ub4 
#define dsword sb4 
#define dword  dsword
#endif  

#define  DUWORDMAXVAL       UB4MAXVAL 
#define  DUWORDMINVAL       UB4MINVAL 
#define  DSWORDMAXVAL       SB4MAXVAL 
#define  DSWORDMINVAL       SB4MINVAL 
#define  MINDUWORDMAXVAL    MINUB4MAXVAL 
#define  MAXDUWORDMINVAL    MAXUB4MINVAL 
#define  MINDSWORDMAXVAL    MINSB4MAXVAL 
#define  MAXDSWORDMINVAL    MAXSB4MINVAL 
#define  DEWORDMAXVAL       EB4MAXVAL 
#define  DEWORDMINVAL       EB4MINVAL 
#define  MINDEWORDMAXVAL    MINEB4MAXVAL 
#define  MAXDEWORDMINVAL    MAXEB4MINVAL 
#define  DWORDMAXVAL        DSWORDMAXVAL 
#define  DWORDMINVAL        DSWORDMINVAL 
   




#ifndef lint  
typedef ub4 dsize_t;  
# else 
# define dsize_t ub4 
#endif 
 
# define DSIZE_TMAXVAL UB4MAXVAL           
# define MINDSIZE_TMAXVAL (dsize_t)65535 
 

#ifndef lint  
typedef sb4 dboolean;  
# else 
# define dboolean sb4 
#endif 
 
 


#ifndef lint 
typedef ub4 dptr_t; 
#else 
#define dptr_t ub4 
#endif 




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




#ifndef lint 
typedef         sb1  b1;                   
#else 
#define         b1 sb1 
#endif  
#define  B1MAXVAL  SB1MAXVAL 
#define  B1MINVAL  SB1MINVAL 
 
#ifndef lint 
typedef         sb2      b2;              
#else 
#define         b2 sb2 
#endif  
#define  B2MAXVAL  SB2MAXVAL 
#define  B2MINVAL  SB2MINVAL 
 
#ifndef lint 
typedef         sb4    b4;                
#else 
#define         b4 sb4 
#endif  
# define  B4MAXVAL  SB4MAXVAL 
# define  B4MINVAL  SB4MINVAL 
 

#ifndef uiXT
typedef   ub1       BITS8;                              
typedef   ub2       BITS16;                              
typedef   ub4       BITS32;                              
#endif
 
#if !defined(LUSEMFC)
# ifdef lint
#  define text unsigned char
#  define OraText unsigned char
# else
   typedef oratext text;
   typedef oratext OraText;
# endif
#endif

#define  M_IDEN    30
		
#ifdef AIXRIOS
# define SLMXFNMLEN 256                  
#else
# define SLMXFNMLEN 512                
#endif 

#ifndef lint
typedef          int eword;                  
typedef unsigned int uword;                  
typedef   signed int sword;                  
#else
#define eword int
#define uword unsigned int
#define sword signed int
#endif 

#define  EWORDMAXVAL  ((eword) INT_MAX)
#define  EWORDMINVAL  ((eword)       0)
#define  UWORDMAXVAL  ((uword)UINT_MAX)
#define  UWORDMINVAL  ((uword)       0)
#define  SWORDMAXVAL  ((sword) INT_MAX)
#define  SWORDMINVAL  ((sword) INT_MIN)
#define  MINEWORDMAXVAL  ((eword)  32767)
#define  MAXEWORDMINVAL  ((eword)      0)
#define  MINUWORDMAXVAL  ((uword)  65535)
#define  MAXUWORDMINVAL  ((uword)      0)
#define  MINSWORDMAXVAL  ((sword)  32767)
#define  MAXSWORDMINVAL  ((sword) -32767)

 

#ifndef lint
typedef unsigned long  ubig_ora;             
typedef   signed long  sbig_ora;             
#else
#define ubig_ora unsigned long
#define sbig_ora signed long
#endif 

#define UBIG_ORAMAXVAL ((ubig_ora)ULONG_MAX)
#define UBIG_ORAMINVAL ((ubig_ora)        0)
#define SBIG_ORAMAXVAL ((sbig_ora) LONG_MAX)
#define SBIG_ORAMINVAL ((sbig_ora) LONG_MIN)
#define MINUBIG_ORAMAXVAL ((ubig_ora) 4294967295)
#define MAXUBIG_ORAMINVAL ((ubig_ora)          0)
#define MINSBIG_ORAMAXVAL ((sbig_ora) 2147483647)
#define MAXSBIG_ORAMINVAL ((sbig_ora)-2147483647)

#define UBIGORABITS      (UB1BITS * sizeof(ubig_ora))


#ifndef lint
#if (__STDC__ != 1)
# define SLU8NATIVE
# define SLS8NATIVE
#endif
#endif

#ifdef __BORLANDC__
#undef SLU8NATIVE
#undef SLS8NATIVE
#endif

#ifdef SLU8NATIVE

#ifdef SS_64BIT_SERVER
# ifndef lint
   typedef unsigned _int64 ub8;
# else
#  define ub8 unsigned _int64
# endif 
#else
# ifndef lint
   typedef unsigned _int64 ub8;
# else
#  define ub8 unsigned _int64 
# endif 
#endif

#define UB8ZERO      ((ub8)0)

#define UB8MINVAL    ((ub8)0)
#define UB8MAXVAL    ((ub8)18446744073709551615)

#define MAXUB8MINVAL ((ub8)0)
#define MINUB8MAXVAL ((ub8)18446744073709551615)

#endif 


#ifdef SLS8NATIVE

#ifdef SS_64BIT_SERVER
# ifndef lint
   typedef signed _int64 sb8;
# else
#  define sb8 signed _int64
# endif 
#else
# ifndef lint
   typedef signed _int64 sb8;
# else
#  define sb8 signed _int64
# endif 
#endif

#define SB8ZERO      ((sb8)0)

#define SB8MINVAL    ((sb8)-9223372036854775808)
#define SB8MAXVAL    ((sb8) 9223372036854775807)

#define MAXSB8MINVAL ((sb8)-9223372036854775807)
#define MINSB8MAXVAL ((sb8) 9223372036854775807)

#endif 



#undef CONST

#ifdef _olint
# define CONST const
#else
#if defined(PMAX) && defined(__STDC__)
#   define CONST const
#else
# ifdef M88OPEN
#  define CONST const
# else 
#  if defined(SEQ_PSX) && defined(__STDC__)
#    define CONST const
#  else 
#    ifdef A_OSF
#      if defined(__STDC__)
#        define CONST const
#      else
#        define CONST
#      endif
#    else
#      define CONST
#    endif 
#  endif 
# endif 
#endif
#endif 



#ifdef lint
# define dvoid void
#else

# ifdef UTS2
#  define dvoid char
# else
# define dvoid void
# endif 

#endif 


typedef void (*lgenfp_t)( void );



#ifndef ORASYS_TYPES
# include <sys/types.h>
# define ORASYS_TYPES
#endif 



#ifndef boolean
# define boolean int
#endif 



#ifdef sparc
# define SIZE_TMAXVAL SB4MAXVAL               
#else
# define SIZE_TMAXVAL UB4MAXVAL              
#endif 

#define MINSIZE_TMAXVAL (size_t)65535


#if !defined(MOTIF) && !defined(LISPL)  && !defined(__cplusplus) && !defined(LUSEMFC)
typedef  oratext *string;        
#endif 

#ifndef lint
typedef unsigned short  utext;
#else
#define utext  unsigned short
#endif

 
#endif 


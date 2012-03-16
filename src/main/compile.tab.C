
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001-2012 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	
*/

volatile const char * IDENT_COMPILE_Y = "$Id: compile.tab.C,v 1.154 2012/03/16 09:24:12 moko Exp $";

/**
	@todo parser4: 
	- cache compiled code from request to request. to do that...
		-#:	make method definitions, @CLASS, @BASE, @USE instructions,
			which would be executed afterwards, and actions
			now performed at compile time would be delayed to run time.
		-#:	make cache expiration on time and on disk-change of class source
		-#:	in apache use subpools for compiled class storage
		-#:	in iis make up specialized Pool object for that
*/

#define YYSTYPE  ArrayOperation* 
#define YYPARSE_PARAM  pc
#define YYLEX_PARAM  pc
#define YYDEBUG  1
#define YYERROR_VERBOSE	1
#define yyerror(msg)  real_yyerror((Parse_control *)pc, msg)
#define YYPRINT(file, type, value)  yyprint(file, type, value)

// includes

#include "compile_tools.h"
#include "pa_value.h"
#include "pa_request.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "pa_globals.h"
#include "pa_vmethod_frame.h"

// defines

#define USE_CONTROL_METHOD_NAME "USE"
#define OPTIONS_CONTROL_METHOD_NAME "OPTIONS"
#define OPTION_ALL_VARS_LOCAL_NAME "locals"
#define OPTION_PARTIAL_CLASS "partial"
#define REM_OPERATOR_NAME "rem"

// forwards

static int real_yyerror(Parse_control* pc, char* s);
static void yyprint(FILE* file, int type, YYSTYPE value);
static int yylex(YYSTYPE* lvalp, void* pc);

static const VBool vfalse(false);
static const VBool vtrue(true);
static const VString vempty;

// local convinient inplace typecast & var
#undef PC
#define PC  (*(Parse_control *)pc)
#undef POOL
#define POOL  (*PC.pool)
#ifndef DOXYGEN


/* Line 189 of yacc.c  */
#line 141 "compile.tab.C"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     EON = 258,
     STRING = 259,
     BOGUS = 260,
     BAD_STRING_COMPARISON_OPERATOR = 261,
     BAD_HEX_LITERAL = 262,
     BAD_METHOD_DECL_START = 263,
     BAD_METHOD_PARAMETER_NAME_CHARACTER = 264,
     BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE = 265,
     LAND = 266,
     LOR = 267,
     LXOR = 268,
     NXOR = 269,
     NLE = 270,
     NGE = 271,
     NEQ = 272,
     NNE = 273,
     NSL = 274,
     NSR = 275,
     SLT = 276,
     SGT = 277,
     SLE = 278,
     SGE = 279,
     SEQ = 280,
     SNE = 281,
     DEF = 282,
     IN = 283,
     FEXISTS = 284,
     DEXISTS = 285,
     IS = 286,
     LITERAL_TRUE = 287,
     LITERAL_FALSE = 288,
     NUNARY = 289
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 217 "compile.tab.C"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  51
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   470

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  89
/* YYNRULES -- Number of rules.  */
#define YYNRULES  172
/* YYNRULES -- Number of states.  */
#define YYNSTATES  263

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   289

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      48,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    46,    60,     2,    52,    43,    37,    61,
      56,    57,    40,    38,     2,    39,    55,    41,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    59,    51,
      34,     2,    35,     2,    47,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    49,    42,    50,    58,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    53,    36,    54,    45,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    44
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,     9,    12,    14,    16,    18,
      23,    25,    27,    29,    32,    35,    37,    39,    40,    49,
      51,    53,    57,    59,    61,    63,    67,    69,    71,    73,
      75,    77,    80,    82,    84,    86,    88,    90,    92,    95,
      98,   100,   104,   106,   108,   110,   113,   115,   118,   122,
     124,   126,   128,   130,   133,   136,   138,   140,   142,   143,
     144,   150,   154,   158,   160,   162,   164,   166,   168,   170,
     173,   175,   176,   177,   184,   186,   188,   191,   193,   195,
     197,   198,   199,   205,   209,   213,   215,   219,   221,   225,
     227,   231,   233,   235,   237,   239,   241,   243,   245,   248,
     250,   253,   256,   258,   260,   263,   265,   267,   269,   271,
     274,   277,   278,   279,   285,   287,   289,   292,   295,   297,
     299,   302,   305,   307,   309,   311,   313,   315,   317,   321,
     325,   329,   332,   335,   338,   341,   344,   347,   350,   353,
     357,   361,   365,   369,   373,   377,   381,   385,   389,   393,
     397,   401,   405,   409,   413,   417,   421,   425,   429,   433,
     437,   441,   445,   449,   453,   457,   461,   463,   465,   467,
     468,   470,   472
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      63,     0,    -1,    65,    -1,    64,    -1,    66,    -1,    64,
      66,    -1,    79,    -1,    67,    -1,    72,    -1,    47,     4,
      48,    68,    -1,   150,    -1,    69,    -1,    70,    -1,    69,
      70,    -1,    71,    48,    -1,   150,    -1,     4,    -1,    -1,
      47,     4,    75,    74,    78,    48,    73,    79,    -1,   150,
      -1,    75,    -1,    49,    76,    50,    -1,   150,    -1,    77,
      -1,     4,    -1,    77,    51,     4,    -1,   150,    -1,     4,
      -1,   150,    -1,    80,    -1,    81,    -1,    80,    81,    -1,
     146,    -1,    82,    -1,    83,    -1,    91,    -1,   106,    -1,
      84,    -1,    52,    85,    -1,    87,     3,    -1,    86,    -1,
      53,    87,    54,    -1,    88,    -1,    89,    -1,    90,    -1,
     139,    90,    -1,   129,    -1,   126,   129,    -1,    52,    92,
      96,    -1,    93,    -1,    94,    -1,    95,    -1,   125,    -1,
      55,   125,    -1,   139,   125,    -1,    97,    -1,   100,    -1,
     101,    -1,    -1,    -1,    49,    98,   102,    99,    50,    -1,
      56,   142,    57,    -1,    53,    79,    54,    -1,   147,    -1,
       4,    -1,   103,    -1,   104,    -1,   105,    -1,    82,    -1,
      81,    80,    -1,   107,    -1,    -1,    -1,    58,   108,   110,
     109,   111,     3,    -1,    87,    -1,   112,    -1,   111,   112,
      -1,   113,    -1,   116,    -1,   117,    -1,    -1,    -1,    49,
     114,   118,   115,    50,    -1,    56,   119,    57,    -1,    53,
     120,    54,    -1,   121,    -1,   118,    51,   121,    -1,   122,
      -1,   119,    51,   122,    -1,   123,    -1,   120,    51,   123,
      -1,   124,    -1,   142,    -1,    79,    -1,   147,    -1,     4,
      -1,   103,    -1,   130,    -1,   126,   130,    -1,   127,    -1,
     126,   127,    -1,   128,    55,    -1,   130,    -1,   130,    -1,
       4,     5,    -1,     4,    -1,   131,    -1,   132,    -1,   133,
      -1,    52,   136,    -1,     4,   137,    -1,    -1,    -1,    49,
     134,    80,   135,    50,    -1,     4,    -1,   138,    -1,   137,
     138,    -1,    52,   136,    -1,   140,    -1,   141,    -1,     4,
      59,    -1,   140,    59,    -1,   143,    -1,   144,    -1,   148,
      -1,   149,    -1,    84,    -1,   107,    -1,    60,   145,    60,
      -1,    61,   145,    61,    -1,    56,   143,    57,    -1,    39,
     143,    -1,    38,   143,    -1,    45,   143,    -1,    46,   143,
      -1,    27,   143,    -1,    28,   143,    -1,    29,   143,    -1,
      30,   143,    -1,   143,    39,   143,    -1,   143,    38,   143,
      -1,   143,    40,   143,    -1,   143,    41,   143,    -1,   143,
      43,   143,    -1,   143,    42,   143,    -1,   143,    19,   143,
      -1,   143,    20,   143,    -1,   143,    37,   143,    -1,   143,
      36,   143,    -1,   143,    14,   143,    -1,   143,    11,   143,
      -1,   143,    12,   143,    -1,   143,    13,   143,    -1,   143,
      34,   143,    -1,   143,    35,   143,    -1,   143,    15,   143,
      -1,   143,    16,   143,    -1,   143,    17,   143,    -1,   143,
      18,   143,    -1,   143,    21,   143,    -1,   143,    22,   143,
      -1,   143,    23,   143,    -1,   143,    24,   143,    -1,   143,
      25,   143,    -1,   143,    26,   143,    -1,   143,    31,   143,
      -1,     4,    -1,    79,    -1,     4,    -1,    -1,    32,    -1,
      33,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   133,   133,   140,   142,   142,   143,   145,   145,   147,
     254,   254,   255,   255,   256,   257,   257,   259,   259,   307,
     307,   308,   309,   309,   310,   310,   312,   312,   316,   316,
     318,   318,   319,   319,   320,   320,   320,   324,   365,   366,
     366,   367,   369,   370,   371,   424,   425,   425,   429,   442,
     443,   444,   445,   471,   476,   479,   480,   481,   483,   486,
     483,   494,   502,   509,   510,   511,   513,   519,   520,   520,
     524,   536,   539,   536,   584,   586,   586,   588,   589,   590,
     592,   595,   592,   598,   599,   601,   602,   605,   606,   609,
     610,   612,   615,   629,   634,   635,   636,   641,   641,   643,
     643,   644,   645,   653,   658,   661,   662,   663,   664,   666,
     670,   679,   682,   679,   690,   695,   695,   696,   702,   703,
     705,   718,   730,   732,   733,   734,   735,   736,   737,   738,
     739,   741,   742,   743,   744,   745,   746,   747,   748,   750,
     751,   752,   753,   754,   755,   756,   757,   758,   759,   760,
     761,   762,   763,   764,   765,   766,   767,   768,   769,   770,
     771,   772,   773,   774,   775,   776,   779,   784,   805,   810,
     811,   812,   814
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EON", "STRING", "BOGUS",
  "BAD_STRING_COMPARISON_OPERATOR", "BAD_HEX_LITERAL",
  "BAD_METHOD_DECL_START", "BAD_METHOD_PARAMETER_NAME_CHARACTER",
  "BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE", "\"&&\"",
  "\"||\"", "\"!||\"", "\"!|\"", "\"<=\"", "\">=\"", "\"==\"", "\"!=\"",
  "\"<<\"", "\">>\"", "\"lt\"", "\"gt\"", "\"le\"", "\"ge\"", "\"eq\"",
  "\"ne\"", "\"def\"", "\"in\"", "\"-f\"", "\"-d\"", "\"is\"", "\"true\"",
  "\"false\"", "'<'", "'>'", "'|'", "'&'", "'+'", "'-'", "'*'", "'/'",
  "'\\\\'", "'%'", "NUNARY", "'~'", "'!'", "'@'", "'\\n'", "'['", "']'",
  "';'", "'$'", "'{'", "'}'", "'.'", "'('", "')'", "'^'", "':'", "'\"'",
  "'\\''", "$accept", "all", "methods", "one_big_piece", "method",
  "control_method", "maybe_control_strings", "control_strings",
  "control_string", "maybe_string", "code_method", "@1",
  "maybe_bracketed_strings", "bracketed_maybe_strings", "maybe_strings",
  "strings", "maybe_comment", "maybe_codes", "codes", "code", "action",
  "get", "get_value", "get_name_value", "name_in_curly_rdive",
  "name_without_curly_rdive", "name_without_curly_rdive_read",
  "name_without_curly_rdive_class", "name_without_curly_rdive_code", "put",
  "name_expr_wdive", "name_expr_wdive_root", "name_expr_wdive_write",
  "name_expr_wdive_class", "construct", "construct_square", "@2", "$@3",
  "construct_round", "construct_curly", "any_constructor_code_value",
  "constructor_code_value", "constructor_code",
  "codes__excluding_sole_str_literal", "call", "call_value", "$@4", "$@5",
  "call_name", "store_params", "store_param", "store_square_param", "@6",
  "$@7", "store_round_param", "store_curly_param",
  "store_code_param_parts", "store_expr_param_parts",
  "store_curly_param_parts", "store_code_param_part",
  "store_expr_param_part", "store_curly_param_part", "code_param_value",
  "name_expr_dive_code", "name_path", "name_step", "name_advance1",
  "name_advance2", "name_expr_value", "name_expr_subvar_value",
  "name_expr_with_subvar_value", "name_square_code_value", "@8", "$@9",
  "subvar_ref_name_rdive", "subvar_get_writes", "subvar__get_write",
  "class_prefix", "class_static_prefix", "class_constructor_prefix",
  "expr_value", "expr", "double_or_STRING", "string_inside_quotes_value",
  "write_string", "empty_value", "true_value", "false_value", "empty", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,    60,    62,   124,    38,    43,    45,
      42,    47,    92,    37,   289,   126,    33,    64,    10,    91,
      93,    59,    36,   123,   125,    46,    40,    41,    94,    58,
      34,    39
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    62,    63,    63,    64,    64,    65,    66,    66,    67,
      68,    68,    69,    69,    70,    71,    71,    73,    72,    74,
      74,    75,    76,    76,    77,    77,    78,    78,    79,    79,
      80,    80,    81,    81,    82,    82,    82,    83,    84,    85,
      85,    86,    87,    87,    88,    89,    90,    90,    91,    92,
      92,    92,    93,    94,    95,    96,    96,    96,    98,    99,
      97,   100,   101,   102,   102,   102,   103,   104,   105,   105,
     106,   108,   109,   107,   110,   111,   111,   112,   112,   112,
     114,   115,   113,   116,   117,   118,   118,   119,   119,   120,
     120,   121,   122,   123,   124,   124,   124,   125,   125,   126,
     126,   127,   128,   129,   129,   130,   130,   130,   130,   131,
     132,   134,   135,   133,   136,   137,   137,   138,   139,   139,
     140,   141,   142,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   144,   145,   146,   147,
     148,   149,   150
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     1,     1,     4,
       1,     1,     1,     2,     2,     1,     1,     0,     8,     1,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     3,     1,     1,     1,     2,     1,     2,     3,     1,
       1,     1,     1,     2,     2,     1,     1,     1,     0,     0,
       5,     3,     3,     1,     1,     1,     1,     1,     1,     2,
       1,     0,     0,     6,     1,     1,     2,     1,     1,     1,
       0,     0,     5,     3,     3,     1,     3,     1,     3,     1,
       3,     1,     1,     1,     1,     1,     1,     1,     2,     1,
       2,     2,     1,     1,     2,     1,     1,     1,     1,     2,
       2,     0,     0,     5,     1,     1,     2,     2,     1,     1,
       2,     2,     1,     1,     1,     1,     1,     1,     3,     3,
       3,     2,     2,     2,     2,     2,     2,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     1,     1,     1,     0,
       1,     1,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     172,   168,     0,     0,    71,     0,     3,     2,     4,     7,
       8,     6,    29,    30,    33,    34,    37,    35,    36,    70,
      32,    28,     0,   105,   111,     0,     0,     0,    38,    40,
       0,    42,    43,    44,     0,    49,    50,    51,    52,     0,
      99,     0,    46,    97,   106,   107,   108,     0,   118,   119,
       0,     1,     5,    31,   172,   172,   172,   104,     0,   120,
     110,   115,     0,   114,   109,     0,     0,   103,     0,   105,
      53,     0,    97,    39,    58,   172,     0,    48,    55,    56,
      57,   105,   100,    47,    98,   101,    45,    54,   121,    74,
      72,    16,     9,    11,    12,     0,    10,    24,     0,    23,
      22,   172,    20,    19,   117,   116,   112,    41,    98,   169,
       0,   166,     0,     0,     0,     0,   170,   171,     0,     0,
       0,     0,     0,     0,   172,   172,   126,   127,     0,   122,
     123,   124,   125,     0,    13,    15,    14,    21,     0,    27,
       0,    26,     0,   168,     0,    33,    59,    65,    66,    67,
      63,    62,   135,   136,   137,   138,   132,   131,   133,   134,
       0,   167,     0,     0,    61,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    80,   172,     0,     0,    75,    77,    78,    79,
      25,    17,   113,    69,     0,   130,   128,   129,   150,   151,
     152,   149,   155,   156,   157,   158,   145,   146,   159,   160,
     161,   162,   163,   164,   165,   153,   154,   148,   147,   140,
     139,   141,   142,   144,   143,   169,    93,     0,    89,     0,
      87,    92,    73,    76,   172,    60,   168,    96,    81,    85,
      91,    94,   172,    84,     0,    83,    18,   169,     0,    90,
      88,    86,    82
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,     6,     7,     8,     9,    92,    93,    94,    95,
      10,   244,   101,    56,    98,    99,   140,   161,    12,    13,
      14,    15,   126,    28,    29,    30,    31,    32,    33,    17,
      34,    35,    36,    37,    77,    78,   109,   204,    79,    80,
     146,   247,   148,   149,    18,   127,    50,   133,    90,   195,
     196,   197,   235,   258,   198,   199,   248,   239,   237,   249,
     240,   238,   250,    38,    66,    40,    41,    42,    67,    44,
      45,    46,    62,   142,    64,    60,    61,    68,    48,    49,
     241,   129,   130,   162,    20,   251,   131,   132,    21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -107
static const yytype_int16 yypact[] =
{
      14,  -107,    13,    45,  -107,    28,   -15,  -107,  -107,  -107,
    -107,  -107,    21,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,    73,    19,  -107,    50,     6,    44,  -107,  -107,
      87,  -107,  -107,  -107,   -34,  -107,  -107,  -107,  -107,    65,
    -107,    48,  -107,     5,  -107,  -107,  -107,    65,    24,  -107,
       6,  -107,  -107,  -107,    88,   109,    74,  -107,    50,  -107,
      76,  -107,    21,  -107,  -107,    86,    65,   101,    65,    76,
    -107,    44,   101,  -107,  -107,    21,     7,  -107,  -107,  -107,
    -107,    18,  -107,  -107,     5,  -107,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,     9,  -107,    94,    99,  -107,    93,   112,
    -107,   146,  -107,  -107,  -107,  -107,    21,  -107,   101,    29,
     110,  -107,     7,     7,     7,     7,  -107,  -107,     7,     7,
       7,     7,    52,     7,    21,    21,  -107,  -107,   113,   323,
    -107,  -107,  -107,    92,  -107,  -107,  -107,  -107,   165,  -107,
     144,  -107,   147,   149,    21,    80,  -107,  -107,  -107,  -107,
    -107,  -107,    96,    96,    96,    96,  -107,  -107,  -107,  -107,
     276,  -107,   138,   139,  -107,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,     7,     7,     7,     7,     7,     7,     7,     7,
       7,     7,  -107,    21,     7,    35,  -107,  -107,  -107,  -107,
    -107,  -107,  -107,    21,   154,  -107,  -107,  -107,   419,   389,
     356,   284,   135,   135,   148,   148,   191,   191,   135,   135,
     135,   135,   148,   148,    96,   135,   135,   182,   427,   140,
     140,  -107,  -107,  -107,  -107,    37,  -107,    31,  -107,   -30,
    -107,  -107,  -107,  -107,    21,  -107,   108,  -107,   155,  -107,
    -107,  -107,    21,  -107,     7,  -107,  -107,    37,   161,  -107,
    -107,  -107,  -107
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -107,  -107,  -107,  -107,   206,  -107,  -107,  -107,   124,  -107,
    -107,  -107,  -107,   170,  -107,  -107,  -107,     1,   -58,    -7,
    -106,  -107,     0,  -107,  -107,    -6,  -107,  -107,   -21,  -107,
    -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,
    -107,   118,  -107,  -107,  -107,     2,  -107,  -107,  -107,  -107,
      43,  -107,  -107,  -107,  -107,  -107,  -107,  -107,  -107,   -17,
     -13,   -16,  -107,   -18,     4,    41,  -107,   -23,     3,  -107,
    -107,  -107,  -107,  -107,   181,  -107,   183,   239,  -107,  -107,
     171,    95,  -107,   123,  -107,   142,  -107,  -107,    64
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -173
static const yytype_int16 yytable[] =
{
      16,    11,    19,   145,   106,    53,    43,    39,  -103,    70,
      23,   111,    16,    91,    19,    74,    83,    22,     1,    75,
      65,   254,    76,    57,    57,     1,    86,   255,    51,    87,
      72,    71,     2,   143,   112,   113,   114,   115,   242,   116,
     117,   246,    84,    83,    89,   118,   119,    86,    69,    23,
      43,    39,   120,   121,    63,    24,    23,  -172,    25,   122,
    -102,     2,    16,   123,    19,     4,     3,   124,   125,    81,
      58,    58,     4,     3,   108,    16,   110,    19,    59,     4,
      82,     3,   252,    88,   192,   253,   203,     4,   193,     3,
      73,   194,    91,    24,    24,     4,    25,    25,    26,    53,
      27,    24,   144,    85,    25,    26,    16,    82,    19,    16,
     168,    19,    82,    97,    24,   173,   174,    25,    96,   100,
     103,    54,    55,    55,    16,    16,    19,    19,    58,   145,
     -68,   -68,   184,   185,   186,   187,   188,   189,   190,   191,
     107,   192,   136,   137,    16,   193,    19,   -15,   194,   168,
     139,   145,   171,   172,   173,   174,  -102,   135,   -95,   -95,
     179,   180,   168,   138,   151,   141,   181,   173,   174,   200,
     164,   184,   185,   186,   187,   188,   189,   190,   191,   181,
     188,   189,   190,   191,   184,   185,   186,   187,   188,   189,
     190,   191,   201,    16,   236,    19,    53,   202,   206,   -64,
     207,   173,   174,    16,   245,    19,   257,   152,   153,   154,
     155,   262,    52,   156,   157,   158,   159,   134,   160,   185,
     186,   187,   188,   189,   190,   191,   102,   147,   144,   186,
     187,   188,   189,   190,   191,    16,   259,    19,   243,   104,
     261,   260,    47,   105,    16,   256,    19,   128,   163,     0,
     144,   150,    16,   236,    19,     0,     0,    16,     0,    19,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   231,   232,   233,   234,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   173,   174,     0,     0,   181,     0,     0,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     184,   185,   186,   187,   188,   189,   190,   191,     0,     0,
       0,     0,     0,   205,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   175,   176,   177,   178,   179,   180,
       0,     0,     0,     0,   181,     0,     0,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,   165,   166,     0,
     168,   169,   170,   171,   172,   173,   174,   175,   176,   177,
     178,   179,   180,     0,     0,     0,     0,   181,     0,     0,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     165,     0,     0,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,     0,     0,     0,     0,
     181,     0,     0,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   173,   174,     0,     0,
     181,     0,     0,   182,   183,   184,   185,   186,   187,   188,
     189,   190,   191,     0,     0,   186,   187,   188,   189,   190,
     191
};

static const yytype_int16 yycheck[] =
{
       0,     0,     0,   109,    62,    12,     3,     3,     3,    27,
       4,     4,    12,     4,    12,    49,    39,     4,     4,    53,
      26,    51,    56,     5,     5,     4,    47,    57,     0,    47,
      27,    27,    47,     4,    27,    28,    29,    30,     3,    32,
      33,     4,    39,    66,    50,    38,    39,    68,     4,     4,
      47,    47,    45,    46,     4,    49,     4,    48,    52,    52,
      55,    47,    62,    56,    62,    58,    52,    60,    61,     4,
      52,    52,    58,    52,    71,    75,    75,    75,    59,    58,
      39,    52,    51,    59,    49,    54,   144,    58,    53,    52,
       3,    56,     4,    49,    49,    58,    52,    52,    53,   106,
      55,    49,   109,    55,    52,    53,   106,    66,   106,   109,
      14,   109,    71,     4,    49,    19,    20,    52,    54,    55,
      56,    48,    49,    49,   124,   125,   124,   125,    52,   235,
      50,    51,    36,    37,    38,    39,    40,    41,    42,    43,
      54,    49,    48,    50,   144,    53,   144,    48,    56,    14,
       4,   257,    17,    18,    19,    20,    55,    93,    50,    51,
      25,    26,    14,    51,    54,   101,    31,    19,    20,     4,
      57,    36,    37,    38,    39,    40,    41,    42,    43,    31,
      40,    41,    42,    43,    36,    37,    38,    39,    40,    41,
      42,    43,    48,   193,   193,   193,   203,    50,    60,    50,
      61,    19,    20,   203,    50,   203,    51,   112,   113,   114,
     115,    50,     6,   118,   119,   120,   121,    93,   123,    37,
      38,    39,    40,    41,    42,    43,    56,   109,   235,    38,
      39,    40,    41,    42,    43,   235,   252,   235,   195,    58,
     257,   254,     3,    60,   244,   244,   244,    76,   125,    -1,
     257,   109,   252,   252,   252,    -1,    -1,   257,    -1,   257,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,   188,   189,   190,   191,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    19,    20,    -1,    -1,    31,    -1,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      36,    37,    38,    39,    40,    41,    42,    43,    -1,    -1,
      -1,    -1,    -1,    57,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    -1,    31,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    43,    11,    12,    -1,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    -1,    -1,    -1,    31,    -1,    -1,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      11,    -1,    -1,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,
      31,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    19,    20,    -1,    -1,
      31,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    -1,    -1,    38,    39,    40,    41,    42,
      43
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,    47,    52,    58,    63,    64,    65,    66,    67,
      72,    79,    80,    81,    82,    83,    84,    91,   106,   107,
     146,   150,     4,     4,    49,    52,    53,    55,    85,    86,
      87,    88,    89,    90,    92,    93,    94,    95,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   139,   140,   141,
     108,     0,    66,    81,    48,    49,    75,     5,    52,    59,
     137,   138,   134,     4,   136,    87,   126,   130,   139,     4,
     125,   126,   130,     3,    49,    53,    56,    96,    97,   100,
     101,     4,   127,   129,   130,    55,    90,   125,    59,    87,
     110,     4,    68,    69,    70,    71,   150,     4,    76,    77,
     150,    74,    75,   150,   136,   138,    80,    54,   130,    98,
      79,     4,    27,    28,    29,    30,    32,    33,    38,    39,
      45,    46,    52,    56,    60,    61,    84,   107,   142,   143,
     144,   148,   149,   109,    70,   150,    48,    50,    51,     4,
      78,   150,   135,     4,    81,    82,   102,   103,   104,   105,
     147,    54,   143,   143,   143,   143,   143,   143,   143,   143,
     143,    79,   145,   145,    57,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    31,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    49,    53,    56,   111,   112,   113,   116,   117,
       4,    48,    50,    80,    99,    57,    60,    61,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   114,    79,   120,   123,   119,
     122,   142,     3,   112,    73,    50,     4,   103,   118,   121,
     124,   147,    51,    54,    51,    57,    79,    51,   115,   123,
     122,   121,    50
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1455 of yacc.c  */
#line 133 "compile.y"
    {
	Method* method=new Method(Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		(yyvsp[(1) - (1)])/*parser_code*/, 0/*native_code*/);
	PC.cclass->set_method(PC.alias_method(main_method_name), method);
;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 148 "compile.y"
    {
	const String& command=LA2S(*(yyvsp[(2) - (4)]))->trim(String::TRIM_END);
	YYSTYPE strings_code=(yyvsp[(4) - (4)]);
	if(strings_code->count()<1*OPERATIONS_PER_OPVALUE) {
		strcpy(PC.error, "@");
		strcat(PC.error, command.cstr());
		strcat(PC.error, " is empty");
		YYERROR;
	}
	if(command==CLASS_NAME) {
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			// new class' name
			const String& name=LA2S(*strings_code)->trim(String::TRIM_END);
			// creating the class
			VStateless_class* cclass=new VClass;
			PC.cclass_new=cclass;
			PC.cclass_new->set_name(name);
		} else {
			strcpy(PC.error, "@"CLASS_NAME" must contain only one line with class name (contains more then one)");
			YYERROR;
		}
	} else if(command==USE_CONTROL_METHOD_NAME) {
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE) 
			PC.request.use_file(PC.request.main_class, LA2S(*strings_code, i)->trim(String::TRIM_END), PC.request.get_used_filename(PC.file_no));
	} else if(command==BASE_NAME) {
		if(PC.append){
			strcpy(PC.error, "can't set base while appending methods to class '");
			strncat(PC.error, PC.cclass->name().cstr(), MAX_STRING/2);
			strcat(PC.error, "'");
			YYERROR;
		}
		PC.class_add();
		if(PC.cclass->base_class()) { // already changed from default?
			strcpy(PC.error, "class already have a base '");
			strncat(PC.error, PC.cclass->base_class()->name().cstr(), MAX_STRING/2);
			strcat(PC.error, "'");
			YYERROR;
		}
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			const String& base_name=LA2S(*strings_code)->trim(String::TRIM_END);
			if(Value* base_class_value=PC.request.get_class(base_name)) {
				// @CLASS == @BASE sanity check
				if(VStateless_class *base_class=base_class_value->get_class()) {
					if(PC.cclass==base_class) {
						strcpy(PC.error, "@"CLASS_NAME" equals @"BASE_NAME);
						YYERROR;
					}
					PC.cclass->get_class()->set_base(base_class);
				} else { // they asked to derive from a class without methods ['env' & co]
					strcpy(PC.error, base_name.cstr());
					strcat(PC.error, ": you can not derive from this class in @"BASE_NAME);
					YYERROR;
				}
			} else {
				strcpy(PC.error, base_name.cstr());
				strcat(PC.error, ": undefined class in @"BASE_NAME);
				YYERROR;
			}
		} else {
			strcpy(PC.error, "@"BASE_NAME" must contain sole name");
			YYERROR;
		}
	} else if(command==OPTIONS_CONTROL_METHOD_NAME) {
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE) {
			const String& option=LA2S(*strings_code, i)->trim(String::TRIM_END);
			if(option==OPTION_ALL_VARS_LOCAL_NAME){
				PC.set_all_vars_local();
			} else if(option==OPTION_PARTIAL_CLASS){
				if(PC.cclass_new){
					if(VStateless_class* existed=PC.get_existed_class(PC.cclass_new)){
						if(!PC.reuse_existed_class(existed)){
							strcpy(PC.error, "can't append methods to '");
							strncat(PC.error, PC.cclass_new->name().cstr(), MAX_STRING/2);
							strcat(PC.error, "' - the class wasn't marked as partial");
							YYERROR;
						}
					} else {
						// marks the new class as partial. we will be able to add methods here later.
						PC.cclass_new->set_partial();
					}
				} else {
					strcpy(PC.error, "'"OPTION_PARTIAL_CLASS"' option should be used straight after @"CLASS_NAME);
					YYERROR;
				}
			} else if(option==method_call_type_static){
				PC.set_methods_call_type(Method::CT_STATIC);
			} else if(option==method_call_type_dynamic){
				PC.set_methods_call_type(Method::CT_DYNAMIC);
			} else {
				strcpy(PC.error, "'");
				strncat(PC.error, option.cstr(), MAX_STRING/2);
				strcat(PC.error, "' invalid option. valid options are "
					"'"OPTION_PARTIAL_CLASS"', '"OPTION_ALL_VARS_LOCAL_NAME"'"
					", '"METHOD_CALL_TYPE_STATIC"' and '"METHOD_CALL_TYPE_DYNAMIC"'"
					);
				YYERROR;
			}
		}
	} else {
		strcpy(PC.error, "'");
		strncat(PC.error, command.cstr(), MAX_STRING/2);
		strcat(PC.error, "' invalid special name. valid names are "
			"'"CLASS_NAME"', '"USE_CONTROL_METHOD_NAME"', '"BASE_NAME"' and '"OPTIONS_CONTROL_METHOD_NAME"'.");
		YYERROR;
	}
;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 255 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 259 "compile.y"
    { 
	PC.class_add();
	PC.explicit_result=false;

	YYSTYPE params_names_code=(yyvsp[(3) - (6)]);
	ArrayString* params_names=0;
	if(int size=params_names_code->count()) {
		params_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE)
			*params_names+=LA2S(*params_names_code, i);
	}

	YYSTYPE locals_names_code=(yyvsp[(4) - (6)]);
	ArrayString* locals_names=0;
	bool all_vars_local=false;
	if(int size=locals_names_code->count()) {
		locals_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE) {
			const String* local_name=LA2S(*locals_names_code, i);
			if(*local_name==RESULT_VAR_NAME)
				PC.explicit_result=true;
			else if(*local_name==OPTION_ALL_VARS_LOCAL_NAME)
				all_vars_local=true;
			else
				*locals_names+=local_name;
		}
	}
	if(!all_vars_local && PC.cclass && PC.cclass->is_vars_local())
		all_vars_local=true;

	Method* method=new Method(
		//name, 
		GetMethodCallType(PC, *(yyvsp[(2) - (6)])),
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		0/*to be filled later in next {} */, 0, all_vars_local);

	*reinterpret_cast<Method**>(&(yyval))=method;
;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 297 "compile.y"
    {
		Method* method=reinterpret_cast<Method*>((yyvsp[(7) - (8)]));
		// fill in the code
		method->parser_code=(yyvsp[(8) - (8)]);

		// register in class
		const String& name=*LA2S(*(yyvsp[(2) - (8)]));
		PC.cclass->set_method(PC.alias_method(name), method);
;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 308 "compile.y"
    {(yyval)=(yyvsp[(2) - (3)]);;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 310 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]); P(*(yyval), *(yyvsp[(3) - (3)])); ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 318 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 324 "compile.y"
    {
	(yyval)=N();
	YYSTYPE code=(yyvsp[(1) - (1)]);
	size_t count=code->count();

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(
		count!=3
		|| !maybe_change_first_opcode(*code, OP::OP_VALUE__GET_ELEMENT, /*=>*/OP::OP_VALUE__GET_ELEMENT__WRITE)
	)
#endif

#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
	if(
		count!=3
		|| !maybe_change_first_opcode(*code, OP::OP_WITH_SELF__VALUE__GET_ELEMENT, /*=>*/OP::OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE)
	)
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	if(
		count!=5
		|| !maybe_change_first_opcode(*code, OP::OP_GET_OBJECT_ELEMENT, /*=>*/OP::OP_GET_OBJECT_ELEMENT__WRITE)
	)
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	if(
		count!=5
		|| !maybe_change_first_opcode(*code, OP::OP_GET_OBJECT_VAR_ELEMENT, /*=>*/OP::OP_GET_OBJECT_VAR_ELEMENT__WRITE)
	)
#endif
	{
		changetail_or_append(*code,
			OP::OP_GET_ELEMENT, false,  /*=>*/OP::OP_GET_ELEMENT__WRITE,
			/*or */OP::OP_WRITE_VALUE
			); /* value=pop; wcontext.write(value) */
	}

	P(*(yyval), *code);
;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 365 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]); ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 367 "compile.y"
    { (yyval)=(yyvsp[(2) - (3)]); ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 371 "compile.y"
    {
	(yyval)=N(); 
	YYSTYPE diving_code=(yyvsp[(1) - (1)]);
	size_t count=diving_code->count();

	if(maybe_make_self(*(yyval), *diving_code, count)) {
		// $self.
	} else

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	if(maybe_make_get_object_element(*(yyval), *diving_code, count)){
		// optimization for $object.field + ^object.method[
	} else
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	if(maybe_make_get_object_var_element(*(yyval), *diving_code, count)){
		// optimization for $object.$var
	} else
#endif

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(
		count>=4
		&& (*diving_code)[0].code==OP::OP_VALUE
		&& (*diving_code)[3].code==OP::OP_GET_ELEMENT
	){
		 // optimization
		O(*(yyval),
			(PC.in_call_value && count==4)
			? OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR // ^object[ : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT_OR_OPERATOR+origin+string
			: OP::OP_VALUE__GET_ELEMENT             // $object  : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT+origin+string
		);
		P(*(yyval), *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		if(count>4)
			P(*(yyval), *diving_code, 4); // copy tail
	} else {
		O(*(yyval), OP::OP_WITH_READ); /* stack: starting context */
		P(*(yyval), *diving_code);
	}
#else
	{
		O(*(yyval), OP::OP_WITH_READ); /* stack: starting context */

		// ^if OP_ELEMENT => ^if OP_ELEMENT_OR_OPERATOR
		// optimized OP_VALUE+origin+string+OP_GET_ELEMENT. => OP_VALUE+origin+string+OP_GET_ELEMENT_OR_OPERATOR.
		if(PC.in_call_value && count==4)
			diving_code->put(count-1, OP::OP_GET_ELEMENT_OR_OPERATOR);
		P(*(yyval), *diving_code);
	}
#endif
	/* diving code; stack: current context */
;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 424 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 425 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 429 "compile.y"
    {
	(yyval)=N();
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	if(maybe_optimize_construct(*(yyval), *(yyvsp[(2) - (3)]), *(yyvsp[(3) - (3)]))){
		// $a(expr), $.a(expr), $a[value], $.a[value], $self.a[value], $self.a(expr)
	} else 
#endif
	{
		P(*(yyval), *(yyvsp[(2) - (3)])); /* stack: context,name */
		P(*(yyval), *(yyvsp[(3) - (3)])); /* stack: context,name,constructor_value */
	}
;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 445 "compile.y"
    {
	(yyval)=N();
	YYSTYPE diving_code=(yyvsp[(1) - (1)]);
	size_t count=diving_code->count();

	if(maybe_make_self(*(yyval), *diving_code, count)) {
		// $self.
	} else
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(
		count>=4
		&& (*diving_code)[0].code==OP::OP_VALUE
		&& (*diving_code)[3].code==OP::OP_GET_ELEMENT
	){
		O(*(yyval), OP::OP_WITH_ROOT__VALUE__GET_ELEMENT);
		P(*(yyval), *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		if(count>4)
			P(*(yyval), *diving_code, 4); // tail
	} else
#endif
	{
		O(*(yyval), OP::OP_WITH_ROOT); /* stack: starting context */
		P(*(yyval), *diving_code);
	}
	/* diving code; stack: current context */
;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 471 "compile.y"
    {
	(yyval)=N(); 
	O(*(yyval), OP::OP_WITH_WRITE); /* stack: starting context */
	P(*(yyval), *(yyvsp[(2) - (2)])); /* diving code; stack: context,name */
;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 476 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 483 "compile.y"
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&(yyval))=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 486 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&(yyvsp[(2) - (3)]));
;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 488 "compile.y"
    {
	// stack: context, name
	(yyval)=(yyvsp[(3) - (5)]); // stack: context, name, value
	O(*(yyval), OP::OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 494 "compile.y"
    { 
	(yyval)=N(); 
	O(*(yyval), OP::OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(*(yyval), *(yyvsp[(2) - (3)])); // stack: context, name, value
	O(*(yyval), OP::OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 502 "compile.y"
    {
	// stack: context, name
	(yyval)=N(); 
	OA(*(yyval), OP::OP_CURLY_CODE__CONSTRUCT, (yyvsp[(2) - (3)])); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 513 "compile.y"
    {
	(yyval)=N(); 
	OA(*(yyval), OP::OP_OBJECT_POOL, (yyvsp[(1) - (1)])); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 520 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 524 "compile.y"
    {
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
	if((*(yyvsp[(1) - (1)])).count())
#endif
	{
		(yyval)=(yyvsp[(1) - (1)]); /* stack: value */
		if(!maybe_change_first_opcode(*(yyval), OP::OP_CONSTRUCT_OBJECT, /*=>*/OP::OP_CONSTRUCT_OBJECT__WRITE))
			changetail_or_append(*(yyval), 
				OP::OP_CALL, true,  /*=>*/ OP::OP_CALL__WRITE,
				/*or */OP::OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
	}
;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 536 "compile.y"
    { 
					PC.in_call_value=true; 
			;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 539 "compile.y"
    {
				PC.in_call_value=false;
			;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 542 "compile.y"
    { /* ^field.$method{vasya} */
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	const String* operator_name=LA2S(*(yyvsp[(3) - (6)]), 0, OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR);
#else
	const String* operator_name=LA2S(*(yyvsp[(3) - (6)]), 1);
#endif
	if(operator_name && *operator_name==REM_OPERATOR_NAME){
		(yyval)=N();
	} else 
#endif
		{
			YYSTYPE params_code=(yyvsp[(5) - (6)]);
			if(params_code->count()==3) { // probably [] case. [OP::OP_VALUE+origin+Void]
				if(Value* value=LA2V(*params_code)) // it is OP_VALUE+origin+value?
					if(const String * string=value->get_string())
						if(string->is_empty()) // value is empty string?
							params_code=0; // ^zzz[] case. don't append lone empty param.
			}
			/* stack: context, method_junction */

			YYSTYPE var_code=(yyvsp[(3) - (6)]);
			if(
				var_code->count()==8
				&& (*var_code)[0].code==OP::OP_VALUE__GET_CLASS
				&& (*var_code)[3].code==OP::OP_PREPARE_TO_CONSTRUCT_OBJECT
				&& (*var_code)[4].code==OP::OP_VALUE
				&& (*var_code)[7].code==OP::OP_GET_ELEMENT
			){
				yyval=N();
				O(*(yyval), OP::OP_CONSTRUCT_OBJECT);
				P(*(yyval), *var_code, 1/*offset*/, 2/*limit*/); // class name
				P(*(yyval), *var_code, 5/*offset*/, 2/*limit*/); // constructor name
				OA(*(yyval), params_code);
			} else 
				{
					(yyval)=var_code; /* with_xxx,diving code; stack: context,method_junction */
					OA(*(yyval), OP::OP_CALL, params_code); // method_frame=make frame(pop junction); ncontext=pop; call(ncontext,method_frame) stack: value
				}
		}
;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 586 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 592 "compile.y"
    {
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&(yyval))=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 595 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&(yyvsp[(2) - (3)]));
;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 597 "compile.y"
    {(yyval)=(yyvsp[(3) - (5)]);;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 598 "compile.y"
    {(yyval)=(yyvsp[(2) - (3)]);;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 599 "compile.y"
    {(yyval)=(yyvsp[(2) - (3)]);;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 602 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]); P(*(yyval), *(yyvsp[(3) - (3)])); ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 606 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]); P(*(yyval), *(yyvsp[(3) - (3)])); ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 610 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]); P(*(yyval), *(yyvsp[(3) - (3)])); ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 612 "compile.y"
    {
	(yyval)=(yyvsp[(1) - (1)]);
;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 615 "compile.y"
    {
	YYSTYPE expr_code=(yyvsp[(1) - (1)]);
	if(expr_code->count()==3
		&& (*expr_code)[0].code==OP::OP_VALUE) { // optimizing (double/bool/incidently 'string' too) case. [OP::OP_VALUE+origin+Double]. no evaluating
		(yyval)=expr_code; 
	} else {
		YYSTYPE code=N();
		O(*code, OP::OP_PREPARE_TO_EXPRESSION);
		P(*code, *expr_code);
		O(*code, OP::OP_WRITE_EXPR_RESULT);
		(yyval)=N(); 
		OA(*(yyval), OP::OP_EXPR_CODE__STORE_PARAM, code);
	}
;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 629 "compile.y"
    {
	(yyval)=N(); 
	OA(*(yyval), OP::OP_CURLY_CODE__STORE_PARAM, (yyvsp[(1) - (1)]));
;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 641 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 643 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 645 "compile.y"
    {
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	(yyval)=(yyvsp[(1) - (1)]); /* stack: context,name */
	O(*(yyval), OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 653 "compile.y"
    {
	/* stack: context */
	(yyval)=(yyvsp[(1) - (1)]); /* stack: context,name */
	O(*(yyval), OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 666 "compile.y"
    {
	(yyval)=(yyvsp[(2) - (2)]);
	O(*(yyval), OP::OP_GET_ELEMENT);
;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 670 "compile.y"
    {
	YYSTYPE code;
	{
		change_string_literal_to_write_string_literal(*(code=(yyvsp[(1) - (2)])));
		P(*code, *(yyvsp[(2) - (2)]));
	}
	(yyval)=N(); 
	OA(*(yyval), OP::OP_STRING_POOL, code);
;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 679 "compile.y"
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&(yyval))=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 682 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&(yyvsp[(2) - (3)]));
;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 684 "compile.y"
    {
	(yyval)=N(); 
	OA(*(yyval), OP::OP_OBJECT_POOL, (yyvsp[(3) - (5)])); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 690 "compile.y"
    {
	(yyval)=N(); 
	O(*(yyval), OP::OP_WITH_READ);
	P(*(yyval), *(yyvsp[(1) - (1)]));
;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 695 "compile.y"
    { (yyval)=(yyvsp[(1) - (2)]); P(*(yyval), *(yyvsp[(2) - (2)])); ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 696 "compile.y"
    {
	(yyval)=(yyvsp[(2) - (2)]);
	O(*(yyval), OP::OP_GET_ELEMENT__WRITE);
;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 705 "compile.y"
    {
	(yyval)=(yyvsp[(1) - (2)]); // stack: class name string
	if(*LA2S(*(yyval)) == BASE_NAME) { // pseudo BASE class
		if(VStateless_class* base=PC.cclass->base_class()) {
			change_string_literal_value(*(yyval), base->name());
		} else {
			strcpy(PC.error, "no base class declared");
			YYERROR;
		}
	}
	// optimized OP_VALUE+origin+string+OP_GET_CLASS => OP_VALUE__GET_CLASS+origin+string
	maybe_change_first_opcode(*(yyval), OP::OP_VALUE, OP::OP_VALUE__GET_CLASS);
;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 718 "compile.y"
    {
	(yyval)=(yyvsp[(1) - (2)]);
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(*(yyval), OP::OP_PREPARE_TO_CONSTRUCT_OBJECT);
;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 737 "compile.y"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 738 "compile.y"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 739 "compile.y"
    { (yyval) = (yyvsp[(2) - (3)]); ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 741 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);  O(*(yyval), OP::OP_NEG); ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 742 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]); ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 743 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);	 O(*(yyval), OP::OP_INV); ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 744 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);  O(*(yyval), OP::OP_NOT); ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 745 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);  O(*(yyval), OP::OP_DEF); ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 746 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);  O(*(yyval), OP::OP_IN); ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 747 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);  O(*(yyval), OP::OP_FEXISTS); ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 748 "compile.y"
    { (yyval)=(yyvsp[(2) - (2)]);  O(*(yyval), OP::OP_DEXISTS); ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 750 "compile.y"
    {	(yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_SUB); ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 751 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_ADD); ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 752 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_MUL); ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 753 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_DIV); ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 754 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_MOD); ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 755 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_INTDIV); ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 756 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_BIN_SL); ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 757 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_BIN_SR); ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 758 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]); 	P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_BIN_AND); ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 759 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_BIN_OR); ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 760 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_BIN_XOR); ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 761 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  OA(*(yyval), OP::OP_NESTED_CODE, (yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_LOG_AND); ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 762 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  OA(*(yyval), OP::OP_NESTED_CODE, (yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_LOG_OR); ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 763 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_LOG_XOR); ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 764 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_NUM_LT); ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 765 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_NUM_GT); ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 766 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_NUM_LE); ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 767 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_NUM_GE); ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 768 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_NUM_EQ); ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 769 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_NUM_NE); ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 770 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_STR_LT); ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 771 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_STR_GT); ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 772 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_STR_LE); ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 773 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_STR_GE); ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 774 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_STR_EQ); ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 775 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_STR_NE); ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 776 "compile.y"
    { (yyval)=(yyvsp[(1) - (3)]);  P(*(yyval), *(yyvsp[(3) - (3)]));  O(*(yyval), OP::OP_IS); ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 779 "compile.y"
    {
	// optimized OP_STRING => OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*((yyval)=(yyvsp[(1) - (1)])));
;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 784 "compile.y"
    {
#ifdef OPTIMIZE_BYTECODE_STRING_POOL
	// it brakes ^if(" 09 "){...}
	YYSTYPE code=(yyvsp[(1) - (1)]);
	(yyval)=N();
	if(code->count()==3 && maybe_change_first_opcode(*code, OP::OP_STRING__WRITE, OP::OP_VALUE)){
		// optimized OP_STRING__WRITE+origin+value => OP_VALUE+origin+value without starting OP_STRING_POOL
		P(*(yyval), *code);
	} else {
		OA(*(yyval), OP::OP_STRING_POOL, code); /* stack: empty write context */
	}
#else
	(yyval)=N();
	OA(*(yyval), OP::OP_STRING_POOL, (yyvsp[(1) - (1)])); /* stack: empty write context */
#endif
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 805 "compile.y"
    {
	// optimized OP_STRING+OP_WRITE_VALUE => OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*((yyval)=(yyvsp[(1) - (1)])));
;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 810 "compile.y"
    { (yyval)=VL(/*we know that we will not change it*/const_cast<VString*>(&vempty), 0, 0, 0); ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 811 "compile.y"
    { (yyval) = VL(/*we know that we will not change it*/const_cast<VBool*>(&vtrue), 0, 0, 0); ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 812 "compile.y"
    { (yyval) = VL(/*we know that we will not change it*/const_cast<VBool*>(&vfalse), 0, 0, 0); ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 814 "compile.y"
    { (yyval)=N(); ;}
    break;



/* Line 1455 of yacc.c  */
#line 2910 "compile.tab.C"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 816 "compile.y"

#endif

/*
    	000$111(2222)00 
		000$111{3333}00
    	$,^: push,=0
    	1:( { break=pop
    	2:( )  pop
    	3:{ }  pop

    	000^111(2222)4444{33333}4000
    	$,^: push,=0
    	1:( { break=pop
    	2:( )=4
    	3:{ }=4
		4:[^({]=pop
*/

inline void ungetc(Parse_control& pc, uint last_line_end_col) {
	pc.source--;
	if(pc.pos.col==0) {
		--pc.pos.line; pc.pos.col=last_line_end_col;
	} else
		--pc.pos.col;

}
static int yylex(YYSTYPE *lvalp, void *apc) {
	register Parse_control& pc=*static_cast<Parse_control*>(apc);

	#define lexical_brackets_nestage pc.brackets_nestages[pc.ls_sp]
	#define RC {result=c; goto break2; }

	register int c;
	int result;
	
	if(pc.pending_state) {
		result=pc.pending_state;
		pc.pending_state=0;
		return result;
	}
	
	const char *begin=pc.source;
	Pos begin_pos=pc.pos;
	const char *end;
	int skip_analized=0;
	while(true) {
		c=*(end=(pc.source++));
//		fprintf(stderr, "\nchar: %c %02X; nestage: %d, sp=%d", c, c, lexical_brackets_nestage, pc.sp);

		if(c=='\n')
			pc.pos_next_line();
		else
			pc.pos_next_c(c);
//		fprintf(stderr, "\nchar: %c file(%d:%d)", c, pc.pos.line, pc.pos.col);

		if(pc.pos.col==0+1 && c=='@') {
			if(pc.ls==LS_DEF_SPECIAL_BODY) {
				// @SPECIAL
				// ...
				// @<here = 
				pop_LS(pc); // exiting from LS_DEF_SPECIAL_BODY state
			} // continuing checks
			if(pc.ls==LS_USER) {
				push_LS(pc, LS_DEF_NAME);
				RC;
			} else // @ in first column inside some code [when could that be?]
				result=BAD_METHOD_DECL_START;
			goto break2;
		}
		if(c=='^') {
			if(pc.ls==LS_METHOD_AFTER) {
				// handle after-method situation
				pop_LS(pc);
				result=EON;
				skip_analized=-1; // return to punctuation afterwards to assure it's literality
				goto break2;
			}
			switch(pc.ls) {
case LS_EXPRESSION_VAR_NAME_WITH_COLON:
case LS_EXPRESSION_VAR_NAME_WITHOUT_COLON:
case LS_VAR_NAME_SIMPLE_WITH_COLON:
case LS_VAR_NAME_SIMPLE_WITHOUT_COLON:
case LS_VAR_NAME_CURLY:
case LS_METHOD_NAME:
case LS_USER_COMMENT:
case LS_DEF_COMMENT:
	// no literals in names, please
	break;
default:
			switch(*pc.source) {
			// ^escaping some punctuators
			case '^': case '$': case ';': case '@':
			case '(': case ')':
			case '[': case ']':
			case '{': case '}':
			case '"':  case ':':
				if(end!=begin) {
					if(!pc.string_start)
						pc.string_start=begin_pos;
					// append piece till ^
					pc.string.append_strdup_know_length(begin, end-begin);
				}
				// reset piece 'begin' position & line
				begin=pc.source; // ->punctuation
				begin_pos=pc.pos;
				// skip over _ after ^
				pc.source++;  pc.pos.col++;
				// skip analysis = forced literal
				continue;

			// converting ^#HH into char(hex(HH))
			case '#':
				if(end!=begin) {
					if(!pc.string_start)
						pc.string_start=begin_pos;
					// append piece till ^
					pc.string.append_strdup_know_length(begin, end-begin);
				}
				// #HH ?
				if(pc.source[1] && isxdigit(pc.source[1]) && pc.source[2] && isxdigit(pc.source[2])) {
					char c=(char)(
						hex_value[(unsigned char)pc.source[1]]*0x10+
						hex_value[(unsigned char)pc.source[2]]);
					if(c==0) {
						result=BAD_HEX_LITERAL;
						goto break2; // wrong hex value[no ^#00 chars allowed]: bail out
					}
					// append char(hex(HH))
					pc.string.append(c);
					// skip over ^#HH
					pc.source+=3;
					pc.pos.col+=3;
					// reset piece 'begin' position & line
					begin=pc.source; // ->after ^#HH
					begin_pos=pc.pos;
					// skip analysis = forced literal
					continue;
				}
				// just escaped char
				// reset piece 'begin' position & line
				begin=pc.source;
				begin_pos=pc.pos;
				// skip over _ after ^
				pc.source++;  pc.pos.col++;
				// skip analysis = forced literal
				continue;
			}
			break;
			}
		}
		// #comment  start skipping
		if(c=='#' && pc.pos.col==1) {
			if(end!=begin) {
				if(!pc.string_start)
					pc.string_start=begin_pos;
				// append piece till #
				pc.string.append_strdup_know_length(begin, end-begin);
			}
			// fall into COMMENT lexical state [wait for \n]
			push_LS(pc, LS_USER_COMMENT);
			continue;
		}
		switch(pc.ls) {

		// USER'S = NOT OURS
		case LS_USER:
		case LS_NAME_SQUARE_PART: // name.[here].xxx
			if(pc.trim_bof)
				switch(c) {
				case '\n': case ' ': case '\t':
					begin=pc.source;
					begin_pos=pc.pos;
					continue; // skip it
				default:
					pc.trim_bof=false;
				}
			switch(c) {
			case '$':
				push_LS(pc, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			case ']':
				if(pc.ls==LS_NAME_SQUARE_PART)
					if(--lexical_brackets_nestage==0) {// $name.[co<]?>de<]?>
						pop_LS(pc); // $name.[co<]>de<]!>
						RC;
					}
				break;
			case '[': // $name.[co<[>de]
				if(pc.ls==LS_NAME_SQUARE_PART)
					lexical_brackets_nestage++;
				break;
			}
			if(pc.explicit_result && c)
				switch(c) {
				case '\n': case ' ': case '\t':
					begin=pc.source;
					begin_pos=pc.pos;
					continue; // skip it
				default:
					result=BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE;
					goto break2;
				}
			break;
			
		// #COMMENT
		case LS_USER_COMMENT:
			if(c=='\n') {
				// skip comment
				begin=pc.source;
				begin_pos=pc.pos;

				pop_LS(pc);
				continue;
			}
			break;
			
		// STRING IN EXPRESSION
		case LS_EXPRESSION_STRING_QUOTED:
		case LS_EXPRESSION_STRING_APOSTROFED:
			switch(c) {
			case '"':
			case '\'':
				if(
					pc.ls == LS_EXPRESSION_STRING_QUOTED && c=='"' ||
					pc.ls == LS_EXPRESSION_STRING_APOSTROFED && c=='\'') {
					pop_LS(pc); //"abc". | 'abc'.
					RC;
				}
				break;
			case '$':
				push_LS(pc, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			}
			break;

		// METHOD DEFINITION
		case LS_DEF_NAME:
			switch(c) {
			case '[':
				pc.ls=LS_DEF_PARAMS;
				RC;
			case '\n':
				pc.ls=LS_DEF_SPECIAL_BODY;
				RC;
			}
			break;

		case LS_DEF_PARAMS:
			switch(c) {
			case '$': // common error
				result=BAD_METHOD_PARAMETER_NAME_CHARACTER;
				goto break2;
			case ';':
				RC;
			case ']':
				pc.ls=*pc.source=='['?LS_DEF_LOCALS:LS_DEF_COMMENT;
				RC;
			case '\n': // wrong. bailing out
				pop_LS(pc);
				RC;
			}
			break;

		case LS_DEF_LOCALS:
			switch(c) {
			case '[':
			case ';':
				RC;
			case ']':
				pc.ls=LS_DEF_COMMENT;
				RC;
			case '\n': // wrong. bailing out
				pop_LS(pc);
				RC;
			}
			break;

		case LS_DEF_COMMENT:
			if(c=='\n') {
				pop_LS(pc);
				RC;
			}
			break;

		case LS_DEF_SPECIAL_BODY:
			if(c=='\n')
				RC;
			break;

		// (EXPRESSION)
		case LS_VAR_ROUND:
		case LS_METHOD_ROUND:
			switch(c) {
			case ')':
				if(--lexical_brackets_nestage==0)
					if(pc.ls==LS_METHOD_ROUND) // method round param ended
						pc.ls=LS_METHOD_AFTER; // look for method end
					else // pc.ls==LS_VAR_ROUND // variable constructor ended
						pop_LS(pc); // return to normal life
				RC;
			case '#': // comment start skipping
				if(end!=begin) {
					if(!pc.string_start)
						pc.string_start=begin_pos;
					// append piece till #
					pc.string.append_strdup_know_length(begin, end-begin);
				}
				// fall into COMMENT lexical state [wait for \n]
				push_LS(pc, LS_EXPRESSION_COMMENT);
				lexical_brackets_nestage=1;
				continue;
			case '$':
				push_LS(pc, LS_EXPRESSION_VAR_NAME_WITH_COLON);				
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			case '(':
				lexical_brackets_nestage++;
				RC;
			case '-':
				switch(*pc.source) {
				case 'f': // -f
					skip_analized=1;
					result=FEXISTS;
					goto break2;
				case 'd': // -d
					skip_analized=1;
					result=DEXISTS;
					goto break2;
				default: // minus
					result=c;
					goto break2;
				}
				goto break2;
			case '+': case '*': case '/': case '%': case '\\':
			case '~':
			case ';':
				RC;
			case '&': case '|':
				if(*pc.source==c) { // && ||
					result=c=='&'?LAND:LOR;
					skip_analized=1;
				} else
					result=c;
				goto break2;
			case '!':
				switch(pc.source[0]) { 
				case '|': // !| !||
					skip_analized=1;
					if(pc.source[1]=='|') {
						skip_analized++;
						result=LXOR;
					} else
						result=NXOR;
					goto break2;
				case '=': // !=
					skip_analized=1;
					result=NNE; 
					goto break2;
				}
				RC;

			case '<': // <<, <=, <
				switch(*pc.source) {
				case '<': // <[<]
					skip_analized=1; result=NSL; break;
				case '=': // <[=]
					skip_analized=1; result=NLE; break;
				default: // <[]
					result=c; break;
				}
				goto break2;
			case '>': // >>, >=, >
				switch(*pc.source) {
				case '>': // >[>]
					skip_analized=1; result=NSR; break;
				case '=': // >[=]
					skip_analized=1; result=NGE; break;
				default: // >[]
					result=c; break;
				}
				goto break2;
			case '=': // ==
				switch(*pc.source) {
				case '=': // =[=]
					skip_analized=1; result=NEQ; break;
				default: // =[]
					result=c; break; // not used now
				}
				goto break2;

			case '"':
				push_LS(pc, LS_EXPRESSION_STRING_QUOTED);
				RC;
			case '\'':
				push_LS(pc, LS_EXPRESSION_STRING_APOSTROFED);
				RC;
			case 'l': case 'g': case 'e': case 'n':
				if(end==begin) // right after whitespace
					if(isspace(pc.source[1])) {
						switch(*pc.source) {
							//					case '?': // ok [and bad cases, yacc would bark at them]
						case 't': // lt gt [et nt]
							result=c=='l'?SLT:c=='g'?SGT:BAD_STRING_COMPARISON_OPERATOR;
							skip_analized=1;
							goto break2;
						case 'e': // le ge ne [ee]
							result=c=='l'?SLE:c=='g'?SGE:c=='n'?SNE:BAD_STRING_COMPARISON_OPERATOR;
							skip_analized=1;
							goto break2;
						case 'q': // eq [lq gq nq]
							result=c=='e'?SEQ:BAD_STRING_COMPARISON_OPERATOR;
							skip_analized=1;
							goto break2;
						}
					}
				break;
			case 'i':
				if(end==begin) // right after whitespace
					if(isspace(pc.source[1])) {
						switch(pc.source[0]) {
						case 'n': // in
							skip_analized=1;
							result=IN;
							goto break2;
						case 's': // is
							skip_analized=1;
							result=IS;
							goto break2;
						}
					}
				break;
			case 'd':
				if(end==begin) // right after whitespace
					if(pc.source[0]=='e' && pc.source[1]=='f') { // def
						switch(pc.source[2]){
						case ' ': case '\t': case '\n': case '"': case '\'': case '^': case '$': // non-quoted string without whitespace after 'def' is not allowed
							skip_analized=2;
							result=DEF;
							goto break2;
						}
						// error: incorrect char after 'def'
					}
				break;
			case 't':
				if(end==begin) // right after whitespace
					if(pc.source[0]=='r' && pc.source[1]=='u' && pc.source[2]=='e') { // true
						skip_analized=3;
						result=LITERAL_TRUE;
						goto break2;
					}
				break;
			case 'f':
				if(end==begin) // right after whitespace
					if(pc.source[0]=='a' && pc.source[1]=='l' && pc.source[2]=='s' && pc.source[3]=='e') { // false
						skip_analized=4;
						result=LITERAL_FALSE;
						goto break2;
					}
				break;
			case ' ': case '\t': case '\n':
				if(end!=begin) { // there were a string after previous operator?
					result=0; // return that string
					goto break2;
				}
				// that's a leading|traling space or after-operator-space
				// ignoring it
				// reset piece 'begin' position & line
				begin=pc.source; // after whitespace char
				begin_pos=pc.pos;
				continue;
			}
			break;
		case LS_EXPRESSION_COMMENT:
			if(c=='(')
				lexical_brackets_nestage++;
			
			switch(*pc.source) {
			case '\n': case ')':
				if(*pc.source==')')
					if(--lexical_brackets_nestage!=0)
						continue;

				// skip comment
				begin=pc.source;
				begin_pos=pc.pos;

				pop_LS(pc);
				continue;
			}
			break;

		// VARIABLE GET/PUT/WITH
		case LS_VAR_NAME_SIMPLE_WITH_COLON: 
		case LS_VAR_NAME_SIMPLE_WITHOUT_COLON:
		case LS_EXPRESSION_VAR_NAME_WITH_COLON: 
		case LS_EXPRESSION_VAR_NAME_WITHOUT_COLON:
			if(
				pc.ls==LS_EXPRESSION_VAR_NAME_WITH_COLON ||
				pc.ls==LS_EXPRESSION_VAR_NAME_WITHOUT_COLON) {
				// name in expr ends also before 
				switch(c) {
				// expression minus
				case '-': 
				// expression integer division
				case '\\':
					pop_LS(pc);
					pc.ungetc();
					result=EON;
					goto break2;
				}
			}
			if(
				pc.ls==LS_VAR_NAME_SIMPLE_WITHOUT_COLON ||
				pc.ls==LS_EXPRESSION_VAR_NAME_WITHOUT_COLON) {
				// name already has ':', stop before next 
				switch(c) {
				case ':': 
					pop_LS(pc);
					pc.ungetc();
					result=EON;
					goto break2;
				}
			}
			switch(c) {
			case 0:
			case ' ': case '\t': case '\n':
			case ';':
			case ']': case '}': case ')': 
			case '"': case '\'':
			case '<': case '>':  // these stand for HTML brackets AND expression binary ops
			case '+': case '*': case '/': case '\\': case '%': 
			case '&': case '|': 
			case '=': case '!':
			// common delimiters
			case ',': case '?': case '#':
			// mysql column separators
			case '`':
			// before call
			case '^': 
				pop_LS(pc);
				pc.ungetc();
				result=EON;
				goto break2;
			case '[':
				// $name.<[>code]
				if(pc.pos.col>1/*not first column*/ && (
					end[-1]=='$'/*was start of get*/ ||
					end[-1]==':'/*was class name delim */ ||
					end[-1]=='.'/*was name delim */
					)) {
					push_LS(pc, LS_NAME_SQUARE_PART);
					lexical_brackets_nestage=1;
					RC;
				}
				pc.ls=LS_VAR_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				if(begin==end) { // ${name}, no need of EON, switching LS
					pc.ls=LS_VAR_NAME_CURLY; 
				} else {
					pc.ls=LS_VAR_CURLY;
					lexical_brackets_nestage=1;
				}

				RC;
			case '(':
				pc.ls=LS_VAR_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim
			case '$': // name part subvar
			case ':': // class<:>name
				// go to _WITHOUT_COLON state variant...
				if(pc.ls==LS_VAR_NAME_SIMPLE_WITH_COLON)
					pc.ls=LS_VAR_NAME_SIMPLE_WITHOUT_COLON;
				else if(pc.ls==LS_EXPRESSION_VAR_NAME_WITH_COLON)
					pc.ls=LS_EXPRESSION_VAR_NAME_WITHOUT_COLON;
				// ...stop before next ':'
				RC;
			}
			break;

		case LS_VAR_NAME_CURLY:
			switch(c) {
			case '[':
				// ${name.<[>code]}
				push_LS(pc, LS_NAME_SQUARE_PART);
				lexical_brackets_nestage=1;
				RC;
			case '}': // ${name} finished, restoring LS
				pop_LS(pc);
				RC;
			case '.': // name part delim
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
				RC;
			}
			break;

		case LS_VAR_SQUARE:
			switch(c) {
			case '$':
				push_LS(pc, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			case ']':
				if(--lexical_brackets_nestage==0) {
					pop_LS(pc);
					RC;
				}
				break;
			case ';': // operator_or_fmt;value delim
				RC;
			case '[':
				lexical_brackets_nestage++;
				break;
			}
			break;

		case LS_VAR_CURLY:
			switch(c) {
			case '$':
				push_LS(pc, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			case '}':
				if(--lexical_brackets_nestage==0) {
					pop_LS(pc);
					RC;
				}
				break;
			case '{':
				lexical_brackets_nestage++;
				break;
			}
			break;

		// METHOD CALL
		case LS_METHOD_NAME:
			switch(c) {
			case '[':
				// ^name.<[>code].xxx
				if(pc.pos.col>1/*not first column*/ && (
					end[-1]=='^'/*was start of call*/ || // never, ^[ is literal...
					end[-1]==':'/*was class name delim */ ||
					end[-1]=='.'/*was name delim */
					)) {
					push_LS(pc, LS_NAME_SQUARE_PART);
					lexical_brackets_nestage=1;
					RC;
				}
				pc.ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				pc.ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			case '(':
				pc.ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim 
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
			case '^': // ^abc^xxx wrong. bailing out
			case ']': case '}': case ')': // ^abc]}) wrong. bailing out
			case ' ': // ^if ( wrong. bailing out
				RC;
			}
			break;

		case LS_METHOD_SQUARE:
			switch(c) {
			case '$':
				push_LS(pc, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			case ';': // param delim
				RC;
			case ']':
				if(--lexical_brackets_nestage==0) {
					pc.ls=LS_METHOD_AFTER;
					RC;
				}
				break;
			case '[':
				lexical_brackets_nestage++;
				break;
			}
			break;

		case LS_METHOD_CURLY:
			switch(c) {
			case '$':
				push_LS(pc, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(pc, LS_METHOD_NAME);
				RC;
			case ';': // param delim
				RC;
			case '}':
				if(--lexical_brackets_nestage==0) {
					pc.ls=LS_METHOD_AFTER;
					RC;
				}
				break;
			case '{':
				lexical_brackets_nestage++;
				break;
			}
			if(pc.explicit_result && c)
				switch(c) {
				case '\n': case ' ': case '\t':
					begin=pc.source;
					begin_pos=pc.pos;
					continue; // skip it
				default:
					result=BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE;
					goto break2;
				}
			break;

		case LS_METHOD_AFTER:
			if(c=='[') {/* ][ }[ )[ */
				pc.ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			}					   
			if(c=='{') {/* ]{ }{ ){ */
				pc.ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			}					   
			if(c=='(') {/* ]( }( )( */
				pc.ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			}					   
			pop_LS(pc);
			pc.ungetc();
			result=EON;
			goto break2;
		}
		if(c==0) {
			result=-1;
			break;
		}
	}

break2:
	if(end!=begin) { // there is last piece?
		if(c=='@' || c==0) // we are before LS_DEF_NAME or EOF?
			while(end!=begin && end[-1]=='\n') // trim all empty lines before LS_DEF_NAME and EOF
				end--;
		if(end!=begin && pc.ls!=LS_USER_COMMENT) { // last piece still alive and not comment?
			if(!pc.string_start)
				pc.string_start=begin_pos;
			// append it
			pc.string.append_strdup_know_length(begin, end-begin);
		}
	}
	if(!pc.string.is_empty()) { // something accumulated?
		// create STRING value: array of OP_VALUE+origin+vstring
 		*lvalp=VL(
			new VString(*new String(pc.string, String::L_CLEAN)),
			pc.file_no, pc.string_start.line, pc.string_start.col);
		// new pieces storage
		pc.string.clear();
		pc.string_start.clear();
		// make current result be pending for next call, return STRING for now
		pc.pending_state=result;  result=STRING;
	}
	if(skip_analized) {
		pc.source+=skip_analized;  pc.pos.col+=skip_analized;
	}
	return result;
}

static int real_yyerror(Parse_control *pc, char *s) {  // Called by yyparse on error
	   strncpy(PC.error, s, MAX_STRING);
	   return 1;
}

static void yyprint(FILE *file, int type, YYSTYPE value) {
	if(type==STRING)
		fprintf(file, " \"%s\"", LA2S(*value)->cstr());
}


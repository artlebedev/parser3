/* A Bison parser, made by GNU Bison 1.875.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



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
     NUNARY = 287
   };
#endif
#define EON 258
#define STRING 259
#define BOGUS 260
#define BAD_STRING_COMPARISON_OPERATOR 261
#define BAD_HEX_LITERAL 262
#define BAD_METHOD_DECL_START 263
#define BAD_METHOD_PARAMETER_NAME_CHARACTER 264
#define BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE 265
#define LAND 266
#define LOR 267
#define LXOR 268
#define NXOR 269
#define NLE 270
#define NGE 271
#define NEQ 272
#define NNE 273
#define NSL 274
#define NSR 275
#define SLT 276
#define SGT 277
#define SLE 278
#define SGE 279
#define SEQ 280
#define SNE 281
#define DEF 282
#define IN 283
#define FEXISTS 284
#define DEXISTS 285
#define IS 286
#define NUNARY 287




/* Copy the first part of user declarations.  */
#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.tab.C,v 1.105 2005/11/03 10:17:22 paf Exp $
*/

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
#include "pa_vvoid.h"
#include "pa_vmethod_frame.h"

// defines

#define USE_CONTROL_METHOD_NAME "USE"

// forwards

static int real_yyerror(Parse_control* pc, char* s);
static void yyprint(FILE* file, int type, YYSTYPE value);
static int yylex(YYSTYPE* lvalp, void* pc);

// local convinient inplace typecast & var
#undef PC
#define PC  (*(Parse_control *)pc)
#undef POOL
#define POOL  (*PC.pool)
#ifndef DOXYGEN


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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */
#line 208 "compile.tab.C"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# if YYSTACK_USE_ALLOCA
#  define YYSTACK_ALLOC alloca
# else
#  ifndef YYSTACK_USE_ALLOCA
#   if defined (alloca) || defined (_ALLOCA_H)
#    define YYSTACK_ALLOC alloca
#   else
#    ifdef __GNUC__
#     define YYSTACK_ALLOC __builtin_alloca
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC malloc
#  define YYSTACK_FREE free
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short) + sizeof (YYSTYPE))				\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  51
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   479

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  60
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  87
/* YYNRULES -- Number of rules. */
#define YYNRULES  168
/* YYNRULES -- Number of states. */
#define YYNSTATES  259

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   287

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      46,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    44,    58,     2,    50,    41,    35,    59,
      54,    55,    38,    36,     2,    37,    53,    39,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    57,    49,
      32,     2,    33,     2,    45,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    47,    40,    48,    56,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    51,    34,    52,    43,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    42
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short yyprhs[] =
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
     299,   302,   305,   307,   309,   311,   313,   317,   321,   325,
     328,   331,   334,   337,   340,   343,   346,   349,   353,   357,
     361,   365,   369,   373,   377,   381,   385,   389,   393,   397,
     401,   405,   409,   413,   417,   421,   425,   429,   433,   437,
     441,   445,   449,   453,   457,   459,   461,   463,   464
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short yyrhs[] =
{
      61,     0,    -1,    63,    -1,    62,    -1,    64,    -1,    62,
      64,    -1,    77,    -1,    65,    -1,    70,    -1,    45,     4,
      46,    66,    -1,   146,    -1,    67,    -1,    68,    -1,    67,
      68,    -1,    69,    46,    -1,   146,    -1,     4,    -1,    -1,
      45,     4,    73,    72,    76,    46,    71,    77,    -1,   146,
      -1,    73,    -1,    47,    74,    48,    -1,   146,    -1,    75,
      -1,     4,    -1,    75,    49,     4,    -1,   146,    -1,     4,
      -1,   146,    -1,    78,    -1,    79,    -1,    78,    79,    -1,
     144,    -1,    80,    -1,    81,    -1,    89,    -1,   104,    -1,
      82,    -1,    50,    83,    -1,    85,     3,    -1,    84,    -1,
      51,    85,    52,    -1,    86,    -1,    87,    -1,    88,    -1,
     137,    88,    -1,   127,    -1,   124,   127,    -1,    50,    90,
      94,    -1,    91,    -1,    92,    -1,    93,    -1,   123,    -1,
      53,   123,    -1,   137,   123,    -1,    95,    -1,    98,    -1,
      99,    -1,    -1,    -1,    47,    96,   100,    97,    48,    -1,
      54,   140,    55,    -1,    51,    77,    52,    -1,   145,    -1,
       4,    -1,   101,    -1,   102,    -1,   103,    -1,    80,    -1,
      79,    78,    -1,   105,    -1,    -1,    -1,    56,   106,   108,
     107,   109,     3,    -1,    85,    -1,   110,    -1,   109,   110,
      -1,   111,    -1,   114,    -1,   115,    -1,    -1,    -1,    47,
     112,   116,   113,    48,    -1,    54,   117,    55,    -1,    51,
     118,    52,    -1,   119,    -1,   116,    49,   119,    -1,   120,
      -1,   117,    49,   120,    -1,   121,    -1,   118,    49,   121,
      -1,   122,    -1,   140,    -1,    77,    -1,   145,    -1,     4,
      -1,   101,    -1,   128,    -1,   124,   128,    -1,   125,    -1,
     124,   125,    -1,   126,    53,    -1,   128,    -1,   128,    -1,
       4,     5,    -1,     4,    -1,   129,    -1,   130,    -1,   131,
      -1,    50,   134,    -1,     4,   135,    -1,    -1,    -1,    47,
     132,    78,   133,    48,    -1,     4,    -1,   136,    -1,   135,
     136,    -1,    50,   134,    -1,   138,    -1,   139,    -1,     4,
      57,    -1,   138,    57,    -1,   141,    -1,   142,    -1,    82,
      -1,   105,    -1,    58,   143,    58,    -1,    59,   143,    59,
      -1,    54,   141,    55,    -1,    37,   141,    -1,    36,   141,
      -1,    43,   141,    -1,    44,   141,    -1,    27,   141,    -1,
      28,   141,    -1,    29,   141,    -1,    30,   141,    -1,   141,
      37,   141,    -1,   141,    36,   141,    -1,   141,    38,   141,
      -1,   141,    39,   141,    -1,   141,    41,   141,    -1,   141,
      40,   141,    -1,   141,    19,   141,    -1,   141,    20,   141,
      -1,   141,    35,   141,    -1,   141,    34,   141,    -1,   141,
      14,   141,    -1,   141,    11,   141,    -1,   141,    12,   141,
      -1,   141,    13,   141,    -1,   141,    32,   141,    -1,   141,
      33,   141,    -1,   141,    15,   141,    -1,   141,    16,   141,
      -1,   141,    17,   141,    -1,   141,    18,   141,    -1,   141,
      21,   141,    -1,   141,    22,   141,    -1,   141,    23,   141,
      -1,   141,    24,   141,    -1,   141,    25,   141,    -1,   141,
      26,   141,    -1,   141,    31,   141,    -1,     4,    -1,    77,
      -1,     4,    -1,    -1,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short yyrline[] =
{
       0,   121,   121,   128,   130,   130,   131,   133,   133,   135,
     207,   207,   208,   208,   209,   210,   210,   212,   212,   252,
     252,   253,   254,   254,   255,   255,   257,   257,   261,   261,
     263,   263,   264,   264,   265,   265,   265,   269,   276,   277,
     277,   278,   280,   281,   282,   304,   305,   305,   309,   314,
     315,   316,   317,   334,   339,   342,   343,   344,   346,   349,
     346,   357,   365,   372,   373,   374,   376,   382,   383,   383,
     387,   393,   396,   393,   412,   414,   414,   416,   417,   418,
     420,   423,   420,   426,   427,   429,   430,   433,   434,   437,
     438,   440,   444,   458,   463,   464,   465,   470,   470,   472,
     472,   473,   474,   482,   487,   490,   491,   492,   493,   495,
     499,   508,   511,   508,   519,   524,   524,   525,   531,   532,
     534,   546,   558,   560,   561,   562,   563,   564,   565,   567,
     568,   569,   570,   571,   572,   573,   574,   576,   577,   578,
     579,   580,   581,   582,   583,   584,   585,   586,   587,   588,
     589,   590,   591,   592,   593,   594,   595,   596,   597,   598,
     599,   600,   601,   602,   605,   610,   619,   624,   625
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EON", "STRING", "BOGUS", 
  "BAD_STRING_COMPARISON_OPERATOR", "BAD_HEX_LITERAL", 
  "BAD_METHOD_DECL_START", "BAD_METHOD_PARAMETER_NAME_CHARACTER", 
  "BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE", "\"&&\"", 
  "\"||\"", "\"!||\"", "\"!|\"", "\"<=\"", "\">=\"", "\"==\"", "\"!=\"", 
  "\"<<\"", "\">>\"", "\"lt\"", "\"gt\"", "\"le\"", "\"ge\"", "\"eq\"", 
  "\"ne\"", "\"def\"", "\"in\"", "\"-f\"", "\"-d\"", "\"is\"", "'<'", 
  "'>'", "'|'", "'&'", "'+'", "'-'", "'*'", "'/'", "'\\\\'", "'%'", 
  "NUNARY", "'~'", "'!'", "'@'", "'\\n'", "'['", "']'", "';'", "'$'", 
  "'{'", "'}'", "'.'", "'('", "')'", "'^'", "':'", "'\"'", "'''", 
  "$accept", "all", "methods", "one_big_piece", "method", 
  "control_method", "maybe_control_strings", "control_strings", 
  "control_string", "maybe_string", "code_method", "@1", 
  "maybe_bracketed_strings", "bracketed_maybe_strings", "maybe_strings", 
  "strings", "maybe_comment", "maybe_codes", "codes", "code", "action", 
  "get", "get_value", "get_name_value", "name_in_curly_rdive", 
  "name_without_curly_rdive", "name_without_curly_rdive_read", 
  "name_without_curly_rdive_class", "name_without_curly_rdive_code", 
  "put", "name_expr_wdive", "name_expr_wdive_root", 
  "name_expr_wdive_write", "name_expr_wdive_class", "construct", 
  "construct_square", "@2", "@3", "construct_round", "construct_curly", 
  "any_constructor_code_value", "constructor_code_value", 
  "constructor_code", "codes__excluding_sole_str_literal", "call", 
  "call_value", "@4", "@5", "call_name", "store_params", "store_param", 
  "store_square_param", "@6", "@7", "store_round_param", 
  "store_curly_param", "store_code_param_parts", "store_expr_param_parts", 
  "store_curly_param_parts", "store_code_param_part", 
  "store_expr_param_part", "store_curly_param_part", "code_param_value", 
  "name_expr_dive_code", "name_path", "name_step", "name_advance1", 
  "name_advance2", "name_expr_value", "name_expr_subvar_value", 
  "name_expr_with_subvar_value", "name_square_code_value", "@8", "@9", 
  "subvar_ref_name_rdive", "subvar_get_writes", "subvar__get_write", 
  "class_prefix", "class_static_prefix", "class_constructor_prefix", 
  "expr_value", "expr", "double_or_STRING", "string_inside_quotes_value", 
  "write_string", "void_value", "empty", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,    60,    62,   124,    38,    43,    45,    42,    47,
      92,    37,   287,   126,    33,    64,    10,    91,    93,    59,
      36,   123,   125,    46,    40,    41,    94,    58,    34,    39
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    60,    61,    61,    62,    62,    63,    64,    64,    65,
      66,    66,    67,    67,    68,    69,    69,    71,    70,    72,
      72,    73,    74,    74,    75,    75,    76,    76,    77,    77,
      78,    78,    79,    79,    80,    80,    80,    81,    82,    83,
      83,    84,    85,    85,    86,    87,    88,    88,    89,    90,
      90,    90,    91,    92,    93,    94,    94,    94,    96,    97,
      95,    98,    99,   100,   100,   100,   101,   102,   103,   103,
     104,   106,   107,   105,   108,   109,   109,   110,   110,   110,
     112,   113,   111,   114,   115,   116,   116,   117,   117,   118,
     118,   119,   120,   121,   122,   122,   122,   123,   123,   124,
     124,   125,   126,   127,   127,   128,   128,   128,   128,   129,
     130,   132,   133,   131,   134,   135,   135,   136,   137,   137,
     138,   139,   140,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   142,   143,   144,   145,   146
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
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
       2,     2,     1,     1,     1,     1,     3,     3,     3,     2,
       2,     2,     2,     2,     2,     2,     2,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     1,     1,     0,     0
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
{
     168,   166,     0,     0,    71,     0,     3,     2,     4,     7,
       8,     6,    29,    30,    33,    34,    37,    35,    36,    70,
      32,    28,     0,   105,   111,     0,     0,     0,    38,    40,
       0,    42,    43,    44,     0,    49,    50,    51,    52,     0,
      99,     0,    46,    97,   106,   107,   108,     0,   118,   119,
       0,     1,     5,    31,   168,   168,   168,   104,     0,   120,
     110,   115,     0,   114,   109,     0,     0,   103,     0,   105,
      53,     0,    97,    39,    58,   168,     0,    48,    55,    56,
      57,   105,   100,    47,    98,   101,    45,    54,   121,    74,
      72,    16,     9,    11,    12,     0,    10,    24,     0,    23,
      22,   168,    20,    19,   117,   116,   112,    41,    98,   167,
       0,   164,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   168,   168,   124,   125,     0,   122,   123,     0,
      13,    15,    14,    21,     0,    27,     0,    26,     0,   166,
       0,    33,    59,    65,    66,    67,    63,    62,   133,   134,
     135,   136,   130,   129,   131,   132,     0,   165,     0,     0,
      61,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    80,   168,
       0,     0,    75,    77,    78,    79,    25,    17,   113,    69,
       0,   128,   126,   127,   148,   149,   150,   147,   153,   154,
     155,   156,   143,   144,   157,   158,   159,   160,   161,   162,
     163,   151,   152,   146,   145,   138,   137,   139,   140,   142,
     141,   167,    93,     0,    89,     0,    87,    92,    73,    76,
     168,    60,   166,    96,    81,    85,    91,    94,   168,    84,
       0,    83,    18,   167,     0,    90,    88,    86,    82
};

/* YYDEFGOTO[NTERM-NUM]. */
static const short yydefgoto[] =
{
      -1,     5,     6,     7,     8,     9,    92,    93,    94,    95,
      10,   240,   101,    56,    98,    99,   136,   157,    12,    13,
      14,    15,   124,    28,    29,    30,    31,    32,    33,    17,
      34,    35,    36,    37,    77,    78,   109,   200,    79,    80,
     142,   243,   144,   145,    18,   125,    50,   129,    90,   191,
     192,   193,   231,   254,   194,   195,   244,   235,   233,   245,
     236,   234,   246,    38,    66,    40,    41,    42,    67,    44,
      45,    46,    62,   138,    64,    60,    61,    68,    48,    49,
     237,   127,   128,   158,    20,   247,    21
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -106
static const short yypact[] =
{
       4,  -106,    15,    39,  -106,    28,   -13,  -106,  -106,  -106,
    -106,  -106,     5,  -106,  -106,  -106,  -106,  -106,  -106,  -106,
    -106,  -106,    51,    16,  -106,    43,    65,    67,  -106,  -106,
      77,  -106,  -106,  -106,   -29,  -106,  -106,  -106,  -106,    89,
    -106,    34,  -106,    17,  -106,  -106,  -106,    89,    25,  -106,
      65,  -106,  -106,  -106,    97,   106,    69,  -106,    43,  -106,
      63,  -106,     5,  -106,  -106,    82,    89,    85,    89,    63,
    -106,    67,    85,  -106,  -106,     5,     9,  -106,  -106,  -106,
    -106,     6,  -106,  -106,    17,  -106,  -106,  -106,  -106,  -106,
    -106,  -106,  -106,    12,  -106,    98,    99,  -106,    87,    94,
    -106,   142,  -106,  -106,  -106,  -106,     5,  -106,    85,    22,
     115,  -106,     9,     9,     9,     9,     9,     9,     9,     9,
      53,     9,     5,     5,  -106,  -106,    92,   272,  -106,    -3,
    -106,  -106,  -106,  -106,   164,  -106,   148,  -106,   140,   147,
       5,    70,  -106,  -106,  -106,  -106,  -106,  -106,   431,   431,
     431,   431,  -106,  -106,  -106,  -106,   138,  -106,   139,   137,
    -106,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,     9,     9,
       9,     9,     9,     9,     9,     9,     9,     9,  -106,     5,
       9,    37,  -106,  -106,  -106,  -106,  -106,  -106,  -106,     5,
     150,  -106,  -106,  -106,   362,   334,   303,   146,   390,   390,
     403,   403,   189,   189,   390,   390,   390,   390,   403,   403,
     431,   390,   390,   438,   198,    90,    90,  -106,  -106,  -106,
    -106,    29,  -106,   -25,  -106,   -26,  -106,  -106,  -106,  -106,
       5,  -106,    72,  -106,   151,  -106,  -106,  -106,     5,  -106,
       9,  -106,  -106,    29,   154,  -106,  -106,  -106,  -106
};

/* YYPGOTO[NTERM-NUM].  */
static const short yypgoto[] =
{
    -106,  -106,  -106,  -106,   197,  -106,  -106,  -106,   111,  -106,
    -106,  -106,  -106,   149,  -106,  -106,  -106,     1,   -59,    -7,
    -105,  -106,     0,  -106,  -106,    -9,  -106,  -106,   -37,  -106,
    -106,  -106,  -106,  -106,  -106,  -106,  -106,  -106,  -106,  -106,
    -106,   110,  -106,  -106,  -106,     2,  -106,  -106,  -106,  -106,
      24,  -106,  -106,  -106,  -106,  -106,  -106,  -106,  -106,   -47,
     -30,   -27,  -106,   -12,    80,    66,  -106,   -32,     3,  -106,
    -106,  -106,  -106,  -106,   165,  -106,   162,   229,  -106,  -106,
     167,    95,  -106,   121,  -106,   136,    40
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -169
static const short yytable[] =
{
      16,    11,    19,   106,   141,    53,    43,    83,     1,     1,
      86,    57,    16,   111,    19,    70,    91,    65,    74,    22,
    -103,    57,    75,   250,   248,    76,   139,   249,    51,   251,
      72,    86,     2,   242,    83,    87,   112,   113,   114,   115,
     238,    89,    84,    23,   188,   116,   117,    63,   189,     2,
      43,   190,   118,   119,     3,     3,    58,    23,  -168,   120,
       4,     4,    16,   121,    19,     4,    58,   122,   123,    23,
    -102,    69,     3,    59,   108,    16,   110,    19,     4,     3,
      73,   199,    88,    39,   188,     4,    24,    85,   189,    25,
      26,   190,    27,    81,    96,   100,   103,    54,    55,    53,
      24,    91,   140,    25,    26,    82,    16,    71,    19,    16,
      97,    19,    24,    58,    24,    25,    55,    25,   -68,   -68,
     -95,   -95,    16,    16,    19,    19,   141,    39,   184,   185,
     186,   187,    82,   131,   107,   133,    24,    82,  -102,    25,
      16,   137,    19,   134,   132,   -15,   135,   160,   141,   161,
     162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   175,   176,   169,   170,   147,   196,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     180,   181,   182,   183,   184,   185,   186,   187,   198,    16,
     232,    19,    53,   201,   197,   -64,   203,   202,   241,    16,
     253,    19,   258,    52,   130,   102,   257,   148,   149,   150,
     151,   152,   153,   154,   155,   239,   156,   169,   170,   143,
     256,   255,   105,   104,   140,   182,   183,   184,   185,   186,
     187,    16,    47,    19,   182,   183,   184,   185,   186,   187,
      16,   252,    19,   126,   159,   146,   140,     0,    16,   232,
      19,     0,     0,    16,     0,    19,   204,   205,   206,   207,
     208,   209,   210,   211,   212,   213,   214,   215,   216,   217,
     218,   219,   220,   221,   222,   223,   224,   225,   226,   227,
     228,   229,   230,   161,   162,   163,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   161,   162,     0,   164,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
       0,     0,     0,     0,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   161,     0,     0,   164,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,     0,     0,     0,     0,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   164,   165,   166,   167,
     168,   169,   170,   171,   172,   173,   174,   175,   176,     0,
       0,     0,     0,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   164,     0,     0,   167,   168,   169,
     170,     0,     0,     0,     0,   175,   176,   164,     0,     0,
       0,   177,   169,   170,   180,   181,   182,   183,   184,   185,
     186,   187,     0,     0,   177,     0,     0,   180,   181,   182,
     183,   184,   185,   186,   187,   164,     0,     0,     0,     0,
     169,   170,     0,     0,     0,     0,     0,   169,   170,     0,
       0,     0,     0,     0,     0,   180,   181,   182,   183,   184,
     185,   186,   187,   181,   182,   183,   184,   185,   186,   187
};

static const short yycheck[] =
{
       0,     0,     0,    62,   109,    12,     3,    39,     4,     4,
      47,     5,    12,     4,    12,    27,     4,    26,    47,     4,
       3,     5,    51,    49,    49,    54,     4,    52,     0,    55,
      27,    68,    45,     4,    66,    47,    27,    28,    29,    30,
       3,    50,    39,     4,    47,    36,    37,     4,    51,    45,
      47,    54,    43,    44,    50,    50,    50,     4,    46,    50,
      56,    56,    62,    54,    62,    56,    50,    58,    59,     4,
      53,     4,    50,    57,    71,    75,    75,    75,    56,    50,
       3,   140,    57,     3,    47,    56,    47,    53,    51,    50,
      51,    54,    53,     4,    54,    55,    56,    46,    47,   106,
      47,     4,   109,    50,    51,    39,   106,    27,   106,   109,
       4,   109,    47,    50,    47,    50,    47,    50,    48,    49,
      48,    49,   122,   123,   122,   123,   231,    47,    38,    39,
      40,    41,    66,    93,    52,    48,    47,    71,    53,    50,
     140,   101,   140,    49,    46,    46,     4,    55,   253,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    19,    20,    52,     4,    31,
      32,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      34,    35,    36,    37,    38,    39,    40,    41,    48,   189,
     189,   189,   199,    55,    46,    48,    59,    58,    48,   199,
      49,   199,    48,     6,    93,    56,   253,   112,   113,   114,
     115,   116,   117,   118,   119,   191,   121,    19,    20,   109,
     250,   248,    60,    58,   231,    36,    37,    38,    39,    40,
      41,   231,     3,   231,    36,    37,    38,    39,    40,    41,
     240,   240,   240,    76,   123,   109,   253,    -1,   248,   248,
     248,    -1,    -1,   253,    -1,   253,   161,   162,   163,   164,
     165,   166,   167,   168,   169,   170,   171,   172,   173,   174,
     175,   176,   177,   178,   179,   180,   181,   182,   183,   184,
     185,   186,   187,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    11,    12,    -1,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    11,    -1,    -1,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,    -1,    -1,    -1,    -1,    31,    32,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    26,    -1,
      -1,    -1,    -1,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    14,    -1,    -1,    17,    18,    19,
      20,    -1,    -1,    -1,    -1,    25,    26,    14,    -1,    -1,
      -1,    31,    19,    20,    34,    35,    36,    37,    38,    39,
      40,    41,    -1,    -1,    31,    -1,    -1,    34,    35,    36,
      37,    38,    39,    40,    41,    14,    -1,    -1,    -1,    -1,
      19,    20,    -1,    -1,    -1,    -1,    -1,    19,    20,    -1,
      -1,    -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,
      39,    40,    41,    35,    36,    37,    38,    39,    40,    41
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     4,    45,    50,    56,    61,    62,    63,    64,    65,
      70,    77,    78,    79,    80,    81,    82,    89,   104,   105,
     144,   146,     4,     4,    47,    50,    51,    53,    83,    84,
      85,    86,    87,    88,    90,    91,    92,    93,   123,   124,
     125,   126,   127,   128,   129,   130,   131,   137,   138,   139,
     106,     0,    64,    79,    46,    47,    73,     5,    50,    57,
     135,   136,   132,     4,   134,    85,   124,   128,   137,     4,
     123,   124,   128,     3,    47,    51,    54,    94,    95,    98,
      99,     4,   125,   127,   128,    53,    88,   123,    57,    85,
     108,     4,    66,    67,    68,    69,   146,     4,    74,    75,
     146,    72,    73,   146,   134,   136,    78,    52,   128,    96,
      77,     4,    27,    28,    29,    30,    36,    37,    43,    44,
      50,    54,    58,    59,    82,   105,   140,   141,   142,   107,
      68,   146,    46,    48,    49,     4,    76,   146,   133,     4,
      79,    80,   100,   101,   102,   103,   145,    52,   141,   141,
     141,   141,   141,   141,   141,   141,   141,    77,   143,   143,
      55,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    47,    51,
      54,   109,   110,   111,   114,   115,     4,    46,    48,    78,
      97,    55,    58,    59,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   141,   141,   141,   141,   141,   141,
     141,   112,    77,   118,   121,   117,   120,   140,     3,   110,
      71,    48,     4,   101,   116,   119,   122,   145,    49,    52,
      49,    55,    77,    49,   113,   121,   120,   119,    48
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrlab1

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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)         \
  Current.first_line   = Rhs[1].first_line;      \
  Current.first_column = Rhs[1].first_column;    \
  Current.last_line    = Rhs[N].last_line;       \
  Current.last_column  = Rhs[N].last_column;
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (cinluded).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short *bottom, short *top)
#else
static void
yy_stack_print (bottom, top)
    short *bottom;
    short *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylineno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylineno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
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

  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short	yyssa[YYINITDEPTH];
  short *yyss = yyssa;
  register short *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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
#line 121 "compile.y"
    {
	Method& method=*new Method(Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		yyvsp[0]/*parser_code*/, 0/*native_code*/);
	PC.cclass->add_method(PC.alias_method(main_method_name), method);
;}
    break;

  case 9:
#line 136 "compile.y"
    {
	const String& command=*LA2S(*yyvsp[-2]);
	YYSTYPE strings_code=yyvsp[0];
	if(strings_code->count()<1*OPERATIONS_PER_OPVALUE) {
		strcpy(PC.error, "@");
		strcat(PC.error, command.cstr());
		strcat(PC.error, " is empty");
		YYERROR;
	}
	if(command==CLASS_NAME) {
		if(PC.cclass->base_class()) { // already changed from default?
			strcpy(PC.error, "class already have a name '");
			strncat(PC.error, PC.cclass->name().cstr(), 100);
			strcat(PC.error, "'");
			YYERROR;
		}
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			// new class' name
			const String& name=*LA2S(*strings_code);
			// creating the class
			VStateless_class* cclass=new VClass;
			PC.cclass=cclass;
			PC.cclass->set_name(name);
			// append to request's classes
			PC.request.classes().put(name, cclass);
		} else {
			strcpy(PC.error, "@"CLASS_NAME" must contain only one line with class name (contains more then one)");
			YYERROR;
		}
	} else if(command==USE_CONTROL_METHOD_NAME) {
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE) 
			PC.request.use_file(PC.request.main_class, *LA2S(*strings_code, i));
	} else if(command==BASE_NAME) {
		if(PC.cclass->base_class()) { // already changed from default?
			strcpy(PC.error, "class already have a base '");
			strncat(PC.error, PC.cclass->base_class()->name().cstr(), 100);
			strcat(PC.error, "'");
			YYERROR;
		}
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			const String& base_name=*LA2S(*strings_code);
			if(Value* base_class_value=PC.request.classes().get(base_name)) {
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
	} else {
		strcpy(PC.error, "'");
		strncat(PC.error, command.cstr(), MAX_STRING/2);
		strcat(PC.error, "' invalid special name. valid names are "
			"'"CLASS_NAME"', '"USE_CONTROL_METHOD_NAME"' and '"BASE_NAME"'");
		YYERROR;
	}
;}
    break;

  case 13:
#line 208 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 17:
#line 212 "compile.y"
    { 
	PC.explicit_result=false;
	const String& name=*LA2S(*yyvsp[-4]);

	YYSTYPE params_names_code=yyvsp[-3];
	ArrayString* params_names=0;
	if(int size=params_names_code->count()) {
		params_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE)
			*params_names+=LA2S(*params_names_code, i);
	}

	YYSTYPE locals_names_code=yyvsp[-2];
	ArrayString* locals_names=0;
	if(int size=locals_names_code->count()) {
		locals_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE) {
			const String* local_name=LA2S(*locals_names_code, i);
			if(*local_name==RESULT_VAR_NAME)
				PC.explicit_result=true;
			else
				*locals_names+=local_name;
		}
	}

	Method* method=new Method(
		//name, 
		Method::CT_ANY,
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		0/*to be filled later in next {} */, 0);
	PC.cclass->add_method(PC.alias_method(name), *method);
	*reinterpret_cast<Method**>(&yyval)=method;

	// todo: check [][;result;]
;}
    break;

  case 18:
#line 247 "compile.y"
    {
	// fill in the code
	reinterpret_cast<Method*>(yyvsp[-1])->parser_code=yyvsp[0];
;}
    break;

  case 21:
#line 253 "compile.y"
    {yyval=yyvsp[-1];}
    break;

  case 25:
#line 255 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 31:
#line 263 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 37:
#line 269 "compile.y"
    {
	yyval=yyvsp[0]; /* stack: resulting value */ 
	changetail_or_append(*yyval, 
		OP_GET_ELEMENT, false,  /*->*/OP_GET_ELEMENT__WRITE,
		/*or */OP_WRITE_VALUE
		); /* value=pop; wcontext.write(value) */
;}
    break;

  case 38:
#line 276 "compile.y"
    { yyval=yyvsp[0] ;}
    break;

  case 41:
#line 278 "compile.y"
    { yyval=yyvsp[-1] ;}
    break;

  case 44:
#line 282 "compile.y"
    {
	yyval=N(); 
	ArrayOperation* diving_code=yyvsp[0];
	const String* first_name=LA2S(*diving_code);
	// self.xxx... -> xxx...
	// OP_VALUE+origin+string+OP_GET_ELEMENT+... -> OP_WITH_SELF+...
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(*yyval, OP_WITH_SELF); /* stack: starting context */
		P(*yyval, *diving_code, 
			/* skip over... */
			diving_code->count()>=4?4/*OP_VALUE+origin+string+OP_GET_ELEMENTx*/:3/*OP_+origin+string*/);
	} else {
		O(*yyval, OP_WITH_READ); /* stack: starting context */

		// ^if ELEMENT -> ^if ELEMENT_OR_OPERATOR
		// OP_VALUE+origin+string+OP_GET_ELEMENT. -> OP_VALUE+origin+string+OP_GET_ELEMENT_OR_OPERATOR.
		if(PC.in_call_value && diving_code->count()==4)
			diving_code->put(4-1, OP_GET_ELEMENT_OR_OPERATOR);
		P(*yyval, *diving_code);
	}
	/* diving code; stack: current context */
;}
    break;

  case 45:
#line 304 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 47:
#line 305 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 48:
#line 309 "compile.y"
    {
	yyval=yyvsp[-1]; /* stack: context,name */
	P(*yyval, *yyvsp[0]); /* stack: context,name,constructor_value */
;}
    break;

  case 52:
#line 317 "compile.y"
    {
	yyval=N();
	ArrayOperation* diving_code=yyvsp[0];
	const String* first_name=LA2S(*diving_code);
	// $self.xxx... -> $xxx...
	// OP_VALUE+origin+string+OP_GET_ELEMENT+... -> OP_WITH_SELF+...
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(*yyval, OP_WITH_SELF); /* stack: starting context */
		P(*yyval, *diving_code, 
			/* skip over... */
			diving_code->count()>=4?4/*OP_VALUE+origin+string+OP_GET_ELEMENTx*/:3/*OP_+origin+string*/);
	} else {
		O(*yyval, OP_WITH_ROOT); /* stack: starting context */
		P(*yyval, *diving_code);
	}
	/* diving code; stack: current context */
;}
    break;

  case 53:
#line 334 "compile.y"
    {
	yyval=N(); 
	O(*yyval, OP_WITH_WRITE); /* stack: starting context */
	P(*yyval, *yyvsp[0]); /* diving code; stack: context,name */
;}
    break;

  case 54:
#line 339 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 58:
#line 346 "compile.y"
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 59:
#line 349 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;}
    break;

  case 60:
#line 351 "compile.y"
    {
	// stack: context, name
	yyval=yyvsp[-2]; // stack: context, name, value
	O(*yyval, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;}
    break;

  case 61:
#line 357 "compile.y"
    { 
	yyval=N(); 
	O(*yyval, OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(*yyval, *yyvsp[-1]); // stack: context, name, value
	O(*yyval, OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;}
    break;

  case 62:
#line 365 "compile.y"
    {
	// stack: context, name
	yyval=N(); 
	OA(*yyval, OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;}
    break;

  case 66:
#line 376 "compile.y"
    {
	yyval=N(); 
	OA(*yyval, OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;}
    break;

  case 69:
#line 383 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 70:
#line 387 "compile.y"
    {
	yyval=yyvsp[0]; /* stack: value */
	changetail_or_append(*yyval, 
		OP_CALL, true,  /*->*/ OP_CALL__WRITE,
		/*or */OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;}
    break;

  case 71:
#line 393 "compile.y"
    { 
					PC.in_call_value=true; 
			;}
    break;

  case 72:
#line 396 "compile.y"
    {
				PC.in_call_value=false;
			;}
    break;

  case 73:
#line 399 "compile.y"
    { /* ^field.$method{vasya} */
	yyval=yyvsp[-3]; /* with_xxx,diving code; stack: context,method_junction */

	YYSTYPE params_code=yyvsp[-1];
	if(params_code->count()==4) { // probably [] case. [OP_VALUE+origin+Void+STORE_PARAM]
		if(Value* value=LA2V(*params_code)) // it is OP_VALUE+origin+value?
			if(!value->is_defined()) // value is VVoid?
				params_code=0; // ^zzz[] case. don't append lone empty param.
	}
	/* stack: context, method_junction */
	OA(*yyval, OP_CALL, params_code); // method_frame=make frame(pop junction); ncontext=pop; call(ncontext,method_frame) stack: value
;}
    break;

  case 76:
#line 414 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 80:
#line 420 "compile.y"
    {
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 81:
#line 423 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;}
    break;

  case 82:
#line 425 "compile.y"
    {yyval=yyvsp[-2];}
    break;

  case 83:
#line 426 "compile.y"
    {yyval=yyvsp[-1];}
    break;

  case 84:
#line 427 "compile.y"
    {yyval=yyvsp[-1];}
    break;

  case 86:
#line 430 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 88:
#line 434 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 90:
#line 438 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 91:
#line 440 "compile.y"
    {
	yyval=yyvsp[0];
	O(*yyval, OP_STORE_PARAM);
;}
    break;

  case 92:
#line 444 "compile.y"
    {
	YYSTYPE expr_code=yyvsp[0];
	if(expr_code->count()==3) { // optimizing (double) case. [OP_VALUE+origin+Double]
		yyval=expr_code; 
		O(*yyval, OP_STORE_PARAM); // no evaluating
	} else {
		ArrayOperation* code=N();
		O(*code, OP_PREPARE_TO_EXPRESSION);
		P(*code, *expr_code);
		O(*code, OP_WRITE_EXPR_RESULT);
		yyval=N(); 
		OA(*yyval, OP_EXPR_CODE__STORE_PARAM, code);
	}
;}
    break;

  case 93:
#line 458 "compile.y"
    {
	yyval=N(); 
	OA(*yyval, OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
;}
    break;

  case 98:
#line 470 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 100:
#line 472 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 102:
#line 474 "compile.y"
    {
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;}
    break;

  case 103:
#line 482 "compile.y"
    {
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;}
    break;

  case 109:
#line 495 "compile.y"
    {
	yyval=yyvsp[0];
	O(*yyval, OP_GET_ELEMENT);
;}
    break;

  case 110:
#line 499 "compile.y"
    {
	ArrayOperation* code;
	{
		change_string_literal_to_write_string_literal(*(code=yyvsp[-1]));
		P(*code, *yyvsp[0]);
	}
	yyval=N(); 
	OA(*yyval, OP_STRING_POOL, code);
;}
    break;

  case 111:
#line 508 "compile.y"
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 112:
#line 511 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;}
    break;

  case 113:
#line 513 "compile.y"
    {
	yyval=N(); 
	OA(*yyval, OP_OBJECT_POOL, yyvsp[-2]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;}
    break;

  case 114:
#line 519 "compile.y"
    {
	yyval=N(); 
	O(*yyval, OP_WITH_READ);
	P(*yyval, *yyvsp[0]);
;}
    break;

  case 116:
#line 524 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 117:
#line 525 "compile.y"
    {
	yyval=yyvsp[0];
	O(*yyval, OP_GET_ELEMENT__WRITE);
;}
    break;

  case 120:
#line 534 "compile.y"
    {
	yyval=yyvsp[-1]; // stack: class name string
	if(*LA2S(*yyval) == BASE_NAME) { // pseudo BASE class
		if(VStateless_class* base=PC.cclass->base_class()) {
			change_string_literal_value(*yyval, base->name());
		} else {
			strcpy(PC.error, "no base class declared");
			YYERROR;
		}
	}
	O(*yyval, OP_GET_CLASS);
;}
    break;

  case 121:
#line 546 "compile.y"
    {
	yyval=yyvsp[-1];
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(*yyval, OP_PREPARE_TO_CONSTRUCT_OBJECT);
;}
    break;

  case 126:
#line 563 "compile.y"
    { yyval = yyvsp[-1]; ;}
    break;

  case 127:
#line 564 "compile.y"
    { yyval = yyvsp[-1]; ;}
    break;

  case 128:
#line 565 "compile.y"
    { yyval = yyvsp[-1]; ;}
    break;

  case 129:
#line 567 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP_NEG) ;}
    break;

  case 130:
#line 568 "compile.y"
    { yyval=yyvsp[0] ;}
    break;

  case 131:
#line 569 "compile.y"
    { yyval=yyvsp[0];	 O(*yyval, OP_INV) ;}
    break;

  case 132:
#line 570 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP_NOT) ;}
    break;

  case 133:
#line 571 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP_DEF) ;}
    break;

  case 134:
#line 572 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP_IN) ;}
    break;

  case 135:
#line 573 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP_FEXISTS) ;}
    break;

  case 136:
#line 574 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP_DEXISTS) ;}
    break;

  case 137:
#line 576 "compile.y"
    {	yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_SUB) ;}
    break;

  case 138:
#line 577 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_ADD) ;}
    break;

  case 139:
#line 578 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_MUL) ;}
    break;

  case 140:
#line 579 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_DIV) ;}
    break;

  case 141:
#line 580 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_MOD) ;}
    break;

  case 142:
#line 581 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_INTDIV) ;}
    break;

  case 143:
#line 582 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_SL) ;}
    break;

  case 144:
#line 583 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_SR) ;}
    break;

  case 145:
#line 584 "compile.y"
    { yyval=yyvsp[-2]; 	P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_AND) ;}
    break;

  case 146:
#line 585 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_OR) ;}
    break;

  case 147:
#line 586 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_XOR) ;}
    break;

  case 148:
#line 587 "compile.y"
    { yyval=yyvsp[-2];  OA(*yyval, OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP_LOG_AND) ;}
    break;

  case 149:
#line 588 "compile.y"
    { yyval=yyvsp[-2];  OA(*yyval, OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP_LOG_OR) ;}
    break;

  case 150:
#line 589 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_LOG_XOR) ;}
    break;

  case 151:
#line 590 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_LT) ;}
    break;

  case 152:
#line 591 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_GT) ;}
    break;

  case 153:
#line 592 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_LE) ;}
    break;

  case 154:
#line 593 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_GE) ;}
    break;

  case 155:
#line 594 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_EQ) ;}
    break;

  case 156:
#line 595 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_NE) ;}
    break;

  case 157:
#line 596 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_LT) ;}
    break;

  case 158:
#line 597 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_GT) ;}
    break;

  case 159:
#line 598 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_LE) ;}
    break;

  case 160:
#line 599 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_GE) ;}
    break;

  case 161:
#line 600 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_EQ) ;}
    break;

  case 162:
#line 601 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_NE) ;}
    break;

  case 163:
#line 602 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_IS) ;}
    break;

  case 164:
#line 605 "compile.y"
    {
	// optimized from OP_STRING->OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*(yyval=yyvsp[0]));
;}
    break;

  case 165:
#line 610 "compile.y"
    {
	yyval=N();
	OA(*yyval, OP_STRING_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;}
    break;

  case 166:
#line 619 "compile.y"
    {
	// optimized from OP_STRING+OP_WRITE_VALUE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*(yyval=yyvsp[0]))
;}
    break;

  case 167:
#line 624 "compile.y"
    { yyval=VL(new VVoid(), 0, 0, 0) ;}
    break;

  case 168:
#line 625 "compile.y"
    { yyval=N() ;}
    break;


    }

/* Line 991 of yacc.c.  */
#line 2194 "compile.tab.C"

  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  char *yymsg;
	  int yyx, yycount;

	  yycount = 0;
	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  for (yyx = yyn < 0 ? -yyn : 0;
	       yyx < (int) (sizeof (yytname) / sizeof (char *)); yyx++)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      yysize += yystrlen (yytname[yyx]) + 15, yycount++;
	  yysize += yystrlen ("syntax error, unexpected ") + 1;
	  yysize += yystrlen (yytname[yytype]);
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yycount = 0;
		  for (yyx = yyn < 0 ? -yyn : 0;
		       yyx < (int) (sizeof (yytname) / sizeof (char *));
		       yyx++)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			const char *yyq = ! yycount ? ", expecting " : " or ";
			yyp = yystpcpy (yyp, yyq);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yycount++;
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      /* Return failure if at end of input.  */
      if (yychar == YYEOF)
        {
	  /* Pop the error token.  */
          YYPOPSTACK;
	  /* Pop the rest of the stack.  */
	  while (yyss < yyssp)
	    {
	      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
	      yydestruct (yystos[*yyssp], yyvsp);
	      YYPOPSTACK;
	    }
	  YYABORT;
        }

      YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
      yydestruct (yytoken, &yylval);
      yychar = YYEMPTY;

    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab2;


/*----------------------------------------------------.
| yyerrlab1 -- error raised explicitly by an action.  |
`----------------------------------------------------*/
yyerrlab1:

  /* Suppress GCC warning that yyerrlab1 is unused when no action
     invokes YYERROR.  */
#if defined (__GNUC_MINOR__) && 2093 <= (__GNUC__ * 1000 + __GNUC_MINOR__)
  __attribute__ ((__unused__))
#endif


  goto yyerrlab2;


/*---------------------------------------------------------------.
| yyerrlab2 -- pop states until the error token can be shifted.  |
`---------------------------------------------------------------*/
yyerrlab2:
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      yyvsp--;
      yystate = *--yyssp;

      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  return yyresult;
}


#line 627 "compile.y"

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
			case '^': case '$': case ';':
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
				if(pc.source[0]=='#' && pc.source[1] && pc.source[2]) {
					char c=
						hex_value[(unsigned char)pc.source[1]]*0x10+
						hex_value[(unsigned char)pc.source[2]];
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
				break;
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
						skip_analized=2;
						result=DEF;
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
			case '+': case '*': case '/': case '%': 
			case '&': case '|': 
			case '=': case '!':
			// common delimiters
			case ',': case '?': case '#':
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
		if((c=='@' || c==0) && end[-1]=='\n') { // we are before LS_DEF_NAME or EOF?
			// strip last \n
			end--;
			if(end!=begin && end[-1]=='\n') // allow one empty line before LS_DEF_NAME
				end--;
		}
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



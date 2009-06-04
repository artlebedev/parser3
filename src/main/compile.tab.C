/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
     LITERAL_TRUE = 287,
     LITERAL_FALSE = 288,
     NUNARY = 289
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
#define LITERAL_TRUE 287
#define LITERAL_FALSE 288
#define NUNARY 289




/* Copy the first part of user declarations.  */
#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001-2009 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.tab.C,v 1.138 2009/06/04 09:42:23 misha Exp $
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
static const VVoid vvoid;

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
#line 221 "compile.tab.C"

#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
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
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
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
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  51
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   470

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  89
/* YYNRULES -- Number of rules. */
#define YYNRULES  172
/* YYNRULES -- Number of states. */
#define YYNSTATES  263

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   289

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
static const unsigned short int yyprhs[] =
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

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const short int yyrhs[] =
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
static const unsigned short int yyrline[] =
{
       0,   132,   132,   139,   141,   141,   142,   144,   144,   146,
     247,   247,   248,   248,   249,   250,   250,   252,   252,   302,
     302,   303,   304,   304,   305,   305,   307,   307,   311,   311,
     313,   313,   314,   314,   315,   315,   315,   319,   353,   354,
     354,   355,   357,   358,   359,   417,   418,   418,   422,   435,
     436,   437,   438,   455,   460,   463,   464,   465,   467,   470,
     467,   478,   486,   493,   494,   495,   497,   503,   504,   504,
     508,   519,   522,   519,   550,   552,   552,   554,   555,   556,
     558,   561,   558,   564,   565,   567,   568,   571,   572,   575,
     576,   578,   581,   595,   600,   601,   602,   607,   607,   609,
     609,   610,   611,   619,   624,   627,   628,   629,   630,   632,
     636,   645,   648,   645,   656,   661,   661,   662,   668,   669,
     671,   688,   700,   702,   703,   704,   705,   706,   707,   708,
     709,   711,   712,   713,   714,   715,   716,   717,   718,   720,
     721,   722,   723,   724,   725,   726,   727,   728,   729,   730,
     731,   732,   733,   734,   735,   736,   737,   738,   739,   740,
     741,   742,   743,   744,   745,   746,   749,   754,   775,   780,
     781,   782,   784
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
  "\"ne\"", "\"def\"", "\"in\"", "\"-f\"", "\"-d\"", "\"is\"", "\"true\"",
  "\"false\"", "'<'", "'>'", "'|'", "'&'", "'+'", "'-'", "'*'", "'/'",
  "'\\\\'", "'%'", "NUNARY", "'~'", "'!'", "'@'", "'\\n'", "'['", "']'",
  "';'", "'$'", "'{'", "'}'", "'.'", "'('", "')'", "'^'", "':'", "'\"'",
  "'''", "$accept", "all", "methods", "one_big_piece", "method",
  "control_method", "maybe_control_strings", "control_strings",
  "control_string", "maybe_string", "code_method", "@1",
  "maybe_bracketed_strings", "bracketed_maybe_strings", "maybe_strings",
  "strings", "maybe_comment", "maybe_codes", "codes", "code", "action",
  "get", "get_value", "get_name_value", "name_in_curly_rdive",
  "name_without_curly_rdive", "name_without_curly_rdive_read",
  "name_without_curly_rdive_class", "name_without_curly_rdive_code", "put",
  "name_expr_wdive", "name_expr_wdive_root", "name_expr_wdive_write",
  "name_expr_wdive_class", "construct", "construct_square", "@2", "@3",
  "construct_round", "construct_curly", "any_constructor_code_value",
  "constructor_code_value", "constructor_code",
  "codes__excluding_sole_str_literal", "call", "call_value", "@4", "@5",
  "call_name", "store_params", "store_param", "store_square_param", "@6",
  "@7", "store_round_param", "store_curly_param", "store_code_param_parts",
  "store_expr_param_parts", "store_curly_param_parts",
  "store_code_param_part", "store_expr_param_part",
  "store_curly_param_part", "code_param_value", "name_expr_dive_code",
  "name_path", "name_step", "name_advance1", "name_advance2",
  "name_expr_value", "name_expr_subvar_value",
  "name_expr_with_subvar_value", "name_square_code_value", "@8", "@9",
  "subvar_ref_name_rdive", "subvar_get_writes", "subvar__get_write",
  "class_prefix", "class_static_prefix", "class_constructor_prefix",
  "expr_value", "expr", "double_or_STRING", "string_inside_quotes_value",
  "write_string", "void_value", "true_value", "false_value", "empty", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
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
static const unsigned char yyr1[] =
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
static const unsigned char yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
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
static const short int yypact[] =
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
static const short int yypgoto[] =
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
static const short int yytable[] =
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

static const short int yycheck[] =
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
static const unsigned char yystos[] =
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
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
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
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
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

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
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
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

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
	short int *yyss1 = yyss;


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
	short int *yyss1 = yyss;
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
#line 132 "compile.y"
    {
	Method& method=*new Method(Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		yyvsp[0]/*parser_code*/, 0/*native_code*/);
	PC.cclass->add_method(PC.alias_method(main_method_name), method);
;}
    break;

  case 9:
#line 147 "compile.y"
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
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			// new class' name
			const String& name=*LA2S(*strings_code);
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
			PC.request.use_file(PC.request.main_class, *LA2S(*strings_code, i));
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
	} else if(command==OPTIONS_CONTROL_METHOD_NAME) {
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE) {
			const String& option=*LA2S(*strings_code, i);
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
						// mark new class as partial. we can add methods to it later.
						PC.cclass_new->set_partial();
					}
				} else {
					strcpy(PC.error, "'"OPTION_PARTIAL_CLASS"' option should be used straight after @"CLASS_NAME);
					YYERROR;
				}
			} else {
				strcpy(PC.error, "'");
				strncat(PC.error, option.cstr(), MAX_STRING/2);
				strcat(PC.error, "' invalid option. valid options are "
					"'"OPTION_PARTIAL_CLASS"' and '"OPTION_ALL_VARS_LOCAL_NAME"'");
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
#line 248 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 17:
#line 252 "compile.y"
    { 
	PC.class_add();
	PC.explicit_result=false;

	YYSTYPE params_names_code=yyvsp[-3];
	ArrayString* params_names=0;
	if(int size=params_names_code->count()) {
		params_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE)
			*params_names+=LA2S(*params_names_code, i);
	}

	YYSTYPE locals_names_code=yyvsp[-2];
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
		Method::CT_ANY,
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		0/*to be filled later in next {} */, 0, all_vars_local);

	*reinterpret_cast<Method**>(&yyval)=method;

	// todo: check [][;result;]
;}
    break;

  case 18:
#line 292 "compile.y"
    {
		Method& method=*reinterpret_cast<Method*>(yyvsp[-1]);
		// fill in the code
		method.parser_code=yyvsp[0];

		// register in class
		const String& name=*LA2S(*yyvsp[-6]);
		PC.cclass->add_method(PC.alias_method(name), method);
;}
    break;

  case 21:
#line 303 "compile.y"
    {yyval=yyvsp[-1];}
    break;

  case 25:
#line 305 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 31:
#line 313 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 37:
#line 319 "compile.y"
    {
	yyval=N();
	YYSTYPE code=yyvsp[0];

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(!maybe_change_first_opcode(*code, OP::OP_VALUE__GET_ELEMENT, /*=>*/OP::OP_VALUE__GET_ELEMENT__WRITE))
#endif

#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
	if(!maybe_change_first_opcode(*code, OP::OP_WITH_SELF__VALUE__GET_ELEMENT, /*=>*/OP::OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE))
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
		if(
			code->count()!=5
			|| !maybe_change_first_opcode(*code, OP::OP_GET_OBJECT_ELEMENT, /*=>*/OP::OP_GET_OBJECT_ELEMENT__WRITE)
		)
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
		if(
			code->count()!=5
			|| !maybe_change_first_opcode(*code, OP::OP_GET_OBJECT_VAR_ELEMENT, /*=>*/OP::OP_GET_OBJECT_VAR_ELEMENT__WRITE)
		)
#endif
		{
			changetail_or_append(*code, 
				OP::OP_GET_ELEMENT, false,  /*=>*/OP::OP_GET_ELEMENT__WRITE,
				/*or */OP::OP_WRITE_VALUE
				); /* value=pop; wcontext.write(value) */
		}

	P(*yyval, *code);
;}
    break;

  case 38:
#line 353 "compile.y"
    { yyval=yyvsp[0] ;}
    break;

  case 41:
#line 355 "compile.y"
    { yyval=yyvsp[-1] ;}
    break;

  case 44:
#line 359 "compile.y"
    {
	yyval=N(); 
	YYSTYPE diving_code=yyvsp[0];
	const String* first_name=LA2S(*diving_code);
	size_t count=diving_code->count();
	// self.xxx... => xxx...
	// OP_VALUE+origin+string+OP_GET_ELEMENT+... -> OP_WITH_SELF+...
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
		if(maybe_make_with_self_get_element(*yyval, *diving_code, count)){
			// optimization for $self.field and ^self.method
		} else
#endif
			{
				O(*yyval, OP::OP_WITH_SELF); /* stack: starting context */
				P(*yyval, *diving_code, 
					/* skip over... */
					count>=4?4/*OP_VALUE+origin+string+OP_GET_ELEMENTx*/:3/*OP::OP_+origin+string*/);
			}
	}

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	else if(maybe_make_get_object_element(*yyval, *diving_code, count)){
		// optimization for $object.field + ^object.method[
	}
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	else if(maybe_make_get_object_var_element(*yyval, *diving_code, count)){
		// optimization for $object.$var
	}
#endif

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	else if(count==4){ // optimization
		O(*yyval,
			(PC.in_call_value)
			? OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR // ^object[ : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT_OR_OPERATOR+origin+string
			: OP::OP_VALUE__GET_ELEMENT             // $object  : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT+origin+string
		);
		P(*yyval, *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
	} else {
		O(*yyval, OP::OP_WITH_READ); /* stack: starting context */
		P(*yyval, *diving_code);
	}
#else
	else {
		O(*yyval, OP::OP_WITH_READ); /* stack: starting context */

		// ^if OP_ELEMENT => ^if OP_ELEMENT_OR_OPERATOR
		// optimized OP_VALUE+origin+string+OP_GET_ELEMENT. => OP_VALUE+origin+string+OP_GET_ELEMENT_OR_OPERATOR.
		if(PC.in_call_value && count==4)
			diving_code->put(count-1, OP::OP_GET_ELEMENT_OR_OPERATOR);
		P(*yyval, *diving_code);
	}
#endif
	/* diving code; stack: current context */
;}
    break;

  case 45:
#line 417 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 47:
#line 418 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 48:
#line 422 "compile.y"
    {
	yyval=N();
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	if(maybe_optimize_construct(*yyval, *yyvsp[-1], *yyvsp[0])){
		// $a(expr), $.a(expr), $a[value], $.a[value]
	} else 
#endif
	{
		P(*yyval, *yyvsp[-1]); /* stack: context,name */
		P(*yyval, *yyvsp[0]); /* stack: context,name,constructor_value */
	}
;}
    break;

  case 52:
#line 438 "compile.y"
    {
	yyval=N();
	YYSTYPE diving_code=yyvsp[0];
	const String* first_name=LA2S(*diving_code);
	// $self.xxx... => $xxx...
	// OP_VALUE+origin+string+OP_GET_ELEMENT+... => OP_WITH_SELF+...
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(*yyval, OP::OP_WITH_SELF); /* stack: starting context */
		P(*yyval, *diving_code, 
			/* skip over... */
			diving_code->count()>=4?4/*OP::OP_VALUE+origin+string+OP::OP_GET_ELEMENTx*/:3/*OP::OP_+origin+string*/);
	} else {
		O(*yyval, OP::OP_WITH_ROOT); /* stack: starting context */
		P(*yyval, *diving_code);
	}
	/* diving code; stack: current context */
;}
    break;

  case 53:
#line 455 "compile.y"
    {
	yyval=N(); 
	O(*yyval, OP::OP_WITH_WRITE); /* stack: starting context */
	P(*yyval, *yyvsp[0]); /* diving code; stack: context,name */
;}
    break;

  case 54:
#line 460 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 58:
#line 467 "compile.y"
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 59:
#line 470 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;}
    break;

  case 60:
#line 472 "compile.y"
    {
	// stack: context, name
	yyval=yyvsp[-2]; // stack: context, name, value
	O(*yyval, OP::OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;}
    break;

  case 61:
#line 478 "compile.y"
    { 
	yyval=N(); 
	O(*yyval, OP::OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(*yyval, *yyvsp[-1]); // stack: context, name, value
	O(*yyval, OP::OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;}
    break;

  case 62:
#line 486 "compile.y"
    {
	// stack: context, name
	yyval=N(); 
	OA(*yyval, OP::OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;}
    break;

  case 66:
#line 497 "compile.y"
    {
	yyval=N(); 
	OA(*yyval, OP::OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;}
    break;

  case 69:
#line 504 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 70:
#line 508 "compile.y"
    {
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
	if((*yyvsp[0]).count())
#endif
	{
		yyval=yyvsp[0]; /* stack: value */
		changetail_or_append(*yyval, 
			OP::OP_CALL, true,  /*=>*/ OP::OP_CALL__WRITE,
			/*or */OP::OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
	}
;}
    break;

  case 71:
#line 519 "compile.y"
    { 
					PC.in_call_value=true; 
			;}
    break;

  case 72:
#line 522 "compile.y"
    {
				PC.in_call_value=false;
			;}
    break;

  case 73:
#line 525 "compile.y"
    { /* ^field.$method{vasya} */
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	const String* operator_name=LA2S(*yyvsp[-3], 0, OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR);
#else
	const String* operator_name=LA2S(*yyvsp[-3], 1);
#endif
	if(operator_name && *operator_name == REM_OPERATOR_NAME){
		yyval=N();
	} else 
#endif
		{
			yyval=yyvsp[-3]; /* with_xxx,diving code; stack: context,method_junction */

			YYSTYPE params_code=yyvsp[-1];
			if(params_code->count()==3) { // probably [] case. [OP::OP_VALUE+origin+Void]
				if(Value* value=LA2V(*params_code)) // it is OP_VALUE+origin+value?
					if(value->is_void()) // value is VVoid?
						params_code=0; // ^zzz[] case. don't append lone empty param.
			}
			/* stack: context, method_junction */
			OA(*yyval, OP::OP_CALL, params_code); // method_frame=make frame(pop junction); ncontext=pop; call(ncontext,method_frame) stack: value
		}
;}
    break;

  case 76:
#line 552 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 80:
#line 558 "compile.y"
    {
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 81:
#line 561 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;}
    break;

  case 82:
#line 563 "compile.y"
    {yyval=yyvsp[-2];}
    break;

  case 83:
#line 564 "compile.y"
    {yyval=yyvsp[-1];}
    break;

  case 84:
#line 565 "compile.y"
    {yyval=yyvsp[-1];}
    break;

  case 86:
#line 568 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 88:
#line 572 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 90:
#line 576 "compile.y"
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 91:
#line 578 "compile.y"
    {
	yyval=yyvsp[0];
;}
    break;

  case 92:
#line 581 "compile.y"
    {
	YYSTYPE expr_code=yyvsp[0];
	if(expr_code->count()==3
		&& (*expr_code)[0].code==OP::OP_VALUE) { // optimizing (double/bool/incidently 'string' too) case. [OP::OP_VALUE+origin+Double]. no evaluating
		yyval=expr_code; 
	} else {
		YYSTYPE code=N();
		O(*code, OP::OP_PREPARE_TO_EXPRESSION);
		P(*code, *expr_code);
		O(*code, OP::OP_WRITE_EXPR_RESULT);
		yyval=N(); 
		OA(*yyval, OP::OP_EXPR_CODE__STORE_PARAM, code);
	}
;}
    break;

  case 93:
#line 595 "compile.y"
    {
	yyval=N(); 
	OA(*yyval, OP::OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
;}
    break;

  case 98:
#line 607 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 100:
#line 609 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 102:
#line 611 "compile.y"
    {
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;}
    break;

  case 103:
#line 619 "compile.y"
    {
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;}
    break;

  case 109:
#line 632 "compile.y"
    {
	yyval=yyvsp[0];
	O(*yyval, OP::OP_GET_ELEMENT);
;}
    break;

  case 110:
#line 636 "compile.y"
    {
	YYSTYPE code;
	{
		change_string_literal_to_write_string_literal(*(code=yyvsp[-1]));
		P(*code, *yyvsp[0]);
	}
	yyval=N(); 
	OA(*yyval, OP::OP_STRING_POOL, code);
;}
    break;

  case 111:
#line 645 "compile.y"
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;}
    break;

  case 112:
#line 648 "compile.y"
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;}
    break;

  case 113:
#line 650 "compile.y"
    {
	yyval=N(); 
	OA(*yyval, OP::OP_OBJECT_POOL, yyvsp[-2]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;}
    break;

  case 114:
#line 656 "compile.y"
    {
	yyval=N(); 
	O(*yyval, OP::OP_WITH_READ);
	P(*yyval, *yyvsp[0]);
;}
    break;

  case 116:
#line 661 "compile.y"
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;}
    break;

  case 117:
#line 662 "compile.y"
    {
	yyval=yyvsp[0];
	O(*yyval, OP::OP_GET_ELEMENT__WRITE);
;}
    break;

  case 120:
#line 671 "compile.y"
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
#ifdef OPTIMIZE_BYTECODE_GET_CLASS
	// optimized OP_VALUE+origin+string+OP_GET_CLASS => OP_VALUE__GET_CLASS+origin+string
	maybe_change_first_opcode(*yyval, OP::OP_VALUE, OP::OP_VALUE__GET_CLASS)
#else
	O(*yyval, OP::OP_GET_CLASS);
#endif
;}
    break;

  case 121:
#line 688 "compile.y"
    {
	yyval=yyvsp[-1];
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(*yyval, OP::OP_PREPARE_TO_CONSTRUCT_OBJECT);
;}
    break;

  case 128:
#line 707 "compile.y"
    { yyval = yyvsp[-1] ;}
    break;

  case 129:
#line 708 "compile.y"
    { yyval = yyvsp[-1] ;}
    break;

  case 130:
#line 709 "compile.y"
    { yyval = yyvsp[-1]; ;}
    break;

  case 131:
#line 711 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP::OP_NEG) ;}
    break;

  case 132:
#line 712 "compile.y"
    { yyval=yyvsp[0] ;}
    break;

  case 133:
#line 713 "compile.y"
    { yyval=yyvsp[0];	 O(*yyval, OP::OP_INV) ;}
    break;

  case 134:
#line 714 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP::OP_NOT) ;}
    break;

  case 135:
#line 715 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP::OP_DEF) ;}
    break;

  case 136:
#line 716 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP::OP_IN) ;}
    break;

  case 137:
#line 717 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP::OP_FEXISTS) ;}
    break;

  case 138:
#line 718 "compile.y"
    { yyval=yyvsp[0];  O(*yyval, OP::OP_DEXISTS) ;}
    break;

  case 139:
#line 720 "compile.y"
    {	yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_SUB) ;}
    break;

  case 140:
#line 721 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_ADD) ;}
    break;

  case 141:
#line 722 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_MUL) ;}
    break;

  case 142:
#line 723 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_DIV) ;}
    break;

  case 143:
#line 724 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_MOD) ;}
    break;

  case 144:
#line 725 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_INTDIV) ;}
    break;

  case 145:
#line 726 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_SL) ;}
    break;

  case 146:
#line 727 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_SR) ;}
    break;

  case 147:
#line 728 "compile.y"
    { yyval=yyvsp[-2]; 	P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_AND) ;}
    break;

  case 148:
#line 729 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_OR) ;}
    break;

  case 149:
#line 730 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_XOR) ;}
    break;

  case 150:
#line 731 "compile.y"
    { yyval=yyvsp[-2];  OA(*yyval, OP::OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP::OP_LOG_AND) ;}
    break;

  case 151:
#line 732 "compile.y"
    { yyval=yyvsp[-2];  OA(*yyval, OP::OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP::OP_LOG_OR) ;}
    break;

  case 152:
#line 733 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_LOG_XOR) ;}
    break;

  case 153:
#line 734 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_LT) ;}
    break;

  case 154:
#line 735 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_GT) ;}
    break;

  case 155:
#line 736 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_LE) ;}
    break;

  case 156:
#line 737 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_GE) ;}
    break;

  case 157:
#line 738 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_EQ) ;}
    break;

  case 158:
#line 739 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_NE) ;}
    break;

  case 159:
#line 740 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_LT) ;}
    break;

  case 160:
#line 741 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_GT) ;}
    break;

  case 161:
#line 742 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_LE) ;}
    break;

  case 162:
#line 743 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_GE) ;}
    break;

  case 163:
#line 744 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_EQ) ;}
    break;

  case 164:
#line 745 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_NE) ;}
    break;

  case 165:
#line 746 "compile.y"
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_IS) ;}
    break;

  case 166:
#line 749 "compile.y"
    {
	// optimized OP_STRING => OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*(yyval=yyvsp[0]));
;}
    break;

  case 167:
#line 754 "compile.y"
    {
#ifdef OPTIMIZE_BYTECODE_STRING_POOL
	// it brakes ^if(" 09 "){...}
	YYSTYPE code=yyvsp[0];
	yyval=N();
	if(code->count()==3 && maybe_change_first_opcode(*code, OP::OP_STRING__WRITE, OP::OP_VALUE)){
		// optimized OP_STRING__WRITE+origin+value => OP_VALUE+origin+value without starting OP_STRING_POOL
		P(*yyval, *code);
	} else {
		OA(*yyval, OP::OP_STRING_POOL, code); /* stack: empty write context */
	}
#else
	yyval=N();
	OA(*yyval, OP::OP_STRING_POOL, yyvsp[0]); /* stack: empty write context */
#endif
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;}
    break;

  case 168:
#line 775 "compile.y"
    {
	// optimized OP_STRING+OP_WRITE_VALUE => OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*(yyval=yyvsp[0]))
;}
    break;

  case 169:
#line 780 "compile.y"
    { yyval=VL(/*we know that we will not change it*/const_cast<VVoid*>(&vvoid), 0, 0, 0) ;}
    break;

  case 170:
#line 781 "compile.y"
    { yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vtrue), 0, 0, 0) ;}
    break;

  case 171:
#line 782 "compile.y"
    { yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vfalse), 0, 0, 0) ;}
    break;

  case 172:
#line 784 "compile.y"
    { yyval=N() ;}
    break;


    }

/* Line 1010 of yacc.c.  */
#line 2378 "compile.tab.C"

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
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
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

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
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

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
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


#line 786 "compile.y"

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
			case '+': case '*': case '/': case '%': 
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



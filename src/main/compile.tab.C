
/*  A Bison parser, made from compile.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	EON	257
#define	STRING	258
#define	BOGUS	259
#define	BAD_STRING_COMPARISON_OPERATOR	260
#define	BAD_HEX_LITERAL	261
#define	BAD_METHOD_DECL_START	262
#define	BAD_METHOD_PARAMETER_NAME_CHARACTER	263
#define	BAD_MATH_OPERATOR_CHARACTER	264
#define	LAND	265
#define	LOR	266
#define	LXOR	267
#define	NXOR	268
#define	NLE	269
#define	NGE	270
#define	NEQ	271
#define	NNE	272
#define	SLT	273
#define	SGT	274
#define	SLE	275
#define	SGE	276
#define	SEQ	277
#define	SNE	278
#define	DEF	279
#define	IN	280
#define	FEXISTS	281
#define	DEXISTS	282
#define	IS	283
#define	NEG	284

#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.tab.C,v 1.65 2002/09/13 13:41:48 paf Exp $
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

#define YYSTYPE  Array/*<Operation>*/ *
#define YYPARSE_PARAM  pc
#define YYLEX_PARAM  pc
#define YYDEBUG  1
#define YYERROR_VERBOSE	1
#define yyerror(msg)  real_yyerror((parse_control *)pc, msg)
#define YYPRINT(file, type, value)  yyprint(file, type, value)

#include "compile_tools.h"
#include "pa_value.h"
#include "pa_request.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "pa_globals.h"
#include "pa_vvoid.h"

#define SELF_ELEMENT_NAME "self"
#define USE_CONTROL_METHOD_NAME "USE"

static int real_yyerror(parse_control *pc, char *s);
static void yyprint(FILE *file, int type, YYSTYPE value);
static int yylex(YYSTYPE *lvalp, void *pc);


// local convinient inplace typecast & var
#define PC  (*(parse_control *)pc)
#define POOL  (*PC.pool)
#undef NEW
#define NEW new(POOL)
#ifndef DOXYGEN
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		248
#define	YYFLAG		-32768
#define	YYNTBASE	58

#define YYTRANSLATE(x) ((unsigned)(x) <= 284 ? yytranslate[x] : 137)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    44,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    32,    56,     2,    48,    40,    34,    57,    52,
    53,    38,    37,     2,    36,    51,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    55,    47,    30,
     2,    31,     2,    43,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    45,    41,    46,    54,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    49,    33,    50,    35,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
    27,    28,    29,    42
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     9,    11,    13,    15,    20,    22,
    24,    26,    29,    32,    34,    36,    44,    46,    48,    52,
    54,    56,    58,    62,    64,    66,    68,    70,    72,    75,
    77,    79,    81,    83,    85,    87,    90,    93,    95,    99,
   101,   103,   105,   108,   110,   113,   117,   119,   121,   123,
   125,   128,   131,   133,   135,   137,   141,   145,   149,   151,
   153,   155,   157,   159,   161,   164,   166,   167,   168,   175,
   177,   179,   182,   184,   186,   188,   192,   196,   200,   202,
   206,   208,   212,   214,   218,   220,   222,   224,   226,   228,
   230,   232,   234,   237,   239,   242,   245,   247,   249,   252,
   254,   256,   258,   260,   263,   266,   270,   272,   274,   277,
   280,   282,   284,   287,   290,   292,   294,   296,   298,   302,
   306,   310,   313,   316,   319,   322,   325,   328,   331,   334,
   338,   342,   346,   350,   354,   358,   362,   366,   370,   374,
   378,   382,   386,   390,   394,   398,   402,   406,   410,   414,
   418,   422,   426,   430,   434,   436,   438,   439
};

static const short yyrhs[] = {    60,
     0,    59,     0,    61,     0,    59,    61,     0,    73,     0,
    62,     0,    67,     0,    43,     4,    44,    63,     0,   136,
     0,    64,     0,    65,     0,    64,    65,     0,    66,    44,
     0,   136,     0,     4,     0,    43,     4,    69,    68,    72,
    44,    73,     0,   136,     0,    69,     0,    45,    70,    46,
     0,   136,     0,    71,     0,     4,     0,    71,    47,     4,
     0,   136,     0,     4,     0,   136,     0,    74,     0,    75,
     0,    74,    75,     0,   134,     0,    76,     0,    77,     0,
    85,     0,    98,     0,    78,     0,    48,    79,     0,    81,
     3,     0,    80,     0,    49,    81,    50,     0,    82,     0,
    83,     0,    84,     0,   128,    84,     0,   120,     0,   117,
   120,     0,    48,    86,    90,     0,    87,     0,    88,     0,
    89,     0,   116,     0,    51,   116,     0,   128,   116,     0,
    91,     0,    92,     0,    93,     0,    45,    94,    46,     0,
    52,   131,    53,     0,    49,    73,    50,     0,   135,     0,
     4,     0,    95,     0,    96,     0,    97,     0,    76,     0,
    75,    74,     0,    99,     0,     0,     0,    54,   100,   102,
   101,   103,     3,     0,    81,     0,   104,     0,   103,   104,
     0,   105,     0,   106,     0,   107,     0,    45,   108,    46,
     0,    52,   109,    53,     0,    49,   110,    50,     0,   111,
     0,   108,    47,   111,     0,   112,     0,   109,    47,   112,
     0,   113,     0,   110,    47,   113,     0,   114,     0,   115,
     0,    73,     0,   135,     0,     4,     0,    95,     0,   131,
     0,   121,     0,   117,   121,     0,   118,     0,   117,   118,
     0,   119,    51,     0,   121,     0,   121,     0,     4,     5,
     0,     4,     0,   122,     0,   123,     0,   124,     0,    48,
   125,     0,     4,   126,     0,    45,    74,    46,     0,     4,
     0,   127,     0,   126,   127,     0,    48,   125,     0,   129,
     0,   130,     0,     4,    55,     0,   129,    55,     0,   132,
     0,     4,     0,    78,     0,    99,     0,    56,   133,    56,
     0,    57,   133,    57,     0,    52,   132,    53,     0,    36,
   132,     0,    37,   132,     0,    35,   132,     0,    32,   132,
     0,    25,   132,     0,    26,   132,     0,    27,   132,     0,
    28,   132,     0,   132,    36,   132,     0,   132,    37,   132,
     0,   132,    38,   132,     0,   132,    39,   132,     0,   132,
    40,   132,     0,   132,    41,   132,     0,   132,    34,   132,
     0,   132,    33,   132,     0,   132,    14,   132,     0,   132,
    11,   132,     0,   132,    12,   132,     0,   132,    13,   132,
     0,   132,    30,   132,     0,   132,    31,   132,     0,   132,
    15,   132,     0,   132,    16,   132,     0,   132,    17,   132,
     0,   132,    18,   132,     0,   132,    19,   132,     0,   132,
    20,   132,     0,   132,    21,   132,     0,   132,    22,   132,
     0,   132,    23,   132,     0,   132,    24,   132,     0,   132,
    29,   132,     0,    73,     0,     4,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   110,   120,   122,   122,   123,   125,   125,   127,   194,   194,
   195,   195,   196,   197,   197,   199,   228,   228,   229,   230,
   230,   231,   231,   233,   233,   237,   237,   239,   239,   240,
   240,   241,   241,   241,   245,   252,   253,   253,   254,   255,
   257,   258,   280,   281,   281,   285,   289,   291,   292,   293,
   310,   315,   317,   319,   320,   322,   328,   336,   342,   344,
   345,   347,   353,   354,   354,   358,   364,   367,   370,   383,
   385,   385,   386,   388,   389,   391,   392,   393,   394,   396,
   398,   400,   402,   404,   406,   410,   414,   418,   420,   421,
   423,   432,   432,   434,   434,   435,   436,   444,   449,   451,
   453,   454,   455,   457,   461,   470,   476,   481,   481,   482,
   487,   489,   491,   503,   515,   520,   522,   523,   524,   525,
   526,   528,   529,   530,   531,   532,   533,   534,   535,   537,
   538,   539,   540,   541,   542,   543,   544,   545,   546,   547,
   548,   549,   550,   551,   552,   553,   554,   555,   556,   557,
   558,   559,   560,   561,   564,   573,   578,   579
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EON","STRING",
"BOGUS","BAD_STRING_COMPARISON_OPERATOR","BAD_HEX_LITERAL","BAD_METHOD_DECL_START",
"BAD_METHOD_PARAMETER_NAME_CHARACTER","BAD_MATH_OPERATOR_CHARACTER","\"&&\"",
"\"||\"","\"!||\"","\"!|\"","\"<=\"","\">=\"","\"==\"","\"!=\"","\"lt\"","\"gt\"",
"\"le\"","\"ge\"","\"eq\"","\"ne\"","\"def\"","\"in\"","\"-f\"","\"-d\"","\"is\"",
"'<'","'>'","'!'","'|'","'&'","'~'","'-'","'+'","'*'","'/'","'%'","'\\\\'","NEG",
"'@'","'\\n'","'['","']'","';'","'$'","'{'","'}'","'.'","'('","')'","'^'","':'",
"'\\\"'","'\\''","all","methods","one_big_piece","method","control_method","maybe_control_strings",
"control_strings","control_string","maybe_string","code_method","maybe_bracketed_strings",
"bracketed_maybe_strings","maybe_strings","strings","maybe_comment","maybe_codes",
"codes","code","action","get","get_value","get_name_value","name_in_curly_rdive",
"name_without_curly_rdive","name_without_curly_rdive_read","name_without_curly_rdive_class",
"name_without_curly_rdive_code","put","name_expr_wdive","name_expr_wdive_root",
"name_expr_wdive_write","name_expr_wdive_class","construct","construct_square",
"construct_round","construct_curly","any_constructor_code_value","constructor_code_value",
"constructor_code","codes__excluding_sole_str_literal","call","call_value","@1",
"@2","call_name","store_params","store_param","store_square_param","store_round_param",
"store_curly_param","store_code_param_parts","store_expr_param_parts","store_curly_param_parts",
"store_code_param_part","store_expr_param_part","store_curly_param_part","code_param_value",
"write_expr_value","name_expr_dive_code","name_path","name_step","name_advance1",
"name_advance2","name_expr_value","name_expr_subvar_value","name_expr_with_subvar_value",
"name_square_code_value","subvar_ref_name_rdive","subvar_get_writes","subvar__get_write",
"class_prefix","class_static_prefix","class_constructor_prefix","expr_value",
"expr","string_inside_quotes_value","write_string","void_value","empty", NULL
};
#endif

static const short yyr1[] = {     0,
    58,    58,    59,    59,    60,    61,    61,    62,    63,    63,
    64,    64,    65,    66,    66,    67,    68,    68,    69,    70,
    70,    71,    71,    72,    72,    73,    73,    74,    74,    75,
    75,    76,    76,    76,    77,    78,    79,    79,    80,    81,
    81,    82,    83,    84,    84,    85,    86,    86,    86,    87,
    88,    89,    90,    90,    90,    91,    92,    93,    94,    94,
    94,    95,    96,    97,    97,    98,   100,   101,    99,   102,
   103,   103,   104,   104,   104,   105,   106,   107,   108,   108,
   109,   109,   110,   110,   111,   112,   113,   114,   114,   114,
   115,   116,   116,   117,   117,   118,   119,   120,   120,   121,
   121,   121,   121,   122,   123,   124,   125,   126,   126,   127,
   128,   128,   129,   130,   131,   132,   132,   132,   132,   132,
   132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
   132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
   132,   132,   132,   132,   132,   132,   132,   132,   132,   132,
   132,   132,   132,   132,   133,   134,   135,   136
};

static const short yyr2[] = {     0,
     1,     1,     1,     2,     1,     1,     1,     4,     1,     1,
     1,     2,     2,     1,     1,     7,     1,     1,     3,     1,
     1,     1,     3,     1,     1,     1,     1,     1,     2,     1,
     1,     1,     1,     1,     1,     2,     2,     1,     3,     1,
     1,     1,     2,     1,     2,     3,     1,     1,     1,     1,
     2,     2,     1,     1,     1,     3,     3,     3,     1,     1,
     1,     1,     1,     1,     2,     1,     0,     0,     6,     1,
     1,     2,     1,     1,     1,     3,     3,     3,     1,     3,
     1,     3,     1,     3,     1,     1,     1,     1,     1,     1,
     1,     1,     2,     1,     2,     2,     1,     1,     2,     1,
     1,     1,     1,     2,     2,     3,     1,     1,     2,     2,
     1,     1,     2,     2,     1,     1,     1,     1,     3,     3,
     3,     2,     2,     2,     2,     2,     2,     2,     2,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     1,     1,     0,     0
};

static const short yydefact[] = {   158,
   156,     0,     0,    67,     2,     1,     3,     6,     7,     5,
    27,    28,    31,    32,    35,    33,    34,    66,    30,    26,
     0,   100,     0,     0,     0,     0,    36,    38,     0,    40,
    41,    42,     0,    47,    48,    49,    50,     0,    94,     0,
    44,    92,   101,   102,   103,     0,   111,   112,     0,     4,
    29,   158,   158,   158,    99,     0,   113,   105,   108,     0,
   107,   104,     0,     0,    98,     0,   100,    51,     0,    92,
    37,   157,   158,     0,    46,    53,    54,    55,   100,    95,
    45,    93,    96,    43,    52,   114,    70,    68,    15,     8,
    10,    11,     0,     9,    22,     0,    21,    20,   158,    18,
    17,   110,   109,   106,    39,    93,   156,     0,    31,     0,
    61,    62,    63,    59,     0,   116,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   158,   158,   117,   118,
     0,   115,     0,    12,    14,    13,    19,     0,    25,     0,
    24,    65,    56,    58,   126,   127,   128,   129,   125,   124,
   122,   123,     0,   155,     0,     0,    57,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,   157,   158,     0,     0,    71,    73,    74,    75,
    23,   158,   121,   119,   120,   139,   140,   141,   138,   144,
   145,   146,   147,   148,   149,   150,   151,   152,   153,   154,
   142,   143,   137,   136,   130,   131,   132,   133,   134,   135,
   156,    90,     0,    79,    85,    88,    87,     0,    83,     0,
    81,    86,    91,    69,    72,    16,    76,   157,   158,    78,
     0,    77,    80,    84,    82,     0,     0,     0
};

static const short yydefgoto[] = {   246,
     5,     6,     7,     8,    90,    91,    92,    93,     9,    99,
    54,    96,    97,   140,   154,    11,    12,    13,    14,   129,
    27,    28,    29,    30,    31,    32,    16,    33,    34,    35,
    36,    75,    76,    77,    78,   110,   222,   112,   113,    17,
   130,    49,   133,    88,   186,   187,   188,   189,   190,   223,
   230,   228,   224,   231,   229,   225,   232,    37,    64,    39,
    40,    41,    65,    43,    44,    45,    62,    58,    59,    66,
    47,    48,   233,   132,   155,    19,   226,    20
};

static const short yypact[] = {    15,
-32768,    12,    55,-32768,   -25,-32768,-32768,-32768,-32768,-32768,
    16,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    54,     1,    16,    17,    93,    98,-32768,-32768,    42,-32768,
-32768,-32768,    -6,-32768,-32768,-32768,-32768,   103,-32768,    40,
-32768,    37,-32768,-32768,-32768,   103,    13,-32768,    93,-32768,
-32768,    97,   113,   100,-32768,    17,-32768,    99,-32768,    33,
-32768,-32768,   118,   103,   121,   103,    99,-32768,    98,   121,
-32768,    23,    16,    57,-32768,-32768,-32768,-32768,    19,-32768,
-32768,    37,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    22,-32768,   105,   125,-32768,   104,   123,-32768,   169,-32768,
-32768,-32768,-32768,-32768,-32768,   121,   142,    16,    87,   143,
-32768,-32768,-32768,-32768,   140,-32768,    57,    57,    57,    57,
    57,    57,    57,    57,    91,    57,    16,    16,-32768,-32768,
   138,   275,    -1,-32768,-32768,-32768,-32768,   189,-32768,   178,
-32768,    16,-32768,-32768,    82,    82,    82,    82,    82,   196,
-32768,-32768,   232,-32768,   173,   200,-32768,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    57,    57,    57,    57,    57,    57,    57,    57,
    57,    57,    32,    16,    57,     5,-32768,-32768,-32768,-32768,
-32768,    16,-32768,-32768,-32768,   365,   337,   306,   141,   393,
   393,   406,   406,   393,   393,   393,   393,   406,   406,    82,
   393,   393,   187,   196,   -10,   -10,-32768,-32768,-32768,-32768,
   107,-32768,   119,-32768,-32768,-32768,-32768,   -33,-32768,    84,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    32,    16,-32768,
    57,-32768,-32768,-32768,-32768,   230,   258,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   254,-32768,-32768,-32768,   176,-32768,-32768,-32768,
   206,-32768,-32768,-32768,     3,   -18,    -7,   -71,-32768,     0,
-32768,-32768,   -15,-32768,-32768,   -24,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   192,-32768,-32768,-32768,
     2,-32768,-32768,-32768,-32768,    88,-32768,-32768,-32768,-32768,
-32768,-32768,    39,    34,    41,-32768,-32768,   106,     6,   -31,
-32768,   -23,     9,-32768,-32768,-32768,   220,-32768,   221,   278,
-32768,-32768,   204,    38,   154,-32768,   211,    72
};


#define	YYLAST		447


static const short yytable[] = {    15,
   109,    18,    10,    51,    60,    55,    80,   234,    38,    63,
    15,    42,    18,   239,    81,    21,   240,     2,     1,     1,
    61,    84,    15,    55,    18,    89,   107,   179,   180,   181,
   182,    69,    80,    87,    70,   221,     1,    80,    72,   -98,
    81,    84,    73,   183,    71,    74,    82,   184,    56,   183,
   185,    38,    51,   184,    42,    57,   185,     2,    22,    15,
   116,    18,     3,     3,   108,  -158,    56,    86,     4,     4,
     3,    15,    15,    18,    18,   115,     4,   106,   104,     3,
     3,   117,   118,   119,   120,     4,     4,   -97,   121,   142,
    83,   122,   123,   124,    22,   161,    22,    52,    53,    23,
    89,    67,    24,    25,   125,    26,    79,    15,   126,    18,
     4,   109,   127,   128,   175,   176,    95,   177,   178,   179,
   180,   181,   182,    94,    98,   101,    15,    15,    18,    18,
   241,    68,   -64,   -64,    51,    23,   242,    23,    24,    25,
    24,    15,    23,    18,    53,    24,    56,    23,   136,   137,
    24,    85,   -89,   -89,   145,   146,   147,   148,   149,   150,
   151,   152,   135,   153,   237,   238,   109,   105,   -14,   138,
   141,   -97,   139,   175,   176,   108,   177,   178,   179,   180,
   181,   182,    15,    15,    18,    18,   227,   -60,   143,   144,
   157,    15,   191,    18,   236,   196,   197,   198,   199,   200,
   201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
   211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
   176,   192,   177,   178,   179,   180,   181,   182,   194,   247,
   108,   177,   178,   179,   180,   181,   182,    15,    15,    18,
    18,   227,   158,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   168,   169,   170,   171,   195,   248,    50,   100,
   172,   173,   174,   111,   175,   176,   134,   177,   178,   179,
   180,   181,   182,   235,   245,   102,   243,   131,   103,   244,
    46,   156,   114,     0,   193,   158,   159,   160,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,   171,     0,
     0,     0,     0,   172,   173,   174,     0,   175,   176,     0,
   177,   178,   179,   180,   181,   182,   158,   159,     0,   161,
   162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
     0,     0,     0,     0,   172,   173,   174,     0,   175,   176,
     0,   177,   178,   179,   180,   181,   182,   158,     0,     0,
   161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
   171,     0,     0,     0,     0,   172,   173,   174,     0,   175,
   176,     0,   177,   178,   179,   180,   181,   182,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,   171,     0,
     0,     0,     0,   172,   173,   174,     0,   175,   176,     0,
   177,   178,   179,   180,   181,   182,   161,     0,     0,   164,
   165,     0,     0,     0,     0,   170,   171,     0,     0,   161,
     0,   172,     0,     0,     0,   175,   176,     0,   177,   178,
   179,   180,   181,   182,   172,     0,     0,     0,   175,   176,
     0,   177,   178,   179,   180,   181,   182
};

static const short yycheck[] = {     0,
    72,     0,     0,    11,    23,     5,    38,     3,     3,    25,
    11,     3,    11,    47,    38,     4,    50,    43,     4,     4,
     4,    46,    23,     5,    23,     4,     4,    38,    39,    40,
    41,    26,    64,    49,    26,     4,     4,    69,    45,     3,
    64,    66,    49,    45,     3,    52,    38,    49,    48,    45,
    52,    46,    60,    49,    46,    55,    52,    43,     4,    60,
     4,    60,    48,    48,    72,    44,    48,    55,    54,    54,
    48,    72,    73,    72,    73,    73,    54,    69,    46,    48,
    48,    25,    26,    27,    28,    54,    54,    51,    32,   108,
    51,    35,    36,    37,     4,    14,     4,    44,    45,    45,
     4,     4,    48,    49,    48,    51,     4,   108,    52,   108,
    54,   183,    56,    57,    33,    34,     4,    36,    37,    38,
    39,    40,    41,    52,    53,    54,   127,   128,   127,   128,
    47,    26,    46,    47,   142,    45,    53,    45,    48,    49,
    48,   142,    45,   142,    45,    48,    48,    45,    44,    46,
    48,    46,    46,    47,   117,   118,   119,   120,   121,   122,
   123,   124,    91,   126,    46,    47,   238,    50,    44,    47,
    99,    51,     4,    33,    34,   183,    36,    37,    38,    39,
    40,    41,   183,   184,   183,   184,   184,    46,    46,    50,
    53,   192,     4,   192,   192,   158,   159,   160,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
   173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
    34,    44,    36,    37,    38,    39,    40,    41,    56,     0,
   238,    36,    37,    38,    39,    40,    41,   238,   239,   238,
   239,   239,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    57,     0,     5,    54,
    29,    30,    31,    72,    33,    34,    91,    36,    37,    38,
    39,    40,    41,   186,   241,    56,   238,    74,    58,   239,
     3,   128,    72,    -1,    53,    11,    12,    13,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
    -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,    -1,
    36,    37,    38,    39,    40,    41,    11,    12,    -1,    14,
    15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
    -1,    -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,
    -1,    36,    37,    38,    39,    40,    41,    11,    -1,    -1,
    14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
    24,    -1,    -1,    -1,    -1,    29,    30,    31,    -1,    33,
    34,    -1,    36,    37,    38,    39,    40,    41,    14,    15,
    16,    17,    18,    19,    20,    21,    22,    23,    24,    -1,
    -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,    -1,
    36,    37,    38,    39,    40,    41,    14,    -1,    -1,    17,
    18,    -1,    -1,    -1,    -1,    23,    24,    -1,    -1,    14,
    -1,    29,    -1,    -1,    -1,    33,    34,    -1,    36,    37,
    38,    39,    40,    41,    29,    -1,    -1,    -1,    33,    34,
    -1,    36,    37,    38,    39,    40,    41
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/share/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/share/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 111 "compile.y"
{
	Method& method=*NEW Method(POOL, 
		PC.request->main_method_name, 
		Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		yyvsp[0]/*parser_code*/, 0/*native_code*/);
	PC.cclass->add_method(PC.request->main_method_name, method);
;
    break;}
case 8:
#line 128 "compile.y"
{
	const String& command=*LA2S(yyvsp[-2]);
	YYSTYPE strings_code=yyvsp[0];
	if(strings_code->size()<1*2) {
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
		if(strings_code->size()==1*2) {
			// new class' name
			const String *name=LA2S(strings_code);
			// creating the class
			PC.cclass=NEW VClass(POOL);
			PC.cclass->set_name(*name);
			// append to request's classes
			PC.request->classes().put(*name, PC.cclass);
		} else {
			strcpy(PC.error, "@"CLASS_NAME" must contain sole name");
			YYERROR;
		}
	} else if(command==USE_CONTROL_METHOD_NAME) {
		for(int i=0; i<strings_code->size(); i+=2) 
			PC.request->use_file(*LA2S(strings_code, i));
	} else if(command==BASE_NAME) {
		if(PC.cclass->base_class()) { // already changed from default?
			strcpy(PC.error, "class already have a base '");
			strncat(PC.error, PC.cclass->base_class()->name().cstr(), 100);
			strcat(PC.error, "'");
			YYERROR;
		}
		if(strings_code->size()==1*2) {
			const String& base_name=*LA2S(strings_code);
			Value *vbase_class=static_cast<VClass *>(
				PC.request->classes().get(base_name));
			VStateless_class *base_class=vbase_class?vbase_class->get_class():0;
			if(!base_class) {
				strcpy(PC.error, base_name.cstr());
				strcat(PC.error, ": undefined class in @"BASE_NAME);
				YYERROR;
			}
			// @CLASS == @BASE sanity check
			if(PC.cclass==base_class) {
				strcpy(PC.error, "@"CLASS_NAME" equals @"BASE_NAME);
				YYERROR;
			}
			PC.cclass->set_base(base_class);
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
;
    break;}
case 12:
#line 195 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 16:
#line 200 "compile.y"
{
	const String *name=LA2S(yyvsp[-5]);

	YYSTYPE params_names_code=yyvsp[-4];
	Array *params_names=0;
	if(int size=params_names_code->size()) {
		params_names=NEW Array(POOL);
		for(int i=0; i<size; i+=2)
			*params_names+=LA2S(params_names_code, i);
	}

	YYSTYPE locals_names_code=yyvsp[-3];
	Array *locals_names=0;
	if(int size=locals_names_code->size()) {
		locals_names=NEW Array(POOL);
		for(int i=0; i<size; i+=2)
			*locals_names+=LA2S(locals_names_code, i);
	}

	Method& method=*NEW Method(POOL, 
		*name, 
		Method::CT_ANY,
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		yyvsp[0], 0);
	PC.cclass->add_method(*name, method);
;
    break;}
case 19:
#line 229 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 23:
#line 231 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 29:
#line 239 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 35:
#line 245 "compile.y"
{
	yyval=yyvsp[0]; /* stack: resulting value */ 
	changetail_or_append(yyval, 
		OP_GET_ELEMENT, false,  /*->*/OP_GET_ELEMENT__WRITE,
		/*or */OP_WRITE_VALUE
		); /* value=pop; wcontext.write(value) */
;
    break;}
case 36:
#line 252 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 39:
#line 254 "compile.y"
{ yyval=yyvsp[-1] ;
    break;}
case 42:
#line 258 "compile.y"
{
	yyval=N(POOL); 
	Array *diving_code=yyvsp[0];
	const String *first_name=LA2S(diving_code);
	// self.xxx... -> xxx...
	// OP_VALUE+string+OP_GET_ELEMENT+... -> OP_WITH_SELF+...
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(yyval, OP_WITH_SELF); /* stack: starting context */
		P(yyval, diving_code, 
			/* skip over... */
			diving_code->size()>=3?3/*OP_VALUE+string+OP_GET_ELEMENTx*/:2/*OP_+string*/);
	} else {
		O(yyval, OP_WITH_READ); /* stack: starting context */

		// ^if ELEMENT -> ^if ELEMENT_OR_OPERATOR
		// OP_VALUE+string+OP_GET_ELEMENT. -> OP_VALUE+string+OP_GET_ELEMENT_OR_OPERATOR.
		if(PC.in_call_value && diving_code->size()==3)
			diving_code->put_int(2, OP_GET_ELEMENT_OR_OPERATOR);
		P(yyval, diving_code);
	}
	/* diving code; stack: current context */
;
    break;}
case 43:
#line 280 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 45:
#line 281 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 46:
#line 285 "compile.y"
{
	yyval=yyvsp[-1]; /* stack: context,name */
	P(yyval, yyvsp[0]); /* stack: context,name,constructor_value */
;
    break;}
case 50:
#line 293 "compile.y"
{
	yyval=N(POOL);
	Array *diving_code=yyvsp[0];
	const String *first_name=LA2S(diving_code);
	// $self.xxx... -> $xxx...
	// OP_VALUE+string+OP_GET_ELEMENT+... -> OP_WITH_SELF+...
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(yyval, OP_WITH_SELF); /* stack: starting context */
		P(yyval, diving_code, 
			/* skip over... */
			diving_code->size()>=3?3/*OP_VALUE+string+OP_GET_ELEMENTx*/:2/*OP_+string*/);
	} else {
		O(yyval, OP_WITH_ROOT); /* stack: starting context */
		P(yyval, diving_code);
	}
	/* diving code; stack: current context */
;
    break;}
case 51:
#line 310 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_WITH_WRITE); /* stack: starting context */
	P(yyval, yyvsp[0]); /* diving code; stack: context,name */
;
    break;}
case 52:
#line 315 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 56:
#line 322 "compile.y"
{
	// stack: context, name
	yyval=yyvsp[-1]; // stack: context, name, value
	O(yyval, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 57:
#line 328 "compile.y"
{ 
	yyval=N(POOL); 
	O(yyval, OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(yyval, yyvsp[-1]); // stack: context, name, value
	O(yyval, OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 58:
#line 336 "compile.y"
{
	// stack: context, name
	yyval=N(POOL); 
	OA(yyval, OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;
    break;}
case 62:
#line 347 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 65:
#line 354 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 66:
#line 358 "compile.y"
{
	yyval=yyvsp[0]; /* stack: value */
	changetail_or_append(yyval, 
		OP_CALL, true,  /*->*/ OP_CALL__WRITE,
		/*or */OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;
    break;}
case 67:
#line 364 "compile.y"
{ 
					PC.in_call_value=true; 
			;
    break;}
case 68:
#line 367 "compile.y"
{
				PC.in_call_value=false;
			;
    break;}
case 69:
#line 370 "compile.y"
{ /* ^field.$method{vasya} */
	yyval=yyvsp[-3]; /* with_xxx,diving code; stack: context,method_junction */

	YYSTYPE params_code=yyvsp[-1];
	if(params_code->size()==3) { // probably [] case. [OP_VALUE + Void + STORE_PARAM]
		if(Value *value=LA2V(params_code)) // it is OP_VALUE + value?
			if(!value->is_defined()) // value is VVoid?
				params_code=0; // ^zzz[] case. don't append lone empty param.
	}
	/* stack: context, method_junction */
	OA(yyval, OP_CALL, params_code); // method_frame=make frame(pop junction); ncontext=pop; call(ncontext,method_frame) stack: value
;
    break;}
case 72:
#line 385 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 76:
#line 391 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 77:
#line 392 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 78:
#line 393 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 80:
#line 396 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 82:
#line 400 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 84:
#line 404 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 85:
#line 406 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_STORE_PARAM);
;
    break;}
case 86:
#line 410 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_EXPR_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 87:
#line 414 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 91:
#line 423 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_PREPARE_TO_EXPRESSION);
	P(yyval, yyvsp[0]);
	O(yyval, OP_WRITE_EXPR_RESULT);
;
    break;}
case 93:
#line 432 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 95:
#line 434 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 97:
#line 436 "compile.y"
{
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 98:
#line 444 "compile.y"
{
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 104:
#line 457 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_GET_ELEMENT);
;
    break;}
case 105:
#line 461 "compile.y"
{
	Array *code;
	{
		change_string_literal_to_write_string_literal(code=yyvsp[-1]);
		P(code, yyvsp[0]);
	}
	yyval=N(POOL); 
	OA(yyval, OP_STRING_POOL, code);
;
    break;}
case 106:
#line 470 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_OBJECT_POOL, yyvsp[-1]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 107:
#line 476 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_WITH_READ);
	P(yyval, yyvsp[0]);
;
    break;}
case 109:
#line 481 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 110:
#line 482 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_GET_ELEMENT__WRITE);
;
    break;}
case 113:
#line 491 "compile.y"
{
	yyval=yyvsp[-1]; // stack: class name string
	if(*LA2S(yyval) == BASE_NAME) { // pseude BASE class
		if(VStateless_class *base=PC.cclass->base_class()) {
			change_string_literal_value(yyval, base->name());
		} else {
			strcpy(PC.error, "no base class declared");
			YYERROR;
		}
	}
	O(yyval, OP_GET_CLASS);
;
    break;}
case 114:
#line 503 "compile.y"
{
	yyval=yyvsp[-1];
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(yyval, OP_PREPARE_TO_CONSTRUCT_OBJECT);
;
    break;}
case 115:
#line 515 "compile.y"
{
	// see OP_PREPARE_TO_EXPRESSION!!
	if((yyval=yyvsp[0])->size()==2) // only one string literal in there?
		change_string_literal_to_double_literal(yyval); // make that string literal Double
;
    break;}
case 119:
#line 524 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 120:
#line 525 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 121:
#line 526 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 122:
#line 528 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_NEG) ;
    break;}
case 123:
#line 529 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 124:
#line 530 "compile.y"
{ yyval=yyvsp[0];	 O(yyval, OP_INV) ;
    break;}
case 125:
#line 531 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_NOT) ;
    break;}
case 126:
#line 532 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_DEF) ;
    break;}
case 127:
#line 533 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_IN) ;
    break;}
case 128:
#line 534 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_FEXISTS) ;
    break;}
case 129:
#line 535 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_DEXISTS) ;
    break;}
case 130:
#line 537 "compile.y"
{	yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_SUB) ;
    break;}
case 131:
#line 538 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_ADD) ;
    break;}
case 132:
#line 539 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_MUL) ;
    break;}
case 133:
#line 540 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_DIV) ;
    break;}
case 134:
#line 541 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_MOD) ;
    break;}
case 135:
#line 542 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_INTDIV) ;
    break;}
case 136:
#line 543 "compile.y"
{ yyval=yyvsp[-2]; 	P(yyval, yyvsp[0]);  O(yyval, OP_BIN_AND) ;
    break;}
case 137:
#line 544 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_BIN_OR) ;
    break;}
case 138:
#line 545 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_BIN_XOR) ;
    break;}
case 139:
#line 546 "compile.y"
{ yyval=yyvsp[-2];  OA(yyval, OP_NESTED_CODE, yyvsp[0]);  O(yyval, OP_LOG_AND) ;
    break;}
case 140:
#line 547 "compile.y"
{ yyval=yyvsp[-2];  OA(yyval, OP_NESTED_CODE, yyvsp[0]);  O(yyval, OP_LOG_OR) ;
    break;}
case 141:
#line 548 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_LOG_XOR) ;
    break;}
case 142:
#line 549 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_LT) ;
    break;}
case 143:
#line 550 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_GT) ;
    break;}
case 144:
#line 551 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_LE) ;
    break;}
case 145:
#line 552 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_GE) ;
    break;}
case 146:
#line 553 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_EQ) ;
    break;}
case 147:
#line 554 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_NE) ;
    break;}
case 148:
#line 555 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_LT) ;
    break;}
case 149:
#line 556 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_GT) ;
    break;}
case 150:
#line 557 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_LE) ;
    break;}
case 151:
#line 558 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_GE) ;
    break;}
case 152:
#line 559 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_EQ) ;
    break;}
case 153:
#line 560 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_NE) ;
    break;}
case 154:
#line 561 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_IS) ;
    break;}
case 155:
#line 564 "compile.y"
{
	yyval=N(POOL);
	OA(yyval, OP_STRING_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;
    break;}
case 156:
#line 573 "compile.y"
{
	// optimized from OP_STRING+OP_WRITE_VALUE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal(yyval=yyvsp[0])
;
    break;}
case 157:
#line 578 "compile.y"
{ yyval=VL(NEW VVoid(POOL)) ;
    break;}
case 158:
#line 579 "compile.y"
{ yyval=N(POOL) ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/share/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 581 "compile.y"

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

static int yylex(YYSTYPE *lvalp, void *pc) {
	#define lexical_brackets_nestage PC.brackets_nestages[PC.ls_sp]
	#define RC {result=c; goto break2; }

    register int c;
    int result;
	
	if(PC.pending_state) {
		result=PC.pending_state;
		PC.pending_state=0;
		return result;
	}
	
	const char *begin=PC.source;
	const char *end;
	int begin_line=PC.line;
	int skip_analized=0;
	while(true) {
		c=*(end=(PC.source++));
//		fprintf(stderr, "\nchar: %c %02X; nestage: %d, sp=%d", c, c, lexical_brackets_nestage, PC.sp);

		if(c=='\n') {
			PC.line++;
			PC.col=0;
		} else
			PC.col++;

		if(c=='@' && PC.col==0+1) {
			if(PC.ls==LS_DEF_SPECIAL_BODY) {
				// @SPECIAL
				// ...
				// @<here = 
				pop_LS(PC); // exiting from LS_DEF_SPECIAL_BODY state
			} // continuing checks
			if(PC.ls==LS_USER) {
				push_LS(PC, LS_DEF_NAME);
				RC;
			} else // @ in first column inside some code [when could that be?]
				result=BAD_METHOD_DECL_START;
			goto break2;
		} else if(c=='^')
			switch(PC.ls) {
case LS_EXPRESSION_VAR_NAME_WITH_COLON:
case LS_EXPRESSION_VAR_NAME_WITHOUT_COLON:
case LS_VAR_NAME_SIMPLE_WITH_COLON:
case LS_VAR_NAME_SIMPLE_WITHOUT_COLON:
case LS_VAR_NAME_CURLY:
case LS_METHOD_NAME:
case LS_COMMENT:
case LS_DEF_COMMENT:
	// no literals in names, please
	break;
default:
			switch(*PC.source) {
			// ^escaping some punctuators
			case '^': case '$': case ';':
			case '(': case ')':
			case '[': case ']':
			case '{': case '}':
			case '"':  case ':':
				if(end!=begin) {
					// append piece till ^
					PC.string->APPEND_CLEAN(begin, end-begin, PC.file, begin_line);
				}
				// reset piece 'begin' position & line
				end=begin=PC.source; // ^
				begin_line=PC.line;
				if(PC.ls==LS_METHOD_AFTER) {
					pop_LS(PC);
					result=EON;
					skip_analized=-1; // return to ^ afterwards to assure it's literality
					goto break2;
				} else {
					// skip over _ after ^
					PC.source++;  PC.col++;
					// skip analysis = forced literal
					continue;
				}

			// converting ^#HH into char(hex(HH))
			case '#':
				if(end!=begin) {
					// append piece till ^
					PC.string->APPEND_CLEAN(begin, end-begin, PC.file, begin_line);
				}
				// #HH ?
				if(PC.source[0]=='#' && PC.source[1] && PC.source[2]) {
					char *hex=(char *)POOL.malloc(1);
					hex[0]=
						hex_value[(unsigned char)PC.source[1]]*0x10+
						hex_value[(unsigned char)PC.source[2]];
					if(hex[0]==0) {
						result=BAD_HEX_LITERAL;
						goto break2; // wrong hex value[no ^#00 chars allowed]: bail out
					}
					// append char(hex(HH))
					PC.string->APPEND_CLEAN(hex, 1, PC.file, begin_line);
					// skip over ^#HH
					PC.source+=3;
					PC.col+=3;
					// reset piece 'begin' position & line
					begin=PC.source; // ^
					begin_line=PC.line;
					continue;
				}
				break;
			}
			break;
		}
		// #comment  start skipping
		if(c=='#' && PC.col==1) {
			if(end!=begin) {
				// append piece till #
				PC.string->APPEND_CLEAN(begin, end-begin, PC.file, begin_line);
			}
			// fall into COMMENT lexical state [wait for \n]
			push_LS(PC, LS_COMMENT);
			continue;
		}
		switch(PC.ls) {

		// USER'S = NOT OURS
		case LS_USER:
        case LS_NAME_SQUARE_PART: // name.[here].xxx
			if(PC.trim_bof)
				switch(c) {
				case '\n': case ' ': case '\t':
					begin=PC.source;
					begin_line=PC.line;
					continue; // skip it
				default:
					PC.trim_bof=false;
				}
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case ']':
				if(PC.ls==LS_NAME_SQUARE_PART)
					if(--lexical_brackets_nestage==0) {// $name.[co<]?>de<]?>
						pop_LS(PC); // $name.[co<]>de<]!>
						RC;
					}
				break;
			case '[': // $name.[co<[>de]
				if(PC.ls==LS_NAME_SQUARE_PART)
					lexical_brackets_nestage++;
				break;
			}
			break;
			
		// #COMMENT
		case LS_COMMENT:
			if(c=='\n') {
				// skip comment
				begin=PC.source;
				begin_line=PC.line;

				pop_LS(PC);
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
					PC.ls == LS_EXPRESSION_STRING_QUOTED && c=='"' ||
					PC.ls == LS_EXPRESSION_STRING_APOSTROFED && c=='\'') {
					pop_LS(PC); //"abc". | 'abc'.
					RC;
				}
				break;
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			}
			break;

		// METHOD DEFINITION
		case LS_DEF_NAME:
			switch(c) {
			case '[':
				PC.ls=LS_DEF_PARAMS;
				RC;
			case '\n':
				PC.ls=LS_DEF_SPECIAL_BODY;
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
				PC.ls=*PC.source=='['?LS_DEF_LOCALS:LS_DEF_COMMENT;
				RC;
			case '\n': // wrong. bailing out
				pop_LS(PC);
				RC;
			}
			break;

		case LS_DEF_LOCALS:
			switch(c) {
			case '[':
			case ';':
				RC;
			case ']':
				PC.ls=LS_DEF_COMMENT;
				RC;
			case '\n': // wrong. bailing out
				pop_LS(PC);
				RC;
			}
			break;

		case LS_DEF_COMMENT:
			if(c=='\n') {
				pop_LS(PC);
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
					if(PC.ls==LS_METHOD_ROUND) // method round param ended
						PC.ls=LS_METHOD_AFTER; // look for method end
					else // PC.ls==LS_VAR_ROUND // variable constructor ended
						pop_LS(PC); // return to normal life
				RC;
			case '$':
				push_LS(PC, LS_EXPRESSION_VAR_NAME_WITH_COLON);				
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '(':
				lexical_brackets_nestage++;
				RC;
			case '-':
				switch(*PC.source) {
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
			//case '#': // comment start
			case '&': case '|':
				if(*PC.source==c) { // && ||
					result=c=='&'?LAND:LOR;
					skip_analized=1;
				} else
					result=c;
				goto break2;
			case '!':
				switch(PC.source[0]) { 
				case '|': // !| !||
					skip_analized=1;
					if(PC.source[1]=='|') {
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
			case '<': case '>': case '=': 
				if(*PC.source=='=') { // <= >= ==
					skip_analized=1;
					switch(c) {
					case '<': result=NLE; break;
					case '>': result=NGE; break;
					case '=': result=NEQ; break;
					}
				} else
					result=c;
				goto break2;
			case '"':
				push_LS(PC, LS_EXPRESSION_STRING_QUOTED);
				RC;
			case '\'':
				push_LS(PC, LS_EXPRESSION_STRING_APOSTROFED);
				RC;
			case 'l': case 'g': case 'e': case 'n':
				if(end==begin) // right after whitespace
					if(isspace(PC.source[1])) {
						switch(*PC.source) {
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
					if(isspace(PC.source[1])) {
						switch(PC.source[0]) {
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
					if(PC.source[0]=='e' && PC.source[1]=='f') { // def
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
				begin=PC.source; // after whitespace char
				begin_line=PC.line;
				continue;
			}
			break;

		// VARIABLE GET/PUT/WITH
		case LS_VAR_NAME_SIMPLE_WITH_COLON: 
		case LS_VAR_NAME_SIMPLE_WITHOUT_COLON:
		case LS_EXPRESSION_VAR_NAME_WITH_COLON: 
		case LS_EXPRESSION_VAR_NAME_WITHOUT_COLON:
			if(
				PC.ls==LS_EXPRESSION_VAR_NAME_WITH_COLON ||
				PC.ls==LS_EXPRESSION_VAR_NAME_WITHOUT_COLON) {
				// name in expr ends also before 
				switch(c) {
				// expression minus
				case '-': 
				// expression integer division
				case '\\':
					pop_LS(PC);
					PC.source--;  if(--PC.col<0) { PC.line--;  PC.col=-1; }
					result=EON;
					goto break2;
				}
			}
			if(
				PC.ls==LS_VAR_NAME_SIMPLE_WITHOUT_COLON ||
				PC.ls==LS_EXPRESSION_VAR_NAME_WITHOUT_COLON) {
				// name already has ':', stop before next 
				switch(c) {
				case ':': 
					pop_LS(PC);
					PC.source--;  if(--PC.col<0) { PC.line--;  PC.col=-1; }
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
				pop_LS(PC);
				PC.source--;  if(--PC.col<0) { PC.line--;  PC.col=-1; }
				result=EON;
				goto break2;
			case '[':
				// $name.<[>code]
				if(PC.col>1/*not first column*/ && (
					end[-1]=='$'/*was start of get*/ ||
					end[-1]==':'/*was class name delim */ ||
					end[-1]=='.'/*was name delim */
					)) {
					push_LS(PC, LS_NAME_SQUARE_PART);
					lexical_brackets_nestage=1;
					RC;
				}
				PC.ls=LS_VAR_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				if(begin==end) { // ${name}, no need of EON, switching LS
					PC.ls=LS_VAR_NAME_CURLY; 
				} else {
					PC.ls=LS_VAR_CURLY;
					lexical_brackets_nestage=1;
				}

				RC;
			case '(':
				PC.ls=LS_VAR_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim
			case '$': // name part subvar
			case ':': // class<:>name
				// go to _WITHOUT_COLON state variant...
				if(PC.ls==LS_VAR_NAME_SIMPLE_WITH_COLON)
					PC.ls=LS_VAR_NAME_SIMPLE_WITHOUT_COLON;
				else if(PC.ls==LS_EXPRESSION_VAR_NAME_WITH_COLON)
					PC.ls=LS_EXPRESSION_VAR_NAME_WITHOUT_COLON;
				// ...stop before next ':'
				RC;
			}
			break;

		case LS_VAR_NAME_CURLY:
			switch(c) {
			case '[':
				// ${name.<[>code]}
				push_LS(PC, LS_NAME_SQUARE_PART);
				lexical_brackets_nestage=1;
				RC;
			case '}': // ${name} finished, restoring LS
				pop_LS(PC);
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
				push_LS(PC, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case ']':
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
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
				push_LS(PC, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '}':
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
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
				if(PC.col>1/*not first column*/ && (
					end[-1]=='^'/*was start of call*/ || // never, ^[ is literal...
					end[-1]==':'/*was class name delim */ ||
					end[-1]=='.'/*was name delim */
					)) {
					push_LS(PC, LS_NAME_SQUARE_PART);
					lexical_brackets_nestage=1;
					RC;
				}
				PC.ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				PC.ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			case '(':
				PC.ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim 
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
			case '^': // ^abc^xxx wrong. bailing out
			case ']': case '}': case ')': // ^abc]}) wrong. bailing out
				RC;
			}
			break;

		case LS_METHOD_SQUARE:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case ';': // param delim
				RC;
			case ']':
				if(--lexical_brackets_nestage==0) {
					PC.ls=LS_METHOD_AFTER;
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
				push_LS(PC, LS_VAR_NAME_SIMPLE_WITH_COLON);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case ';': // param delim
				RC;
			case '}':
				if(--lexical_brackets_nestage==0) {
					PC.ls=LS_METHOD_AFTER;
					RC;
				}
				break;
			case '{':
				lexical_brackets_nestage++;
				break;
			}
			break;

		case LS_METHOD_AFTER:
			if(c=='[') {/* ][ }[ )[ */
				PC.ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			}					   
			if(c=='{') {/* ]{ }{ ){ */
				PC.ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			}					   
			if(c=='(') {/* ]( }( )( */
				PC.ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			}					   
			pop_LS(PC);
			PC.source--;  if(--PC.col<0) { PC.line--;  PC.col=-1; }
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
		if(end!=begin && PC.ls!=LS_COMMENT) { // last piece still alive and not comment?
			// append it
			PC.string->APPEND_CLEAN(begin, end-begin, PC.file, begin_line/*, start_col*/);
		}
	}
	if(PC.string->size()) { // something accumulated?
		// create STRING value: array of OP_VALUE+vstring
		*lvalp=VL(NEW VString(*PC.string));
		// new pieces storage
		PC.string=NEW String(POOL);
		// make current result be pending for next call, return STRING for now
		PC.pending_state=result;  result=STRING;
	}
	if(skip_analized) {
		PC.source+=skip_analized;  PC.col+=skip_analized;
	}
	return result;
}

static int real_yyerror(parse_control *pc, char *s) {  // Called by yyparse on error
	   strncpy(PC.error, s, MAX_STRING);
	   return 1;
}

static void yyprint(FILE *file, int type, YYSTYPE value) {
	if(type==STRING)
		fprintf(file, " \"%s\"", LA2S(value)->cstr());
}

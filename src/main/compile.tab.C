
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
#define	BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE	264
#define	LAND	265
#define	LOR	266
#define	LXOR	267
#define	NXOR	268
#define	NLE	269
#define	NGE	270
#define	NEQ	271
#define	NNE	272
#define	NSL	273
#define	NSR	274
#define	SLT	275
#define	SGT	276
#define	SLE	277
#define	SGE	278
#define	SEQ	279
#define	SNE	280
#define	DEF	281
#define	IN	282
#define	FEXISTS	283
#define	DEXISTS	284
#define	IS	285
#define	NUNARY	286

#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001, 2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.tab.C,v 1.97 2004/04/06 09:01:20 paf Exp $
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
#ifndef YYSTYPE
#define YYSTYPE int
#endif
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		253
#define	YYFLAG		-32768
#define	YYNTBASE	60

#define YYTRANSLATE(x) ((unsigned)(x) <= 286 ? yytranslate[x] : 140)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    46,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    44,    58,     2,    50,    41,    35,    59,    54,
    55,    38,    36,     2,    37,    53,    39,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    57,    49,    32,
     2,    33,     2,    45,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    47,    40,    48,    56,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    51,    34,    52,    43,     2,     2,     2,     2,
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
    27,    28,    29,    30,    31,    42
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     9,    11,    13,    15,    20,    22,
    24,    26,    29,    32,    34,    36,    37,    46,    48,    50,
    54,    56,    58,    60,    64,    66,    68,    70,    72,    74,
    77,    79,    81,    83,    85,    87,    89,    92,    95,    97,
   101,   103,   105,   107,   110,   112,   115,   119,   121,   123,
   125,   127,   130,   133,   135,   137,   139,   143,   147,   151,
   153,   155,   157,   159,   161,   163,   166,   168,   169,   170,
   177,   179,   181,   184,   186,   188,   190,   194,   198,   202,
   204,   208,   210,   214,   216,   220,   222,   224,   226,   228,
   230,   232,   234,   236,   239,   241,   244,   247,   249,   251,
   254,   256,   258,   260,   262,   265,   268,   272,   274,   276,
   279,   282,   284,   286,   289,   292,   294,   296,   298,   300,
   304,   308,   312,   315,   318,   321,   324,   327,   330,   333,
   336,   340,   344,   348,   352,   356,   360,   364,   368,   372,
   376,   380,   384,   388,   392,   396,   400,   404,   408,   412,
   416,   420,   424,   428,   432,   436,   440,   444,   446,   448,
   449
};

static const short yyrhs[] = {    62,
     0,    61,     0,    63,     0,    61,    63,     0,    76,     0,
    64,     0,    69,     0,    45,     4,    46,    65,     0,   139,
     0,    66,     0,    67,     0,    66,    67,     0,    68,    46,
     0,   139,     0,     4,     0,     0,    45,     4,    72,    71,
    75,    46,    70,    76,     0,   139,     0,    72,     0,    47,
    73,    48,     0,   139,     0,    74,     0,     4,     0,    74,
    49,     4,     0,   139,     0,     4,     0,   139,     0,    77,
     0,    78,     0,    77,    78,     0,   137,     0,    79,     0,
    80,     0,    88,     0,   101,     0,    81,     0,    50,    82,
     0,    84,     3,     0,    83,     0,    51,    84,    52,     0,
    85,     0,    86,     0,    87,     0,   131,    87,     0,   123,
     0,   120,   123,     0,    50,    89,    93,     0,    90,     0,
    91,     0,    92,     0,   119,     0,    53,   119,     0,   131,
   119,     0,    94,     0,    95,     0,    96,     0,    47,    97,
    48,     0,    54,   134,    55,     0,    51,    76,    52,     0,
   138,     0,     4,     0,    98,     0,    99,     0,   100,     0,
    79,     0,    78,    77,     0,   102,     0,     0,     0,    56,
   103,   105,   104,   106,     3,     0,    84,     0,   107,     0,
   106,   107,     0,   108,     0,   109,     0,   110,     0,    47,
   111,    48,     0,    54,   112,    55,     0,    51,   113,    52,
     0,   114,     0,   111,    49,   114,     0,   115,     0,   112,
    49,   115,     0,   116,     0,   113,    49,   116,     0,   117,
     0,   118,     0,    76,     0,   138,     0,     4,     0,    98,
     0,   134,     0,   124,     0,   120,   124,     0,   121,     0,
   120,   121,     0,   122,    53,     0,   124,     0,   124,     0,
     4,     5,     0,     4,     0,   125,     0,   126,     0,   127,
     0,    50,   128,     0,     4,   129,     0,    47,    77,    48,
     0,     4,     0,   130,     0,   129,   130,     0,    50,   128,
     0,   132,     0,   133,     0,     4,    57,     0,   132,    57,
     0,   135,     0,     4,     0,    81,     0,   102,     0,    58,
   136,    58,     0,    59,   136,    59,     0,    54,   135,    55,
     0,    37,   135,     0,    36,   135,     0,    43,   135,     0,
    44,   135,     0,    27,   135,     0,    28,   135,     0,    29,
   135,     0,    30,   135,     0,   135,    37,   135,     0,   135,
    36,   135,     0,   135,    38,   135,     0,   135,    39,   135,
     0,   135,    41,   135,     0,   135,    40,   135,     0,   135,
    19,   135,     0,   135,    20,   135,     0,   135,    35,   135,
     0,   135,    34,   135,     0,   135,    14,   135,     0,   135,
    11,   135,     0,   135,    12,   135,     0,   135,    13,   135,
     0,   135,    32,   135,     0,   135,    33,   135,     0,   135,
    15,   135,     0,   135,    16,   135,     0,   135,    17,   135,
     0,   135,    18,   135,     0,   135,    21,   135,     0,   135,
    22,   135,     0,   135,    23,   135,     0,   135,    24,   135,
     0,   135,    25,   135,     0,   135,    26,   135,     0,   135,
    31,   135,     0,    76,     0,     4,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   120,   128,   130,   130,   131,   133,   133,   135,   207,   207,
   208,   208,   209,   210,   210,   212,   247,   252,   252,   253,
   254,   254,   255,   255,   257,   257,   261,   261,   263,   263,
   264,   264,   265,   265,   265,   269,   276,   277,   277,   278,
   279,   281,   282,   304,   305,   305,   309,   313,   315,   316,
   317,   334,   339,   341,   343,   344,   346,   352,   360,   366,
   368,   369,   371,   377,   378,   378,   382,   388,   391,   394,
   407,   409,   409,   410,   412,   413,   415,   416,   417,   418,
   420,   422,   424,   426,   428,   430,   434,   438,   442,   444,
   445,   447,   456,   456,   458,   458,   459,   460,   468,   473,
   475,   477,   478,   479,   481,   485,   494,   500,   505,   505,
   506,   511,   513,   515,   527,   539,   544,   546,   547,   548,
   549,   550,   552,   553,   554,   555,   556,   557,   558,   559,
   561,   562,   563,   564,   565,   566,   567,   568,   569,   570,
   571,   572,   573,   574,   575,   576,   577,   578,   579,   580,
   581,   582,   583,   584,   585,   586,   587,   590,   599,   604,
   605
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EON","STRING",
"BOGUS","BAD_STRING_COMPARISON_OPERATOR","BAD_HEX_LITERAL","BAD_METHOD_DECL_START",
"BAD_METHOD_PARAMETER_NAME_CHARACTER","BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE",
"\"&&\"","\"||\"","\"!||\"","\"!|\"","\"<=\"","\">=\"","\"==\"","\"!=\"","\"<<\"",
"\">>\"","\"lt\"","\"gt\"","\"le\"","\"ge\"","\"eq\"","\"ne\"","\"def\"","\"in\"",
"\"-f\"","\"-d\"","\"is\"","'<'","'>'","'|'","'&'","'+'","'-'","'*'","'/'","'\\\\'",
"'%'","NUNARY","'~'","'!'","'@'","'\\n'","'['","']'","';'","'$'","'{'","'}'",
"'.'","'('","')'","'^'","':'","'\\\"'","'\\''","all","methods","one_big_piece",
"method","control_method","maybe_control_strings","control_strings","control_string",
"maybe_string","code_method","@1","maybe_bracketed_strings","bracketed_maybe_strings",
"maybe_strings","strings","maybe_comment","maybe_codes","codes","code","action",
"get","get_value","get_name_value","name_in_curly_rdive","name_without_curly_rdive",
"name_without_curly_rdive_read","name_without_curly_rdive_class","name_without_curly_rdive_code",
"put","name_expr_wdive","name_expr_wdive_root","name_expr_wdive_write","name_expr_wdive_class",
"construct","construct_square","construct_round","construct_curly","any_constructor_code_value",
"constructor_code_value","constructor_code","codes__excluding_sole_str_literal",
"call","call_value","@2","@3","call_name","store_params","store_param","store_square_param",
"store_round_param","store_curly_param","store_code_param_parts","store_expr_param_parts",
"store_curly_param_parts","store_code_param_part","store_expr_param_part","store_curly_param_part",
"code_param_value","write_expr_value","name_expr_dive_code","name_path","name_step",
"name_advance1","name_advance2","name_expr_value","name_expr_subvar_value","name_expr_with_subvar_value",
"name_square_code_value","subvar_ref_name_rdive","subvar_get_writes","subvar__get_write",
"class_prefix","class_static_prefix","class_constructor_prefix","expr_value",
"expr","string_inside_quotes_value","write_string","void_value","empty", NULL
};
#endif

static const short yyr1[] = {     0,
    60,    60,    61,    61,    62,    63,    63,    64,    65,    65,
    66,    66,    67,    68,    68,    70,    69,    71,    71,    72,
    73,    73,    74,    74,    75,    75,    76,    76,    77,    77,
    78,    78,    79,    79,    79,    80,    81,    82,    82,    83,
    84,    84,    85,    86,    87,    87,    88,    89,    89,    89,
    90,    91,    92,    93,    93,    93,    94,    95,    96,    97,
    97,    97,    98,    99,   100,   100,   101,   103,   104,   102,
   105,   106,   106,   107,   107,   107,   108,   109,   110,   111,
   111,   112,   112,   113,   113,   114,   115,   116,   117,   117,
   117,   118,   119,   119,   120,   120,   121,   122,   123,   123,
   124,   124,   124,   124,   125,   126,   127,   128,   129,   129,
   130,   131,   131,   132,   133,   134,   135,   135,   135,   135,
   135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
   135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
   135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
   135,   135,   135,   135,   135,   135,   135,   136,   137,   138,
   139
};

static const short yyr2[] = {     0,
     1,     1,     1,     2,     1,     1,     1,     4,     1,     1,
     1,     2,     2,     1,     1,     0,     8,     1,     1,     3,
     1,     1,     1,     3,     1,     1,     1,     1,     1,     2,
     1,     1,     1,     1,     1,     1,     2,     2,     1,     3,
     1,     1,     1,     2,     1,     2,     3,     1,     1,     1,
     1,     2,     2,     1,     1,     1,     3,     3,     3,     1,
     1,     1,     1,     1,     1,     2,     1,     0,     0,     6,
     1,     1,     2,     1,     1,     1,     3,     3,     3,     1,
     3,     1,     3,     1,     3,     1,     1,     1,     1,     1,
     1,     1,     1,     2,     1,     2,     2,     1,     1,     2,
     1,     1,     1,     1,     2,     2,     3,     1,     1,     2,
     2,     1,     1,     2,     2,     1,     1,     1,     1,     3,
     3,     3,     2,     2,     2,     2,     2,     2,     2,     2,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     1,     1,     0,
     0
};

static const short yydefact[] = {   161,
   159,     0,     0,    68,     2,     1,     3,     6,     7,     5,
    28,    29,    32,    33,    36,    34,    35,    67,    31,    27,
     0,   101,     0,     0,     0,     0,    37,    39,     0,    41,
    42,    43,     0,    48,    49,    50,    51,     0,    95,     0,
    45,    93,   102,   103,   104,     0,   112,   113,     0,     4,
    30,   161,   161,   161,   100,     0,   114,   106,   109,     0,
   108,   105,     0,     0,    99,     0,   101,    52,     0,    93,
    38,   160,   161,     0,    47,    54,    55,    56,   101,    96,
    46,    94,    97,    44,    53,   115,    71,    69,    15,     8,
    10,    11,     0,     9,    23,     0,    22,    21,   161,    19,
    18,   111,   110,   107,    40,    94,   159,     0,    32,     0,
    62,    63,    64,    60,     0,   117,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,   161,   161,   118,   119,
     0,   116,     0,    12,    14,    13,    20,     0,    26,     0,
    25,    66,    57,    59,   127,   128,   129,   130,   124,   123,
   125,   126,     0,   158,     0,     0,    58,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,   160,   161,     0,     0,    72,    74,
    75,    76,    24,    16,   122,   120,   121,   142,   143,   144,
   141,   147,   148,   149,   150,   137,   138,   151,   152,   153,
   154,   155,   156,   157,   145,   146,   140,   139,   132,   131,
   133,   134,   136,   135,   159,    91,     0,    80,    86,    89,
    88,     0,    84,     0,    82,    87,    92,    70,    73,   161,
    77,   160,   161,    79,     0,    78,    17,    81,    85,    83,
     0,     0,     0
};

static const short yydefgoto[] = {   251,
     5,     6,     7,     8,    90,    91,    92,    93,     9,   240,
    99,    54,    96,    97,   140,   154,    11,    12,    13,    14,
   129,    27,    28,    29,    30,    31,    32,    16,    33,    34,
    35,    36,    75,    76,    77,    78,   110,   226,   112,   113,
    17,   130,    49,   133,    88,   188,   189,   190,   191,   192,
   227,   234,   232,   228,   235,   233,   229,   236,    37,    64,
    39,    40,    41,    65,    43,    44,    45,    62,    58,    59,
    66,    47,    48,   237,   132,   155,    19,   230,    20
};

static const short yypact[] = {     9,
-32768,    16,    43,-32768,   -35,-32768,-32768,-32768,-32768,-32768,
    24,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    64,    32,    24,    80,     3,    79,-32768,-32768,    83,-32768,
-32768,-32768,   -33,-32768,-32768,-32768,-32768,    91,-32768,    54,
-32768,    14,-32768,-32768,-32768,    91,    56,-32768,     3,-32768,
-32768,   119,   121,    92,-32768,    80,-32768,    90,-32768,    31,
-32768,-32768,    78,    91,    96,    91,    90,-32768,    79,    96,
-32768,    41,    24,    12,-32768,-32768,-32768,-32768,    19,-32768,
-32768,    14,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    15,-32768,    97,   104,-32768,   113,   115,-32768,   141,-32768,
-32768,-32768,-32768,-32768,-32768,    96,   117,    24,    86,   118,
-32768,-32768,-32768,-32768,   116,-32768,    12,    12,    12,    12,
    12,    12,    12,    12,    53,    12,    24,    24,-32768,-32768,
   114,   283,   -25,-32768,-32768,-32768,-32768,   166,-32768,   126,
-32768,    24,-32768,-32768,   442,   442,   442,   442,-32768,-32768,
-32768,-32768,   238,-32768,   129,   167,-32768,    12,    12,    12,
    12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
    12,    12,    12,    12,    12,    12,    12,    12,    12,    12,
    12,    12,    12,    12,    42,    24,    12,    55,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   373,   345,   314,
   143,   401,   401,   414,   414,   183,   183,   401,   401,   401,
   401,   414,   414,   442,   401,   401,   246,   427,    77,    77,
-32768,-32768,-32768,-32768,    88,-32768,    99,-32768,-32768,-32768,
-32768,   -16,-32768,   -24,-32768,-32768,-32768,-32768,-32768,    24,
-32768,    42,    24,-32768,    12,-32768,-32768,-32768,-32768,-32768,
   173,   174,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   170,-32768,-32768,-32768,   134,-32768,-32768,-32768,
-32768,   175,-32768,-32768,-32768,     5,   -20,    -9,   -71,-32768,
     0,-32768,-32768,   -19,-32768,-32768,    39,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   155,-32768,-32768,
-32768,     4,-32768,-32768,-32768,-32768,    40,-32768,-32768,-32768,
-32768,-32768,-32768,   -54,   -15,   -12,-32768,-32768,    73,    98,
   -26,-32768,   -30,     6,-32768,-32768,-32768,   176,-32768,   177,
   231,-32768,-32768,   162,    34,   109,-32768,   169,    68
};


#define	YYLAST		483


static const short yytable[] = {    15,
   109,    51,    60,    18,    10,    63,    22,    81,    42,     2,
    15,    80,     1,    72,    18,   116,   -99,    73,    89,    21,
    74,   185,    15,    55,   245,   186,    18,     1,   187,    87,
   246,    70,   243,    81,     1,   244,    55,    80,   117,   118,
   119,   120,    80,    82,   107,   225,    22,   121,   122,    23,
    51,    42,    24,     2,   123,   124,    22,   238,     3,    15,
  -161,   125,   108,    18,     4,   126,   -98,     4,    56,   127,
   128,    15,    15,     3,   106,    18,    18,   115,   104,     4,
     3,    56,    67,    61,    84,    71,     4,   142,    57,    23,
     3,     3,    24,    25,    79,    26,     4,     4,    68,    23,
    38,   185,    24,    25,    84,   186,    83,    15,   187,    52,
    53,    18,    86,   109,   181,   182,   183,   184,    85,    94,
    98,   101,    89,    69,    95,    23,    15,    15,    24,   105,
    18,    18,    51,   -65,   -65,   -90,   -90,    23,    53,    56,
    24,    15,   136,    38,   139,    18,   241,   242,   -98,   -14,
   145,   146,   147,   148,   149,   150,   151,   152,   135,   153,
   137,   166,   167,   138,   -61,   143,   141,   144,   157,   193,
   109,   194,   252,   253,    50,   108,   177,   178,   179,   180,
   181,   182,   183,   184,    15,    15,   196,   248,    18,    18,
   231,   198,   199,   200,   201,   202,   203,   204,   205,   206,
   207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
   217,   218,   219,   220,   221,   222,   223,   224,   179,   180,
   181,   182,   183,   184,   134,   197,   111,   239,   100,   250,
   249,   102,   108,    46,   103,   131,   156,     0,     0,    15,
   114,    15,    15,    18,   247,    18,    18,   231,   158,   159,
   160,   161,   162,   163,   164,   165,   166,   167,   168,   169,
   170,   171,   172,   173,   166,   167,     0,     0,   174,   175,
   176,   177,   178,   179,   180,   181,   182,   183,   184,     0,
   178,   179,   180,   181,   182,   183,   184,     0,     0,     0,
     0,     0,   195,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,     0,
     0,     0,     0,   174,   175,   176,   177,   178,   179,   180,
   181,   182,   183,   184,   158,   159,     0,   161,   162,   163,
   164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     0,     0,     0,     0,   174,   175,   176,   177,   178,   179,
   180,   181,   182,   183,   184,   158,     0,     0,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
   173,     0,     0,     0,     0,   174,   175,   176,   177,   178,
   179,   180,   181,   182,   183,   184,   161,   162,   163,   164,
   165,   166,   167,   168,   169,   170,   171,   172,   173,     0,
     0,     0,     0,   174,   175,   176,   177,   178,   179,   180,
   181,   182,   183,   184,   161,     0,     0,   164,   165,   166,
   167,     0,     0,     0,     0,   172,   173,   161,     0,     0,
     0,   174,   166,   167,   177,   178,   179,   180,   181,   182,
   183,   184,     0,     0,   174,   166,   167,   177,   178,   179,
   180,   181,   182,   183,   184,   161,     0,     0,     0,     0,
   166,   167,   179,   180,   181,   182,   183,   184,     0,     0,
     0,     0,     0,     0,     0,   177,   178,   179,   180,   181,
   182,   183,   184
};

static const short yycheck[] = {     0,
    72,    11,    23,     0,     0,    25,     4,    38,     3,    45,
    11,    38,     4,    47,    11,     4,     3,    51,     4,     4,
    54,    47,    23,     5,    49,    51,    23,     4,    54,    49,
    55,    26,    49,    64,     4,    52,     5,    64,    27,    28,
    29,    30,    69,    38,     4,     4,     4,    36,    37,    47,
    60,    46,    50,    45,    43,    44,     4,     3,    50,    60,
    46,    50,    72,    60,    56,    54,    53,    56,    50,    58,
    59,    72,    73,    50,    69,    72,    73,    73,    48,    56,
    50,    50,     4,     4,    46,     3,    56,   108,    57,    47,
    50,    50,    50,    51,     4,    53,    56,    56,    26,    47,
     3,    47,    50,    51,    66,    51,    53,   108,    54,    46,
    47,   108,    57,   185,    38,    39,    40,    41,    46,    52,
    53,    54,     4,    26,     4,    47,   127,   128,    50,    52,
   127,   128,   142,    48,    49,    48,    49,    47,    47,    50,
    50,   142,    46,    46,     4,   142,    48,    49,    53,    46,
   117,   118,   119,   120,   121,   122,   123,   124,    91,   126,
    48,    19,    20,    49,    48,    48,    99,    52,    55,     4,
   242,    46,     0,     0,     5,   185,    34,    35,    36,    37,
    38,    39,    40,    41,   185,   186,    58,   242,   185,   186,
   186,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   168,   169,   170,   171,   172,   173,   174,   175,   176,
   177,   178,   179,   180,   181,   182,   183,   184,    36,    37,
    38,    39,    40,    41,    91,    59,    72,   188,    54,   245,
   243,    56,   242,     3,    58,    74,   128,    -1,    -1,   240,
    72,   242,   243,   240,   240,   242,   243,   243,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
    23,    24,    25,    26,    19,    20,    -1,    -1,    31,    32,
    33,    34,    35,    36,    37,    38,    39,    40,    41,    -1,
    35,    36,    37,    38,    39,    40,    41,    -1,    -1,    -1,
    -1,    -1,    55,    11,    12,    13,    14,    15,    16,    17,
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
    40,    41,    -1,    -1,    31,    19,    20,    34,    35,    36,
    37,    38,    39,    40,    41,    14,    -1,    -1,    -1,    -1,
    19,    20,    36,    37,    38,    39,    40,    41,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    34,    35,    36,    37,    38,
    39,    40,    41
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
  YYSTYPE yylval=0; /*paf:calm down, vc7 warning level 4*/
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval=0; /*paf:calm down, vc7 warning level 4*/		/*  the variable used to return		*/
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
#line 121 "compile.y"
{
	Method& method=*new Method(Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		yyvsp[0]/*parser_code*/, 0/*native_code*/);
	PC.cclass->add_method(PC.alias_method(main_method_name), method);
;
    break;}
case 8:
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
;
    break;}
case 12:
#line 208 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 16:
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
;
    break;}
case 17:
#line 247 "compile.y"
{
	// fill in the code
	reinterpret_cast<Method*>(yyvsp[-1])->parser_code=yyvsp[0];
;
    break;}
case 20:
#line 253 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 24:
#line 255 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 30:
#line 263 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 36:
#line 269 "compile.y"
{
	yyval=yyvsp[0]; /* stack: resulting value */ 
	changetail_or_append(*yyval, 
		OP_GET_ELEMENT, false,  /*->*/OP_GET_ELEMENT__WRITE,
		/*or */OP_WRITE_VALUE
		); /* value=pop; wcontext.write(value) */
;
    break;}
case 37:
#line 276 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 40:
#line 278 "compile.y"
{ yyval=yyvsp[-1] ;
    break;}
case 43:
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
;
    break;}
case 44:
#line 304 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 46:
#line 305 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 47:
#line 309 "compile.y"
{
	yyval=yyvsp[-1]; /* stack: context,name */
	P(*yyval, *yyvsp[0]); /* stack: context,name,constructor_value */
;
    break;}
case 51:
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
;
    break;}
case 52:
#line 334 "compile.y"
{
	yyval=N(); 
	O(*yyval, OP_WITH_WRITE); /* stack: starting context */
	P(*yyval, *yyvsp[0]); /* diving code; stack: context,name */
;
    break;}
case 53:
#line 339 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 57:
#line 346 "compile.y"
{
	// stack: context, name
	yyval=yyvsp[-1]; // stack: context, name, value
	O(*yyval, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 58:
#line 352 "compile.y"
{ 
	yyval=N(); 
	O(*yyval, OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(*yyval, *yyvsp[-1]); // stack: context, name, value
	O(*yyval, OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 59:
#line 360 "compile.y"
{
	// stack: context, name
	yyval=N(); 
	OA(*yyval, OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;
    break;}
case 63:
#line 371 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 66:
#line 378 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 67:
#line 382 "compile.y"
{
	yyval=yyvsp[0]; /* stack: value */
	changetail_or_append(*yyval, 
		OP_CALL, true,  /*->*/ OP_CALL__WRITE,
		/*or */OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;
    break;}
case 68:
#line 388 "compile.y"
{ 
					PC.in_call_value=true; 
			;
    break;}
case 69:
#line 391 "compile.y"
{
				PC.in_call_value=false;
			;
    break;}
case 70:
#line 394 "compile.y"
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
;
    break;}
case 73:
#line 409 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 77:
#line 415 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 78:
#line 416 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 79:
#line 417 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 81:
#line 420 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 83:
#line 424 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 85:
#line 428 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 86:
#line 430 "compile.y"
{
	yyval=yyvsp[0];
	O(*yyval, OP_STORE_PARAM);
;
    break;}
case 87:
#line 434 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_EXPR_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 88:
#line 438 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 92:
#line 447 "compile.y"
{
	yyval=N(); 
	O(*yyval, OP_PREPARE_TO_EXPRESSION);
	P(*yyval, *yyvsp[0]);
	O(*yyval, OP_WRITE_EXPR_RESULT);
;
    break;}
case 94:
#line 456 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 96:
#line 458 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 98:
#line 460 "compile.y"
{
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 99:
#line 468 "compile.y"
{
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 105:
#line 481 "compile.y"
{
	yyval=yyvsp[0];
	O(*yyval, OP_GET_ELEMENT);
;
    break;}
case 106:
#line 485 "compile.y"
{
	ArrayOperation* code;
	{
		change_string_literal_to_write_string_literal(*(code=yyvsp[-1]));
		P(*code, *yyvsp[0]);
	}
	yyval=N(); 
	OA(*yyval, OP_STRING_POOL, code);
;
    break;}
case 107:
#line 494 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_OBJECT_POOL, yyvsp[-1]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 108:
#line 500 "compile.y"
{
	yyval=N(); 
	O(*yyval, OP_WITH_READ);
	P(*yyval, *yyvsp[0]);
;
    break;}
case 110:
#line 505 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 111:
#line 506 "compile.y"
{
	yyval=yyvsp[0];
	O(*yyval, OP_GET_ELEMENT__WRITE);
;
    break;}
case 114:
#line 515 "compile.y"
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
;
    break;}
case 115:
#line 527 "compile.y"
{
	yyval=yyvsp[-1];
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(*yyval, OP_PREPARE_TO_CONSTRUCT_OBJECT);
;
    break;}
case 116:
#line 539 "compile.y"
{
	// see OP_PREPARE_TO_EXPRESSION!!
	if((yyval=yyvsp[0])->count()==2) // only one string literal in there?
		change_string_literal_to_double_literal(*yyval); // make that string literal Double
;
    break;}
case 120:
#line 548 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 121:
#line 549 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 122:
#line 550 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 123:
#line 552 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_NEG) ;
    break;}
case 124:
#line 553 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 125:
#line 554 "compile.y"
{ yyval=yyvsp[0];	 O(*yyval, OP_INV) ;
    break;}
case 126:
#line 555 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_NOT) ;
    break;}
case 127:
#line 556 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_DEF) ;
    break;}
case 128:
#line 557 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_IN) ;
    break;}
case 129:
#line 558 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_FEXISTS) ;
    break;}
case 130:
#line 559 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_DEXISTS) ;
    break;}
case 131:
#line 561 "compile.y"
{	yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_SUB) ;
    break;}
case 132:
#line 562 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_ADD) ;
    break;}
case 133:
#line 563 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_MUL) ;
    break;}
case 134:
#line 564 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_DIV) ;
    break;}
case 135:
#line 565 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_MOD) ;
    break;}
case 136:
#line 566 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_INTDIV) ;
    break;}
case 137:
#line 567 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_SL) ;
    break;}
case 138:
#line 568 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_SR) ;
    break;}
case 139:
#line 569 "compile.y"
{ yyval=yyvsp[-2]; 	P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_AND) ;
    break;}
case 140:
#line 570 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_OR) ;
    break;}
case 141:
#line 571 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_XOR) ;
    break;}
case 142:
#line 572 "compile.y"
{ yyval=yyvsp[-2];  OA(*yyval, OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP_LOG_AND) ;
    break;}
case 143:
#line 573 "compile.y"
{ yyval=yyvsp[-2];  OA(*yyval, OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP_LOG_OR) ;
    break;}
case 144:
#line 574 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_LOG_XOR) ;
    break;}
case 145:
#line 575 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_LT) ;
    break;}
case 146:
#line 576 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_GT) ;
    break;}
case 147:
#line 577 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_LE) ;
    break;}
case 148:
#line 578 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_GE) ;
    break;}
case 149:
#line 579 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_EQ) ;
    break;}
case 150:
#line 580 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_NE) ;
    break;}
case 151:
#line 581 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_LT) ;
    break;}
case 152:
#line 582 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_GT) ;
    break;}
case 153:
#line 583 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_LE) ;
    break;}
case 154:
#line 584 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_GE) ;
    break;}
case 155:
#line 585 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_EQ) ;
    break;}
case 156:
#line 586 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_NE) ;
    break;}
case 157:
#line 587 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_IS) ;
    break;}
case 158:
#line 590 "compile.y"
{
	yyval=N();
	OA(*yyval, OP_STRING_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;
    break;}
case 159:
#line 599 "compile.y"
{
	// optimized from OP_STRING+OP_WRITE_VALUE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*(yyval=yyvsp[0]))
;
    break;}
case 160:
#line 604 "compile.y"
{ yyval=VL(new VVoid(), 0, 0, 0) ;
    break;}
case 161:
#line 605 "compile.y"
{ yyval=N() ;
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
#line 607 "compile.y"

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

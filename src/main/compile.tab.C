
/*  A Bison parser, made from compile.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	EON	257
#define	STRING	258
#define	BOGUS	259
#define	BAD_STRING_COMPARISON_OPERATOR	260
#define	BAD_HEX_LITERAL	261
#define	BAD_METHOD_DECL_START	262
#define	LAND	263
#define	LOR	264
#define	LXOR	265
#define	NLE	266
#define	NGE	267
#define	NEQ	268
#define	NNE	269
#define	SLT	270
#define	SGT	271
#define	SLE	272
#define	SGE	273
#define	SEQ	274
#define	SNE	275
#define	DEF	276
#define	IN	277
#define	FEXISTS	278
#define	DEXISTS	279
#define	IS	280
#define	NEG	281

#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.tab.C,v 1.59 2002/08/12 14:21:51 paf Exp $
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
#define	YYNTBASE	56

#define YYTRANSLATE(x) ((unsigned)(x) <= 281 ? yytranslate[x] : 135)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    42,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    29,    54,    30,    46,    38,    32,    55,    50,
    51,    36,    35,     2,    34,    49,    37,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    53,    45,    27,
     2,    28,     2,    41,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    43,    39,    44,    52,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    47,    31,    48,    33,     2,     2,     2,     2,
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
    40
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

static const short yyrhs[] = {    58,
     0,    57,     0,    59,     0,    57,    59,     0,    71,     0,
    60,     0,    65,     0,    41,     4,    42,    61,     0,   134,
     0,    62,     0,    63,     0,    62,    63,     0,    64,    42,
     0,   134,     0,     4,     0,    41,     4,    67,    66,    70,
    42,    71,     0,   134,     0,    67,     0,    43,    68,    44,
     0,   134,     0,    69,     0,     4,     0,    69,    45,     4,
     0,   134,     0,     4,     0,   134,     0,    72,     0,    73,
     0,    72,    73,     0,   132,     0,    74,     0,    75,     0,
    83,     0,    96,     0,    76,     0,    46,    77,     0,    79,
     3,     0,    78,     0,    47,    79,    48,     0,    80,     0,
    81,     0,    82,     0,   126,    82,     0,   118,     0,   115,
   118,     0,    46,    84,    88,     0,    85,     0,    86,     0,
    87,     0,   114,     0,    49,   114,     0,   126,   114,     0,
    89,     0,    90,     0,    91,     0,    43,    92,    44,     0,
    50,   129,    51,     0,    47,    71,    48,     0,   133,     0,
     4,     0,    93,     0,    94,     0,    95,     0,    74,     0,
    73,    72,     0,    97,     0,     0,     0,    52,    98,   100,
    99,   101,     3,     0,    79,     0,   102,     0,   101,   102,
     0,   103,     0,   104,     0,   105,     0,    43,   106,    44,
     0,    50,   107,    51,     0,    47,   108,    48,     0,   109,
     0,   106,    45,   109,     0,   110,     0,   107,    45,   110,
     0,   111,     0,   108,    45,   111,     0,   112,     0,   113,
     0,    71,     0,   133,     0,     4,     0,    93,     0,   129,
     0,   119,     0,   115,   119,     0,   116,     0,   115,   116,
     0,   117,    49,     0,   119,     0,   119,     0,     4,     5,
     0,     4,     0,   120,     0,   121,     0,   122,     0,    46,
   123,     0,     4,   124,     0,    43,    72,    44,     0,     4,
     0,   125,     0,   124,   125,     0,    46,   123,     0,   127,
     0,   128,     0,     4,    53,     0,   127,    53,     0,   130,
     0,     4,     0,    76,     0,    97,     0,    54,   131,    54,
     0,    55,   131,    55,     0,    50,   130,    51,     0,    34,
   130,     0,    35,   130,     0,    33,   130,     0,    29,   130,
     0,    22,   130,     0,    23,   130,     0,    24,   130,     0,
    25,   130,     0,   130,    34,   130,     0,   130,    35,   130,
     0,   130,    36,   130,     0,   130,    37,   130,     0,   130,
    38,   130,     0,   130,    39,   130,     0,   130,    32,   130,
     0,   130,    31,   130,     0,   130,    30,   130,     0,   130,
     9,   130,     0,   130,    10,   130,     0,   130,    11,   130,
     0,   130,    27,   130,     0,   130,    28,   130,     0,   130,
    12,   130,     0,   130,    13,   130,     0,   130,    14,   130,
     0,   130,    15,   130,     0,   130,    16,   130,     0,   130,
    17,   130,     0,   130,    18,   130,     0,   130,    19,   130,
     0,   130,    20,   130,     0,   130,    21,   130,     0,   130,
    26,   130,     0,    71,     0,     4,     0,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   107,   117,   119,   119,   120,   122,   122,   124,   190,   190,
   191,   191,   192,   193,   193,   195,   224,   224,   225,   226,
   226,   227,   227,   229,   229,   233,   233,   235,   235,   236,
   236,   237,   237,   237,   241,   248,   249,   249,   250,   251,
   253,   254,   276,   277,   277,   281,   285,   287,   288,   289,
   306,   311,   313,   315,   316,   318,   324,   332,   338,   340,
   341,   343,   349,   350,   350,   354,   360,   363,   366,   379,
   381,   381,   382,   384,   385,   387,   388,   389,   390,   392,
   394,   396,   398,   400,   402,   406,   410,   414,   416,   417,
   419,   428,   428,   430,   430,   431,   432,   440,   445,   447,
   449,   450,   451,   453,   457,   466,   472,   477,   477,   478,
   483,   485,   487,   491,   503,   508,   510,   511,   512,   513,
   514,   516,   517,   518,   519,   520,   521,   522,   523,   525,
   526,   527,   528,   529,   530,   531,   532,   533,   534,   535,
   536,   537,   538,   539,   540,   541,   542,   543,   544,   545,
   546,   547,   548,   549,   552,   561,   566,   567
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EON","STRING",
"BOGUS","BAD_STRING_COMPARISON_OPERATOR","BAD_HEX_LITERAL","BAD_METHOD_DECL_START",
"\"&&\"","\"||\"","\"##\"","\"<=\"","\">=\"","\"==\"","\"!=\"","\"lt\"","\"gt\"",
"\"le\"","\"ge\"","\"eq\"","\"ne\"","\"def\"","\"in\"","\"-f\"","\"-d\"","\"is\"",
"'<'","'>'","'!'","'#'","'|'","'&'","'~'","'-'","'+'","'*'","'/'","'%'","'\\\\'",
"NEG","'@'","'\\n'","'['","']'","';'","'$'","'{'","'}'","'.'","'('","')'","'^'",
"':'","'\\\"'","'\\''","all","methods","one_big_piece","method","control_method",
"maybe_control_strings","control_strings","control_string","maybe_string","code_method",
"maybe_bracketed_strings","bracketed_maybe_strings","maybe_strings","strings",
"maybe_comment","maybe_codes","codes","code","action","get","get_value","get_name_value",
"name_in_curly_rdive","name_without_curly_rdive","name_without_curly_rdive_read",
"name_without_curly_rdive_class","name_without_curly_rdive_code","put","name_expr_wdive",
"name_expr_wdive_root","name_expr_wdive_write","name_expr_wdive_class","construct",
"construct_square","construct_round","construct_curly","any_constructor_code_value",
"constructor_code_value","constructor_code","codes__excluding_sole_str_literal",
"call","call_value","@1","@2","call_name","store_params","store_param","store_square_param",
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
    56,    56,    57,    57,    58,    59,    59,    60,    61,    61,
    62,    62,    63,    64,    64,    65,    66,    66,    67,    68,
    68,    69,    69,    70,    70,    71,    71,    72,    72,    73,
    73,    74,    74,    74,    75,    76,    77,    77,    78,    79,
    79,    80,    81,    82,    82,    83,    84,    84,    84,    85,
    86,    87,    88,    88,    88,    89,    90,    91,    92,    92,
    92,    93,    94,    95,    95,    96,    98,    99,    97,   100,
   101,   101,   102,   102,   102,   103,   104,   105,   106,   106,
   107,   107,   108,   108,   109,   110,   111,   112,   112,   112,
   113,   114,   114,   115,   115,   116,   117,   118,   118,   119,
   119,   119,   119,   120,   121,   122,   123,   124,   124,   125,
   126,   126,   127,   128,   129,   130,   130,   130,   130,   130,
   130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
   130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
   130,   130,   130,   130,   130,   130,   130,   130,   130,   130,
   130,   130,   130,   130,   131,   132,   133,   134
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
    23,   158,   121,   119,   120,   139,   140,   141,   144,   145,
   146,   147,   148,   149,   150,   151,   152,   153,   154,   142,
   143,   138,   137,   136,   130,   131,   132,   133,   134,   135,
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

static const short yypact[] = {     5,
-32768,    20,    75,-32768,   -34,-32768,-32768,-32768,-32768,-32768,
    18,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   -27,    31,    18,    27,    97,   102,-32768,-32768,    39,-32768,
-32768,-32768,    91,-32768,-32768,-32768,-32768,   103,-32768,    32,
-32768,    42,-32768,-32768,-32768,   103,    34,-32768,    97,-32768,
-32768,    89,    96,    52,-32768,    27,-32768,    56,-32768,    17,
-32768,-32768,    61,   103,    62,   103,    56,-32768,   102,    62,
-32768,    36,    18,     4,-32768,-32768,-32768,-32768,    25,-32768,
-32768,    42,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     6,-32768,    74,    78,-32768,    87,    94,-32768,   115,-32768,
-32768,-32768,-32768,-32768,-32768,    62,    88,    18,    41,   106,
-32768,-32768,-32768,-32768,   105,-32768,     4,     4,     4,     4,
     4,     4,     4,     4,    90,     4,    18,    18,-32768,-32768,
   101,   277,   104,-32768,-32768,-32768,-32768,   159,-32768,   123,
-32768,    18,-32768,-32768,   191,   191,   191,   191,   191,   198,
-32768,-32768,   234,-32768,   112,   113,-32768,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,     4,     4,     4,     4,     4,     4,     4,     4,
     4,     4,    37,    18,     4,    49,-32768,-32768,-32768,-32768,
-32768,    18,-32768,-32768,-32768,   367,   339,   308,   393,   393,
   143,   143,   393,   393,   393,   393,   143,   143,   191,   393,
   393,   243,   401,   198,   152,   152,-32768,-32768,-32768,-32768,
    70,-32768,    81,-32768,-32768,-32768,-32768,   -28,-32768,    53,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,    37,    18,-32768,
     4,-32768,-32768,-32768,-32768,   170,   171,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   167,-32768,-32768,-32768,   133,-32768,-32768,-32768,
   139,-32768,-32768,-32768,     3,   -18,    -7,   -71,-32768,     0,
-32768,-32768,    -6,-32768,-32768,    51,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   184,-32768,-32768,-32768,
     2,-32768,-32768,-32768,-32768,    71,-32768,-32768,-32768,-32768,
-32768,-32768,    21,    22,    19,-32768,-32768,   -12,    77,   -20,
-32768,   -32,     9,-32768,-32768,-32768,   211,-32768,   218,   280,
-32768,-32768,   210,    38,   172,-32768,   227,    14
};


#define	YYLAST		440


static const short yytable[] = {    15,
   109,    18,    10,    51,    60,    81,     2,   116,     1,    89,
    15,    42,    18,    68,    52,    53,   239,    80,    63,   240,
     1,     1,    15,    21,    18,   117,   118,   119,   120,    55,
    61,    81,   121,    85,    70,    55,   122,   123,   124,   107,
   221,    71,    87,    80,   -98,     2,    82,  -158,    80,   125,
     3,   234,    51,   126,    42,     4,     4,   127,   128,    15,
   104,    18,     3,     3,   108,    94,    98,   101,     4,     4,
    56,    15,    15,    18,    18,   115,    56,   106,    22,    38,
    83,     3,     3,    57,   -64,   -64,    86,     4,     4,   142,
   -97,   183,    89,    22,    53,   184,    84,   241,   185,    95,
    22,    56,    69,   242,   135,    67,    79,    15,   105,    18,
   -97,   109,   141,   -89,   -89,   136,    84,    23,   139,   -14,
    24,    25,    38,    26,   237,   238,    15,    15,    18,    18,
   137,   -60,    23,    72,    51,    24,    25,    73,   138,    23,
    74,    15,    24,    18,    23,    23,   183,    24,    24,   143,
   184,   157,   144,   185,   145,   146,   147,   148,   149,   150,
   151,   152,   191,   153,   192,   194,   109,   195,   171,   247,
   248,    50,   174,   175,   176,   108,   177,   178,   179,   180,
   181,   182,    15,    15,    18,    18,   227,   179,   180,   181,
   182,    15,   100,    18,   236,   196,   197,   198,   199,   200,
   201,   202,   203,   204,   205,   206,   207,   208,   209,   210,
   211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
   174,   175,   176,   134,   177,   178,   179,   180,   181,   182,
   108,   177,   178,   179,   180,   181,   182,    15,    15,    18,
    18,   227,   158,   159,   160,   161,   162,   163,   164,   165,
   166,   167,   168,   169,   170,   111,   235,   244,   243,   171,
   172,   173,   245,   174,   175,   176,   102,   177,   178,   179,
   180,   181,   182,   175,   176,   103,   177,   178,   179,   180,
   181,   182,    46,   131,   193,   158,   159,   160,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,   114,   156,
     0,     0,   171,   172,   173,     0,   174,   175,   176,     0,
   177,   178,   179,   180,   181,   182,   158,   159,     0,   161,
   162,   163,   164,   165,   166,   167,   168,   169,   170,     0,
     0,     0,     0,   171,   172,   173,     0,   174,   175,   176,
     0,   177,   178,   179,   180,   181,   182,   158,     0,     0,
   161,   162,   163,   164,   165,   166,   167,   168,   169,   170,
     0,     0,     0,     0,   171,   172,   173,     0,   174,   175,
   176,     0,   177,   178,   179,   180,   181,   182,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,     0,     0,
     0,     0,   171,   172,   173,     0,   174,   175,   176,     0,
   177,   178,   179,   180,   181,   182,   163,   164,     0,     0,
     0,     0,   169,   170,     0,     0,     0,     0,   171,     0,
     0,     0,   174,   175,   176,     0,   177,   178,   179,   180,
   181,   182,   176,     0,   177,   178,   179,   180,   181,   182
};

static const short yycheck[] = {     0,
    72,     0,     0,    11,    23,    38,    41,     4,     4,     4,
    11,     3,    11,    26,    42,    43,    45,    38,    25,    48,
     4,     4,    23,     4,    23,    22,    23,    24,    25,     5,
     4,    64,    29,    46,    26,     5,    33,    34,    35,     4,
     4,     3,    49,    64,     3,    41,    38,    42,    69,    46,
    46,     3,    60,    50,    46,    52,    52,    54,    55,    60,
    44,    60,    46,    46,    72,    52,    53,    54,    52,    52,
    46,    72,    73,    72,    73,    73,    46,    69,     4,     3,
    49,    46,    46,    53,    44,    45,    53,    52,    52,   108,
    49,    43,     4,     4,    43,    47,    46,    45,    50,     4,
     4,    46,    26,    51,    91,     4,     4,   108,    48,   108,
    49,   183,    99,    44,    45,    42,    66,    43,     4,    42,
    46,    47,    46,    49,    44,    45,   127,   128,   127,   128,
    44,    44,    43,    43,   142,    46,    47,    47,    45,    43,
    50,   142,    46,   142,    43,    43,    43,    46,    46,    44,
    47,    51,    48,    50,   117,   118,   119,   120,   121,   122,
   123,   124,     4,   126,    42,    54,   238,    55,    26,     0,
     0,     5,    30,    31,    32,   183,    34,    35,    36,    37,
    38,    39,   183,   184,   183,   184,   184,    36,    37,    38,
    39,   192,    54,   192,   192,   158,   159,   160,   161,   162,
   163,   164,   165,   166,   167,   168,   169,   170,   171,   172,
   173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
    30,    31,    32,    91,    34,    35,    36,    37,    38,    39,
   238,    34,    35,    36,    37,    38,    39,   238,   239,   238,
   239,   239,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    72,   186,   239,   238,    26,
    27,    28,   241,    30,    31,    32,    56,    34,    35,    36,
    37,    38,    39,    31,    32,    58,    34,    35,    36,    37,
    38,    39,     3,    74,    51,     9,    10,    11,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    72,   128,
    -1,    -1,    26,    27,    28,    -1,    30,    31,    32,    -1,
    34,    35,    36,    37,    38,    39,     9,    10,    -1,    12,
    13,    14,    15,    16,    17,    18,    19,    20,    21,    -1,
    -1,    -1,    -1,    26,    27,    28,    -1,    30,    31,    32,
    -1,    34,    35,    36,    37,    38,    39,     9,    -1,    -1,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    -1,    -1,    -1,    -1,    26,    27,    28,    -1,    30,    31,
    32,    -1,    34,    35,    36,    37,    38,    39,    12,    13,
    14,    15,    16,    17,    18,    19,    20,    21,    -1,    -1,
    -1,    -1,    26,    27,    28,    -1,    30,    31,    32,    -1,
    34,    35,    36,    37,    38,    39,    14,    15,    -1,    -1,
    -1,    -1,    20,    21,    -1,    -1,    -1,    -1,    26,    -1,
    -1,    -1,    30,    31,    32,    -1,    34,    35,    36,    37,
    38,    39,    32,    -1,    34,    35,    36,    37,    38,    39
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
#line 108 "compile.y"
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
#line 125 "compile.y"
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
			VClass *base=static_cast<VClass *>(
				PC.request->classes().get(base_name));
			if(!base) {
				strcpy(PC.error, base_name.cstr());
				strcat(PC.error, ": undefined class in @"BASE_NAME);
				YYERROR;
			}
			// @CLASS == @BASE sanity check
			if(PC.cclass==base) {
				strcpy(PC.error, "@"CLASS_NAME" equals @"BASE_NAME);
				YYERROR;
			}
			PC.cclass->set_base(*base);
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
#line 191 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 16:
#line 196 "compile.y"
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
#line 225 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 23:
#line 227 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 29:
#line 235 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 35:
#line 241 "compile.y"
{
	yyval=yyvsp[0]; /* stack: resulting value */ 
	changetail_or_append(yyval, 
		OP_GET_ELEMENT, false,  /*->*/OP_GET_ELEMENT__WRITE,
		/*or */OP_WRITE_VALUE
		); /* value=pop; wcontext.write(value) */
;
    break;}
case 36:
#line 248 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 39:
#line 250 "compile.y"
{ yyval=yyvsp[-1] ;
    break;}
case 42:
#line 254 "compile.y"
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
#line 276 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 45:
#line 277 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 46:
#line 281 "compile.y"
{
	yyval=yyvsp[-1]; /* stack: context,name */
	P(yyval, yyvsp[0]); /* stack: context,name,constructor_value */
;
    break;}
case 50:
#line 289 "compile.y"
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
#line 306 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_WITH_WRITE); /* stack: starting context */
	P(yyval, yyvsp[0]); /* diving code; stack: context,name */
;
    break;}
case 52:
#line 311 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 56:
#line 318 "compile.y"
{
	// stack: context, name
	yyval=yyvsp[-1]; // stack: context, name, value
	O(yyval, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 57:
#line 324 "compile.y"
{ 
	yyval=N(POOL); 
	O(yyval, OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(yyval, yyvsp[-1]); // stack: context, name, value
	O(yyval, OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 58:
#line 332 "compile.y"
{
	// stack: context, name
	yyval=N(POOL); 
	OA(yyval, OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;
    break;}
case 62:
#line 343 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 65:
#line 350 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 66:
#line 354 "compile.y"
{
	yyval=yyvsp[0]; /* stack: value */
	changetail_or_append(yyval, 
		OP_CALL, true,  /*->*/ OP_CALL__WRITE,
		/*or */OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;
    break;}
case 67:
#line 360 "compile.y"
{ 
					PC.in_call_value=true; 
			;
    break;}
case 68:
#line 363 "compile.y"
{
				PC.in_call_value=false;
			;
    break;}
case 69:
#line 366 "compile.y"
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
#line 381 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 76:
#line 387 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 77:
#line 388 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 78:
#line 389 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 80:
#line 392 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 82:
#line 396 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 84:
#line 400 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 85:
#line 402 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_STORE_PARAM);
;
    break;}
case 86:
#line 406 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_EXPR_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 87:
#line 410 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 91:
#line 419 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_PREPARE_TO_EXPRESSION);
	P(yyval, yyvsp[0]);
	O(yyval, OP_WRITE_EXPR_RESULT);
;
    break;}
case 93:
#line 428 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 95:
#line 430 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 97:
#line 432 "compile.y"
{
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 98:
#line 440 "compile.y"
{
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 104:
#line 453 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_GET_ELEMENT);
;
    break;}
case 105:
#line 457 "compile.y"
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
#line 466 "compile.y"
{
	yyval=N(POOL); 
	OA(yyval, OP_OBJECT_POOL, yyvsp[-1]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 107:
#line 472 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_WITH_READ);
	P(yyval, yyvsp[0]);
;
    break;}
case 109:
#line 477 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 110:
#line 478 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_GET_ELEMENT__WRITE);
;
    break;}
case 113:
#line 487 "compile.y"
{
	yyval=yyvsp[-1]; // stack: class name string
	O(yyval, OP_GET_CLASS);
;
    break;}
case 114:
#line 491 "compile.y"
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
#line 503 "compile.y"
{
	// see OP_PREPARE_TO_EXPRESSION!!
	if((yyval=yyvsp[0])->size()==2) // only one string literal in there?
		change_string_literal_to_double_literal(yyval); // make that string literal Double
;
    break;}
case 119:
#line 512 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 120:
#line 513 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 121:
#line 514 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 122:
#line 516 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_NEG) ;
    break;}
case 123:
#line 517 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 124:
#line 518 "compile.y"
{ yyval=yyvsp[0];	 O(yyval, OP_INV) ;
    break;}
case 125:
#line 519 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_NOT) ;
    break;}
case 126:
#line 520 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_DEF) ;
    break;}
case 127:
#line 521 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_IN) ;
    break;}
case 128:
#line 522 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_FEXISTS) ;
    break;}
case 129:
#line 523 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_DEXISTS) ;
    break;}
case 130:
#line 525 "compile.y"
{	yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_SUB) ;
    break;}
case 131:
#line 526 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_ADD) ;
    break;}
case 132:
#line 527 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_MUL) ;
    break;}
case 133:
#line 528 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_DIV) ;
    break;}
case 134:
#line 529 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_MOD) ;
    break;}
case 135:
#line 530 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_INTDIV) ;
    break;}
case 136:
#line 531 "compile.y"
{ yyval=yyvsp[-2]; 	P(yyval, yyvsp[0]);  O(yyval, OP_BIN_AND) ;
    break;}
case 137:
#line 532 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_BIN_OR) ;
    break;}
case 138:
#line 533 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_BIN_XOR) ;
    break;}
case 139:
#line 534 "compile.y"
{ yyval=yyvsp[-2];  OA(yyval, OP_NESTED_CODE, yyvsp[0]);  O(yyval, OP_LOG_AND) ;
    break;}
case 140:
#line 535 "compile.y"
{ yyval=yyvsp[-2];  OA(yyval, OP_NESTED_CODE, yyvsp[0]);  O(yyval, OP_LOG_OR) ;
    break;}
case 141:
#line 536 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_LOG_XOR) ;
    break;}
case 142:
#line 537 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_LT) ;
    break;}
case 143:
#line 538 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_GT) ;
    break;}
case 144:
#line 539 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_LE) ;
    break;}
case 145:
#line 540 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_GE) ;
    break;}
case 146:
#line 541 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_EQ) ;
    break;}
case 147:
#line 542 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_NE) ;
    break;}
case 148:
#line 543 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_LT) ;
    break;}
case 149:
#line 544 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_GT) ;
    break;}
case 150:
#line 545 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_LE) ;
    break;}
case 151:
#line 546 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_GE) ;
    break;}
case 152:
#line 547 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_EQ) ;
    break;}
case 153:
#line 548 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_NE) ;
    break;}
case 154:
#line 549 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_IS) ;
    break;}
case 155:
#line 552 "compile.y"
{
	yyval=N(POOL);
	OA(yyval, OP_STRING_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;
    break;}
case 156:
#line 561 "compile.y"
{
	// optimized from OP_STRING+OP_WRITE_VALUE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal(yyval=yyvsp[0])
;
    break;}
case 157:
#line 566 "compile.y"
{ yyval=VL(NEW VVoid(POOL)) ;
    break;}
case 158:
#line 567 "compile.y"
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
#line 569 "compile.y"

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
				default:
					result=c;
					goto break2;
				}
				goto break2;
			case '+': case '*': case '/': case '%': case '\\':
			case '~':
			case ';':
				RC;
			case '&': case '|':  case '#':
				if(*PC.source==c) { // && ||
					result=c=='#'?LXOR:c=='&'?LAND:LOR;
					skip_analized=1;
				} else
					result=c;
				goto break2;
			case '<': case '>': case '=': case '!': 
				if(*PC.source=='=') { // <= >= == !=
					skip_analized=1;
					switch(c) {
					case '<': result=NLE; break;
					case '>': result=NGE; break;
					case '=': result=NEQ; break;
					case '!': result=NNE; break;
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
			case ',': case '?':
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

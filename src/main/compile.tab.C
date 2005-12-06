
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
#define	LITERAL_TRUE	286
#define	LITERAL_FALSE	287
#define	NUNARY	288

#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001-2005 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: compile.tab.C,v 1.109 2005/12/06 08:19:12 paf Exp $
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

static const VBool vfalse(false);
static const VBool vtrue(true);
static const VVoid vvoid;

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



#define	YYFINAL		263
#define	YYFLAG		-32768
#define	YYNTBASE	62

#define YYTRANSLATE(x) ((unsigned)(x) <= 288 ? yytranslate[x] : 150)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    48,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    46,    60,     2,    52,    43,    37,    61,    56,
    57,    40,    38,     2,    39,    55,    41,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    59,    51,    34,
     2,    35,     2,    47,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    49,    42,    50,    58,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    53,    36,    54,    45,     2,     2,     2,     2,
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
    27,    28,    29,    30,    31,    32,    33,    44
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     6,     9,    11,    13,    15,    20,    22,
    24,    26,    29,    32,    34,    36,    37,    46,    48,    50,
    54,    56,    58,    60,    64,    66,    68,    70,    72,    74,
    77,    79,    81,    83,    85,    87,    89,    92,    95,    97,
   101,   103,   105,   107,   110,   112,   115,   119,   121,   123,
   125,   127,   130,   133,   135,   137,   139,   140,   141,   147,
   151,   155,   157,   159,   161,   163,   165,   167,   170,   172,
   173,   174,   181,   183,   185,   188,   190,   192,   194,   195,
   196,   202,   206,   210,   212,   216,   218,   222,   224,   228,
   230,   232,   234,   236,   238,   240,   242,   245,   247,   250,
   253,   255,   257,   260,   262,   264,   266,   268,   271,   274,
   275,   276,   282,   284,   286,   289,   292,   294,   296,   299,
   302,   304,   306,   308,   310,   312,   314,   318,   322,   326,
   329,   332,   335,   338,   341,   344,   347,   350,   354,   358,
   362,   366,   370,   374,   378,   382,   386,   390,   394,   398,
   402,   406,   410,   414,   418,   422,   426,   430,   434,   438,
   442,   446,   450,   454,   458,   460,   462,   464,   465,   467,
   469
};

static const short yyrhs[] = {    64,
     0,    63,     0,    65,     0,    63,    65,     0,    78,     0,
    66,     0,    71,     0,    47,     4,    48,    67,     0,   149,
     0,    68,     0,    69,     0,    68,    69,     0,    70,    48,
     0,   149,     0,     4,     0,     0,    47,     4,    74,    73,
    77,    48,    72,    78,     0,   149,     0,    74,     0,    49,
    75,    50,     0,   149,     0,    76,     0,     4,     0,    76,
    51,     4,     0,   149,     0,     4,     0,   149,     0,    79,
     0,    80,     0,    79,    80,     0,   145,     0,    81,     0,
    82,     0,    90,     0,   105,     0,    83,     0,    52,    84,
     0,    86,     3,     0,    85,     0,    53,    86,    54,     0,
    87,     0,    88,     0,    89,     0,   138,    89,     0,   128,
     0,   125,   128,     0,    52,    91,    95,     0,    92,     0,
    93,     0,    94,     0,   124,     0,    55,   124,     0,   138,
   124,     0,    96,     0,    99,     0,   100,     0,     0,     0,
    49,    97,   101,    98,    50,     0,    56,   141,    57,     0,
    53,    78,    54,     0,   146,     0,     4,     0,   102,     0,
   103,     0,   104,     0,    81,     0,    80,    79,     0,   106,
     0,     0,     0,    58,   107,   109,   108,   110,     3,     0,
    86,     0,   111,     0,   110,   111,     0,   112,     0,   115,
     0,   116,     0,     0,     0,    49,   113,   117,   114,    50,
     0,    56,   118,    57,     0,    53,   119,    54,     0,   120,
     0,   117,    51,   120,     0,   121,     0,   118,    51,   121,
     0,   122,     0,   119,    51,   122,     0,   123,     0,   141,
     0,    78,     0,   146,     0,     4,     0,   102,     0,   129,
     0,   125,   129,     0,   126,     0,   125,   126,     0,   127,
    55,     0,   129,     0,   129,     0,     4,     5,     0,     4,
     0,   130,     0,   131,     0,   132,     0,    52,   135,     0,
     4,   136,     0,     0,     0,    49,   133,    79,   134,    50,
     0,     4,     0,   137,     0,   136,   137,     0,    52,   135,
     0,   139,     0,   140,     0,     4,    59,     0,   139,    59,
     0,   142,     0,   143,     0,   147,     0,   148,     0,    83,
     0,   106,     0,    60,   144,    60,     0,    61,   144,    61,
     0,    56,   142,    57,     0,    39,   142,     0,    38,   142,
     0,    45,   142,     0,    46,   142,     0,    27,   142,     0,
    28,   142,     0,    29,   142,     0,    30,   142,     0,   142,
    39,   142,     0,   142,    38,   142,     0,   142,    40,   142,
     0,   142,    41,   142,     0,   142,    43,   142,     0,   142,
    42,   142,     0,   142,    19,   142,     0,   142,    20,   142,
     0,   142,    37,   142,     0,   142,    36,   142,     0,   142,
    14,   142,     0,   142,    11,   142,     0,   142,    12,   142,
     0,   142,    13,   142,     0,   142,    34,   142,     0,   142,
    35,   142,     0,   142,    15,   142,     0,   142,    16,   142,
     0,   142,    17,   142,     0,   142,    18,   142,     0,   142,
    21,   142,     0,   142,    22,   142,     0,   142,    23,   142,
     0,   142,    24,   142,     0,   142,    25,   142,     0,   142,
    26,   142,     0,   142,    31,   142,     0,     4,     0,    78,
     0,     4,     0,     0,    32,     0,    33,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   127,   135,   137,   137,   138,   140,   140,   142,   214,   214,
   215,   215,   216,   217,   217,   219,   254,   259,   259,   260,
   261,   261,   262,   262,   264,   264,   268,   268,   270,   270,
   271,   271,   272,   272,   272,   276,   283,   284,   284,   285,
   286,   288,   289,   311,   312,   312,   316,   320,   322,   323,
   324,   341,   346,   348,   350,   351,   353,   356,   358,   364,
   372,   378,   380,   381,   383,   389,   390,   390,   394,   400,
   403,   406,   419,   421,   421,   422,   424,   425,   427,   430,
   432,   433,   434,   435,   437,   439,   441,   443,   445,   447,
   451,   466,   470,   472,   473,   478,   478,   480,   480,   481,
   482,   490,   495,   497,   499,   500,   501,   503,   507,   516,
   519,   521,   527,   532,   532,   533,   538,   540,   542,   554,
   566,   567,   569,   570,   571,   572,   573,   574,   575,   577,
   578,   579,   580,   581,   582,   583,   584,   586,   587,   588,
   589,   590,   591,   592,   593,   594,   595,   596,   597,   598,
   599,   600,   601,   602,   603,   604,   605,   606,   607,   608,
   609,   610,   611,   612,   615,   620,   629,   634,   635,   636,
   638
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EON","STRING",
"BOGUS","BAD_STRING_COMPARISON_OPERATOR","BAD_HEX_LITERAL","BAD_METHOD_DECL_START",
"BAD_METHOD_PARAMETER_NAME_CHARACTER","BAD_NONWHITESPACE_CHARACTER_IN_EXPLICIT_RESULT_MODE",
"\"&&\"","\"||\"","\"!||\"","\"!|\"","\"<=\"","\">=\"","\"==\"","\"!=\"","\"<<\"",
"\">>\"","\"lt\"","\"gt\"","\"le\"","\"ge\"","\"eq\"","\"ne\"","\"def\"","\"in\"",
"\"-f\"","\"-d\"","\"is\"","\"true\"","\"false\"","'<'","'>'","'|'","'&'","'+'",
"'-'","'*'","'/'","'\\\\'","'%'","NUNARY","'~'","'!'","'@'","'\\n'","'['","']'",
"';'","'$'","'{'","'}'","'.'","'('","')'","'^'","':'","'\\\"'","'\\''","all",
"methods","one_big_piece","method","control_method","maybe_control_strings",
"control_strings","control_string","maybe_string","code_method","@1","maybe_bracketed_strings",
"bracketed_maybe_strings","maybe_strings","strings","maybe_comment","maybe_codes",
"codes","code","action","get","get_value","get_name_value","name_in_curly_rdive",
"name_without_curly_rdive","name_without_curly_rdive_read","name_without_curly_rdive_class",
"name_without_curly_rdive_code","put","name_expr_wdive","name_expr_wdive_root",
"name_expr_wdive_write","name_expr_wdive_class","construct","construct_square",
"@2","@3","construct_round","construct_curly","any_constructor_code_value","constructor_code_value",
"constructor_code","codes__excluding_sole_str_literal","call","call_value","@4",
"@5","call_name","store_params","store_param","store_square_param","@6","@7",
"store_round_param","store_curly_param","store_code_param_parts","store_expr_param_parts",
"store_curly_param_parts","store_code_param_part","store_expr_param_part","store_curly_param_part",
"code_param_value","name_expr_dive_code","name_path","name_step","name_advance1",
"name_advance2","name_expr_value","name_expr_subvar_value","name_expr_with_subvar_value",
"name_square_code_value","@8","@9","subvar_ref_name_rdive","subvar_get_writes",
"subvar__get_write","class_prefix","class_static_prefix","class_constructor_prefix",
"expr_value","expr","double_or_STRING","string_inside_quotes_value","write_string",
"void_value","true_value","false_value","empty", NULL
};
#endif

static const short yyr1[] = {     0,
    62,    62,    63,    63,    64,    65,    65,    66,    67,    67,
    68,    68,    69,    70,    70,    72,    71,    73,    73,    74,
    75,    75,    76,    76,    77,    77,    78,    78,    79,    79,
    80,    80,    81,    81,    81,    82,    83,    84,    84,    85,
    86,    86,    87,    88,    89,    89,    90,    91,    91,    91,
    92,    93,    94,    95,    95,    95,    97,    98,    96,    99,
   100,   101,   101,   101,   102,   103,   104,   104,   105,   107,
   108,   106,   109,   110,   110,   111,   111,   111,   113,   114,
   112,   115,   116,   117,   117,   118,   118,   119,   119,   120,
   121,   122,   123,   123,   123,   124,   124,   125,   125,   126,
   127,   128,   128,   129,   129,   129,   129,   130,   131,   133,
   134,   132,   135,   136,   136,   137,   138,   138,   139,   140,
   141,   142,   142,   142,   142,   142,   142,   142,   142,   142,
   142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
   142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
   142,   142,   142,   142,   142,   142,   142,   142,   142,   142,
   142,   142,   142,   142,   143,   144,   145,   146,   147,   148,
   149
};

static const short yyr2[] = {     0,
     1,     1,     1,     2,     1,     1,     1,     4,     1,     1,
     1,     2,     2,     1,     1,     0,     8,     1,     1,     3,
     1,     1,     1,     3,     1,     1,     1,     1,     1,     2,
     1,     1,     1,     1,     1,     1,     2,     2,     1,     3,
     1,     1,     1,     2,     1,     2,     3,     1,     1,     1,
     1,     2,     2,     1,     1,     1,     0,     0,     5,     3,
     3,     1,     1,     1,     1,     1,     1,     2,     1,     0,
     0,     6,     1,     1,     2,     1,     1,     1,     0,     0,
     5,     3,     3,     1,     3,     1,     3,     1,     3,     1,
     1,     1,     1,     1,     1,     1,     2,     1,     2,     2,
     1,     1,     2,     1,     1,     1,     1,     2,     2,     0,
     0,     5,     1,     1,     2,     2,     1,     1,     2,     2,
     1,     1,     1,     1,     1,     1,     3,     3,     3,     2,
     2,     2,     2,     2,     2,     2,     2,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     1,     1,     1,     0,     1,     1,
     0
};

static const short yydefact[] = {   171,
   167,     0,     0,    70,     2,     1,     3,     6,     7,     5,
    28,    29,    32,    33,    36,    34,    35,    69,    31,    27,
     0,   104,   110,     0,     0,     0,    37,    39,     0,    41,
    42,    43,     0,    48,    49,    50,    51,     0,    98,     0,
    45,    96,   105,   106,   107,     0,   117,   118,     0,     4,
    30,   171,   171,   171,   103,     0,   119,   109,   114,     0,
   113,   108,     0,     0,   102,     0,   104,    52,     0,    96,
    38,    57,   171,     0,    47,    54,    55,    56,   104,    99,
    46,    97,   100,    44,    53,   120,    73,    71,    15,     8,
    10,    11,     0,     9,    23,     0,    22,    21,   171,    19,
    18,   116,   115,   111,    40,    97,   168,     0,   165,     0,
     0,     0,     0,   169,   170,     0,     0,     0,     0,     0,
     0,   171,   171,   125,   126,     0,   121,   122,   123,   124,
     0,    12,    14,    13,    20,     0,    26,     0,    25,     0,
   167,     0,    32,    58,    64,    65,    66,    62,    61,   134,
   135,   136,   137,   131,   130,   132,   133,     0,   166,     0,
     0,    60,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    79,
   171,     0,     0,    74,    76,    77,    78,    24,    16,   112,
    68,     0,   129,   127,   128,   149,   150,   151,   148,   154,
   155,   156,   157,   144,   145,   158,   159,   160,   161,   162,
   163,   164,   152,   153,   147,   146,   139,   138,   140,   141,
   143,   142,   168,    92,     0,    88,     0,    86,    91,    72,
    75,   171,    59,   167,    95,    80,    84,    90,    93,   171,
    83,     0,    82,    17,   168,     0,    89,    87,    85,    81,
     0,     0,     0
};

static const short yydefgoto[] = {   261,
     5,     6,     7,     8,    90,    91,    92,    93,     9,   242,
    99,    54,    96,    97,   138,   159,    11,    12,    13,    14,
   124,    27,    28,    29,    30,    31,    32,    16,    33,    34,
    35,    36,    75,    76,   107,   202,    77,    78,   144,   245,
   146,   147,    17,   125,    49,   131,    88,   193,   194,   195,
   233,   256,   196,   197,   246,   237,   235,   247,   238,   236,
   248,    37,    64,    39,    40,    41,    65,    43,    44,    45,
    60,   140,    62,    58,    59,    66,    47,    48,   239,   127,
   128,   160,    19,   249,   129,   130,    20
};

static const short yypact[] = {     6,
-32768,    23,    50,-32768,   -17,-32768,-32768,-32768,-32768,-32768,
    18,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
   -31,    19,-32768,    43,    42,    44,-32768,-32768,    52,-32768,
-32768,-32768,   -33,-32768,-32768,-32768,-32768,    65,-32768,    22,
-32768,     4,-32768,-32768,-32768,    65,    24,-32768,    42,-32768,
-32768,    97,   104,    64,-32768,    43,-32768,    78,-32768,    18,
-32768,-32768,    77,    65,    88,    65,    78,-32768,    44,    88,
-32768,-32768,    18,     5,-32768,-32768,-32768,-32768,    16,-32768,
-32768,     4,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
     8,-32768,    85,    86,-32768,   100,   117,-32768,   166,-32768,
-32768,-32768,-32768,    18,-32768,    88,    27,   119,-32768,     5,
     5,     5,     5,-32768,-32768,     5,     5,     5,     5,    63,
     5,    18,    18,-32768,-32768,   114,   274,-32768,-32768,-32768,
    79,-32768,-32768,-32768,-32768,   170,-32768,   137,-32768,   136,
   138,    18,    60,-32768,-32768,-32768,-32768,-32768,-32768,   443,
   443,   443,   443,-32768,-32768,-32768,-32768,   141,-32768,   127,
   128,-32768,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,     5,
     5,     5,     5,     5,     5,     5,     5,     5,     5,-32768,
    18,     5,    39,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    18,   140,-32768,-32768,-32768,   370,   340,   307,   428,   400,
   400,   413,   413,    98,    98,   400,   400,   400,   400,   413,
   413,   443,   400,   400,   180,   453,   105,   105,-32768,-32768,
-32768,-32768,    32,-32768,    35,-32768,   -32,-32768,-32768,-32768,
-32768,    18,-32768,    70,-32768,   144,-32768,-32768,-32768,    18,
-32768,     5,-32768,-32768,    32,   146,-32768,-32768,-32768,-32768,
   197,   202,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   199,-32768,-32768,-32768,   118,-32768,-32768,-32768,
-32768,   156,-32768,-32768,-32768,     1,   -55,    -7,  -104,-32768,
     0,-32768,-32768,   -10,-32768,-32768,   -38,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,   108,
-32768,-32768,-32768,     2,-32768,-32768,-32768,-32768,    31,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,   -30,   -25,   -22,
-32768,    72,   123,   -24,-32768,   -12,     3,-32768,-32768,-32768,
-32768,-32768,   173,-32768,   172,   228,-32768,-32768,   158,    95,
-32768,   111,-32768,   129,-32768,-32768,    28
};


#define	YYLAST		496


static const short yytable[] = {    15,
    10,    18,   143,    51,   104,    42,  -102,    84,   109,     1,
    15,    89,    18,    80,    63,    72,    52,    53,   252,    73,
    55,     1,    74,    55,   253,    81,    21,    84,    70,     2,
   141,   110,   111,   112,   113,   244,   114,   115,    87,    80,
    82,   240,   116,   117,    80,    22,    61,    67,    42,   118,
   119,    81,     2,    22,    71,  -171,   120,     3,  -101,    15,
   121,    18,     4,     4,   122,   123,    22,    56,    79,     3,
    56,   106,    15,   108,    18,     4,    83,    57,     3,    94,
    98,   101,    86,     3,     4,   250,   201,   190,   251,     4,
    23,   191,    23,    24,   192,    24,    51,    68,    23,   142,
    89,    24,    25,    15,    26,    18,    15,    95,    18,   -67,
   -67,    23,    53,    23,    24,    25,    24,    85,   133,   -94,
   -94,    15,    15,    18,    18,    38,   139,   190,   143,    56,
   105,   191,   134,   -14,   192,   184,   185,   186,   187,   188,
   189,    15,  -101,    18,   186,   187,   188,   189,    69,   135,
   143,   163,   164,   165,   166,   167,   168,   169,   170,   171,
   172,   173,   174,   175,   176,   177,   178,   136,    38,   137,
   162,   179,   149,   198,   180,   181,   182,   183,   184,   185,
   186,   187,   188,   189,   199,   200,   204,   -63,   205,   243,
    15,   234,    18,    51,   255,   260,   262,   203,   171,   172,
    15,   263,    18,    50,   150,   151,   152,   153,   132,   100,
   154,   155,   156,   157,   145,   158,   183,   184,   185,   186,
   187,   188,   189,   241,   259,   142,   258,   257,   102,   103,
    46,   126,    15,   161,    18,   148,     0,     0,     0,     0,
     0,    15,   254,    18,     0,     0,     0,   142,     0,    15,
   234,    18,     0,     0,    15,     0,    18,   206,   207,   208,
   209,   210,   211,   212,   213,   214,   215,   216,   217,   218,
   219,   220,   221,   222,   223,   224,   225,   226,   227,   228,
   229,   230,   231,   232,   163,   164,   165,   166,   167,   168,
   169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     0,     0,     0,     0,   179,     0,     0,   180,   181,   182,
   183,   184,   185,   186,   187,   188,   189,   163,   164,     0,
   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   176,   177,   178,     0,     0,     0,     0,   179,     0,     0,
   180,   181,   182,   183,   184,   185,   186,   187,   188,   189,
   163,     0,     0,   166,   167,   168,   169,   170,   171,   172,
   173,   174,   175,   176,   177,   178,     0,     0,     0,     0,
   179,     0,     0,   180,   181,   182,   183,   184,   185,   186,
   187,   188,   189,   166,   167,   168,   169,   170,   171,   172,
   173,   174,   175,   176,   177,   178,     0,     0,     0,     0,
   179,     0,     0,   180,   181,   182,   183,   184,   185,   186,
   187,   188,   189,   166,     0,     0,   169,   170,   171,   172,
     0,     0,     0,     0,   177,   178,   166,     0,     0,     0,
   179,   171,   172,     0,     0,   182,   183,   184,   185,   186,
   187,   188,   189,   179,     0,     0,   171,   172,   182,   183,
   184,   185,   186,   187,   188,   189,   166,     0,     0,     0,
     0,   171,   172,   182,   183,   184,   185,   186,   187,   188,
   189,   171,   172,     0,     0,     0,     0,     0,   182,   183,
   184,   185,   186,   187,   188,   189,     0,     0,     0,     0,
   184,   185,   186,   187,   188,   189
};

static const short yycheck[] = {     0,
     0,     0,   107,    11,    60,     3,     3,    46,     4,     4,
    11,     4,    11,    38,    25,    49,    48,    49,    51,    53,
     5,     4,    56,     5,    57,    38,     4,    66,    26,    47,
     4,    27,    28,    29,    30,     4,    32,    33,    49,    64,
    38,     3,    38,    39,    69,     4,     4,     4,    46,    45,
    46,    64,    47,     4,     3,    48,    52,    52,    55,    60,
    56,    60,    58,    58,    60,    61,     4,    52,     4,    52,
    52,    69,    73,    73,    73,    58,    55,    59,    52,    52,
    53,    54,    59,    52,    58,    51,   142,    49,    54,    58,
    49,    53,    49,    52,    56,    52,   104,    26,    49,   107,
     4,    52,    53,   104,    55,   104,   107,     4,   107,    50,
    51,    49,    49,    49,    52,    53,    52,    46,    91,    50,
    51,   122,   123,   122,   123,     3,    99,    49,   233,    52,
    54,    53,    48,    48,    56,    38,    39,    40,    41,    42,
    43,   142,    55,   142,    40,    41,    42,    43,    26,    50,
   255,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    21,    22,    23,    24,    25,    26,    51,    46,     4,
    57,    31,    54,     4,    34,    35,    36,    37,    38,    39,
    40,    41,    42,    43,    48,    50,    60,    50,    61,    50,
   191,   191,   191,   201,    51,    50,     0,    57,    19,    20,
   201,     0,   201,     5,   110,   111,   112,   113,    91,    54,
   116,   117,   118,   119,   107,   121,    37,    38,    39,    40,
    41,    42,    43,   193,   255,   233,   252,   250,    56,    58,
     3,    74,   233,   123,   233,   107,    -1,    -1,    -1,    -1,
    -1,   242,   242,   242,    -1,    -1,    -1,   255,    -1,   250,
   250,   250,    -1,    -1,   255,    -1,   255,   163,   164,   165,
   166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
   176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
   186,   187,   188,   189,    11,    12,    13,    14,    15,    16,
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
    21,    22,    23,    24,    25,    26,    -1,    -1,    -1,    -1,
    31,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
    41,    42,    43,    14,    -1,    -1,    17,    18,    19,    20,
    -1,    -1,    -1,    -1,    25,    26,    14,    -1,    -1,    -1,
    31,    19,    20,    -1,    -1,    36,    37,    38,    39,    40,
    41,    42,    43,    31,    -1,    -1,    19,    20,    36,    37,
    38,    39,    40,    41,    42,    43,    14,    -1,    -1,    -1,
    -1,    19,    20,    36,    37,    38,    39,    40,    41,    42,
    43,    19,    20,    -1,    -1,    -1,    -1,    -1,    36,    37,
    38,    39,    40,    41,    42,    43,    -1,    -1,    -1,    -1,
    38,    39,    40,    41,    42,    43
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
#line 128 "compile.y"
{
	Method& method=*new Method(Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		yyvsp[0]/*parser_code*/, 0/*native_code*/);
	PC.cclass->add_method(PC.alias_method(main_method_name), method);
;
    break;}
case 8:
#line 143 "compile.y"
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
#line 215 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 16:
#line 219 "compile.y"
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
#line 254 "compile.y"
{
	// fill in the code
	reinterpret_cast<Method*>(yyvsp[-1])->parser_code=yyvsp[0];
;
    break;}
case 20:
#line 260 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 24:
#line 262 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 30:
#line 270 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 36:
#line 276 "compile.y"
{
	yyval=yyvsp[0]; /* stack: resulting value */ 
	changetail_or_append(*yyval, 
		OP_GET_ELEMENT, false,  /*->*/OP_GET_ELEMENT__WRITE,
		/*or */OP_WRITE_VALUE
		); /* value=pop; wcontext.write(value) */
;
    break;}
case 37:
#line 283 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 40:
#line 285 "compile.y"
{ yyval=yyvsp[-1] ;
    break;}
case 43:
#line 289 "compile.y"
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
#line 311 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 46:
#line 312 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 47:
#line 316 "compile.y"
{
	yyval=yyvsp[-1]; /* stack: context,name */
	P(*yyval, *yyvsp[0]); /* stack: context,name,constructor_value */
;
    break;}
case 51:
#line 324 "compile.y"
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
#line 341 "compile.y"
{
	yyval=N(); 
	O(*yyval, OP_WITH_WRITE); /* stack: starting context */
	P(*yyval, *yyvsp[0]); /* diving code; stack: context,name */
;
    break;}
case 53:
#line 346 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 57:
#line 353 "compile.y"
{
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;
    break;}
case 58:
#line 356 "compile.y"
{
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;
    break;}
case 59:
#line 358 "compile.y"
{
	// stack: context, name
	yyval=yyvsp[-2]; // stack: context, name, value
	O(*yyval, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 60:
#line 364 "compile.y"
{ 
	yyval=N(); 
	O(*yyval, OP_PREPARE_TO_EXPRESSION);
	// stack: context, name
	P(*yyval, *yyvsp[-1]); // stack: context, name, value
	O(*yyval, OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 61:
#line 372 "compile.y"
{
	// stack: context, name
	yyval=N(); 
	OA(*yyval, OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;
    break;}
case 65:
#line 383 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 68:
#line 390 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 69:
#line 394 "compile.y"
{
	yyval=yyvsp[0]; /* stack: value */
	changetail_or_append(*yyval, 
		OP_CALL, true,  /*->*/ OP_CALL__WRITE,
		/*or */OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;
    break;}
case 70:
#line 400 "compile.y"
{ 
					PC.in_call_value=true; 
			;
    break;}
case 71:
#line 403 "compile.y"
{
				PC.in_call_value=false;
			;
    break;}
case 72:
#line 406 "compile.y"
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
case 75:
#line 421 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 79:
#line 427 "compile.y"
{
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;
    break;}
case 80:
#line 430 "compile.y"
{
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;
    break;}
case 81:
#line 432 "compile.y"
{yyval=yyvsp[-2];
    break;}
case 82:
#line 433 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 83:
#line 434 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 85:
#line 437 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 87:
#line 441 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 89:
#line 445 "compile.y"
{ yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]) ;
    break;}
case 90:
#line 447 "compile.y"
{
	yyval=yyvsp[0];
	O(*yyval, OP_STORE_PARAM);
;
    break;}
case 91:
#line 451 "compile.y"
{
	YYSTYPE expr_code=yyvsp[0];
	if(expr_code->count()==3
		&& (*expr_code)[0].code==OP_VALUE) { // optimizing (double/bool/incidently 'string' too) case. [OP_VALUE+origin+Double]
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
;
    break;}
case 92:
#line 466 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
;
    break;}
case 97:
#line 478 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 99:
#line 480 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 101:
#line 482 "compile.y"
{
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 102:
#line 490 "compile.y"
{
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(*yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 108:
#line 503 "compile.y"
{
	yyval=yyvsp[0];
	O(*yyval, OP_GET_ELEMENT);
;
    break;}
case 109:
#line 507 "compile.y"
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
case 110:
#line 516 "compile.y"
{
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
;
    break;}
case 111:
#line 519 "compile.y"
{
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
;
    break;}
case 112:
#line 521 "compile.y"
{
	yyval=N(); 
	OA(*yyval, OP_OBJECT_POOL, yyvsp[-2]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
;
    break;}
case 113:
#line 527 "compile.y"
{
	yyval=N(); 
	O(*yyval, OP_WITH_READ);
	P(*yyval, *yyvsp[0]);
;
    break;}
case 115:
#line 532 "compile.y"
{ yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]) ;
    break;}
case 116:
#line 533 "compile.y"
{
	yyval=yyvsp[0];
	O(*yyval, OP_GET_ELEMENT__WRITE);
;
    break;}
case 119:
#line 542 "compile.y"
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
case 120:
#line 554 "compile.y"
{
	yyval=yyvsp[-1];
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(*yyval, OP_PREPARE_TO_CONSTRUCT_OBJECT);
;
    break;}
case 127:
#line 573 "compile.y"
{ yyval = yyvsp[-1] ;
    break;}
case 128:
#line 574 "compile.y"
{ yyval = yyvsp[-1] ;
    break;}
case 129:
#line 575 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 130:
#line 577 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_NEG) ;
    break;}
case 131:
#line 578 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 132:
#line 579 "compile.y"
{ yyval=yyvsp[0];	 O(*yyval, OP_INV) ;
    break;}
case 133:
#line 580 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_NOT) ;
    break;}
case 134:
#line 581 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_DEF) ;
    break;}
case 135:
#line 582 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_IN) ;
    break;}
case 136:
#line 583 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_FEXISTS) ;
    break;}
case 137:
#line 584 "compile.y"
{ yyval=yyvsp[0];  O(*yyval, OP_DEXISTS) ;
    break;}
case 138:
#line 586 "compile.y"
{	yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_SUB) ;
    break;}
case 139:
#line 587 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_ADD) ;
    break;}
case 140:
#line 588 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_MUL) ;
    break;}
case 141:
#line 589 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_DIV) ;
    break;}
case 142:
#line 590 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_MOD) ;
    break;}
case 143:
#line 591 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_INTDIV) ;
    break;}
case 144:
#line 592 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_SL) ;
    break;}
case 145:
#line 593 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_SR) ;
    break;}
case 146:
#line 594 "compile.y"
{ yyval=yyvsp[-2]; 	P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_AND) ;
    break;}
case 147:
#line 595 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_OR) ;
    break;}
case 148:
#line 596 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_BIN_XOR) ;
    break;}
case 149:
#line 597 "compile.y"
{ yyval=yyvsp[-2];  OA(*yyval, OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP_LOG_AND) ;
    break;}
case 150:
#line 598 "compile.y"
{ yyval=yyvsp[-2];  OA(*yyval, OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP_LOG_OR) ;
    break;}
case 151:
#line 599 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_LOG_XOR) ;
    break;}
case 152:
#line 600 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_LT) ;
    break;}
case 153:
#line 601 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_GT) ;
    break;}
case 154:
#line 602 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_LE) ;
    break;}
case 155:
#line 603 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_GE) ;
    break;}
case 156:
#line 604 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_EQ) ;
    break;}
case 157:
#line 605 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_NUM_NE) ;
    break;}
case 158:
#line 606 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_LT) ;
    break;}
case 159:
#line 607 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_GT) ;
    break;}
case 160:
#line 608 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_LE) ;
    break;}
case 161:
#line 609 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_GE) ;
    break;}
case 162:
#line 610 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_EQ) ;
    break;}
case 163:
#line 611 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_STR_NE) ;
    break;}
case 164:
#line 612 "compile.y"
{ yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP_IS) ;
    break;}
case 165:
#line 615 "compile.y"
{
	// optimized from OP_STRING->OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*(yyval=yyvsp[0]));
;
    break;}
case 166:
#line 620 "compile.y"
{
	yyval=N();
	OA(*yyval, OP_STRING_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
;
    break;}
case 167:
#line 629 "compile.y"
{
	// optimized from OP_STRING+OP_WRITE_VALUE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*(yyval=yyvsp[0]))
;
    break;}
case 168:
#line 634 "compile.y"
{ yyval=VL(/*we know that we will not change it*/const_cast<VVoid*>(&vvoid), 0, 0, 0) ;
    break;}
case 169:
#line 635 "compile.y"
{ yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vtrue), 0, 0, 0) ;
    break;}
case 170:
#line 636 "compile.y"
{ yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vfalse), 0, 0, 0) ;
    break;}
case 171:
#line 638 "compile.y"
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
#line 640 "compile.y"

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
			case 't':
				if(end==begin) // right after whitespace
					if(pc.source[0]=='r' && pc.source[1]=='u' && pc.source[2]=='e') { // def
						skip_analized=3;
						result=LITERAL_TRUE;
						goto break2;
					}
				break;
			case 'f':
				if(end==begin) // right after whitespace
					if(pc.source[0]=='a' && pc.source[1]=='l' && pc.source[2]=='s' && pc.source[3]=='e') { // def
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

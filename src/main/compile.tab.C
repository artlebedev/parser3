
/*  A Bison parser, made from compile.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	EON	257
#define	STRING	258
#define	BOGUS	259
#define	BAD_STRING_COMPARISON_OPERATOR	260
#define	BAD_HEX_LITERAL	261
#define	LAND	262
#define	LOR	263
#define	LXOR	264
#define	NLE	265
#define	NGE	266
#define	NEQ	267
#define	NNE	268
#define	SLT	269
#define	SGT	270
#define	SLE	271
#define	SGE	272
#define	SEQ	273
#define	SNE	274
#define	DEF	275
#define	IN	276
#define	FEXISTS	277
#define	DEXISTS	278
#define	IS	279
#define	NEG	280

#line 8 "compile.y"

static char *RCSId="$Id: compile.tab.C,v 1.3 2001/08/10 12:36:55 parser Exp $"; 

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



#define	YYFINAL		244
#define	YYFLAG		-32768
#define	YYNTBASE	54

#define YYTRANSLATE(x) ((unsigned)(x) <= 280 ? yytranslate[x] : 133)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    40,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    28,    52,    29,    44,    37,    31,    53,    48,
    49,    35,    34,     2,    33,    47,    36,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    51,    43,    26,
     2,    27,     2,    39,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    41,     2,    42,    50,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,    45,    30,    46,    32,     2,     2,     2,     2,
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
    17,    18,    19,    20,    21,    22,    23,    24,    25,    38
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
   306,   310,   313,   316,   319,   322,   325,   328,   331,   335,
   339,   343,   347,   351,   355,   359,   363,   367,   371,   375,
   379,   383,   387,   391,   395,   399,   403,   407,   411,   415,
   419,   423,   427,   429,   431,   432
};

static const short yyrhs[] = {    56,
     0,    55,     0,    57,     0,    55,    57,     0,    69,     0,
    58,     0,    63,     0,    39,     4,    40,    59,     0,   132,
     0,    60,     0,    61,     0,    60,    61,     0,    62,    40,
     0,   132,     0,     4,     0,    39,     4,    65,    64,    68,
    40,    69,     0,   132,     0,    65,     0,    41,    66,    42,
     0,   132,     0,    67,     0,     4,     0,    67,    43,     4,
     0,   132,     0,     4,     0,   132,     0,    70,     0,    71,
     0,    70,    71,     0,   130,     0,    72,     0,    73,     0,
    81,     0,    94,     0,    74,     0,    44,    75,     0,    77,
     3,     0,    76,     0,    45,    77,    46,     0,    78,     0,
    79,     0,    80,     0,   124,    80,     0,   116,     0,   113,
   116,     0,    44,    82,    86,     0,    83,     0,    84,     0,
    85,     0,   112,     0,    47,   112,     0,   124,   112,     0,
    87,     0,    88,     0,    89,     0,    41,    90,    42,     0,
    48,   127,    49,     0,    45,    69,    46,     0,   131,     0,
     4,     0,    91,     0,    92,     0,    93,     0,    72,     0,
    71,    70,     0,    95,     0,     0,     0,    50,    96,    98,
    97,    99,     3,     0,    77,     0,   100,     0,    99,   100,
     0,   101,     0,   102,     0,   103,     0,    41,   104,    42,
     0,    48,   105,    49,     0,    45,   106,    46,     0,   107,
     0,   104,    43,   107,     0,   108,     0,   105,    43,   108,
     0,   109,     0,   106,    43,   109,     0,   110,     0,   111,
     0,    69,     0,   131,     0,     4,     0,    91,     0,   127,
     0,   117,     0,   113,   117,     0,   114,     0,   113,   114,
     0,   115,    47,     0,   117,     0,   117,     0,     4,     5,
     0,     4,     0,   118,     0,   119,     0,   120,     0,    44,
   121,     0,     4,   122,     0,    48,    70,    49,     0,     4,
     0,   123,     0,   122,   123,     0,    44,   121,     0,   125,
     0,   126,     0,     4,    51,     0,   125,    51,     0,   128,
     0,     4,     0,    74,     0,    95,     0,    52,   129,    52,
     0,    53,   129,    53,     0,    48,   128,    49,     0,    33,
   128,     0,    32,   128,     0,    28,   128,     0,    21,   128,
     0,    22,   128,     0,    23,   128,     0,    24,   128,     0,
   128,    33,   128,     0,   128,    34,   128,     0,   128,    35,
   128,     0,   128,    36,   128,     0,   128,    37,   128,     0,
   128,    31,   128,     0,   128,    30,   128,     0,   128,    29,
   128,     0,   128,     8,   128,     0,   128,     9,   128,     0,
   128,    10,   128,     0,   128,    26,   128,     0,   128,    27,
   128,     0,   128,    11,   128,     0,   128,    12,   128,     0,
   128,    13,   128,     0,   128,    14,   128,     0,   128,    15,
   128,     0,   128,    16,   128,     0,   128,    17,   128,     0,
   128,    18,   128,     0,   128,    19,   128,     0,   128,    20,
   128,     0,   128,    25,   128,     0,    69,     0,     4,     0,
     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   106,   116,   118,   118,   119,   121,   121,   123,   189,   189,
   190,   190,   191,   192,   192,   194,   223,   223,   224,   225,
   225,   226,   226,   228,   228,   232,   232,   234,   234,   235,
   235,   236,   236,   236,   240,   244,   245,   245,   246,   247,
   249,   250,   265,   266,   266,   270,   274,   276,   277,   278,
   293,   298,   300,   301,   302,   305,   311,   317,   323,   325,
   326,   328,   334,   335,   335,   339,   343,   344,   345,   359,
   361,   361,   362,   364,   365,   367,   368,   369,   370,   372,
   374,   376,   378,   380,   382,   386,   390,   394,   396,   397,
   399,   406,   406,   408,   408,   409,   410,   415,   420,   422,
   424,   425,   426,   428,   432,   440,   446,   451,   451,   452,
   457,   459,   461,   465,   477,   481,   483,   484,   485,   486,
   487,   489,   490,   491,   492,   493,   494,   495,   497,   498,
   499,   500,   501,   502,   503,   504,   505,   506,   507,   508,
   509,   510,   511,   512,   513,   514,   515,   516,   517,   518,
   519,   520,   523,   532,   537,   538
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","EON","STRING",
"BOGUS","BAD_STRING_COMPARISON_OPERATOR","BAD_HEX_LITERAL","\"&&\"","\"||\"",
"\"##\"","\"<=\"","\">=\"","\"==\"","\"!=\"","\"lt\"","\"gt\"","\"le\"","\"ge\"",
"\"eq\"","\"ne\"","\"def\"","\"in\"","\"-f\"","\"-d\"","\"is\"","'<'","'>'",
"'!'","'#'","'|'","'&'","'~'","'-'","'+'","'*'","'/'","'%'","NEG","'@'","'\\n'",
"'['","']'","';'","'$'","'{'","'}'","'.'","'('","')'","'^'","':'","'\\\"'","'\\''",
"all","methods","one_big_piece","method","control_method","maybe_control_strings",
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
"name_curly_code_value","subvar_ref_name_rdive","subvar_get_writes","subvar__get_write",
"class_prefix","class_static_prefix","class_constructor_prefix","expr_value",
"expr","string_inside_quotes_value","write_string","void_value","empty", NULL
};
#endif

static const short yyr1[] = {     0,
    54,    54,    55,    55,    56,    57,    57,    58,    59,    59,
    60,    60,    61,    62,    62,    63,    64,    64,    65,    66,
    66,    67,    67,    68,    68,    69,    69,    70,    70,    71,
    71,    72,    72,    72,    73,    74,    75,    75,    76,    77,
    77,    78,    79,    80,    80,    81,    82,    82,    82,    83,
    84,    85,    86,    86,    86,    87,    88,    89,    90,    90,
    90,    91,    92,    93,    93,    94,    96,    97,    95,    98,
    99,    99,   100,   100,   100,   101,   102,   103,   104,   104,
   105,   105,   106,   106,   107,   108,   109,   110,   110,   110,
   111,   112,   112,   113,   113,   114,   115,   116,   116,   117,
   117,   117,   117,   118,   119,   120,   121,   122,   122,   123,
   124,   124,   125,   126,   127,   128,   128,   128,   128,   128,
   128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
   128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
   128,   128,   128,   128,   128,   128,   128,   128,   128,   128,
   128,   128,   129,   130,   131,   132
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
     3,     2,     2,     2,     2,     2,     2,     2,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
     3,     3,     1,     1,     0,     0
};

static const short yydefact[] = {   156,
   154,     0,     0,    67,     2,     1,     3,     6,     7,     5,
    27,    28,    31,    32,    35,    33,    34,    66,    30,    26,
     0,   100,     0,     0,     0,     0,    36,    38,     0,    40,
    41,    42,     0,    47,    48,    49,    50,     0,    94,     0,
    44,    92,   101,   102,   103,     0,   111,   112,     0,     4,
    29,   156,   156,   156,    99,     0,   113,   105,   108,   107,
   104,     0,     0,    98,     0,   100,    51,     0,    92,     0,
    37,   155,   156,     0,    46,    53,    54,    55,   100,    95,
    45,    93,    96,    43,    52,   114,    70,    68,    15,     8,
    10,    11,     0,     9,    22,     0,    21,    20,   156,    18,
    17,   110,   109,    39,    93,   106,   154,     0,    31,     0,
    61,    62,    63,    59,     0,   116,     0,     0,     0,     0,
     0,     0,     0,     0,     0,   156,   156,   117,   118,     0,
   115,     0,    12,    14,    13,    19,     0,    25,     0,    24,
    65,    56,    58,   125,   126,   127,   128,   124,   123,   122,
     0,   153,     0,     0,    57,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,   155,
   156,     0,     0,    71,    73,    74,    75,    23,   156,   121,
   119,   120,   137,   138,   139,   142,   143,   144,   145,   146,
   147,   148,   149,   150,   151,   152,   140,   141,   136,   135,
   134,   129,   130,   131,   132,   133,   154,    90,     0,    79,
    85,    88,    87,     0,    83,     0,    81,    86,    91,    69,
    72,    16,    76,   155,   156,    78,     0,    77,    80,    84,
    82,     0,     0,     0
};

static const short yydefgoto[] = {   242,
     5,     6,     7,     8,    90,    91,    92,    93,     9,    99,
    54,    96,    97,   139,   152,    11,    12,    13,    14,   128,
    27,    28,    29,    30,    31,    32,    16,    33,    34,    35,
    36,    75,    76,    77,    78,   110,   218,   112,   113,    17,
   129,    49,   132,    88,   183,   184,   185,   186,   187,   219,
   226,   224,   220,   227,   225,   221,   228,    37,    63,    39,
    40,    41,    64,    43,    44,    45,    61,    58,    59,    65,
    47,    48,   229,   131,   153,    19,   222,    20
};

static const short yypact[] = {     6,
-32768,    14,    57,-32768,   -17,-32768,-32768,-32768,-32768,-32768,
    16,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    73,    35,    23,    51,    76,    16,-32768,-32768,    80,-32768,
-32768,-32768,   -24,-32768,-32768,-32768,-32768,    77,-32768,    47,
-32768,    22,-32768,-32768,-32768,    77,    46,-32768,    51,-32768,
-32768,    96,   107,    88,-32768,    23,-32768,    71,-32768,-32768,
-32768,   141,    77,    90,    77,    71,-32768,    76,    90,     8,
-32768,    38,    16,    15,-32768,-32768,-32768,-32768,     9,-32768,
-32768,    22,-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,
    94,-32768,    83,   142,-32768,   144,   145,-32768,   186,-32768,
-32768,-32768,-32768,-32768,    90,-32768,   150,    16,   123,   152,
-32768,-32768,-32768,-32768,   173,-32768,    15,    15,    15,    15,
    15,    15,    15,    74,    15,    16,    16,-32768,-32768,   171,
   274,    91,-32768,-32768,-32768,-32768,   217,-32768,   182,-32768,
    16,-32768,-32768,   140,   140,   140,   140,   140,   118,-32768,
   232,-32768,   180,   170,-32768,    15,    15,    15,    15,    15,
    15,    15,    15,    15,    15,    15,    15,    15,    15,    15,
    15,    15,    15,    15,    15,    15,    15,    15,    15,    40,
    16,    15,    48,-32768,-32768,-32768,-32768,-32768,    16,-32768,
-32768,-32768,   361,   334,   304,   386,   386,   113,   113,   386,
   386,   386,   386,   113,   113,   140,   386,   386,   194,   239,
   118,    -4,    -4,-32768,-32768,-32768,   125,-32768,   136,-32768,
-32768,-32768,-32768,    60,-32768,   -20,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    40,    16,-32768,    15,-32768,-32768,-32768,
-32768,   233,   236,-32768
};

static const short yypgoto[] = {-32768,
-32768,-32768,   248,-32768,-32768,-32768,   163,-32768,-32768,-32768,
   201,-32768,-32768,-32768,     2,   -21,    -8,   -71,-32768,     0,
-32768,-32768,    67,-32768,-32768,   -30,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,-32768,   184,-32768,-32768,-32768,
     4,-32768,-32768,-32768,-32768,    81,-32768,-32768,-32768,-32768,
-32768,-32768,    26,    34,    42,-32768,-32768,   -12,    82,    72,
-32768,    54,     3,-32768,-32768,-32768,   222,-32768,   221,   277,
-32768,-32768,   223,    39,   168,-32768,   224,   -45
};


#define	YYLAST		423


static const short yytable[] = {    15,
   109,    10,    51,    18,    70,    42,    94,    98,   101,     1,
    15,     1,    67,    55,    18,    84,    72,    21,   116,     1,
    73,     2,   237,    74,   -98,    15,    60,    69,   238,    18,
   177,   178,   179,    85,    84,   117,   118,   119,   120,    55,
    82,   107,   121,   217,     2,   134,   122,   123,    42,     3,
   230,     3,    56,   140,    22,     4,   106,     4,   124,     3,
    22,    51,   125,   108,     4,     4,   126,   127,   -97,    15,
   105,    15,    15,    18,   115,    18,    18,    22,    56,    66,
    79,     3,    71,     3,    38,    57,   141,     4,   180,     4,
    62,    81,   181,    83,    23,   182,    86,    89,    26,    89,
    23,    24,   235,    25,    26,   236,    68,    15,   109,    80,
    95,    18,    52,    53,    56,    87,    81,    23,    24,    23,
    23,    26,   135,    26,    26,    15,    15,    38,    53,    18,
    18,   180,    51,  -156,    80,   181,   -97,   169,   182,    80,
    15,   172,   173,   174,    18,   175,   176,   177,   178,   179,
   175,   176,   177,   178,   179,   144,   145,   146,   147,   148,
   149,   150,   109,   151,   -64,   -64,   -89,   -89,   172,   173,
   174,   108,   175,   176,   177,   178,   179,   233,   234,    15,
    15,   -14,   223,    18,    18,   136,   104,   137,    15,   138,
   232,   -60,    18,   142,   193,   194,   195,   196,   197,   198,
   199,   200,   201,   202,   203,   204,   205,   206,   207,   208,
   209,   210,   211,   212,   213,   214,   215,   216,   143,   155,
   188,   189,   192,   173,   174,   108,   175,   176,   177,   178,
   179,   191,   243,    15,    15,   244,   223,    18,    18,   156,
   157,   158,   159,   160,   161,   162,   163,   164,   165,   166,
   167,   168,    50,   133,   100,   111,   169,   170,   171,   239,
   172,   173,   174,   231,   175,   176,   177,   178,   179,   174,
   241,   175,   176,   177,   178,   179,   240,   102,   103,    46,
   190,   156,   157,   158,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,   154,   114,   130,     0,   169,   170,
   171,     0,   172,   173,   174,     0,   175,   176,   177,   178,
   179,   156,   157,     0,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,     0,     0,     0,     0,   169,   170,
   171,     0,   172,   173,   174,     0,   175,   176,   177,   178,
   179,   156,     0,     0,   159,   160,   161,   162,   163,   164,
   165,   166,   167,   168,     0,     0,     0,     0,   169,   170,
   171,     0,   172,   173,   174,     0,   175,   176,   177,   178,
   179,   159,   160,   161,   162,   163,   164,   165,   166,   167,
   168,     0,     0,     0,     0,   169,   170,   171,     0,   172,
   173,   174,     0,   175,   176,   177,   178,   179,   161,   162,
     0,     0,     0,     0,   167,   168,     0,     0,     0,     0,
   169,     0,     0,     0,   172,   173,   174,     0,   175,   176,
   177,   178,   179
};

static const short yycheck[] = {     0,
    72,     0,    11,     0,    26,     3,    52,    53,    54,     4,
    11,     4,    25,     5,    11,    46,    41,     4,     4,     4,
    45,    39,    43,    48,     3,    26,     4,    25,    49,    26,
    35,    36,    37,    46,    65,    21,    22,    23,    24,     5,
    38,     4,    28,     4,    39,    91,    32,    33,    46,    44,
     3,    44,    44,    99,     4,    50,    49,    50,    44,    44,
     4,    70,    48,    72,    50,    50,    52,    53,    47,    70,
    68,    72,    73,    70,    73,    72,    73,     4,    44,     4,
     4,    44,     3,    44,     3,    51,   108,    50,    41,    50,
    24,    38,    45,    47,    44,    48,    51,     4,    48,     4,
    44,    45,    43,    47,    48,    46,    25,   108,   180,    38,
     4,   108,    40,    41,    44,    49,    63,    44,    45,    44,
    44,    48,    40,    48,    48,   126,   127,    46,    41,   126,
   127,    41,   141,    40,    63,    45,    47,    25,    48,    68,
   141,    29,    30,    31,   141,    33,    34,    35,    36,    37,
    33,    34,    35,    36,    37,   117,   118,   119,   120,   121,
   122,   123,   234,   125,    42,    43,    42,    43,    29,    30,
    31,   180,    33,    34,    35,    36,    37,    42,    43,   180,
   181,    40,   181,   180,   181,    42,    46,    43,   189,     4,
   189,    42,   189,    42,   156,   157,   158,   159,   160,   161,
   162,   163,   164,   165,   166,   167,   168,   169,   170,   171,
   172,   173,   174,   175,   176,   177,   178,   179,    46,    49,
     4,    40,    53,    30,    31,   234,    33,    34,    35,    36,
    37,    52,     0,   234,   235,     0,   235,   234,   235,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,     5,    91,    54,    72,    25,    26,    27,   234,
    29,    30,    31,   183,    33,    34,    35,    36,    37,    31,
   237,    33,    34,    35,    36,    37,   235,    56,    58,     3,
    49,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,   127,    72,    74,    -1,    25,    26,
    27,    -1,    29,    30,    31,    -1,    33,    34,    35,    36,
    37,     8,     9,    -1,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    -1,    -1,    -1,    -1,    25,    26,
    27,    -1,    29,    30,    31,    -1,    33,    34,    35,    36,
    37,     8,    -1,    -1,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    -1,    -1,    -1,    -1,    25,    26,
    27,    -1,    29,    30,    31,    -1,    33,    34,    35,    36,
    37,    11,    12,    13,    14,    15,    16,    17,    18,    19,
    20,    -1,    -1,    -1,    -1,    25,    26,    27,    -1,    29,
    30,    31,    -1,    33,    34,    35,    36,    37,    13,    14,
    -1,    -1,    -1,    -1,    19,    20,    -1,    -1,    -1,    -1,
    25,    -1,    -1,    -1,    29,    30,    31,    -1,    33,    34,
    35,    36,    37
};
#define YYPURE 1

/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "D:/cygwin/usr/share/bison.simple"
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

#line 217 "D:/cygwin/usr/share/bison.simple"

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
#line 107 "compile.y"
{
	Method& method=*NEW Method(POOL, 
		*main_method_name, 
		Method::CT_ANY,
		0, 0, /*min, max numbered_params_count*/
		0/*param_names*/, 0/*local_names*/, 
		yyvsp[0]/*parser_code*/, 0/*native_code*/);
	PC.cclass->add_method(*main_method_name, method);
;
    break;}
case 8:
#line 124 "compile.y"
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
		if(PC.cclass->base()) { // already changed from default?
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
		if(PC.cclass->base()) { // already changed from default?
			strcpy(PC.error, "class already have a base '");
			strncat(PC.error, PC.cclass->base()->name().cstr(), 100);
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
#line 190 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 16:
#line 195 "compile.y"
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
#line 224 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 23:
#line 226 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 29:
#line 234 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 35:
#line 240 "compile.y"
{
	yyval=yyvsp[0]; /* stack: resulting value */
	O(yyval, OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;
    break;}
case 36:
#line 244 "compile.y"
{ yyval=yyvsp[0] ;
    break;}
case 39:
#line 246 "compile.y"
{ yyval=yyvsp[-1] ;
    break;}
case 42:
#line 250 "compile.y"
{
	yyval=N(POOL); 
	Array *diving_code=yyvsp[0];
	const String *first_name=LA2S(diving_code);
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(yyval, OP_WITH_SELF); /* stack: starting context */
		P(yyval, diving_code, 
			/* skip over... */
			diving_code->size()>2?3/*OP_+string+get_element*/:2/*OP_+string*/);
	} else {
		O(yyval, OP_WITH_READ); /* stack: starting context */
		P(yyval, diving_code);
	}
	/* diving code; stack: current context */
;
    break;}
case 43:
#line 265 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 45:
#line 266 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 46:
#line 270 "compile.y"
{
	yyval=yyvsp[-1]; /* stack: context,name */
	P(yyval, yyvsp[0]); /* stack: context,name,constructor_value */
;
    break;}
case 50:
#line 278 "compile.y"
{
	yyval=N(POOL);
	Array *diving_code=yyvsp[0];
	const String *first_name=LA2S(diving_code);
	if(first_name && *first_name==SELF_ELEMENT_NAME) {
		O(yyval, OP_WITH_SELF); /* stack: starting context */
		P(yyval, diving_code, 
			/* skip over... */
			diving_code->size()>2?3/*OP_+string+get_element*/:2/*OP_+string*/);
	} else {
		O(yyval, OP_WITH_ROOT); /* stack: starting context */
		P(yyval, diving_code);
	}
	/* diving code; stack: current context */
;
    break;}
case 51:
#line 293 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_WITH_WRITE); /* stack: starting context */
	P(yyval, yyvsp[0]); /* diving code; stack: context,name */
;
    break;}
case 52:
#line 298 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 56:
#line 305 "compile.y"
{
	// stack: context, name
	yyval=yyvsp[-1]; // stack: context, name, value
	O(yyval, OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 57:
#line 311 "compile.y"
{ 
	// stack: context, name
	yyval=yyvsp[-1]; // stack: context, name, value
	O(yyval, OP_CONSTRUCT_EXPR); /* value=pop; name=pop; context=pop; construct(context,name,value) */
;
    break;}
case 58:
#line 317 "compile.y"
{
	// stack: context, name
	yyval=N(POOL); 
	CCA(yyval, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
;
    break;}
case 62:
#line 328 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_CREATE_EWPOOL); /* stack: empty write context */
	P(yyval, yyvsp[0]); /* some code that writes to that context */
	O(yyval, OP_REDUCE_EWPOOL); /* context=pop; stack: context.value() */
;
    break;}
case 65:
#line 335 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 66:
#line 339 "compile.y"
{
	yyval=yyvsp[0]; /* stack: value */
	O(yyval, OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
;
    break;}
case 67:
#line 343 "compile.y"
{ PC.object_constructor_allowed=true ;
    break;}
case 68:
#line 344 "compile.y"
{ PC.object_constructor_allowed=false ;
    break;}
case 69:
#line 345 "compile.y"
{ /* ^field.$method{vasya} */
	yyval=yyvsp[-3]; /* with_xxx,diving code; stack: context,method_junction */
	O(yyval, OP_GET_METHOD_FRAME); /* stack: context,method_frame */

	YYSTYPE params_code=yyvsp[-1];
	if(params_code->size()==3) // probably [] case. [OP_VALUE + Void + STORE_PARAM]
		if(Value *value=LA2V(params_code)) // it is OP_VALUE + value?
			if(!value->is_defined()) // value is VVoid?
				params_code=0; // ^zzz[] case. don't append lone empty param.
	if(params_code)
		P(yyval, params_code); // filling method_frame.store_params
	O(yyval, OP_CALL); // method_frame=pop; ncontext=pop; call(ncontext,method_frame) stack: value
;
    break;}
case 72:
#line 361 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 76:
#line 367 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 77:
#line 368 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 78:
#line 369 "compile.y"
{yyval=yyvsp[-1];
    break;}
case 80:
#line 372 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 82:
#line 376 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 84:
#line 380 "compile.y"
{ yyval=yyvsp[-2]; P(yyval, yyvsp[0]) ;
    break;}
case 85:
#line 382 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_STORE_PARAM);
;
    break;}
case 86:
#line 386 "compile.y"
{
	yyval=N(POOL); 
	PEA(yyval, yyvsp[0]);
;
    break;}
case 87:
#line 390 "compile.y"
{
	yyval=N(POOL); 
	PCA(yyval, yyvsp[0]);
;
    break;}
case 91:
#line 399 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_WRITE_EXPR_RESULT);
;
    break;}
case 93:
#line 406 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 95:
#line 408 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 97:
#line 410 "compile.y"
{
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 98:
#line 415 "compile.y"
{
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
	O(yyval, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
;
    break;}
case 104:
#line 428 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_GET_ELEMENT);
;
    break;}
case 105:
#line 432 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_CREATE_EWPOOL);
	P(yyval, yyvsp[-1]);
	O(yyval, OP_WRITE_VALUE);
	P(yyval, yyvsp[0]);
	O(yyval, OP_REDUCE_EWPOOL);
;
    break;}
case 106:
#line 440 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_CREATE_EWPOOL);
	P(yyval, yyvsp[-1]);
	O(yyval, OP_REDUCE_EWPOOL);
;
    break;}
case 107:
#line 446 "compile.y"
{
	yyval=N(POOL); 
	O(yyval, OP_WITH_READ);
	P(yyval, yyvsp[0]);
;
    break;}
case 109:
#line 451 "compile.y"
{ yyval=yyvsp[-1]; P(yyval, yyvsp[0]) ;
    break;}
case 110:
#line 452 "compile.y"
{
	yyval=yyvsp[0];
	O(yyval, OP_GET_ELEMENT__WRITE);
;
    break;}
case 113:
#line 461 "compile.y"
{
	yyval=yyvsp[-1]; // stack: class name string
	O(yyval, OP_GET_CLASS);
;
    break;}
case 114:
#line 465 "compile.y"
{
	yyval=yyvsp[-1];
	if(!PC.object_constructor_allowed) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(yyval, OP_PREPARE_TO_CONSTRUCT_OBJECT);
;
    break;}
case 115:
#line 477 "compile.y"
{
	if((yyval=yyvsp[0])->size()==2) // only one string literal in there?
		change_string_literal_to_double_literal(yyval); // make that string literal Double
;
    break;}
case 119:
#line 485 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 120:
#line 486 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 121:
#line 487 "compile.y"
{ yyval = yyvsp[-1]; ;
    break;}
case 122:
#line 489 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_NEG) ;
    break;}
case 123:
#line 490 "compile.y"
{ yyval=yyvsp[0];	 O(yyval, OP_INV) ;
    break;}
case 124:
#line 491 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_NOT) ;
    break;}
case 125:
#line 492 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_DEF) ;
    break;}
case 126:
#line 493 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_IN) ;
    break;}
case 127:
#line 494 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_FEXISTS) ;
    break;}
case 128:
#line 495 "compile.y"
{ yyval=yyvsp[0];  O(yyval, OP_DEXISTS) ;
    break;}
case 129:
#line 497 "compile.y"
{	yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_SUB) ;
    break;}
case 130:
#line 498 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_ADD) ;
    break;}
case 131:
#line 499 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_MUL) ;
    break;}
case 132:
#line 500 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_DIV) ;
    break;}
case 133:
#line 501 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_MOD) ;
    break;}
case 134:
#line 502 "compile.y"
{ yyval=yyvsp[-2]; 	P(yyval, yyvsp[0]);  O(yyval, OP_BIN_AND) ;
    break;}
case 135:
#line 503 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_BIN_OR) ;
    break;}
case 136:
#line 504 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_BIN_XOR) ;
    break;}
case 137:
#line 505 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_LOG_AND) ;
    break;}
case 138:
#line 506 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_LOG_OR) ;
    break;}
case 139:
#line 507 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_LOG_XOR) ;
    break;}
case 140:
#line 508 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_LT) ;
    break;}
case 141:
#line 509 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_GT) ;
    break;}
case 142:
#line 510 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_LE) ;
    break;}
case 143:
#line 511 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_GE) ;
    break;}
case 144:
#line 512 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_EQ) ;
    break;}
case 145:
#line 513 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_NUM_NE) ;
    break;}
case 146:
#line 514 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_LT) ;
    break;}
case 147:
#line 515 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_GT) ;
    break;}
case 148:
#line 516 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_LE) ;
    break;}
case 149:
#line 517 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_GE) ;
    break;}
case 150:
#line 518 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_EQ) ;
    break;}
case 151:
#line 519 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_STR_NE) ;
    break;}
case 152:
#line 520 "compile.y"
{ yyval=yyvsp[-2];  P(yyval, yyvsp[0]);  O(yyval, OP_IS) ;
    break;}
case 153:
#line 523 "compile.y"
{
	yyval=N(POOL);
	O(yyval, OP_CREATE_SWPOOL); /* stack: empty write context */
	P(yyval, yyvsp[0]); /* some code that writes to that context */
	O(yyval, OP_REDUCE_SWPOOL); /* context=pop; stack: context.get_string() */
;
    break;}
case 154:
#line 532 "compile.y"
{
	// optimized from OP_STRING+OP_WRITE_VALUE to OP_STRING__WRITE
	change_string_literal_to_write_string_literal(yyval=yyvsp[0])
;
    break;}
case 155:
#line 537 "compile.y"
{ yyval=VL(NEW VVoid(POOL)) ;
    break;}
case 156:
#line 538 "compile.y"
{ yyval=N(POOL) ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "D:/cygwin/usr/share/bison.simple"

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
#line 540 "compile.y"

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
	#define lexical_brackets_nestage PC.brackets_nestages[PC.sp]
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

		if(c=='\n') {
			PC.line++;
			PC.col=0;
		} else
			PC.col++;

		if(c=='^' && PC.ls!=LS_COMMENT && PC.ls!=LS_DEF_COMMENT) 
			switch(*PC.source) {
			// escaping: ^^ & co
			case '^': case '$': case ';':
			case '(': case ')':
			case '[': case ']':
			case '{': case '}':
			case '"': 
				if(end!=begin) {
					// append piece till ^
					PC.string->APPEND_CLEAN(begin, end-begin, PC.file, begin_line);
				}
				// reset piece 'begin' position & line
				begin=PC.source; // ^
				begin_line=PC.line;
				// skip over ^ and _
				PC.source++;  PC.col++;
				// skip analysis = forced literal
				continue;

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
		// #comment  start skipping
		if(c=='#' && PC.col==1) {
			if(end!=begin) {
				// append piece till #
				PC.string->APPEND_CLEAN(begin, end-begin, PC.file, begin_line);
			}
			// fall into COMMENT lexical state [wait for \n]
			push_LS(PC, LS_COMMENT);
		}
		switch(PC.ls) {

		// USER'S = NOT OURS
		case LS_USER:
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
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				RC;
			case '^':
				push_LS(PC, LS_METHOD_NAME);
				RC;
			case '@':
				if(PC.col==0+1) {
					push_LS(PC, LS_DEF_NAME);
					RC;
				}
				break;
			case ')': // $name.(code<)>
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
					RC;
				}
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
				push_LS(PC, LS_VAR_NAME_SIMPLE);
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
			//                          @todo in case
			// ################
			// @next-method
			// we are here with c=='@'
			// which is wrong, and need action
			if(c=='\n') {
				switch(*PC.source) {
				case '@': case 0: // end of special_code
					pop_LS(PC);
					break;
				}
				RC;
			}
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
				push_LS(PC, LS_EXPRESSION_VAR_NAME);				
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
			case '+': case '*': case '/': case '%': 
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
		case LS_VAR_NAME_SIMPLE:
		case LS_EXPRESSION_VAR_NAME:
		case LS_VAR_NAME_NO_COLON:
			if(PC.ls==LS_EXPRESSION_VAR_NAME) {
				// name in expr ends also before binary operators 
				switch(c) {
				case '-': 
					pop_LS(PC);
					PC.source--;  if(--PC.col<0) { PC.line--;  PC.col=-1; }
					result=EON;
					goto break2;
				}
			}
			if(PC.ls==LS_VAR_NAME_NO_COLON) {
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
				// $name.<(>code)
				if(PC.col>1/*not first column*/ && end[-1]=='.'/*was dot */) {
					push_LS(PC, LS_USER);
					lexical_brackets_nestage=1;
					RC;
				}
				PC.ls=LS_VAR_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim
			case '$': // name part subvar
			case ':': // class<:>name
				PC.ls=LS_VAR_NAME_NO_COLON; // stop before next ':'
				RC;
			}
			break;

		case LS_VAR_NAME_CURLY:
			switch(c) {
			case '(':
				// ${name.<(>code)}
				push_LS(PC, LS_USER);
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
				push_LS(PC, LS_VAR_NAME_SIMPLE);
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
				push_LS(PC, LS_VAR_NAME_SIMPLE);
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
				PC.ls=LS_METHOD_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				PC.ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				RC;
			case '(':
				// $name.<(>code)
				if(PC.col>1/*not first column*/ && end[-1]=='.'/*was dot */) {
					push_LS(PC, LS_USER);
					lexical_brackets_nestage=1;
					RC;
				}
				PC.ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				RC;
			case '.': // name part delim 
			case '$': // name part subvar
			case ':': // ':name' or 'class:name'
				RC;
			}
			break;

		case LS_METHOD_SQUARE:
			switch(c) {
			case '$':
				push_LS(PC, LS_VAR_NAME_SIMPLE);
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
				push_LS(PC, LS_VAR_NAME_SIMPLE);
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

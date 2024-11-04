/* A Bison parser, made by GNU Bison 3.3.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2019 Free Software Foundation,
   Inc.

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

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.3.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "compile.y" /* yacc.c:337  */

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001-2024 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>

	
*/

volatile const char * IDENT_COMPILE_Y = "$Id: compile.tab.C,v 1.188 2024/11/04 03:53:25 moko Exp $";

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
#define YYDEBUG  1
#define YYERROR_VERBOSE  1
#define yyerror(pc, msg)  real_yyerror(pc, msg)
#define YYPRINT(file, type, value)  yyprint(file, type, value)
#define YYMALLOC pa_malloc
#define YYFREE pa_free

// includes

#include "compile_tools.h"
#include "pa_value.h"
#include "pa_request.h"
#include "pa_vobject.h"
#include "pa_vdouble.h"
#include "pa_globals.h"
#include "pa_vmethod_frame.h"

// defines

#define CLASS_NAME "CLASS"
#define USE_CONTROL_METHOD_NAME "USE"
#define OPTIONS_CONTROL_METHOD_NAME "OPTIONS"

// forwards

static int real_yyerror(Parse_control* pc, const char* s);
static void yyprint(FILE* file, int type, YYSTYPE value);
static int yylex(YYSTYPE* lvalp, void* pc);

static const VBool vfalse(false);
static const VBool vtrue(true);
static const VString vempty;

// local convinient inplace typecast & var
#undef PC
#define PC  (*pc)
#undef POOL
#define POOL  (*PC.pool)
#ifndef DOXYGEN

#define CLASS_ADD if(!PC.class_add()){						\
	PC.error=pa_strcat(PC.cclass->type(), " - class is already defined");	\
	YYERROR;								\
}

#define YYERROR3(header, value, footer) {		\
	PC.error=pa_strcat(header, value, footer);	\
	YYERROR;					\
}

#define YYERROR1(value) {				\
	PC.error=value;					\
	YYERROR;					\
}


#line 151 "compile.tab.C" /* yacc.c:337  */
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    EON = 258,
    STRING = 259,
    BOGUS = 260,
    BAD_STRING_COMPARISON_OPERATOR = 261,
    BAD_HEX_LITERAL = 262,
    BAD_METHOD_DECL_START = 263,
    BAD_METHOD_PARAMETER_NAME_CHARACTER = 264,
    LAND = 265,
    LOR = 266,
    LXOR = 267,
    NXOR = 268,
    NLE = 269,
    NGE = 270,
    NEQ = 271,
    NNE = 272,
    NSL = 273,
    NSR = 274,
    SLT = 275,
    SGT = 276,
    SLE = 277,
    SGE = 278,
    SEQ = 279,
    SNE = 280,
    DEF = 281,
    IN = 282,
    FEXISTS = 283,
    DEXISTS = 284,
    IS = 285,
    LITERAL_TRUE = 286,
    LITERAL_FALSE = 287,
    NUNARY = 288
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif



int yyparse (Parse_control* pc);





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
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  51
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   510

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  61
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  90
/* YYNRULES -- Number of rules.  */
#define YYNRULES  174
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  266

#define YYUNDEFTOK  2
#define YYMAXUTOK   288

/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  ((unsigned) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      47,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    45,    59,     2,    51,    42,    36,    60,
      55,    56,    39,    37,     2,    38,    54,    40,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    58,    50,
      33,     2,    34,     2,    46,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    48,    41,    49,    57,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    52,    35,    53,    44,     2,     2,     2,
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
      25,    26,    27,    28,    29,    30,    31,    32,    43
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   148,   148,   152,   154,   154,   155,   157,   157,   159,
     230,   230,   231,   231,   232,   233,   233,   235,   235,   282,
     282,   283,   284,   284,   285,   285,   287,   287,   291,   291,
     293,   293,   294,   294,   295,   295,   295,   299,   330,   331,
     331,   332,   334,   335,   336,   385,   386,   386,   390,   403,
     404,   405,   406,   428,   433,   436,   437,   438,   440,   444,
     440,   459,   460,   462,   469,   476,   477,   478,   480,   486,
     487,   487,   491,   502,   505,   502,   554,   570,   570,   572,
     573,   574,   576,   579,   576,   582,   583,   585,   586,   589,
     590,   593,   594,   596,   599,   612,   617,   618,   619,   624,
     624,   626,   626,   627,   628,   640,   649,   652,   653,   654,
     655,   657,   661,   670,   673,   670,   686,   691,   691,   692,
     698,   699,   701,   720,   730,   732,   733,   734,   735,   736,
     737,   738,   739,   741,   742,   743,   744,   745,   746,   747,
     748,   750,   751,   752,   753,   754,   755,   756,   757,   758,
     759,   760,   761,   762,   763,   764,   765,   766,   767,   768,
     769,   770,   771,   772,   773,   774,   775,   776,   779,   784,
     805,   810,   811,   812,   814
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "EON", "STRING", "BOGUS",
  "BAD_STRING_COMPARISON_OPERATOR", "BAD_HEX_LITERAL",
  "BAD_METHOD_DECL_START", "BAD_METHOD_PARAMETER_NAME_CHARACTER", "\"&&\"",
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
  "any_constructor_code_values", "construct_round", "construct_curly",
  "any_constructor_code_value", "constructor_code_value",
  "constructor_code", "codes__excluding_sole_str_literal", "call",
  "call_value", "$@4", "$@5", "call_name", "store_params", "store_param",
  "store_square_param", "@6", "$@7", "store_round_param",
  "store_curly_param", "store_code_param_parts", "store_expr_param_parts",
  "store_curly_param_parts", "store_code_param_part",
  "store_expr_param_part", "store_curly_param_part", "code_param_value",
  "name_expr_dive_code", "name_path", "name_step", "name_advance1",
  "name_advance2", "name_expr_value", "name_expr_subvar_value",
  "name_expr_with_subvar_value", "name_square_code_value", "@8", "$@9",
  "subvar_ref_name_rdive", "subvar_get_writes", "subvar__get_write",
  "class_prefix", "class_static_prefix", "class_constructor_prefix",
  "expr_value", "expr", "double_or_STRING", "string_inside_quotes_value",
  "write_string", "empty_value", "true_value", "false_value", "empty", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,    60,    62,   124,    38,    43,    45,    42,
      47,    92,    37,   288,   126,    33,    64,    10,    91,    93,
      59,    36,   123,   125,    46,    40,    41,    94,    58,    34,
      39
};
# endif

#define YYPACT_NINF -215

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-215)))

#define YYTABLE_NINF -175

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       6,  -215,    20,    36,  -215,    29,   -10,  -215,  -215,  -215,
    -215,  -215,     4,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,   -21,     8,  -215,    34,     5,    43,  -215,  -215,
      38,  -215,  -215,  -215,    49,  -215,  -215,  -215,  -215,    47,
    -215,    -6,  -215,    18,  -215,  -215,  -215,    47,    54,  -215,
       5,  -215,  -215,  -215,   101,   118,    83,  -215,    34,  -215,
      92,  -215,     4,  -215,  -215,    94,    47,    91,    47,    92,
    -215,    43,    91,  -215,  -215,     4,   125,  -215,  -215,  -215,
    -215,    17,  -215,  -215,    18,  -215,  -215,  -215,  -215,  -215,
    -215,  -215,  -215,     7,  -215,   102,   103,  -215,    99,   108,
    -215,   155,  -215,  -215,  -215,  -215,     4,  -215,    91,    14,
     107,  -215,   125,   125,   125,   125,  -215,  -215,   125,   125,
     125,   125,    41,   125,     4,     4,  -215,  -215,   105,   327,
    -215,  -215,  -215,    55,  -215,  -215,  -215,  -215,   160,  -215,
     119,  -215,   116,    32,     4,    71,   121,  -215,  -215,  -215,
    -215,  -215,  -215,   293,   293,   293,   293,  -215,  -215,  -215,
    -215,   280,  -215,   113,   114,  -215,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,   125,   125,   125,   125,   125,   125,   125,
     125,   125,   125,  -215,     4,   125,    31,  -215,  -215,  -215,
    -215,  -215,  -215,  -215,     4,    14,   124,  -215,  -215,  -215,
     423,   393,   360,   461,   453,   453,   100,   100,   184,   184,
     453,   453,   453,   453,   100,   100,   293,   453,   453,   468,
     149,    75,    75,  -215,  -215,  -215,  -215,    16,  -215,   -34,
    -215,   -13,  -215,  -215,  -215,  -215,     4,  -215,  -215,    84,
    -215,   127,  -215,  -215,  -215,     4,  -215,   125,  -215,  -215,
      16,   126,  -215,  -215,  -215,  -215
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     174,   170,     0,     0,    73,     0,     3,     2,     4,     7,
       8,     6,    29,    30,    33,    34,    37,    35,    36,    72,
      32,    28,     0,   107,   113,     0,     0,     0,    38,    40,
       0,    42,    43,    44,     0,    49,    50,    51,    52,     0,
     101,     0,    46,    99,   108,   109,   110,     0,   120,   121,
       0,     1,     5,    31,   174,   174,   174,   106,     0,   122,
     112,   117,     0,   116,   111,     0,     0,   105,     0,   107,
      53,     0,    99,    39,    58,   174,     0,    48,    55,    56,
      57,   107,   102,    47,   100,   103,    45,    54,   123,    76,
      74,    16,     9,    11,    12,     0,    10,    24,     0,    23,
      22,   174,    20,    19,   119,   118,   114,    41,   100,   171,
       0,   168,     0,     0,     0,     0,   172,   173,     0,     0,
       0,     0,     0,     0,   174,   174,   128,   129,     0,   124,
     125,   126,   127,     0,    13,    15,    14,    21,     0,    27,
       0,    26,     0,   170,     0,    33,    59,    61,    67,    68,
      69,    65,    64,   137,   138,   139,   140,   134,   133,   135,
     136,     0,   169,     0,     0,    63,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    82,   174,     0,     0,    77,    79,    80,
      81,    25,    17,   115,    71,   171,     0,   132,   130,   131,
     152,   153,   154,   151,   157,   158,   159,   160,   147,   148,
     161,   162,   163,   164,   165,   166,   167,   155,   156,   150,
     149,   142,   141,   143,   144,   146,   145,   171,    95,     0,
      91,     0,    89,    94,    75,    78,   174,    62,    60,   170,
      98,    83,    87,    93,    96,   174,    86,     0,    85,    18,
     171,     0,    92,    90,    88,    84
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -215,  -215,  -215,  -215,   172,  -215,  -215,  -215,    86,  -215,
    -215,  -215,  -215,   136,  -215,  -215,  -215,     1,   -59,    -7,
    -105,  -215,     0,  -215,  -215,   -11,  -215,  -215,   -40,  -215,
    -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
    -215,   -24,  -214,  -215,  -215,  -215,     2,  -215,  -215,  -215,
    -215,    -3,  -215,  -215,  -215,  -215,  -215,  -215,  -215,  -215,
     -77,   -58,   -55,  -215,    33,    22,    57,  -215,   -22,     3,
    -215,  -215,  -215,  -215,  -215,   143,  -215,   142,   200,  -215,
    -215,   132,    97,  -215,    88,  -215,  -202,  -215,  -215,   -23
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     5,     6,     7,     8,     9,    92,    93,    94,    95,
      10,   246,   101,    56,    98,    99,   140,   162,    12,    13,
      14,    15,   126,    28,    29,    30,    31,    32,    33,    17,
      34,    35,    36,    37,    77,    78,   109,   206,   146,    79,
      80,   147,   148,   149,   150,    18,   127,    50,   133,    90,
     196,   197,   198,   237,   261,   199,   200,   251,   241,   239,
     252,   242,   240,   253,    38,    66,    40,    41,    42,    67,
      44,    45,    46,    62,   142,    64,    60,    61,    68,    48,
      49,   243,   129,   130,   163,    20,   151,   131,   132,    21
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      16,    11,    19,   106,   145,    53,    43,    86,     1,    23,
       1,    91,    16,    57,    19,    65,   255,    83,   143,   256,
     249,  -105,    57,   250,    22,    39,    54,    55,    86,    51,
      72,    96,   100,   103,   244,   254,     2,   257,    63,    89,
      23,    73,    84,   258,    83,    23,   250,    69,    85,    71,
      43,    81,     2,    24,  -174,     3,    25,     3,   254,    58,
      70,     4,    16,     4,    19,     3,    59,     3,    58,    39,
     135,     4,  -104,     4,   108,    16,   110,    19,   141,   193,
      87,   -66,   -66,   194,    24,   204,   195,    25,    26,    24,
      27,    24,    25,    26,    25,    24,    82,    74,    25,    53,
     145,    75,   144,   193,    76,    91,    16,   194,    19,    16,
     195,    19,    88,   169,   189,   190,   191,   192,   174,   175,
     -70,   -70,    97,    82,    16,    16,    19,    19,    82,   111,
     182,    55,   145,   -97,   -97,   185,   186,   187,   188,   189,
     190,   191,   192,    58,    16,  -104,    19,   107,   137,   136,
     -15,   112,   113,   114,   115,   145,   116,   117,   138,   139,
     152,   165,   118,   119,   201,   203,   202,   174,   175,   120,
     121,   205,   208,   248,   209,   265,   122,   260,    52,   134,
     123,   247,     4,   264,   124,   125,   187,   188,   189,   190,
     191,   192,   102,   245,    16,   238,    19,    53,   144,   263,
     262,   104,   105,    47,    16,    16,    19,    19,   128,   153,
     154,   155,   156,   164,     0,   157,   158,   159,   160,     0,
     161,   187,   188,   189,   190,   191,   192,     0,     0,     0,
     144,     0,     0,     0,     0,     0,     0,    16,     0,    19,
       0,     0,     0,     0,     0,     0,    16,   259,    19,     0,
       0,     0,     0,   144,     0,    16,   238,    19,     0,     0,
      16,     0,    19,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   225,   226,
     227,   228,   229,   230,   231,   232,   233,   234,   235,   236,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   169,     0,     0,     0,
     182,   174,   175,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,     0,     0,     0,     0,     0,   185,   186,
     187,   188,   189,   190,   191,   192,   207,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   175,   176,   177,   178,
     179,   180,   181,     0,     0,     0,     0,   182,     0,     0,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
     166,   167,     0,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,     0,     0,     0,     0,
     182,     0,     0,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   166,     0,     0,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,     0,
       0,     0,     0,   182,     0,     0,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,     0,
       0,     0,     0,   182,     0,     0,   183,   184,   185,   186,
     187,   188,   189,   190,   191,   192,   169,     0,     0,   172,
     173,   174,   175,     0,     0,     0,     0,   180,   181,   174,
     175,     0,     0,   182,     0,     0,   174,   175,   185,   186,
     187,   188,   189,   190,   191,   192,   185,   186,   187,   188,
     189,   190,   191,   192,   186,   187,   188,   189,   190,   191,
     192
};

static const yytype_int16 yycheck[] =
{
       0,     0,     0,    62,   109,    12,     3,    47,     4,     4,
       4,     4,    12,     5,    12,    26,    50,    39,     4,    53,
       4,     3,     5,   237,     4,     3,    47,    48,    68,     0,
      27,    54,    55,    56,     3,   237,    46,    50,     4,    50,
       4,     3,    39,    56,    66,     4,   260,     4,    54,    27,
      47,     4,    46,    48,    47,    51,    51,    51,   260,    51,
      27,    57,    62,    57,    62,    51,    58,    51,    51,    47,
      93,    57,    54,    57,    71,    75,    75,    75,   101,    48,
      47,    49,    50,    52,    48,   144,    55,    51,    52,    48,
      54,    48,    51,    52,    51,    48,    39,    48,    51,   106,
     205,    52,   109,    48,    55,     4,   106,    52,   106,   109,
      55,   109,    58,    13,    39,    40,    41,    42,    18,    19,
      49,    50,     4,    66,   124,   125,   124,   125,    71,     4,
      30,    48,   237,    49,    50,    35,    36,    37,    38,    39,
      40,    41,    42,    51,   144,    54,   144,    53,    49,    47,
      47,    26,    27,    28,    29,   260,    31,    32,    50,     4,
      53,    56,    37,    38,     4,    49,    47,    18,    19,    44,
      45,    50,    59,    49,    60,    49,    51,    50,     6,    93,
      55,   205,    57,   260,    59,    60,    37,    38,    39,    40,
      41,    42,    56,   196,   194,   194,   194,   204,   205,   257,
     255,    58,    60,     3,   204,   205,   204,   205,    76,   112,
     113,   114,   115,   125,    -1,   118,   119,   120,   121,    -1,
     123,    37,    38,    39,    40,    41,    42,    -1,    -1,    -1,
     237,    -1,    -1,    -1,    -1,    -1,    -1,   237,    -1,   237,
      -1,    -1,    -1,    -1,    -1,    -1,   246,   246,   246,    -1,
      -1,    -1,    -1,   260,    -1,   255,   255,   255,    -1,    -1,
     260,    -1,   260,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   189,   190,   191,   192,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    13,    -1,    -1,    -1,
      30,    18,    19,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    -1,    -1,    -1,    -1,    -1,    35,    36,
      37,    38,    39,    40,    41,    42,    56,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    -1,    -1,    -1,    -1,    30,    -1,    -1,
      33,    34,    35,    36,    37,    38,    39,    40,    41,    42,
      10,    11,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    -1,    -1,    -1,    -1,
      30,    -1,    -1,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    10,    -1,    -1,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    -1,
      -1,    -1,    -1,    30,    -1,    -1,    33,    34,    35,    36,
      37,    38,    39,    40,    41,    42,    13,    -1,    -1,    16,
      17,    18,    19,    -1,    -1,    -1,    -1,    24,    25,    18,
      19,    -1,    -1,    30,    -1,    -1,    18,    19,    35,    36,
      37,    38,    39,    40,    41,    42,    35,    36,    37,    38,
      39,    40,    41,    42,    36,    37,    38,    39,    40,    41,
      42
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,    46,    51,    57,    62,    63,    64,    65,    66,
      71,    78,    79,    80,    81,    82,    83,    90,   106,   107,
     146,   150,     4,     4,    48,    51,    52,    54,    84,    85,
      86,    87,    88,    89,    91,    92,    93,    94,   125,   126,
     127,   128,   129,   130,   131,   132,   133,   139,   140,   141,
     108,     0,    65,    80,    47,    48,    74,     5,    51,    58,
     137,   138,   134,     4,   136,    86,   126,   130,   139,     4,
     125,   126,   130,     3,    48,    52,    55,    95,    96,   100,
     101,     4,   127,   129,   130,    54,    89,   125,    58,    86,
     110,     4,    67,    68,    69,    70,   150,     4,    75,    76,
     150,    73,    74,   150,   136,   138,    79,    53,   130,    97,
      78,     4,    26,    27,    28,    29,    31,    32,    37,    38,
      44,    45,    51,    55,    59,    60,    83,   107,   142,   143,
     144,   148,   149,   109,    69,   150,    47,    49,    50,     4,
      77,   150,   135,     4,    80,    81,    99,   102,   103,   104,
     105,   147,    53,   143,   143,   143,   143,   143,   143,   143,
     143,   143,    78,   145,   145,    56,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    30,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    48,    52,    55,   111,   112,   113,   116,
     117,     4,    47,    49,    79,    50,    98,    56,    59,    60,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   114,    78,   120,
     123,   119,   122,   142,     3,   112,    72,   102,    49,     4,
     103,   118,   121,   124,   147,    50,    53,    50,    56,    78,
      50,   115,   123,   122,   121,    49
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    61,    62,    62,    63,    63,    64,    65,    65,    66,
      67,    67,    68,    68,    69,    70,    70,    72,    71,    73,
      73,    74,    75,    75,    76,    76,    77,    77,    78,    78,
      79,    79,    80,    80,    81,    81,    81,    82,    83,    84,
      84,    85,    86,    86,    87,    88,    89,    89,    90,    91,
      91,    91,    92,    93,    94,    95,    95,    95,    97,    98,
      96,    99,    99,   100,   101,   102,   102,   102,   103,   104,
     105,   105,   106,   108,   109,   107,   110,   111,   111,   112,
     112,   112,   114,   115,   113,   116,   117,   118,   118,   119,
     119,   120,   120,   121,   122,   123,   124,   124,   124,   125,
     125,   126,   126,   127,   128,   129,   129,   130,   130,   130,
     130,   131,   132,   134,   135,   133,   136,   137,   137,   138,
     139,   139,   140,   141,   142,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   144,   145,
     146,   147,   148,   149,   150
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     1,     2,     1,     1,     1,     4,
       1,     1,     1,     2,     2,     1,     1,     0,     8,     1,
       1,     3,     1,     1,     1,     3,     1,     1,     1,     1,
       1,     2,     1,     1,     1,     1,     1,     1,     2,     2,
       1,     3,     1,     1,     1,     2,     1,     2,     3,     1,
       1,     1,     1,     2,     2,     1,     1,     1,     0,     0,
       5,     1,     3,     3,     3,     1,     1,     1,     1,     1,
       1,     2,     1,     0,     0,     6,     1,     1,     2,     1,
       1,     1,     0,     0,     5,     3,     3,     1,     3,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     1,     1,
       2,     1,     2,     2,     1,     1,     2,     1,     1,     1,
       1,     2,     2,     0,     0,     5,     1,     1,     2,     2,
       1,     1,     2,     2,     1,     1,     1,     1,     1,     1,
       3,     3,     3,     2,     2,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     1,     1,
       1,     0,     1,     1,     0
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (pc, YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, pc); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, Parse_control* pc)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  YYUSE (pc);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep, Parse_control* pc)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep, pc);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, Parse_control* pc)
{
  unsigned long yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              , pc);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, pc); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
            else
              goto append;

          append:
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

  return (YYSIZE_T) (yystpcpy (yyres, yystr) - yyres);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, Parse_control* pc)
{
  YYUSE (yyvaluep);
  YYUSE (pc);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (Parse_control* pc)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yynewstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  *yyssp = (yytype_int16) yystate;

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = (YYSIZE_T) (yyssp - yyss + 1);

# if defined yyoverflow
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
# else /* defined YYSTACK_RELOCATE */
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
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval, pc);
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

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
#line 148 "compile.y" /* yacc.c:1652  */
    {
	Method* method=new Method(Method::CT_ANY, 0, 0 /*min, max numbered_params_count*/, 0 /*param_names*/, 0 /*local_names*/, yyvsp[0] /*parser_code*/, 0 /*native_code*/, PC.cclass->is_vars_local());
	PC.cclass->set_method(PC.alias_method(main_method_name), method);
}
#line 1595 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 9:
#line 160 "compile.y" /* yacc.c:1652  */
    {
	const String& command=LA2S(*yyvsp[-2])->trim(String::TRIM_END);
	YYSTYPE strings_code=yyvsp[0];
	if(strings_code->count()<1*OPERATIONS_PER_OPVALUE)
		YYERROR3("@", command.cstr(), " is empty");
	if(command==CLASS_NAME) {
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			CLASS_ADD;
			// new class' name
			const String& name=LA2S(*strings_code)->trim(String::TRIM_END);
			// creating the class
			VStateless_class* cclass=new VClass(name.cstr(), PC.request.get_used_filespec(PC.file_no));
			PC.cclass_new=cclass;
			PC.append=false;
		} else {
			YYERROR1("@" CLASS_NAME " must contain only one line with class name (contains more than one)");
		}
	} else if(command==USE_CONTROL_METHOD_NAME) {
		CLASS_ADD;
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE){
			PC.request.use_file(LA2S(*strings_code, i)->trim(String::TRIM_END), PC.request.get_used_filespec(PC.file_no), strings_code->get(i+1).origin);
		}
	} else if(command==BASE_NAME) {
		if(PC.append)
			YYERROR3("can't set base while appending methods to class '", PC.cclass->type(), "'");
		CLASS_ADD;
		if(PC.cclass->base_class()) // already changed from default?
			YYERROR3("class already have a base '", PC.cclass->base_class()->type(), "'");
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			const String& base_name=LA2S(*strings_code)->trim(String::TRIM_END);
			if(VStateless_class *base_class=PC.request.get_class(base_name)) {
				// @CLASS == @BASE sanity check
				if(PC.cclass==base_class)
					YYERROR1("@" CLASS_NAME " equals @" BASE_NAME);
				PC.cclass->get_class()->set_base(base_class);
			} else {
				YYERROR3("'", base_name.cstr(), "': undefined class in @" BASE_NAME);
			}
		} else {
			YYERROR1("@" BASE_NAME " must contain sole name");
		}
	} else if(command==OPTIONS_CONTROL_METHOD_NAME) {
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE) {
			const String& option=LA2S(*strings_code, i)->trim(String::TRIM_END);
			if(option==Symbols::LOCALS_SYMBOL){
				PC.set_all_vars_local();
			} else if(option==Symbols::PARTIAL_SYMBOL){
				if(PC.cclass_new){
					if(VStateless_class* existed=PC.get_existed_class(PC.cclass_new)){
						if(!PC.reuse_existed_class(existed))
							YYERROR3("can't append methods to '", PC.cclass_new->type(), "' - the class wasn't marked as partial");
					} else {
						// marks the new class as partial. we will be able to add methods here later.
						PC.cclass_new->set_partial();
					}
				} else {
					YYERROR1("'partial' option should be used straight after @" CLASS_NAME);
				}
			} else if(option==Symbols::STATIC_SYMBOL){
				PC.set_methods_call_type(Method::CT_STATIC);
			} else if(option==Symbols::DYNAMIC_SYMBOL){
				PC.set_methods_call_type(Method::CT_DYNAMIC);
			} else {
				YYERROR3("'", option.cstr(), "' invalid option. valid options are 'partial', 'locals', 'static' and 'dynamic'");
			}
		}
	} else {
		YYERROR3("'", command.cstr(), "' invalid special name. valid names are '" CLASS_NAME "', '" USE_CONTROL_METHOD_NAME "', '" BASE_NAME "' and '" OPTIONS_CONTROL_METHOD_NAME "'.");
	}
}
#line 1670 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 13:
#line 231 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 1676 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 17:
#line 235 "compile.y" /* yacc.c:1652  */
    { 
	CLASS_ADD;
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
	bool all_vars_local=PC.cclass->is_vars_local();

	if(int size=locals_names_code->count()) {
		locals_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE) {
			const String* local_name=LA2S(*locals_names_code, i);
			if(SYMBOLS_EQ(*local_name,RESULT_SYMBOL))
				PC.explicit_result=true;
			else if(SYMBOLS_EQ(*local_name,LOCALS_SYMBOL))
				all_vars_local=true;
			else
				*locals_names+=local_name;
		}
	}

	Method* method=new Method(
		//name, 
		GetMethodCallType(PC, *yyvsp[-4]),
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		0/*to be filled later in next {} */, 0, all_vars_local);

	*reinterpret_cast<Method**>(&yyval)=method;
}
#line 1719 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 18:
#line 272 "compile.y" /* yacc.c:1652  */
    {
		Method* method=reinterpret_cast<Method*>(yyvsp[-1]);
		// fill in the code
		method->parser_code=yyvsp[0];

		// register in class
		const String& name=*LA2S(*yyvsp[-6]);
		PC.cclass->set_method(PC.alias_method(name), method);
}
#line 1733 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 21:
#line 283 "compile.y" /* yacc.c:1652  */
    {yyval=yyvsp[-1];}
#line 1739 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 25:
#line 285 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 1745 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 31:
#line 293 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 1751 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 37:
#line 299 "compile.y" /* yacc.c:1652  */
    {
	yyval=N();
	YYSTYPE code=yyvsp[0];
	size_t count=code->count();

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(count!=3 || !change_first(*code, OP::OP_VALUE__GET_ELEMENT, /*=>*/OP::OP_VALUE__GET_ELEMENT__WRITE) )
#endif

#ifdef OPTIMIZE_BYTECODE_GET_SELF_ELEMENT
	if(count!=3 || !change_first(*code, OP::OP_WITH_SELF__VALUE__GET_ELEMENT, /*=>*/OP::OP_WITH_SELF__VALUE__GET_ELEMENT__WRITE) )
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	if(count!=5 || !change_first(*code, OP::OP_GET_OBJECT_ELEMENT, /*=>*/OP::OP_GET_OBJECT_ELEMENT__WRITE) )
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	if(count!=5 || !change_first(*code, OP::OP_GET_OBJECT_VAR_ELEMENT, /*=>*/OP::OP_GET_OBJECT_VAR_ELEMENT__WRITE) )
#endif

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	if(!change(*code, count-1/* last */, OP::OP_GET_ELEMENT__SPECIAL, /*=>*/OP::OP_GET_ELEMENT__SPECIAL__WRITE) )
#endif

	{
		change_or_append(*code, count-1 /* last */, OP::OP_GET_ELEMENT, /*=>*/OP::OP_GET_ELEMENT__WRITE, /*or */OP::OP_WRITE_VALUE ); /* value=pop; wcontext.write(value) */
	}

	P(*yyval, *code);
}
#line 1787 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 38:
#line 330 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0]; }
#line 1793 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 41:
#line 332 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; }
#line 1799 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 44:
#line 336 "compile.y" /* yacc.c:1652  */
    {
	yyval=N(); 
	YYSTYPE diving_code=yyvsp[0];
	size_t count=diving_code->count();

	if(maybe_make_self(*yyval, *diving_code, count)) {
		// $self.
	} else

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	if(maybe_make_get_object_element(*yyval, *diving_code, count)){
		// optimization for $object.field + ^object.method[
	} else
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	if(maybe_make_get_object_var_element(*yyval, *diving_code, count)){
		// optimization for $object.$var
	} else
#endif

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(count>=4 && (*diving_code)[0].code==OP::OP_VALUE && (*diving_code)[3].code==OP::OP_GET_ELEMENT ){
		 // optimization
		O(*yyval,
			(PC.in_call_value && count==4)
			? OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR // ^object[ : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT_OR_OPERATOR+origin+string
			: OP::OP_VALUE__GET_ELEMENT             // $object  : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT+origin+string
		);
		P(*yyval, *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		if(count>4)
			P(*yyval, *diving_code, 4); // copy tail
	} else {
		O(*yyval, OP::OP_WITH_READ); /* stack: starting context */
		P(*yyval, *diving_code);
	}
#else
	{
		O(*yyval, OP::OP_WITH_READ); /* stack: starting context */

		// ^if OP_ELEMENT => ^if OP_ELEMENT_OR_OPERATOR
		// optimized OP_VALUE+origin+string+OP_GET_ELEMENT. => OP_VALUE+origin+string+OP_GET_ELEMENT_OR_OPERATOR.
		if(PC.in_call_value && count==4)
			diving_code->put(count-1, OP::OP_GET_ELEMENT_OR_OPERATOR);
		P(*yyval, *diving_code);
	}
#endif
	/* diving code; stack: current context */
}
#line 1853 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 45:
#line 385 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 1859 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 47:
#line 386 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 1865 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 48:
#line 390 "compile.y" /* yacc.c:1652  */
    {
	yyval=N();
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	if(maybe_optimize_construct(*yyval, *yyvsp[-1], *yyvsp[0])){
		// $a(expr), $.a(expr), $a[value], $.a[value], $self.a[value], $self.a(expr)
	} else 
#endif
	{
		P(*yyval, *yyvsp[-1]); /* stack: context,name */
		P(*yyval, *yyvsp[0]); /* stack: context,name,constructor_value */
	}
}
#line 1882 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 52:
#line 406 "compile.y" /* yacc.c:1652  */
    {
	yyval=N();
	YYSTYPE diving_code=yyvsp[0];
	size_t count=diving_code->count();

	if(maybe_make_self(*yyval, *diving_code, count)) {
		// $self.
	} else
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(count>=4 && (*diving_code)[0].code==OP::OP_VALUE && (*diving_code)[3].code==OP::OP_GET_ELEMENT ){
		O(*yyval, OP::OP_WITH_ROOT__VALUE__GET_ELEMENT);
		P(*yyval, *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		if(count>4)
			P(*yyval, *diving_code, 4); // tail
	} else
#endif
	{
		O(*yyval, OP::OP_WITH_ROOT); /* stack: starting context */
		P(*yyval, *diving_code);
	}
	/* diving code; stack: current context */
}
#line 1909 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 53:
#line 428 "compile.y" /* yacc.c:1652  */
    {
	yyval=N(); 
	O(*yyval, OP::OP_WITH_WRITE); /* stack: starting context */
	P(*yyval, *yyvsp[0]); /* diving code; stack: context,name */
}
#line 1919 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 54:
#line 433 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 1925 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 58:
#line 440 "compile.y" /* yacc.c:1652  */
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
	PC.array=false; // no need to save current value as if() is right after PC.array=true;
}
#line 1935 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 59:
#line 444 "compile.y" /* yacc.c:1652  */
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
}
#line 1943 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 60:
#line 446 "compile.y" /* yacc.c:1652  */
    {
	// stack: context, name
	if(!PC.array){
		yyval=yyvsp[-2]; // stack: context, name, value
		O(*yyval, OP::OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
	} else {
		yyval = N();
		OA(*yyval, OP::OP_CONSTRUCT_ARRAY, yyvsp[-2]);
		PC.array=false;
	}
}
#line 1959 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 61:
#line 459 "compile.y" /* yacc.c:1652  */
    { yyval = yyvsp[0]; }
#line 1965 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 62:
#line 460 "compile.y" /* yacc.c:1652  */
    { yyval = yyvsp[-2]; P(*yyval, *yyvsp[0]); PC.array=true; }
#line 1971 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 63:
#line 462 "compile.y" /* yacc.c:1652  */
    { 
	yyval=N(); 
	// stack: context, name
	P(*yyval, *yyvsp[-1]); // stack: context, name, value
	O(*yyval, OP::OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
}
#line 1982 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 64:
#line 469 "compile.y" /* yacc.c:1652  */
    {
	// stack: context, name
	yyval=N(); 
	OA(*yyval, OP::OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
}
#line 1992 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 68:
#line 480 "compile.y" /* yacc.c:1652  */
    {
	yyval=N(); 
	OA(*yyval, OP::OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
}
#line 2003 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 71:
#line 487 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2009 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 72:
#line 491 "compile.y" /* yacc.c:1652  */
    {
	size_t count=yyvsp[0]->count();
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
	if(count)
#endif
	{
		yyval=yyvsp[0]; /* stack: value */
		if(!change_first(*yyval, OP::OP_CONSTRUCT_OBJECT, /*=>*/ OP::OP_CONSTRUCT_OBJECT__WRITE))
			change_or_append(*yyval, count-2 /* second last */, OP::OP_CALL, /*=>*/ OP::OP_CALL__WRITE, /*or */ OP::OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
	}
}
#line 2025 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 73:
#line 502 "compile.y" /* yacc.c:1652  */
    { 
	PC.in_call_value=true; 
}
#line 2033 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 74:
#line 505 "compile.y" /* yacc.c:1652  */
    {
	PC.in_call_value=false;
}
#line 2041 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 75:
#line 508 "compile.y" /* yacc.c:1652  */
    { /* ^field.$method{vasya} */
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	const String* operator_name=LA2S(*yyvsp[-3], 0, OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR);
#else
	const String* operator_name=LA2S(*yyvsp[-3], 1);
#endif
	if(operator_name && SYMBOLS_EQ(*operator_name,REM_SYMBOL)){
		yyval=N();
	} else 
#endif
		{
			YYSTYPE params_code=yyvsp[-1];
			if(params_code->count()==3) { // probably [] case. [OP::OP_VALUE+origin+Void]
				if(Value* value=LA2V(*params_code)) // it is OP_VALUE+origin+value?
					if(const String * string=value->get_string())
						if(string->is_empty()) // value is empty string?
							params_code=0; // ^zzz[] case. don't append lone empty param.
			}
			/* stack: context, method_junction */

			YYSTYPE var_code=yyvsp[-3];
			if(
				var_code->count()==8
				&& ( (*var_code)[0].code==OP::OP_VALUE__GET_CLASS || (*var_code)[0].code==OP::OP_VALUE__GET_BASE_CLASS )
				&& (*var_code)[3].code==OP::OP_PREPARE_TO_CONSTRUCT_OBJECT
				&& (*var_code)[4].code==OP::OP_VALUE
#ifdef FEATURE_GET_ELEMENT4CALL
				&& (*var_code)[7].code==OP::OP_GET_ELEMENT4CALL
#else
				&& (*var_code)[7].code==OP::OP_GET_ELEMENT
#endif
			){
				yyval=N();
				O(*yyval, OP::OP_CONSTRUCT_OBJECT);
				P(*yyval, *var_code, 1/*offset*/, 2/*limit*/); // class name
				P(*yyval, *var_code, 5/*offset*/, 2/*limit*/); // constructor name
				OA(*yyval, params_code);
			} else 
				{
					yyval=var_code; /* with_xxx,diving code; stack: context,method_junction */
					OA(*yyval, OP::OP_CALL, params_code); // method_frame=make frame(pop junction); ncontext=pop; call(ncontext,method_frame) stack: value
				}
		}
}
#line 2091 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 76:
#line 554 "compile.y" /* yacc.c:1652  */
    {
#ifdef FEATURE_GET_ELEMENT4CALL
	size_t count=yyvsp[0]->count();
	if(count){
		yyval=yyvsp[0];
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
		!(count==5 && change_first(*yyval, OP::OP_GET_OBJECT_ELEMENT, OP::OP_GET_OBJECT_ELEMENT4CALL)) &&
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
		!(count==5 && change_first(*yyval, OP::OP_GET_OBJECT_VAR_ELEMENT, OP::OP_GET_OBJECT_VAR_ELEMENT4CALL)) &&
#endif
		!change(*yyval, count-1, OP::OP_GET_ELEMENT, OP::OP_GET_ELEMENT4CALL);
	}
#endif
}
#line 2111 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 78:
#line 570 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2117 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 82:
#line 576 "compile.y" /* yacc.c:1652  */
    {
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
}
#line 2126 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 83:
#line 579 "compile.y" /* yacc.c:1652  */
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
}
#line 2134 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 84:
#line 581 "compile.y" /* yacc.c:1652  */
    {yyval=yyvsp[-2];}
#line 2140 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 85:
#line 582 "compile.y" /* yacc.c:1652  */
    {yyval=yyvsp[-1];}
#line 2146 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 86:
#line 583 "compile.y" /* yacc.c:1652  */
    {yyval=yyvsp[-1];}
#line 2152 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 88:
#line 586 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2158 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 90:
#line 590 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2164 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 92:
#line 594 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2170 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 93:
#line 596 "compile.y" /* yacc.c:1652  */
    {
	yyval=yyvsp[0];
}
#line 2178 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 94:
#line 599 "compile.y" /* yacc.c:1652  */
    {
	YYSTYPE expr_code=yyvsp[0];
	if(expr_code->count()==3
		&& (*expr_code)[0].code==OP::OP_VALUE) { // optimizing (double/bool/incidently 'string' too) case. [OP::OP_VALUE+origin+Double]. no evaluating
		yyval=expr_code; 
	} else {
		YYSTYPE code=N();
		P(*code, *expr_code);
		O(*code, OP::OP_WRITE_EXPR_RESULT);
		yyval=N(); 
		OA(*yyval, OP::OP_EXPR_CODE__STORE_PARAM, code);
	}
}
#line 2196 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 95:
#line 612 "compile.y" /* yacc.c:1652  */
    {
	yyval=N(); 
	OA(*yyval, OP::OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
}
#line 2205 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 100:
#line 624 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2211 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 102:
#line 626 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2217 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 104:
#line 628 "compile.y" /* yacc.c:1652  */
    {
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	O(*yyval, is_special_element(*yyval) ? OP::OP_GET_ELEMENT__SPECIAL : OP::OP_GET_ELEMENT);
#else
	O(*yyval, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
#endif
}
#line 2234 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 105:
#line 640 "compile.y" /* yacc.c:1652  */
    {
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	O(*yyval, is_special_element(*yyval) ? OP::OP_GET_ELEMENT__SPECIAL : OP::OP_GET_ELEMENT);
#else
	O(*yyval, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
#endif
}
#line 2248 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 111:
#line 657 "compile.y" /* yacc.c:1652  */
    {
	yyval=yyvsp[0];
	O(*yyval, OP::OP_GET_ELEMENT);
}
#line 2257 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 112:
#line 661 "compile.y" /* yacc.c:1652  */
    {
	YYSTYPE code;
	{
		change_string_literal_to_write_string_literal(*(code=yyvsp[-1]));
		P(*code, *yyvsp[0]);
	}
	yyval=N(); 
	OA(*yyval, OP::OP_STRING_POOL, code);
}
#line 2271 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 113:
#line 670 "compile.y" /* yacc.c:1652  */
    {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
}
#line 2280 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 114:
#line 673 "compile.y" /* yacc.c:1652  */
    {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
}
#line 2288 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 115:
#line 675 "compile.y" /* yacc.c:1652  */
    {
	yyval=N(); 
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	if(!maybe_append_simple_diving_code(*yyval, *yyvsp[-2]))
#endif
	{
		OA(*yyval, OP::OP_OBJECT_POOL, yyvsp[-2]); /* stack: empty write context */
		/* some code that writes to that context */
		/* context=pop; stack: context.value() */
	}
}
#line 2304 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 116:
#line 686 "compile.y" /* yacc.c:1652  */
    {
	yyval=N(); 
	O(*yyval, OP::OP_WITH_READ);
	P(*yyval, *yyvsp[0]);
}
#line 2314 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 118:
#line 691 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2320 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 119:
#line 692 "compile.y" /* yacc.c:1652  */
    {
	yyval=yyvsp[0];
	O(*yyval, OP::OP_GET_ELEMENT__WRITE);
}
#line 2329 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 122:
#line 701 "compile.y" /* yacc.c:1652  */
    {
	yyval=yyvsp[-1]; // stack: class name string
	OP::OPCODE code = OP::OP_VALUE__GET_CLASS;
	if(*LA2S(*yyval) == BASE_NAME) { // pseudo BASE class
		if(VStateless_class* base=PC.cclass->base_class()) {
			change_string_literal_value(*yyval, *new String(base->type()));
		} else {
			YYERROR1("no base class declared");
		}
		code = OP::OP_VALUE__GET_BASE_CLASS;
	} else {
		// can't use get_class because it will call @autouse[] if the class wasn't loaded
		VStateless_class* base=PC.request.classes().get(*LA2S(*yyval));
		if(base && PC.cclass->derived_from(*base))
			code = OP::OP_VALUE__GET_BASE_CLASS;
	}
	// optimized OP_VALUE+origin+string+OP_GET_CLASS => OP_VALUE__GET_CLASS+origin+string
	change_first(*yyval, OP::OP_VALUE, code);
}
#line 2353 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 123:
#line 720 "compile.y" /* yacc.c:1652  */
    {
	yyval=yyvsp[-1];
	if(!PC.in_call_value)
		YYERROR1(":: not allowed here");
	O(*yyval, OP::OP_PREPARE_TO_CONSTRUCT_OBJECT);
}
#line 2364 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 130:
#line 737 "compile.y" /* yacc.c:1652  */
    { yyval = yyvsp[-1]; }
#line 2370 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 131:
#line 738 "compile.y" /* yacc.c:1652  */
    { yyval = yyvsp[-1]; }
#line 2376 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 132:
#line 739 "compile.y" /* yacc.c:1652  */
    { yyval = yyvsp[-1]; }
#line 2382 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 133:
#line 741 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];  O(*yyval, OP::OP_NEG); }
#line 2388 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 134:
#line 742 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0]; }
#line 2394 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 135:
#line 743 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];	 O(*yyval, OP::OP_INV); }
#line 2400 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 136:
#line 744 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];  O(*yyval, OP::OP_NOT); }
#line 2406 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 137:
#line 745 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];  O(*yyval, OP::OP_DEF); }
#line 2412 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 138:
#line 746 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];  O(*yyval, OP::OP_IN); }
#line 2418 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 139:
#line 747 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];  O(*yyval, OP::OP_FEXISTS); }
#line 2424 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 140:
#line 748 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[0];  O(*yyval, OP::OP_DEXISTS); }
#line 2430 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 141:
#line 750 "compile.y" /* yacc.c:1652  */
    {	yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_SUB); }
#line 2436 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 142:
#line 751 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_ADD); }
#line 2442 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 143:
#line 752 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_MUL); }
#line 2448 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 144:
#line 753 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_DIV); }
#line 2454 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 145:
#line 754 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_MOD); }
#line 2460 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 146:
#line 755 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_INTDIV); }
#line 2466 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 147:
#line 756 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_SL); }
#line 2472 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 148:
#line 757 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_SR); }
#line 2478 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 149:
#line 758 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2]; 	P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_AND); }
#line 2484 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 150:
#line 759 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_OR); }
#line 2490 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 151:
#line 760 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_XOR); }
#line 2496 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 152:
#line 761 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  OA(*yyval, OP::OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP::OP_LOG_AND); }
#line 2502 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 153:
#line 762 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  OA(*yyval, OP::OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP::OP_LOG_OR); }
#line 2508 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 154:
#line 763 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_LOG_XOR); }
#line 2514 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 155:
#line 764 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_LT); }
#line 2520 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 156:
#line 765 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_GT); }
#line 2526 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 157:
#line 766 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_LE); }
#line 2532 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 158:
#line 767 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_GE); }
#line 2538 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 159:
#line 768 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_EQ); }
#line 2544 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 160:
#line 769 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_NE); }
#line 2550 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 161:
#line 770 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_LT); }
#line 2556 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 162:
#line 771 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_GT); }
#line 2562 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 163:
#line 772 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_LE); }
#line 2568 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 164:
#line 773 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_GE); }
#line 2574 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 165:
#line 774 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_EQ); }
#line 2580 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 166:
#line 775 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_NE); }
#line 2586 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 167:
#line 776 "compile.y" /* yacc.c:1652  */
    { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_IS); }
#line 2592 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 168:
#line 779 "compile.y" /* yacc.c:1652  */
    {
	// optimized OP_STRING => OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*(yyval=yyvsp[0]));
}
#line 2601 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 169:
#line 784 "compile.y" /* yacc.c:1652  */
    {
#ifdef OPTIMIZE_BYTECODE_STRING_POOL
	// it brakes ^if(" 09 "){...}
	YYSTYPE code=yyvsp[0];
	yyval=N();
	if(code->count()==3 && change_first(*code, OP::OP_STRING__WRITE, OP::OP_VALUE)){
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
}
#line 2624 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 170:
#line 805 "compile.y" /* yacc.c:1652  */
    {
	// optimized OP_STRING+OP_WRITE_VALUE => OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*(yyval=yyvsp[0]));
}
#line 2633 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 171:
#line 810 "compile.y" /* yacc.c:1652  */
    { yyval=VL(/*we know that we will not change it*/const_cast<VString*>(&vempty), 0, 0, 0); }
#line 2639 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 172:
#line 811 "compile.y" /* yacc.c:1652  */
    { yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vtrue), 0, 0, 0); }
#line 2645 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 173:
#line 812 "compile.y" /* yacc.c:1652  */
    { yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vfalse), 0, 0, 0); }
#line 2651 "compile.tab.C" /* yacc.c:1652  */
    break;

  case 174:
#line 814 "compile.y" /* yacc.c:1652  */
    { yyval=N(); }
#line 2657 "compile.tab.C" /* yacc.c:1652  */
    break;


#line 2661 "compile.tab.C" /* yacc.c:1652  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (pc, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (pc, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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
                      yytoken, &yylval, pc);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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
                  yystos[yystate], yyvsp, pc);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (pc, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, pc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, pc);
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
  return yyresult;
}
#line 816 "compile.y" /* yacc.c:1918  */

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
	Parse_control& pc=*static_cast<Parse_control*>(apc);

	#define lexical_brackets_nestage pc.brackets_nestages[pc.ls_sp]
	#define RC {result=c; goto break2; }

	int c;
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
				default:
					pc.string.append(c);
				case '\n': case ' ': case '\t':
					begin=pc.source;
					begin_pos=pc.pos;
					continue;
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
					(pc.ls == LS_EXPRESSION_STRING_QUOTED && c=='"') ||
					(pc.ls == LS_EXPRESSION_STRING_APOSTROFED && c=='\'') ) {
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
				if(--lexical_brackets_nestage==0) {
					if(pc.ls==LS_METHOD_ROUND) // method round param ended
						pc.ls=LS_METHOD_AFTER; // look for method end
					else // pc.ls==LS_VAR_ROUND // variable constructor ended
						pop_LS(pc); // return to normal life
				}
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
				default:
					pc.string.append(c);
				case '\n': case ' ': case '\t':
					begin=pc.source;
					begin_pos=pc.pos;
					continue;
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
#ifdef SYMBOLS_CACHING
		Value *lookup=symbols->get(pc.string);
#else
		Value *lookup=0;
#endif
		*lvalp=VL(lookup ? lookup : new VString(*new String(pc.string, String::L_CLEAN)), pc.file_no, pc.string_start.line, pc.string_start.col);
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

static int real_yyerror(Parse_control *pc, const char *s) {  // Called by yyparse on error
	PC.error=pa_strdup(s);
	return 1;
}

static void yyprint(FILE *file, int type, YYSTYPE value) {
	if(type==STRING)
		fprintf(file, " \"%s\"", LA2S(*value)->cstr());
}

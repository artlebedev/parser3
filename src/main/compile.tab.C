/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "compile.y"

/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>, Alexandr Petrosian <paf@design.ru>

	
*/

volatile const char * IDENT_COMPILE_Y = "$Id: compile.tab.C,v 1.198 2026/11/27 23:58:36 moko Exp $";

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

// Windows headers define IN
#ifdef IN
#undef IN
#endif

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


#line 156 "compile.tab.C"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
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


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    EON = 258,                     /* EON  */
    STRING = 259,                  /* STRING  */
    BOGUS = 260,                   /* BOGUS  */
    BAD_STRING_COMPARISON_OPERATOR = 261, /* BAD_STRING_COMPARISON_OPERATOR  */
    BAD_HEX_LITERAL = 262,         /* BAD_HEX_LITERAL  */
    BAD_METHOD_DECL_START = 263,   /* BAD_METHOD_DECL_START  */
    BAD_METHOD_PARAMETER_NAME_CHARACTER = 264, /* BAD_METHOD_PARAMETER_NAME_CHARACTER  */
    LAND = 265,                    /* "&&"  */
    LOR = 266,                     /* "||"  */
    LXOR = 267,                    /* "!||"  */
    NXOR = 268,                    /* "!|"  */
    NLE = 269,                     /* "<="  */
    NGE = 270,                     /* ">="  */
    NEQ = 271,                     /* "=="  */
    NNE = 272,                     /* "!="  */
    NSL = 273,                     /* "<<"  */
    NSR = 274,                     /* ">>"  */
    SLT = 275,                     /* "lt"  */
    SGT = 276,                     /* "gt"  */
    SLE = 277,                     /* "le"  */
    SGE = 278,                     /* "ge"  */
    SEQ = 279,                     /* "eq"  */
    SNE = 280,                     /* "ne"  */
    DEF = 281,                     /* "def"  */
    IN = 282,                      /* "in"  */
    FEXISTS = 283,                 /* "-f"  */
    DEXISTS = 284,                 /* "-d"  */
    LEXISTS = 285,                 /* "-l"  */
    IS = 286,                      /* "is"  */
    LITERAL_TRUE = 287,            /* "true"  */
    LITERAL_FALSE = 288,           /* "false"  */
    NUNARY = 289                   /* NUNARY  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif




int yyparse (Parse_control* pc);



/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_EON = 3,                        /* EON  */
  YYSYMBOL_STRING = 4,                     /* STRING  */
  YYSYMBOL_BOGUS = 5,                      /* BOGUS  */
  YYSYMBOL_BAD_STRING_COMPARISON_OPERATOR = 6, /* BAD_STRING_COMPARISON_OPERATOR  */
  YYSYMBOL_BAD_HEX_LITERAL = 7,            /* BAD_HEX_LITERAL  */
  YYSYMBOL_BAD_METHOD_DECL_START = 8,      /* BAD_METHOD_DECL_START  */
  YYSYMBOL_BAD_METHOD_PARAMETER_NAME_CHARACTER = 9, /* BAD_METHOD_PARAMETER_NAME_CHARACTER  */
  YYSYMBOL_LAND = 10,                      /* "&&"  */
  YYSYMBOL_LOR = 11,                       /* "||"  */
  YYSYMBOL_LXOR = 12,                      /* "!||"  */
  YYSYMBOL_NXOR = 13,                      /* "!|"  */
  YYSYMBOL_NLE = 14,                       /* "<="  */
  YYSYMBOL_NGE = 15,                       /* ">="  */
  YYSYMBOL_NEQ = 16,                       /* "=="  */
  YYSYMBOL_NNE = 17,                       /* "!="  */
  YYSYMBOL_NSL = 18,                       /* "<<"  */
  YYSYMBOL_NSR = 19,                       /* ">>"  */
  YYSYMBOL_SLT = 20,                       /* "lt"  */
  YYSYMBOL_SGT = 21,                       /* "gt"  */
  YYSYMBOL_SLE = 22,                       /* "le"  */
  YYSYMBOL_SGE = 23,                       /* "ge"  */
  YYSYMBOL_SEQ = 24,                       /* "eq"  */
  YYSYMBOL_SNE = 25,                       /* "ne"  */
  YYSYMBOL_DEF = 26,                       /* "def"  */
  YYSYMBOL_IN = 27,                        /* "in"  */
  YYSYMBOL_FEXISTS = 28,                   /* "-f"  */
  YYSYMBOL_DEXISTS = 29,                   /* "-d"  */
  YYSYMBOL_LEXISTS = 30,                   /* "-l"  */
  YYSYMBOL_IS = 31,                        /* "is"  */
  YYSYMBOL_LITERAL_TRUE = 32,              /* "true"  */
  YYSYMBOL_LITERAL_FALSE = 33,             /* "false"  */
  YYSYMBOL_34_ = 34,                       /* '<'  */
  YYSYMBOL_35_ = 35,                       /* '>'  */
  YYSYMBOL_36_ = 36,                       /* '|'  */
  YYSYMBOL_37_ = 37,                       /* '&'  */
  YYSYMBOL_38_ = 38,                       /* '+'  */
  YYSYMBOL_39_ = 39,                       /* '-'  */
  YYSYMBOL_40_ = 40,                       /* '*'  */
  YYSYMBOL_41_ = 41,                       /* '/'  */
  YYSYMBOL_42_ = 42,                       /* '\\'  */
  YYSYMBOL_43_ = 43,                       /* '%'  */
  YYSYMBOL_NUNARY = 44,                    /* NUNARY  */
  YYSYMBOL_45_ = 45,                       /* '~'  */
  YYSYMBOL_46_ = 46,                       /* '!'  */
  YYSYMBOL_47_ = 47,                       /* '@'  */
  YYSYMBOL_48_n_ = 48,                     /* '\n'  */
  YYSYMBOL_49_ = 49,                       /* '['  */
  YYSYMBOL_50_ = 50,                       /* ']'  */
  YYSYMBOL_51_ = 51,                       /* ';'  */
  YYSYMBOL_52_ = 52,                       /* '$'  */
  YYSYMBOL_53_ = 53,                       /* '{'  */
  YYSYMBOL_54_ = 54,                       /* '}'  */
  YYSYMBOL_55_ = 55,                       /* '.'  */
  YYSYMBOL_56_ = 56,                       /* '('  */
  YYSYMBOL_57_ = 57,                       /* ')'  */
  YYSYMBOL_58_ = 58,                       /* '^'  */
  YYSYMBOL_59_ = 59,                       /* ':'  */
  YYSYMBOL_60_ = 60,                       /* '"'  */
  YYSYMBOL_61_ = 61,                       /* '\''  */
  YYSYMBOL_YYACCEPT = 62,                  /* $accept  */
  YYSYMBOL_all = 63,                       /* all  */
  YYSYMBOL_methods = 64,                   /* methods  */
  YYSYMBOL_one_big_piece = 65,             /* one_big_piece  */
  YYSYMBOL_method = 66,                    /* method  */
  YYSYMBOL_control_method = 67,            /* control_method  */
  YYSYMBOL_maybe_control_strings = 68,     /* maybe_control_strings  */
  YYSYMBOL_control_strings = 69,           /* control_strings  */
  YYSYMBOL_control_string = 70,            /* control_string  */
  YYSYMBOL_maybe_string = 71,              /* maybe_string  */
  YYSYMBOL_code_method = 72,               /* code_method  */
  YYSYMBOL_73_1 = 73,                      /* @1  */
  YYSYMBOL_maybe_bracketed_strings = 74,   /* maybe_bracketed_strings  */
  YYSYMBOL_bracketed_maybe_strings = 75,   /* bracketed_maybe_strings  */
  YYSYMBOL_maybe_strings = 76,             /* maybe_strings  */
  YYSYMBOL_strings = 77,                   /* strings  */
  YYSYMBOL_maybe_comment = 78,             /* maybe_comment  */
  YYSYMBOL_maybe_codes = 79,               /* maybe_codes  */
  YYSYMBOL_codes = 80,                     /* codes  */
  YYSYMBOL_code = 81,                      /* code  */
  YYSYMBOL_action = 82,                    /* action  */
  YYSYMBOL_get = 83,                       /* get  */
  YYSYMBOL_get_value = 84,                 /* get_value  */
  YYSYMBOL_get_name_value = 85,            /* get_name_value  */
  YYSYMBOL_name_in_curly_rdive = 86,       /* name_in_curly_rdive  */
  YYSYMBOL_name_without_curly_rdive = 87,  /* name_without_curly_rdive  */
  YYSYMBOL_name_without_curly_rdive_read = 88, /* name_without_curly_rdive_read  */
  YYSYMBOL_name_without_curly_rdive_class = 89, /* name_without_curly_rdive_class  */
  YYSYMBOL_name_without_curly_rdive_code = 90, /* name_without_curly_rdive_code  */
  YYSYMBOL_put = 91,                       /* put  */
  YYSYMBOL_name_expr_wdive = 92,           /* name_expr_wdive  */
  YYSYMBOL_name_expr_wdive_root = 93,      /* name_expr_wdive_root  */
  YYSYMBOL_name_expr_wdive_write = 94,     /* name_expr_wdive_write  */
  YYSYMBOL_name_expr_wdive_class = 95,     /* name_expr_wdive_class  */
  YYSYMBOL_construct = 96,                 /* construct  */
  YYSYMBOL_construct_square = 97,          /* construct_square  */
  YYSYMBOL_98_2 = 98,                      /* @2  */
  YYSYMBOL_99_3 = 99,                      /* $@3  */
  YYSYMBOL_any_constructor_code_values = 100, /* any_constructor_code_values  */
  YYSYMBOL_construct_round = 101,          /* construct_round  */
  YYSYMBOL_construct_curly = 102,          /* construct_curly  */
  YYSYMBOL_any_constructor_code_value = 103, /* any_constructor_code_value  */
  YYSYMBOL_constructor_code_value = 104,   /* constructor_code_value  */
  YYSYMBOL_constructor_code = 105,         /* constructor_code  */
  YYSYMBOL_codes__excluding_sole_str_literal = 106, /* codes__excluding_sole_str_literal  */
  YYSYMBOL_call = 107,                     /* call  */
  YYSYMBOL_call_value = 108,               /* call_value  */
  YYSYMBOL_109_4 = 109,                    /* $@4  */
  YYSYMBOL_110_5 = 110,                    /* $@5  */
  YYSYMBOL_call_name = 111,                /* call_name  */
  YYSYMBOL_store_params = 112,             /* store_params  */
  YYSYMBOL_store_param = 113,              /* store_param  */
  YYSYMBOL_store_square_param = 114,       /* store_square_param  */
  YYSYMBOL_115_6 = 115,                    /* @6  */
  YYSYMBOL_116_7 = 116,                    /* $@7  */
  YYSYMBOL_store_round_param = 117,        /* store_round_param  */
  YYSYMBOL_store_curly_param = 118,        /* store_curly_param  */
  YYSYMBOL_store_code_param_parts = 119,   /* store_code_param_parts  */
  YYSYMBOL_store_expr_param_parts = 120,   /* store_expr_param_parts  */
  YYSYMBOL_store_curly_param_parts = 121,  /* store_curly_param_parts  */
  YYSYMBOL_store_code_param_part = 122,    /* store_code_param_part  */
  YYSYMBOL_store_expr_param_part = 123,    /* store_expr_param_part  */
  YYSYMBOL_store_curly_param_part = 124,   /* store_curly_param_part  */
  YYSYMBOL_code_param_value = 125,         /* code_param_value  */
  YYSYMBOL_name_expr_dive_code = 126,      /* name_expr_dive_code  */
  YYSYMBOL_name_path = 127,                /* name_path  */
  YYSYMBOL_name_step = 128,                /* name_step  */
  YYSYMBOL_name_advance1 = 129,            /* name_advance1  */
  YYSYMBOL_name_advance2 = 130,            /* name_advance2  */
  YYSYMBOL_name_expr_value = 131,          /* name_expr_value  */
  YYSYMBOL_name_expr_subvar_value = 132,   /* name_expr_subvar_value  */
  YYSYMBOL_name_expr_with_subvar_value = 133, /* name_expr_with_subvar_value  */
  YYSYMBOL_name_square_code_value = 134,   /* name_square_code_value  */
  YYSYMBOL_135_8 = 135,                    /* @8  */
  YYSYMBOL_136_9 = 136,                    /* $@9  */
  YYSYMBOL_name_round_expr_value = 137,    /* name_round_expr_value  */
  YYSYMBOL_subvar_ref_name_rdive = 138,    /* subvar_ref_name_rdive  */
  YYSYMBOL_subvar_get_writes = 139,        /* subvar_get_writes  */
  YYSYMBOL_subvar__get_write = 140,        /* subvar__get_write  */
  YYSYMBOL_class_prefix = 141,             /* class_prefix  */
  YYSYMBOL_class_static_prefix = 142,      /* class_static_prefix  */
  YYSYMBOL_class_constructor_prefix = 143, /* class_constructor_prefix  */
  YYSYMBOL_expr_value = 144,               /* expr_value  */
  YYSYMBOL_expr = 145,                     /* expr  */
  YYSYMBOL_double_or_STRING = 146,         /* double_or_STRING  */
  YYSYMBOL_string_inside_quotes_value = 147, /* string_inside_quotes_value  */
  YYSYMBOL_write_string = 148,             /* write_string  */
  YYSYMBOL_empty_value = 149,              /* empty_value  */
  YYSYMBOL_true_value = 150,               /* true_value  */
  YYSYMBOL_false_value = 151,              /* false_value  */
  YYSYMBOL_empty = 152                     /* empty  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if 1

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
#endif /* 1 */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  53
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   564

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  62
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  91
/* YYNRULES -- Number of rules.  */
#define YYNRULES  177
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  272

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   289


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
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
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   154,   154,   159,   161,   161,   162,   164,   164,   166,
     242,   242,   243,   243,   244,   245,   245,   247,   247,   295,
     295,   296,   297,   297,   298,   298,   300,   300,   304,   304,
     306,   306,   307,   307,   308,   308,   308,   312,   343,   344,
     344,   345,   347,   348,   349,   398,   399,   399,   403,   416,
     417,   418,   419,   441,   446,   449,   450,   451,   453,   457,
     453,   472,   473,   475,   482,   489,   490,   491,   493,   499,
     500,   500,   504,   515,   518,   515,   567,   583,   583,   585,
     586,   587,   589,   592,   589,   595,   596,   598,   599,   602,
     603,   606,   607,   609,   612,   625,   630,   631,   632,   637,
     637,   639,   639,   640,   641,   653,   662,   665,   666,   667,
     668,   669,   671,   675,   684,   687,   684,   697,   701,   706,
     706,   707,   713,   714,   716,   735,   745,   747,   748,   749,
     750,   751,   752,   753,   754,   756,   757,   758,   759,   760,
     761,   762,   763,   764,   766,   767,   768,   769,   770,   771,
     772,   773,   774,   775,   776,   777,   778,   779,   780,   781,
     782,   783,   784,   785,   786,   787,   788,   789,   790,   791,
     792,   795,   800,   821,   826,   827,   828,   830
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if 1
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "EON", "STRING",
  "BOGUS", "BAD_STRING_COMPARISON_OPERATOR", "BAD_HEX_LITERAL",
  "BAD_METHOD_DECL_START", "BAD_METHOD_PARAMETER_NAME_CHARACTER", "\"&&\"",
  "\"||\"", "\"!||\"", "\"!|\"", "\"<=\"", "\">=\"", "\"==\"", "\"!=\"",
  "\"<<\"", "\">>\"", "\"lt\"", "\"gt\"", "\"le\"", "\"ge\"", "\"eq\"",
  "\"ne\"", "\"def\"", "\"in\"", "\"-f\"", "\"-d\"", "\"-l\"", "\"is\"",
  "\"true\"", "\"false\"", "'<'", "'>'", "'|'", "'&'", "'+'", "'-'", "'*'",
  "'/'", "'\\\\'", "'%'", "NUNARY", "'~'", "'!'", "'@'", "'\\n'", "'['",
  "']'", "';'", "'$'", "'{'", "'}'", "'.'", "'('", "')'", "'^'", "':'",
  "'\"'", "'\\''", "$accept", "all", "methods", "one_big_piece", "method",
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
  "name_round_expr_value", "subvar_ref_name_rdive", "subvar_get_writes",
  "subvar__get_write", "class_prefix", "class_static_prefix",
  "class_constructor_prefix", "expr_value", "expr", "double_or_STRING",
  "string_inside_quotes_value", "write_string", "empty_value",
  "true_value", "false_value", "empty", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-238)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-178)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
       3,  -238,    24,    53,  -238,    39,   -11,  -238,  -238,  -238,
    -238,  -238,     7,  -238,  -238,  -238,  -238,  -238,  -238,  -238,
    -238,  -238,    -8,    18,  -238,    45,    30,    43,   250,  -238,
    -238,    55,  -238,  -238,  -238,    74,  -238,  -238,  -238,  -238,
      93,  -238,    12,  -238,    48,  -238,  -238,  -238,  -238,    93,
      28,  -238,    30,  -238,  -238,  -238,    52,    56,    13,  -238,
      45,  -238,    46,  -238,     7,  -238,  -238,    37,    93,    73,
      93,    46,  -238,    43,    73,  -238,   250,   250,   250,   250,
     250,  -238,  -238,   250,   250,   250,   250,    80,   250,     7,
       7,  -238,  -238,    50,   350,  -238,  -238,  -238,  -238,  -238,
       7,   250,  -238,  -238,  -238,  -238,    17,  -238,  -238,    48,
    -238,  -238,  -238,  -238,  -238,  -238,  -238,  -238,    15,  -238,
      63,    89,  -238,    88,    92,  -238,   136,  -238,  -238,  -238,
    -238,     7,  -238,    73,   488,   488,   488,   488,   488,  -238,
    -238,  -238,  -238,   302,  -238,    90,    97,  -238,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,   250,   250,   250,   250,   250,
     250,   250,   250,   250,   250,    14,   106,   104,    95,  -238,
    -238,  -238,  -238,   158,  -238,   115,  -238,   122,  -238,  -238,
    -238,   449,   418,   384,   310,   480,   480,   128,   128,   173,
     173,   480,   480,   480,   480,   128,   128,   488,   480,   480,
     495,   521,   112,   112,  -238,  -238,  -238,  -238,     2,     7,
      67,   123,  -238,  -238,  -238,  -238,  -238,  -238,  -238,  -238,
       7,   250,    32,  -238,  -238,  -238,  -238,  -238,  -238,  -238,
       7,    14,   126,    22,  -238,   -21,  -238,   -26,  -238,  -238,
    -238,  -238,     7,  -238,  -238,    75,  -238,   127,  -238,  -238,
    -238,     7,  -238,   250,  -238,  -238,    22,   130,  -238,  -238,
    -238,  -238
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
     177,   173,     0,     0,    73,     0,     3,     2,     4,     7,
       8,     6,    29,    30,    33,    34,    37,    35,    36,    72,
      32,    28,     0,   107,   114,     0,     0,     0,     0,    38,
      40,     0,    42,    43,    44,     0,    49,    50,    51,    52,
       0,   101,     0,    46,    99,   108,   109,   110,   111,     0,
     122,   123,     0,     1,     5,    31,   177,   177,   177,   106,
       0,   124,   113,   119,     0,   118,   112,     0,     0,   105,
       0,   107,    53,     0,    99,   171,     0,     0,     0,     0,
       0,   175,   176,     0,     0,     0,     0,     0,     0,   177,
     177,   130,   131,     0,   126,   127,   128,   129,    39,    58,
     177,     0,    48,    55,    56,    57,   107,   102,    47,   100,
     103,    45,    54,   125,    76,    74,    16,     9,    11,    12,
       0,    10,    24,     0,    23,    22,   177,    20,    19,   121,
     120,   115,    41,   100,   139,   140,   141,   142,   143,   136,
     135,   137,   138,     0,   172,     0,     0,   117,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   174,     0,     0,     0,    13,
      15,    14,    21,     0,    27,     0,    26,     0,   134,   132,
     133,   155,   156,   157,   154,   160,   161,   162,   163,   150,
     151,   164,   165,   166,   167,   168,   169,   170,   158,   159,
     153,   152,   145,   144,   146,   147,   149,   148,   173,     0,
      33,    59,    61,    67,    68,    69,    65,    64,    63,    82,
     177,     0,     0,    77,    79,    80,    81,    25,    17,   116,
      71,   174,     0,   174,    95,     0,    91,     0,    89,    94,
      75,    78,   177,    62,    60,   173,    98,    83,    87,    93,
      96,   177,    86,     0,    85,    18,   174,     0,    92,    90,
      88,    84
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -238,  -238,  -238,  -238,   167,  -238,  -238,  -238,    59,  -238,
    -238,  -238,  -238,   124,  -238,  -238,  -238,     1,   -62,     8,
    -170,  -238,     0,  -238,  -238,    -9,  -238,  -238,   -22,  -238,
    -238,  -238,  -238,  -238,  -238,  -238,  -238,  -238,  -238,  -238,
    -238,   -60,  -237,  -238,  -238,  -238,     4,  -238,  -238,  -238,
    -238,   -15,  -238,  -238,  -238,  -238,  -238,  -238,  -238,  -238,
     -48,   -42,   -41,  -238,    -3,   107,   -31,  -238,   -30,     5,
    -238,  -238,  -238,  -238,  -238,  -238,   162,  -238,   163,   221,
    -238,  -238,   -25,    36,  -238,   137,  -238,  -222,  -238,  -238,
     -43
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,     5,     6,     7,     8,     9,   117,   118,   119,   120,
      10,   252,   126,    58,   123,   124,   185,   144,    12,    13,
      14,    15,    91,    29,    30,    31,    32,    33,    34,    17,
      35,    36,    37,    38,   102,   103,   175,   242,   221,   104,
     105,   222,   223,   224,   225,    18,    92,    52,   178,   115,
     232,   233,   234,   243,   267,   235,   236,   257,   247,   245,
     258,   248,   246,   259,    39,    68,    41,    42,    43,    69,
      45,    46,    47,    64,   187,    48,    66,    62,    63,    70,
      50,    51,   249,    94,    95,   145,    20,   226,    96,    97,
      21
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      16,    11,   131,    93,    19,   220,   256,     1,    44,   107,
     108,     1,    16,   121,   125,   128,    19,    67,   218,   116,
      55,   260,    59,    59,    72,   263,   255,   111,    22,   256,
     261,   264,    74,   262,    23,   250,     2,   107,   108,    53,
      56,    57,   107,   114,   260,   109,   112,    71,   111,    65,
       2,  -105,   -66,   -66,    44,     3,   116,    23,    98,     3,
     122,     4,    57,  -177,    16,     4,     3,   110,    19,    60,
      60,   220,     4,   220,     3,   180,   177,    61,   133,    24,
       4,   229,    25,   186,    23,   230,    28,   113,   231,    16,
      16,   132,    24,    19,    19,    25,   220,   106,    60,    28,
      16,   176,    24,  -104,    19,    25,    26,   147,    27,    28,
      40,   181,   134,   135,   136,   137,   138,   -70,   -70,   139,
     140,   141,   142,    99,   143,   -97,   -97,   100,  -104,    24,
     101,    16,    25,    26,    73,    19,    28,   -15,   182,    55,
     184,   151,    24,   183,   229,    25,   156,   157,   230,    28,
     189,   231,   171,   172,   173,   174,    40,   240,   190,   164,
     227,   228,   237,   238,   167,   168,   169,   170,   171,   172,
     173,   174,   239,    54,   241,    16,   254,   179,   266,    19,
     271,   253,   127,   219,   191,   192,   193,   194,   195,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   169,   170,   171,   172,   173,   174,   251,   270,    16,
     268,   269,   129,    19,    49,   130,     0,   146,     0,     0,
      16,   244,     0,     0,    19,     0,     0,     0,     0,     0,
      16,    16,     0,    16,    19,    19,     0,    19,    55,   219,
       0,   219,    16,   265,    75,     0,    19,     0,     0,     0,
       0,    16,   244,     0,     0,    19,    16,     0,     0,     0,
      19,     0,     0,     0,   219,     0,    76,    77,    78,    79,
      80,     0,    81,    82,     0,     0,     0,     0,    83,    84,
       0,     0,     0,     0,     0,    85,    86,     0,     0,     0,
       0,     0,    87,     0,     0,     0,    88,     0,     4,     0,
      89,    90,   148,   149,   150,   151,   152,   153,   154,   155,
     156,   157,   158,   159,   160,   161,   162,   163,   156,   157,
       0,     0,     0,   164,     0,     0,   165,   166,   167,   168,
     169,   170,   171,   172,   173,   174,   167,   168,   169,   170,
     171,   172,   173,   174,     0,     0,     0,     0,     0,   188,
     148,   149,   150,   151,   152,   153,   154,   155,   156,   157,
     158,   159,   160,   161,   162,   163,     0,     0,     0,     0,
       0,   164,     0,     0,   165,   166,   167,   168,   169,   170,
     171,   172,   173,   174,   148,   149,     0,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
       0,     0,     0,     0,     0,   164,     0,     0,   165,   166,
     167,   168,   169,   170,   171,   172,   173,   174,   148,     0,
       0,   151,   152,   153,   154,   155,   156,   157,   158,   159,
     160,   161,   162,   163,     0,     0,     0,     0,     0,   164,
       0,     0,   165,   166,   167,   168,   169,   170,   171,   172,
     173,   174,   151,   152,   153,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,     0,     0,     0,     0,     0,
     164,     0,     0,   165,   166,   167,   168,   169,   170,   171,
     172,   173,   174,   151,     0,     0,   154,   155,   156,   157,
       0,   151,     0,     0,   162,   163,   156,   157,     0,     0,
       0,   164,     0,   156,   157,     0,   167,   168,   169,   170,
     171,   172,   173,   174,   167,   168,   169,   170,   171,   172,
     173,   174,   168,   169,   170,   171,   172,   173,   174,   156,
     157,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   169,
     170,   171,   172,   173,   174
};

static const yytype_int16 yycheck[] =
{
       0,     0,    64,    28,     0,   175,   243,     4,     3,    40,
      40,     4,    12,    56,    57,    58,    12,    26,     4,     4,
      12,   243,     5,     5,    27,    51,     4,    49,     4,   266,
      51,    57,    27,    54,     4,     3,    47,    68,    68,     0,
      48,    49,    73,    52,   266,    40,    49,     4,    70,     4,
      47,     3,    50,    51,    49,    52,     4,     4,     3,    52,
       4,    58,    49,    48,    64,    58,    52,    55,    64,    52,
      52,   241,    58,   243,    52,   118,   101,    59,    73,    49,
      58,    49,    52,   126,     4,    53,    56,    59,    56,    89,
      90,    54,    49,    89,    90,    52,   266,     4,    52,    56,
     100,   100,    49,    55,   100,    52,    53,    57,    55,    56,
       3,    48,    76,    77,    78,    79,    80,    50,    51,    83,
      84,    85,    86,    49,    88,    50,    51,    53,    55,    49,
      56,   131,    52,    53,    27,   131,    56,    48,    50,   131,
       4,    13,    49,    51,    49,    52,    18,    19,    53,    56,
      60,    56,    40,    41,    42,    43,    49,   219,    61,    31,
      54,    57,     4,    48,    36,    37,    38,    39,    40,    41,
      42,    43,    50,     6,    51,   175,    50,   118,    51,   175,
      50,   241,    58,   175,   148,   149,   150,   151,   152,   153,
     154,   155,   156,   157,   158,   159,   160,   161,   162,   163,
     164,   165,   166,   167,   168,   169,   170,   171,   172,   173,
     174,    38,    39,    40,    41,    42,    43,   232,   266,   219,
     261,   263,    60,   219,     3,    62,    -1,    90,    -1,    -1,
     230,   230,    -1,    -1,   230,    -1,    -1,    -1,    -1,    -1,
     240,   241,    -1,   243,   240,   241,    -1,   243,   240,   241,
      -1,   243,   252,   252,     4,    -1,   252,    -1,    -1,    -1,
      -1,   261,   261,    -1,    -1,   261,   266,    -1,    -1,    -1,
     266,    -1,    -1,    -1,   266,    -1,    26,    27,    28,    29,
      30,    -1,    32,    33,    -1,    -1,    -1,    -1,    38,    39,
      -1,    -1,    -1,    -1,    -1,    45,    46,    -1,    -1,    -1,
      -1,    -1,    52,    -1,    -1,    -1,    56,    -1,    58,    -1,
      60,    61,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    24,    25,    18,    19,
      -1,    -1,    -1,    31,    -1,    -1,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    36,    37,    38,    39,
      40,    41,    42,    43,    -1,    -1,    -1,    -1,    -1,    57,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    -1,    -1,    -1,    -1,
      -1,    31,    -1,    -1,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    10,    11,    -1,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      -1,    -1,    -1,    -1,    -1,    31,    -1,    -1,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    10,    -1,
      -1,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,    31,
      -1,    -1,    34,    35,    36,    37,    38,    39,    40,    41,
      42,    43,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    -1,    -1,    -1,    -1,    -1,
      31,    -1,    -1,    34,    35,    36,    37,    38,    39,    40,
      41,    42,    43,    13,    -1,    -1,    16,    17,    18,    19,
      -1,    13,    -1,    -1,    24,    25,    18,    19,    -1,    -1,
      -1,    31,    -1,    18,    19,    -1,    36,    37,    38,    39,
      40,    41,    42,    43,    36,    37,    38,    39,    40,    41,
      42,    43,    37,    38,    39,    40,    41,    42,    43,    18,
      19,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    38,
      39,    40,    41,    42,    43
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,    47,    52,    58,    63,    64,    65,    66,    67,
      72,    79,    80,    81,    82,    83,    84,    91,   107,   108,
     148,   152,     4,     4,    49,    52,    53,    55,    56,    85,
      86,    87,    88,    89,    90,    92,    93,    94,    95,   126,
     127,   128,   129,   130,   131,   132,   133,   134,   137,   141,
     142,   143,   109,     0,    66,    81,    48,    49,    75,     5,
      52,    59,   139,   140,   135,     4,   138,    87,   127,   131,
     141,     4,   126,   127,   131,     4,    26,    27,    28,    29,
      30,    32,    33,    38,    39,    45,    46,    52,    56,    60,
      61,    84,   108,   144,   145,   146,   150,   151,     3,    49,
      53,    56,    96,    97,   101,   102,     4,   128,   130,   131,
      55,    90,   126,    59,    87,   111,     4,    68,    69,    70,
      71,   152,     4,    76,    77,   152,    74,    75,   152,   138,
     140,    80,    54,   131,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,    79,   147,   147,    57,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    31,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    98,    79,   144,   110,    70,
     152,    48,    50,    51,     4,    78,   152,   136,    57,    60,
      61,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,     4,    81,
      82,   100,   103,   104,   105,   106,   149,    54,    57,    49,
      53,    56,   112,   113,   114,   117,   118,     4,    48,    50,
      80,    51,    99,   115,    79,   121,   124,   120,   123,   144,
       3,   113,    73,   103,    50,     4,   104,   119,   122,   125,
     149,    51,    54,    51,    57,    79,    51,   116,   124,   123,
     122,    50
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    62,    63,    63,    64,    64,    65,    66,    66,    67,
      68,    68,    69,    69,    70,    71,    71,    73,    72,    74,
      74,    75,    76,    76,    77,    77,    78,    78,    79,    79,
      80,    80,    81,    81,    82,    82,    82,    83,    84,    85,
      85,    86,    87,    87,    88,    89,    90,    90,    91,    92,
      92,    92,    93,    94,    95,    96,    96,    96,    98,    99,
      97,   100,   100,   101,   102,   103,   103,   103,   104,   105,
     106,   106,   107,   109,   110,   108,   111,   112,   112,   113,
     113,   113,   115,   116,   114,   117,   118,   119,   119,   120,
     120,   121,   121,   122,   123,   124,   125,   125,   125,   126,
     126,   127,   127,   128,   129,   130,   130,   131,   131,   131,
     131,   131,   132,   133,   135,   136,   134,   137,   138,   139,
     139,   140,   141,   141,   142,   143,   144,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     145,   146,   147,   148,   149,   150,   151,   152
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
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
       1,     1,     2,     2,     0,     0,     5,     3,     1,     1,
       2,     2,     1,     1,     2,     2,     1,     1,     1,     1,
       1,     1,     3,     3,     3,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     1,     1,     1,     0,     1,     1,     0
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, pc); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, Parse_control* pc)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (pc);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, Parse_control* pc)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep, pc);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule, Parse_control* pc)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)], pc);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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


/* Context of a parse error.  */
typedef struct
{
  yy_state_t *yyssp;
  yysymbol_kind_t yytoken;
} yypcontext_t;

/* Put in YYARG at most YYARGN of the expected tokens given the
   current YYCTX, and return the number of tokens stored in YYARG.  If
   YYARG is null, return the number of expected tokens (guaranteed to
   be less than YYNTOKENS).  Return YYENOMEM on memory exhaustion.
   Return 0 if there are more than YYARGN expected tokens, yet fill
   YYARG up to YYARGN. */
static int
yypcontext_expected_tokens (const yypcontext_t *yyctx,
                            yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
  int yycount = 0;
  int yyn = yypact[+*yyctx->yyssp];
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
        if (yycheck[yyx + yyn] == yyx && yyx != YYSYMBOL_YYerror
            && !yytable_value_is_error (yytable[yyx + yyn]))
          {
            if (!yyarg)
              ++yycount;
            else if (yycount == yyargn)
              return 0;
            else
              yyarg[yycount++] = YY_CAST (yysymbol_kind_t, yyx);
          }
    }
  if (yyarg && yycount == 0 && 0 < yyargn)
    yyarg[0] = YYSYMBOL_YYEMPTY;
  return yycount;
}




#ifndef yystrlen
# if defined __GLIBC__ && defined _STRING_H
#  define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
# else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
# endif
#endif

#ifndef yystpcpy
# if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#  define yystpcpy stpcpy
# else
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
# endif
#endif

#ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
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

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
#endif


static int
yy_syntax_error_arguments (const yypcontext_t *yyctx,
                           yysymbol_kind_t yyarg[], int yyargn)
{
  /* Actual size of YYARG. */
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
  if (yyctx->yytoken != YYSYMBOL_YYEMPTY)
    {
      int yyn;
      if (yyarg)
        yyarg[yycount] = yyctx->yytoken;
      ++yycount;
      yyn = yypcontext_expected_tokens (yyctx,
                                        yyarg ? yyarg + 1 : yyarg, yyargn - 1);
      if (yyn == YYENOMEM)
        return YYENOMEM;
      else
        yycount += yyn;
    }
  return yycount;
}

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return -1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return YYENOMEM if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                const yypcontext_t *yyctx)
{
  enum { YYARGS_MAX = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  yysymbol_kind_t yyarg[YYARGS_MAX];
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* Actual size of YYARG. */
  int yycount = yy_syntax_error_arguments (yyctx, yyarg, YYARGS_MAX);
  if (yycount == YYENOMEM)
    return YYENOMEM;

  switch (yycount)
    {
#define YYCASE_(N, S)                       \
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
#undef YYCASE_
    }

  /* Compute error message size.  Don't count the "%s"s, but reserve
     room for the terminator.  */
  yysize = yystrlen (yyformat) - 2 * yycount + 1;
  {
    int yyi;
    for (yyi = 0; yyi < yycount; ++yyi)
      {
        YYPTRDIFF_T yysize1
          = yysize + yytnamerr (YY_NULLPTR, yytname[yyarg[yyi]]);
        if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
          yysize = yysize1;
        else
          return YYENOMEM;
      }
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return -1;
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
          yyp += yytnamerr (yyp, yytname[yyarg[yyi++]]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, Parse_control* pc)
{
  YY_USE (yyvaluep);
  YY_USE (pc);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}






/*----------.
| yyparse.  |
`----------*/

int
yyparse (Parse_control* pc)
{
/* Lookahead token kind.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs = 0;

    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

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
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex (&yylval, pc);
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
  case 2: /* all: one_big_piece  */
#line 154 "compile.y"
                      {
	Method* method=new Method(Method::CT_ANY, 0, 0 /*min, max numbered_params_count*/, 0 /*param_names*/, 0 /*local_names*/, yyvsp[0] /*parser_code*/, 0 /*native_code*/, PC.cclass->is_vars_local());
	PC.cclass->set_method(PC.alias_method(main_method_name), method);
	RECORD(MAIN, &PC.alias_method(main_method_name), method);
}
#line 1928 "compile.tab.C"
    break;

  case 9: /* control_method: '@' STRING '\n' maybe_control_strings  */
#line 167 "compile.y"
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
			RECORD(CLASS, &name);
		} else {
			YYERROR1("@" CLASS_NAME " must contain only one line with class name (contains more than one)");
		}
	} else if(command==USE_CONTROL_METHOD_NAME) {
		CLASS_ADD;
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE){
			const String& file_name=LA2S(*strings_code, i)->trim(String::TRIM_END);
			PC.request.use_file(file_name, PC.request.get_used_filespec(PC.file_no), strings_code->get(i+1).origin);
			RECORD_USE(&file_name, strings_code->get(i+1).origin);
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
				RECORD(BASE, &base_name);
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
			RECORD(OPTION, &option);
		}
	} else {
		YYERROR3("'", command.cstr(), "' invalid special name. valid names are '" CLASS_NAME "', '" USE_CONTROL_METHOD_NAME "', '" BASE_NAME "' and '" OPTIONS_CONTROL_METHOD_NAME "'.");
	}
}
#line 2008 "compile.tab.C"
    break;

  case 13: /* control_strings: control_strings control_string  */
#line 243 "compile.y"
                                                                 { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2014 "compile.tab.C"
    break;

  case 17: /* @1: %empty  */
#line 247 "compile.y"
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
#line 2057 "compile.tab.C"
    break;

  case 18: /* code_method: '@' STRING bracketed_maybe_strings maybe_bracketed_strings maybe_comment '\n' @1 maybe_codes  */
#line 284 "compile.y"
              {
		Method* method=reinterpret_cast<Method*>(yyvsp[-1]);
		// fill in the code
		method->parser_code=yyvsp[0];

		// register in class
		const String& name=*LA2S(*yyvsp[-6]);
		PC.cclass->set_method(PC.alias_method(name), method);
		RECORD(METHOD, &PC.alias_method(name), method);
}
#line 2072 "compile.tab.C"
    break;

  case 21: /* bracketed_maybe_strings: '[' maybe_strings ']'  */
#line 296 "compile.y"
                                               {yyval=yyvsp[-1];}
#line 2078 "compile.tab.C"
    break;

  case 25: /* strings: strings ';' STRING  */
#line 298 "compile.y"
                                     { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2084 "compile.tab.C"
    break;

  case 31: /* codes: codes code  */
#line 306 "compile.y"
                         { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2090 "compile.tab.C"
    break;

  case 37: /* get: get_value  */
#line 312 "compile.y"
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
#line 2126 "compile.tab.C"
    break;

  case 38: /* get_value: '$' get_name_value  */
#line 343 "compile.y"
                              { yyval=yyvsp[0]; }
#line 2132 "compile.tab.C"
    break;

  case 41: /* name_in_curly_rdive: '{' name_without_curly_rdive '}'  */
#line 345 "compile.y"
                                                      { yyval=yyvsp[-1]; }
#line 2138 "compile.tab.C"
    break;

  case 44: /* name_without_curly_rdive_read: name_without_curly_rdive_code  */
#line 349 "compile.y"
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
#line 2192 "compile.tab.C"
    break;

  case 45: /* name_without_curly_rdive_class: class_prefix name_without_curly_rdive_code  */
#line 398 "compile.y"
                                                                           { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2198 "compile.tab.C"
    break;

  case 47: /* name_without_curly_rdive_code: name_path name_advance2  */
#line 399 "compile.y"
                                                                       { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2204 "compile.tab.C"
    break;

  case 48: /* put: '$' name_expr_wdive construct  */
#line 403 "compile.y"
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
#line 2221 "compile.tab.C"
    break;

  case 52: /* name_expr_wdive_root: name_expr_dive_code  */
#line 419 "compile.y"
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
#line 2248 "compile.tab.C"
    break;

  case 53: /* name_expr_wdive_write: '.' name_expr_dive_code  */
#line 441 "compile.y"
                                               {
	yyval=N(); 
	O(*yyval, OP::OP_WITH_WRITE); /* stack: starting context */
	P(*yyval, *yyvsp[0]); /* diving code; stack: context,name */
}
#line 2258 "compile.tab.C"
    break;

  case 54: /* name_expr_wdive_class: class_prefix name_expr_dive_code  */
#line 446 "compile.y"
                                                        { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2264 "compile.tab.C"
    break;

  case 58: /* @2: %empty  */
#line 453 "compile.y"
                      {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
	PC.array=false; // no need to save current value as if() is right after PC.array=true;
}
#line 2274 "compile.tab.C"
    break;

  case 59: /* $@3: %empty  */
#line 457 "compile.y"
                              {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
}
#line 2282 "compile.tab.C"
    break;

  case 60: /* construct_square: '[' @2 any_constructor_code_values $@3 ']'  */
#line 459 "compile.y"
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
#line 2298 "compile.tab.C"
    break;

  case 61: /* any_constructor_code_values: any_constructor_code_value  */
#line 472 "compile.y"
                                   { yyval = yyvsp[0]; }
#line 2304 "compile.tab.C"
    break;

  case 62: /* any_constructor_code_values: any_constructor_code_values ';' any_constructor_code_value  */
#line 473 "compile.y"
                                                                     { yyval = yyvsp[-2]; P(*yyval, *yyvsp[0]); PC.array=true; }
#line 2310 "compile.tab.C"
    break;

  case 63: /* construct_round: '(' expr_value ')'  */
#line 475 "compile.y"
                                    { 
	yyval=N(); 
	// stack: context, name
	P(*yyval, *yyvsp[-1]); // stack: context, name, value
	O(*yyval, OP::OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
}
#line 2321 "compile.tab.C"
    break;

  case 64: /* construct_curly: '{' maybe_codes '}'  */
#line 482 "compile.y"
                                     {
	// stack: context, name
	yyval=N(); 
	OA(*yyval, OP::OP_CURLY_CODE__CONSTRUCT, yyvsp[-1]); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
}
#line 2331 "compile.tab.C"
    break;

  case 68: /* constructor_code_value: constructor_code  */
#line 493 "compile.y"
                                         {
	yyval=N(); 
	OA(*yyval, OP::OP_OBJECT_POOL, yyvsp[0]); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
}
#line 2342 "compile.tab.C"
    break;

  case 71: /* codes__excluding_sole_str_literal: code codes  */
#line 500 "compile.y"
                                                       { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2348 "compile.tab.C"
    break;

  case 72: /* call: call_value  */
#line 504 "compile.y"
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
#line 2364 "compile.tab.C"
    break;

  case 73: /* $@4: %empty  */
#line 515 "compile.y"
                { 
	PC.in_call_value=true; 
}
#line 2372 "compile.tab.C"
    break;

  case 74: /* $@5: %empty  */
#line 518 "compile.y"
          {
	PC.in_call_value=false;
}
#line 2380 "compile.tab.C"
    break;

  case 75: /* call_value: '^' $@4 call_name $@5 store_params EON  */
#line 521 "compile.y"
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
#line 2430 "compile.tab.C"
    break;

  case 76: /* call_name: name_without_curly_rdive  */
#line 567 "compile.y"
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
#line 2450 "compile.tab.C"
    break;

  case 78: /* store_params: store_params store_param  */
#line 583 "compile.y"
                                                     { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2456 "compile.tab.C"
    break;

  case 82: /* @6: %empty  */
#line 589 "compile.y"
                        {
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
}
#line 2465 "compile.tab.C"
    break;

  case 83: /* $@7: %empty  */
#line 592 "compile.y"
                         {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
}
#line 2473 "compile.tab.C"
    break;

  case 84: /* store_square_param: '[' @6 store_code_param_parts $@7 ']'  */
#line 594 "compile.y"
      {yyval=yyvsp[-2];}
#line 2479 "compile.tab.C"
    break;

  case 85: /* store_round_param: '(' store_expr_param_parts ')'  */
#line 595 "compile.y"
                                                  {yyval=yyvsp[-1];}
#line 2485 "compile.tab.C"
    break;

  case 86: /* store_curly_param: '{' store_curly_param_parts '}'  */
#line 596 "compile.y"
                                                   {yyval=yyvsp[-1];}
#line 2491 "compile.tab.C"
    break;

  case 88: /* store_code_param_parts: store_code_param_parts ';' store_code_param_part  */
#line 599 "compile.y"
                                                         { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2497 "compile.tab.C"
    break;

  case 90: /* store_expr_param_parts: store_expr_param_parts ';' store_expr_param_part  */
#line 603 "compile.y"
                                                         { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2503 "compile.tab.C"
    break;

  case 92: /* store_curly_param_parts: store_curly_param_parts ';' store_curly_param_part  */
#line 607 "compile.y"
                                                           { yyval=yyvsp[-2]; P(*yyval, *yyvsp[0]); }
#line 2509 "compile.tab.C"
    break;

  case 93: /* store_code_param_part: code_param_value  */
#line 609 "compile.y"
                                        {
	yyval=yyvsp[0];
}
#line 2517 "compile.tab.C"
    break;

  case 94: /* store_expr_param_part: expr_value  */
#line 612 "compile.y"
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
#line 2535 "compile.tab.C"
    break;

  case 95: /* store_curly_param_part: maybe_codes  */
#line 625 "compile.y"
                                    {
	yyval=N(); 
	OA(*yyval, OP::OP_CURLY_CODE__STORE_PARAM, yyvsp[0]);
}
#line 2544 "compile.tab.C"
    break;

  case 100: /* name_expr_dive_code: name_path name_expr_value  */
#line 637 "compile.y"
                                                                 { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2550 "compile.tab.C"
    break;

  case 102: /* name_path: name_path name_step  */
#line 639 "compile.y"
                                           { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2556 "compile.tab.C"
    break;

  case 104: /* name_advance1: name_expr_value  */
#line 641 "compile.y"
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
#line 2573 "compile.tab.C"
    break;

  case 105: /* name_advance2: name_expr_value  */
#line 653 "compile.y"
                               {
	/* stack: context */
	yyval=yyvsp[0]; /* stack: context,name */
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	O(*yyval, is_special_element(*yyval) ? OP::OP_GET_ELEMENT__SPECIAL : OP::OP_GET_ELEMENT);
#else
	O(*yyval, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
#endif
}
#line 2587 "compile.tab.C"
    break;

  case 112: /* name_expr_subvar_value: '$' subvar_ref_name_rdive  */
#line 671 "compile.y"
                                                  {
	yyval=yyvsp[0];
	O(*yyval, OP::OP_GET_ELEMENT);
}
#line 2596 "compile.tab.C"
    break;

  case 113: /* name_expr_with_subvar_value: STRING subvar_get_writes  */
#line 675 "compile.y"
                                                      {
	YYSTYPE code;
	{
		change_string_literal_to_write_string_literal(*(code=yyvsp[-1]));
		P(*code, *yyvsp[0]);
	}
	yyval=N(); 
	OA(*yyval, OP::OP_STRING_POOL, code);
}
#line 2610 "compile.tab.C"
    break;

  case 114: /* @8: %empty  */
#line 684 "compile.y"
                            {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&yyval)=PC.explicit_result; PC.explicit_result=false;
}
#line 2619 "compile.tab.C"
    break;

  case 115: /* $@9: %empty  */
#line 687 "compile.y"
        {
	PC.explicit_result=*reinterpret_cast<bool*>(&yyvsp[-1]);
}
#line 2627 "compile.tab.C"
    break;

  case 116: /* name_square_code_value: '[' @8 codes $@9 ']'  */
#line 689 "compile.y"
      {
	yyval=N();
	if(!maybe_append_simple_diving_code(*yyval, *yyvsp[-2])) {
		OA(*yyval, OP::OP_OBJECT_POOL, yyvsp[-2]); /* stack: empty write context */
		/* some code that writes to that context */
		/* context=pop; stack: context.value() */
	}
}
#line 2640 "compile.tab.C"
    break;

  case 117: /* name_round_expr_value: '(' expr_value ')'  */
#line 697 "compile.y"
                                          {
	yyval = N();
	P(*yyval, *yyvsp[-1]);
}
#line 2649 "compile.tab.C"
    break;

  case 118: /* subvar_ref_name_rdive: STRING  */
#line 701 "compile.y"
                              {
	yyval=N(); 
	O(*yyval, OP::OP_WITH_READ);
	P(*yyval, *yyvsp[0]);
}
#line 2659 "compile.tab.C"
    break;

  case 120: /* subvar_get_writes: subvar_get_writes subvar__get_write  */
#line 706 "compile.y"
                                                                           { yyval=yyvsp[-1]; P(*yyval, *yyvsp[0]); }
#line 2665 "compile.tab.C"
    break;

  case 121: /* subvar__get_write: '$' subvar_ref_name_rdive  */
#line 707 "compile.y"
                                             {
	yyval=yyvsp[0];
	O(*yyval, OP::OP_GET_ELEMENT__WRITE);
}
#line 2674 "compile.tab.C"
    break;

  case 124: /* class_static_prefix: STRING ':'  */
#line 716 "compile.y"
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
#line 2698 "compile.tab.C"
    break;

  case 125: /* class_constructor_prefix: class_static_prefix ':'  */
#line 735 "compile.y"
                                                  {
	yyval=yyvsp[-1];
	if(!PC.in_call_value)
		YYERROR1(":: not allowed here");
	O(*yyval, OP::OP_PREPARE_TO_CONSTRUCT_OBJECT);
}
#line 2709 "compile.tab.C"
    break;

  case 132: /* expr: '"' string_inside_quotes_value '"'  */
#line 752 "compile.y"
                                           { yyval = yyvsp[-1]; }
#line 2715 "compile.tab.C"
    break;

  case 133: /* expr: '\'' string_inside_quotes_value '\''  */
#line 753 "compile.y"
                                             { yyval = yyvsp[-1]; }
#line 2721 "compile.tab.C"
    break;

  case 134: /* expr: '(' expr ')'  */
#line 754 "compile.y"
                     { yyval = yyvsp[-1]; }
#line 2727 "compile.tab.C"
    break;

  case 135: /* expr: '-' expr  */
#line 756 "compile.y"
                              { yyval=yyvsp[0];  O(*yyval, OP::OP_NEG); }
#line 2733 "compile.tab.C"
    break;

  case 136: /* expr: '+' expr  */
#line 757 "compile.y"
                              { yyval=yyvsp[0]; }
#line 2739 "compile.tab.C"
    break;

  case 137: /* expr: '~' expr  */
#line 758 "compile.y"
                 { yyval=yyvsp[0];	 O(*yyval, OP::OP_INV); }
#line 2745 "compile.tab.C"
    break;

  case 138: /* expr: '!' expr  */
#line 759 "compile.y"
                 { yyval=yyvsp[0];  O(*yyval, OP::OP_NOT); }
#line 2751 "compile.tab.C"
    break;

  case 139: /* expr: "def" expr  */
#line 760 "compile.y"
                   { yyval=yyvsp[0];  O(*yyval, OP::OP_DEF); }
#line 2757 "compile.tab.C"
    break;

  case 140: /* expr: "in" expr  */
#line 761 "compile.y"
                  { yyval=yyvsp[0];  O(*yyval, OP::OP_IN); }
#line 2763 "compile.tab.C"
    break;

  case 141: /* expr: "-f" expr  */
#line 762 "compile.y"
                  { yyval=yyvsp[0];  O(*yyval, OP::OP_FEXISTS); }
#line 2769 "compile.tab.C"
    break;

  case 142: /* expr: "-d" expr  */
#line 763 "compile.y"
                  { yyval=yyvsp[0];  O(*yyval, OP::OP_DEXISTS); }
#line 2775 "compile.tab.C"
    break;

  case 143: /* expr: "-l" expr  */
#line 764 "compile.y"
                  { yyval=yyvsp[0];  O(*yyval, OP::OP_LEXISTS); }
#line 2781 "compile.tab.C"
    break;

  case 144: /* expr: expr '-' expr  */
#line 766 "compile.y"
                      {	yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_SUB); }
#line 2787 "compile.tab.C"
    break;

  case 145: /* expr: expr '+' expr  */
#line 767 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_ADD); }
#line 2793 "compile.tab.C"
    break;

  case 146: /* expr: expr '*' expr  */
#line 768 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_MUL); }
#line 2799 "compile.tab.C"
    break;

  case 147: /* expr: expr '/' expr  */
#line 769 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_DIV); }
#line 2805 "compile.tab.C"
    break;

  case 148: /* expr: expr '%' expr  */
#line 770 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_MOD); }
#line 2811 "compile.tab.C"
    break;

  case 149: /* expr: expr '\\' expr  */
#line 771 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_INTDIV); }
#line 2817 "compile.tab.C"
    break;

  case 150: /* expr: expr "<<" expr  */
#line 772 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_SL); }
#line 2823 "compile.tab.C"
    break;

  case 151: /* expr: expr ">>" expr  */
#line 773 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_SR); }
#line 2829 "compile.tab.C"
    break;

  case 152: /* expr: expr '&' expr  */
#line 774 "compile.y"
                      { yyval=yyvsp[-2]; 	P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_AND); }
#line 2835 "compile.tab.C"
    break;

  case 153: /* expr: expr '|' expr  */
#line 775 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_OR); }
#line 2841 "compile.tab.C"
    break;

  case 154: /* expr: expr "!|" expr  */
#line 776 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_BIN_XOR); }
#line 2847 "compile.tab.C"
    break;

  case 155: /* expr: expr "&&" expr  */
#line 777 "compile.y"
                       { yyval=yyvsp[-2];  OA(*yyval, OP::OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP::OP_LOG_AND); }
#line 2853 "compile.tab.C"
    break;

  case 156: /* expr: expr "||" expr  */
#line 778 "compile.y"
                       { yyval=yyvsp[-2];  OA(*yyval, OP::OP_NESTED_CODE, yyvsp[0]);  O(*yyval, OP::OP_LOG_OR); }
#line 2859 "compile.tab.C"
    break;

  case 157: /* expr: expr "!||" expr  */
#line 779 "compile.y"
                        { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_LOG_XOR); }
#line 2865 "compile.tab.C"
    break;

  case 158: /* expr: expr '<' expr  */
#line 780 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_LT); }
#line 2871 "compile.tab.C"
    break;

  case 159: /* expr: expr '>' expr  */
#line 781 "compile.y"
                      { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_GT); }
#line 2877 "compile.tab.C"
    break;

  case 160: /* expr: expr "<=" expr  */
#line 782 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_LE); }
#line 2883 "compile.tab.C"
    break;

  case 161: /* expr: expr ">=" expr  */
#line 783 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_GE); }
#line 2889 "compile.tab.C"
    break;

  case 162: /* expr: expr "==" expr  */
#line 784 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_EQ); }
#line 2895 "compile.tab.C"
    break;

  case 163: /* expr: expr "!=" expr  */
#line 785 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_NUM_NE); }
#line 2901 "compile.tab.C"
    break;

  case 164: /* expr: expr "lt" expr  */
#line 786 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_LT); }
#line 2907 "compile.tab.C"
    break;

  case 165: /* expr: expr "gt" expr  */
#line 787 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_GT); }
#line 2913 "compile.tab.C"
    break;

  case 166: /* expr: expr "le" expr  */
#line 788 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_LE); }
#line 2919 "compile.tab.C"
    break;

  case 167: /* expr: expr "ge" expr  */
#line 789 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_GE); }
#line 2925 "compile.tab.C"
    break;

  case 168: /* expr: expr "eq" expr  */
#line 790 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_EQ); }
#line 2931 "compile.tab.C"
    break;

  case 169: /* expr: expr "ne" expr  */
#line 791 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_STR_NE); }
#line 2937 "compile.tab.C"
    break;

  case 170: /* expr: expr "is" expr  */
#line 792 "compile.y"
                       { yyval=yyvsp[-2];  P(*yyval, *yyvsp[0]);  O(*yyval, OP::OP_IS); }
#line 2943 "compile.tab.C"
    break;

  case 171: /* double_or_STRING: STRING  */
#line 795 "compile.y"
                         {
	// optimized OP_STRING => OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*(yyval=yyvsp[0]));
}
#line 2952 "compile.tab.C"
    break;

  case 172: /* string_inside_quotes_value: maybe_codes  */
#line 800 "compile.y"
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
#line 2975 "compile.tab.C"
    break;

  case 173: /* write_string: STRING  */
#line 821 "compile.y"
                     {
	// optimized OP_STRING+OP_WRITE_VALUE => OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*(yyval=yyvsp[0]));
}
#line 2984 "compile.tab.C"
    break;

  case 174: /* empty_value: %empty  */
#line 826 "compile.y"
                         { yyval=VL(/*we know that we will not change it*/const_cast<VString*>(&vempty), 0, 0, 0); }
#line 2990 "compile.tab.C"
    break;

  case 175: /* true_value: "true"  */
#line 827 "compile.y"
                   { yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vtrue), 0, 0, 0); }
#line 2996 "compile.tab.C"
    break;

  case 176: /* false_value: "false"  */
#line 828 "compile.y"
                     { yyval = VL(/*we know that we will not change it*/const_cast<VBool*>(&vfalse), 0, 0, 0); }
#line 3002 "compile.tab.C"
    break;

  case 177: /* empty: %empty  */
#line 830 "compile.y"
                   { yyval=N(); }
#line 3008 "compile.tab.C"
    break;


#line 3012 "compile.tab.C"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

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
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      {
        yypcontext_t yyctx
          = {yyssp, yytoken};
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == -1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *,
                             YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (yymsg)
              {
                yysyntax_error_status
                  = yysyntax_error (&yymsg_alloc, &yymsg, &yyctx);
                yymsgp = yymsg;
              }
            else
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = YYENOMEM;
              }
          }
        yyerror (pc, yymsgp);
        if (yysyntax_error_status == YYENOMEM)
          YYNOMEM;
      }
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
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, pc);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (pc, YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, pc);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
  return yyresult;
}

#line 832 "compile.y"

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
				case 'l': // -l
					skip_analized=1;
					result=LEXISTS;
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
				if(	end[-1]=='$' /* was start of get */ ||
					end[-1]==':' /* was class name delim */ ||
					end[-1]=='.' /* was name delim */
					) {
					push_LS(pc, LS_NAME_SQUARE_PART);
					lexical_brackets_nestage=1;
					RC;
				}
				pc.ls=LS_VAR_SQUARE;
				lexical_brackets_nestage=1;
				RC;
			case '{':
				if(end[-1]=='$') { // ${name}, no need of EON, switching LS, not begin==end as $[a]{$b} will fit
					pc.ls=LS_VAR_NAME_CURLY; 
				} else {
					pc.ls=LS_VAR_CURLY;
					lexical_brackets_nestage=1;
				}

				RC;
			case '(':
				// $name.<(>expr)
				if(	// end[-1]=='$' /* $() excluded */ ||
					end[-1]==':' /* was class name delim */ ||
					end[-1]=='.' /* was name delim */
					) {
					push_LS(pc, LS_VAR_ROUND);
					lexical_brackets_nestage=1;
					RC;
				}
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
			case '(':
				// ${name.<(>expr)}
				push_LS(pc, LS_VAR_ROUND);
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
				// ^name.<[>code]
				if(	// end[-1]=='^' /* never, ^[ is literal */ ||
					end[-1]==':' /* was class name delim */ ||
					end[-1]=='.' /* was name delim */
					) {
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
				// ^name.<(>expr)
				if(	// end[-1]=='^' /* never, ^( is literal */ ||
					end[-1]==':' /* was class name delim */ ||
					end[-1]=='.' /* was name delim */
					) {
					push_LS(pc, LS_VAR_ROUND);
					lexical_brackets_nestage=1;
					RC;
				}
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

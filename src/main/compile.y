%{
#define YYSTYPE  Array/*<op>*/ *
#define YYPARSE_PARAM  pc
#define YYLEX_PARAM  pc
#define YYDEBUG  1
#define YYERROR_VERBOSE
#define yyerror(msg)  real_yyerror((parse_control *)pc, msg)
#define YYPRINT(file, type, value)  yyprint(file, type, value)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "compile_tools.h"
#include "pa_value.h"
#include "pa_request.h"

int real_yyerror(parse_control *pc, char *s);
static void yyprint(FILE *file, int type, YYSTYPE value);
int yylex(YYSTYPE *lvalp, void *pc);


// local convinient inplace typecast & var
#define PC  ((parse_control *)pc)
#define pool  *PC->pool
%}

%pure_parser

%token EON
%token STRING
%token BOGUS

%%

all:
	one_big_piece {
	String& name_main=*new(pool) String(pool);
	name_main.APPEND_CONST(MAIN_METHOD_NAME);
	Array& param_names=*new(pool) Array(pool);
	Array& local_names=*new(pool) Array(pool);
	Method *method=new(pool) Method(pool, name_main, param_names, local_names, *$1);
	*PC->methods+=method;
}
|	methods;

methods: method | methods method;
one_big_piece: maybe_codes;

method: '@' STRING bracketed_maybe_strings maybe_bracketed_strings maybe_comment '\n' 
			maybe_codes {
	const String *name=LA2S($2);

	YYSTYPE params_names_code=$3;
	Array& params_names=*new(pool) Array(pool);
	for(int i=0; i<params_names_code->size(); i+=2)
		params_names+=LA2S(params_names_code, i);

	YYSTYPE locals_names_code=$4;
	Array& locals_names=*new(pool) Array(pool);
	for(int i=0; i<locals_names_code->size(); i+=2)
		locals_names+=LA2S(locals_names_code, i);

	Method *method=new(pool) Method(pool, *name, params_names, locals_names, *$7);
	*PC->methods+=method;
};

maybe_bracketed_strings: empty | bracketed_maybe_strings;
bracketed_maybe_strings: '[' maybe_strings ']' {$$=$2};
maybe_strings: empty | strings;
strings: STRING | strings ';' STRING { $$=$1; P($$, $3) };

maybe_comment: empty | STRING;

/* codes */

maybe_codes: empty | codes;

codes: code | codes code { 
	$$=$1; 
	P($$, $2);
};
code: write_str_literal | action;
action: get | put | with | call;

/* get */

get: '$' any_name {
	$$=$2; /* stack: resulting value */
	OP($$, OP_WRITE); /* value=pop; write(value) */
};

any_name: name_without_curly_rdive EON | name_in_curly_rdive;

name_in_curly_rdive: '{' name_without_curly_rdive '}' { $$=$2 };
name_without_curly_rdive: name_without_curly_rdive_read | name_without_curly_rdive_root;
name_without_curly_rdive_read: name_without_curly_rdive_code {
/*
	TODO: подсмотреть в $1, и если там первым элементом self,
		то выкинуть его и делать не OP_WITH_READ, а WITH_SELF
		*/
	$$=N(pool); 
	OP($$, OP_WITH_READ); /* stack: starting context */
	P($$, $1); /* diving code; stack: current context */
};
name_without_curly_rdive_root: ':' name_without_curly_rdive_code {
	$$=N(pool); 
	OP($$, OP_WITH_ROOT); /* stack: starting context */
	P($$, $2); /* diving code; stack: current context */
};
name_without_curly_rdive_code: name_advance2 | name_path name_advance2 { $$=$1; P($$, $2) };

/* put */

put: '$' name_expr_dive '(' constructor_value ')' {
/*
	TODO: подсмотреть в $3, и если там первым элементом self,
		то выкинуть его и делать не OP_WITH_OP_WRITE, а WITH_SELF
		если ничего не осталось - $self(xxx)
			обругать
*/
	$$=$2; /* stack: context,name */
	P($$, $4); /* stack: context,name,constructor_value */
	OP($$, OP_CONSTRUCT); /* value=pop; name=pop; context=pop; construct(context,name,value) */
};
name_expr_dive: name_expr_dive_write | name_expr_dive_root;
name_expr_dive_write: name_expr_dive_code {
	$$=N(pool); 
	OP($$, OP_WITH_WRITE); /* stack: starting context */
	P($$, $1); /* diving code; stack: context,name */
};
name_expr_dive_root: ':' name_expr_dive_code {
	$$=N(pool); 
	OP($$, OP_WITH_ROOT); /* stack: starting context */
	P($$, $2); /* diving code; stack: context,name */
};

constructor_value: 
	constructor_one_param_value
|	constructor_two_params_value /* $var(=;2*2) $var(%d;2*2) $var(+;1) */
;
constructor_one_param_value: 
	empty /* optimized $var() case */
|	STRING /* optimized $var(STRING) case */
|	complex_constructor_param_value /* $var(something complex) */
;
complex_constructor_param_value: complex_constructor_param_body {
	$$=N(pool); 
	OP($$, OP_CREATE_EWPOOL); /* stack: empty write context */
	P($$, $1); /* some codes to that context */
	OP($$, OP_REDUCE_EWPOOL); /* context=pop; stack: context.value() */
};
complex_constructor_param_body:
	codes__excluding_sole_str_literal
|	codes__str__followed_by__excluding_sole_str_literal
;
constructor_two_params_value: STRING ';' constructor_one_param_value {
	char *operator_or_fmt=LA2S($1)->cstr();
	$$=N(pool);
	P($$, $1); /* stack: ncontext name operator_or_fmt */
	P($$, $3); /* stack: ncontext name operator_or_fmt expr */
	switch(operator_or_fmt[0]) {
	case '=': case '%':
		OP($$, OP_EXPRESSION_EVAL);
		break;
	case '+': case '-': case '*': case '/':
		OP($$, OP_MODIFY_EVAL);
		break;
	default:
		strcpy(PC->error, "invalid modification operator");
		YYERROR;
	}
	/* stack: ncontext name value */
};


/* call */

call: '^' name_expr_dive store_params EON { /* ^field.$method{vasya} */
/*
	TODO: подсмотреть в $3, и если там в первом элементе первая буква ":"
		то выкинуть её и делать не OP_WITH_READ, а WITH_ROOT
	TODO: подсмотреть в $3, и если там первым элементом self,
		то выкинуть его и делать не OP_WITH_READ, а WITH_SELF
	TODO:
		если первым в $3 идёт result
		то
			выкинуть его
			если там ещё что-то осталось,
			то
				не OP_WITH_READ, а WITH_RESULT
			иначе  // ^result(value)
				обругать безобразие
*/
	$$=N(pool); 
	OP($$, OP_WITH_READ); /* stack: starting context */
	P($$, $2); /* diving code; stack: context,method_name */
	OP($$, OP_GET_METHOD_FRAME); /* stack: context,method_frame */
	P($$, $3); /* filling method_frame.store_params */
	OP($$, OP_CALL); /* method_frame=pop; ncontext=pop; call(ncontext,method_frame) */
};

store_params: store_param | store_params store_param { $$=$1; P($$, $2) };
store_param: store_round_param | store_curly_param;
store_round_param: '(' store_param_parts ')' {$$=$2};
store_param_parts: store_param_part | store_param_parts ';' store_param_part { $$=$1; P($$, $3) };
store_param_part: constructor_one_param_value {
	$$=$1;
	OP($$, OP_STORE_PARAM);
}
store_curly_param: '{' maybe_codes '}' {
	$$=N(pool); 
	OP($$, OP_CODE_ARRAY);
	AA($$, $2);
	OP($$, OP_CREATE_JUNCTION);
	OP($$, OP_STORE_PARAM);
};

/* name */

name_expr_dive_code: name_expr_value | name_path name_expr_value { $$=$1; P($$, $2) };

name_path: name_step | name_path name_step { $$=$1; P($$, $2) };
name_step: name_advance1 '.';
name_advance1: name_expr_value {
	/* stack: context */
	$$=$1; /* stack: context,name */
	OP($$, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
};
name_advance2: name_expr_value {
	/* stack: context */
	$$=$1; /* stack: context,name */
	OP($$, OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
}
|	STRING BOGUS
;
name_expr_value: 
	STRING /* subname_is_const */
|	name_expr_subvar_value /* $subname_is_var_value */
|	name_expr_with_subvar_value /* xxx$part_of_subname_is_var_value[$...] */
;
name_expr_subvar_value: '$' subvar_ref_name_rdive {
	$$=$2;
	OP($$, OP_GET_ELEMENT);
};
name_expr_with_subvar_value: STRING subvar_get_writes {
	$$=N(pool); 
	OP($$, OP_CREATE_EWPOOL);
	P($$, $1);
	OP($$, OP_WRITE);
	P($$, $2);
	OP($$, OP_REDUCE_EWPOOL);
};
subvar_ref_name_rdive: subvar_ref_name_rdive_read | subvar_ref_name_rdive_root;
subvar_ref_name_rdive_read: STRING {
	$$=N(pool); 
	OP($$, OP_WITH_READ);
	P($$, $1);
};
subvar_ref_name_rdive_root: ':' STRING {
	$$=N(pool); 
	OP($$, OP_WITH_ROOT);
	P($$, $2);
};
subvar_get_writes: subvar__get_write | subvar_get_writes subvar__get_write { $$=$1; P($$, $2) };
subvar__get_write: '$' subvar_ref_name_rdive {
	$$=$2;
	OP($$, OP_GET_ELEMENT__WRITE);
};


/* with */

with: '$' name_without_curly_rdive '{' codes '}' {
	$$=$2;
	OP($$, OP_CREATE_RWPOOL);
	P($$, $4);
	OP($$, OP_REDUCE_RWPOOL);
	OP($$, OP_WRITE);
};

/* codes_in_brackets */

codes__str__followed_by__excluding_sole_str_literal:
	write_str_literal codes__excluding_sole_str_literal {
		$$=$1;
		P($$, $2);
}
;
codes__excluding_sole_str_literal:
	action
|	codes__excluding_sole_str_literal write_str_literal {
		$$=$1;
		P($$, $2);
}
;
write_str_literal: STRING {
	$$=$1;
	OP($$, OP_WRITE);
};

/* */

empty: /* empty */ { $$=N(pool) };

%%

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

int yylex(YYSTYPE *lvalp, void *pc) {
	#define lexical_brackets_nestage PC->brackets_nestages[PC->sp]

    register int c;
    int result;
	
	if(PC->pending_state) {
		result=PC->pending_state;
		PC->pending_state=0;
		return result;
	}
	
	char *begin=PC->source;
	char *end;
	int begin_line=PC->line;
	while(1) {
		c=*(end=(PC->source++));

		if(c=='\n') {
			PC->line++;
			PC->col=0;
		} else
			PC->col++;

		/* escaping: ^^ ^$ ^; ^) ^} ^( ^{ */
		if(c=='^') {
			char pending_c=*PC->source;

			if(pending_c=='^' || pending_c=='$' || pending_c==';' ||
				pending_c=='(' || pending_c==')' ||
				pending_c=='{' || pending_c=='}') {
				/* append piece till ^ */
				PC->string->APPEND(begin, end-begin, PC->file, begin_line);
				/* reset piece 'start' position & line */
				begin=PC->source/*^*/;
				begin_line=PC->line;
				/* skip over ^ and _ */
				PC->source+=2;
				/* skip analysis = forced literal */
				continue;
			}
		}
		switch(PC->ls) {

		// USER'S = NOT OURS
		case LS_USER:
			if(c=='$') {
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				result=c;
				goto break2;
			}
			if(c=='^') {
				push_LS(PC, LS_METHOD_NAME);
				result=c;
				goto break2;
			}
			if(c=='@' && PC->col==0+1) {
				result=c;
				push_LS(PC, LS_DEF_NAME);
				goto break2;
			}
			
			break;

		// METHOD DEFINITION
		case LS_DEF_NAME:
			if(c=='[') {
				result=c;
				PC->ls=LS_DEF_PARAMS;
				goto break2;
			}
			if(c=='\n') { // wrong. bailing out
				result=c;
				pop_LS(PC);
				goto break2;
			}
			break;
		case LS_DEF_PARAMS:
			if(c==';') {
				result=c;
				goto break2;
			}
			if(c==']') {
				result=c;
				PC->ls=*PC->source=='['?LS_DEF_LOCALS:LS_DEF_COMMENT;
				goto break2;
			}
			if(c=='\n') { // wrong. bailing out
				result=c;
				pop_LS(PC);
				goto break2;
			}
			break;
		case LS_DEF_LOCALS:
			if(c=='[' || c==';') {
				result=c;
				goto break2;
			}
			if(c==']') {
				result=c;
				PC->ls=LS_DEF_COMMENT;
				goto break2;
			}
			if(c=='\n') { // wrong. bailing out
				result=c;
				pop_LS(PC);
				goto break2;
			}
			break;
		case LS_DEF_COMMENT:
			if(c=='\n') {
				result=c;
				pop_LS(PC);
				goto break2;
			}
			break;

		// VARIABLE GET/PUT/WITH
		case LS_VAR_NAME_SIMPLE:
			if(c==0 || 
				c==' '|| c=='\t' || c=='\n' || 
				c==')' || c=='}') {
				pop_LS(PC);
				PC->source--;	PC->col--;
				result=EON;
				goto break2;
			}
			if(begin==end && c=='{') { /* ${name}, no need of EON, switching LS */
				PC->ls=LS_VAR_NAME_CURLY; 
				result=c;
				goto break2;
			}
			if(c==':') {
				result=c;
				goto break2;
			}
			if(c=='(') {
				PC->ls=LS_VAR_ROUND;
				lexical_brackets_nestage=1;
				result=c;
				goto break2;
			}
			if(c=='{') {
				PC->ls=LS_VAR_CURLY;
				lexical_brackets_nestage=1;
				result=c;
				goto break2;
			}
			if(c=='.'/* name part delim */ || c=='$'/* name part subvar */) {
				result=c;
				goto break2;
			}
			break;
		case LS_VAR_NAME_CURLY:
			if(c==':') {
				result=c;
				goto break2;
			}
			if(c=='}') {  /* ${name} finished, restoring LS */
				pop_LS(PC);
				result=c;
				goto break2;
			}
			if(c=='.'/* name part delim */ || c=='$'/*name part subvar*/) {
				result=c;
				goto break2;
			}
			break;
		case LS_VAR_ROUND:
			if(c=='$') {
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				result=c;
				goto break2;
			}
			if(c=='^') {
				push_LS(PC, LS_METHOD_NAME);
				result=c;
				goto break2;
			}
			if(c==')') {
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
					result=c;
					goto break2;
				}
			}
			if(c==';'/* operator_or_fmt;value delim */) {
				result=c;
				goto break2;
			}
			if(c=='(')
				lexical_brackets_nestage++;
			break;
		case LS_VAR_CURLY:
			if(c=='$') {
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				result=c;
				goto break2;
			}
			if(c=='^') {
				push_LS(PC, LS_METHOD_NAME);
				result=c;
				goto break2;
			}
			if(c=='}')
				if(--lexical_brackets_nestage==0) {
					pop_LS(PC);
					result=c;
					goto break2;
				}
			if(c=='{')
				lexical_brackets_nestage++;
			break;

		// METHOD CALL
		case LS_METHOD_NAME:
			if(c=='(') {
				PC->ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				result=c;
				goto break2;
			}
			if(c=='{') {
				PC->ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				result=c;
				goto break2;
			}
			if(c=='.'/* name part delim */ || c=='$'/* name part subvar */) {
				result=c;
				goto break2;
			}
			break;
		case LS_METHOD_ROUND:
			if(c=='$') {
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				result=c;
				goto break2;
			}
			if(c=='^') {
				push_LS(PC, LS_METHOD_NAME);
				result=c;
				goto break2;
			}
			if(c==';'/* param delim */) {
				result=c;
				goto break2;
			}
			if(c==')')
				if(--lexical_brackets_nestage==0) {
					PC->ls=LS_METHOD_AFTER;
					result=c;
					goto break2;
				}
			if(c=='(')
				lexical_brackets_nestage++;
			break;
		case LS_METHOD_CURLY:
			if(c=='$') {
				push_LS(PC, LS_VAR_NAME_SIMPLE);
				result=c;
				goto break2;
			}
			if(c=='^') {
				push_LS(PC, LS_METHOD_NAME);
				result=c;
				goto break2;
			}
			if(c=='}')
				if(--lexical_brackets_nestage==0) {
					PC->ls=LS_METHOD_AFTER;
					result=c;
					goto break2;
				}
			if(c=='{')
				lexical_brackets_nestage++;
			break;
		case LS_METHOD_AFTER:
			if(c=='(') {/* )( }( */
				PC->ls=LS_METHOD_ROUND;
				lexical_brackets_nestage=1;
				result=c;
				goto break2;
			}					   
			if(c=='{') {/* ){ }{ */
				PC->ls=LS_METHOD_CURLY;
				lexical_brackets_nestage=1;
				result=c;
				goto break2;
			}					   
			pop_LS(PC);
			PC->source--;  PC->col--;
			result=EON;
			goto break2;
		}
		if(c==0) {
			result=-1;
//			PC->source--;  PC->col--;
			break;
		}
	}

break2:
	if(begin==end)
		return result;
	else {
		PC->pending_state=result;
		// strip last \n before LS_DEF_NAME or EOF
		if((c=='@' || c==0) && end[-1]=='\n')
			end--;
		// append last piece
		PC->string->APPEND(begin, end-begin, PC->file, begin_line/*, start_col*/);
		// create STRING value: array of OP_VALUE+vstring
		*lvalp=L(new(pool) VString(PC->string));
		// new pieces storage
		PC->string=new(pool) String(pool);
		// go!
		return STRING;
	}
}

int real_yyerror(parse_control *pc, char *s)  /* Called by yyparse on error */
     {
       //fprintf(stderr, "[%s]\n", s);

	   s[MAX_STRING-1]=0; strcpy(pc->error, s);
	   return 1;
     }

static void
     yyprint(
          FILE *file,
          int type,
          YYSTYPE value)
     {
       if(type==STRING)
         fprintf(file, " \"%s\"", LA2S(value)->cstr());
     }


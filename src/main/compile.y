%{
/** @file
	Parser: compiler(lexical parser and grammar).

	Copyright (c) 2001-2017 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	
*/

volatile const char * IDENT_COMPILE_Y = "$Id: compile.y,v 1.289 2017/02/07 22:00:40 moko Exp $";

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

#define CLASS_ADD if(PC.class_add()){				\
	strncpy(PC.error, PC.cclass->type(), MAX_STRING/2);	\
	strcat(PC.error, " - class is already defined");	\
	YYERROR;						\
}

#define PC_ERROR(header, value, footer){			\
	strcpy(PC.error, header);				\
	strncat(PC.error, value, MAX_STRING/2);			\
	strcat(PC.error, footer);				\
}

%}

%pure-parser
%lex-param {Parse_control* pc}
%parse-param {Parse_control* pc}

%token EON
%token STRING
%token BOGUS

%token BAD_STRING_COMPARISON_OPERATOR
%token BAD_HEX_LITERAL
%token BAD_METHOD_DECL_START
%token BAD_METHOD_PARAMETER_NAME_CHARACTER

%token LAND "&&"
%token LOR "||"
%token LXOR "!||"
%token NXOR "!|"

%token NLE "<="
%token NGE ">="
%token NEQ "=="
%token NNE "!="
%token NSL "<<"
%token NSR ">>"

%token SLT "lt"
%token SGT "gt"
%token SLE "le"
%token SGE "ge"
%token SEQ "eq"
%token SNE "ne"

%token DEF "def"
%token IN "in"
%token FEXISTS "-f"
%token DEXISTS "-d"
%token IS "is"

%token LITERAL_TRUE "true"
%token LITERAL_FALSE "false"

/* logical */
%left "!||"
%left "||"
%left "&&"
%left '<' '>' "<=" ">="   "lt" "gt" "le" "ge"
%left "==" "!="  "eq" "ne"
%left "is" "def" "in" "-f" "-d"

/* bitwise */
%left "!|"
%left '|'
%left '&' 
%left "<<"  ">>"

/* numerical */
%left '+' '-'
%left '*' '/' '\\' '%'
%left NUNARY   /* unary - + */

/* out-of-group */
%left '~' /* bitwise */
%left '!'  /* logical */

%%
all:
	one_big_piece {
	Method* method=new Method(Method::CT_ANY, 0, 0 /*min, max numbered_params_count*/, 0 /*param_names*/, 0 /*local_names*/, $1 /*parser_code*/, 0 /*native_code*/, PC.cclass->is_vars_local());
	PC.cclass->set_method(PC.alias_method(main_method_name), method);
}
|	methods;

methods: method | methods method;
one_big_piece: maybe_codes;

method: control_method | code_method;

control_method: '@' STRING '\n' 
				maybe_control_strings {
	const String& command=LA2S(*$2)->trim(String::TRIM_END);
	YYSTYPE strings_code=$4;
	if(strings_code->count()<1*OPERATIONS_PER_OPVALUE) {
		PC_ERROR("@", command.cstr(), " is empty");
		YYERROR;
	}
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
			strcpy(PC.error, "@" CLASS_NAME " must contain only one line with class name (contains more then one)");
			YYERROR;
		}
	} else if(command==USE_CONTROL_METHOD_NAME) {
		CLASS_ADD;
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE){
			PC.request.use_file(LA2S(*strings_code, i)->trim(String::TRIM_END), PC.request.get_used_filespec(PC.file_no), strings_code->get(i+1).origin);
		}
	} else if(command==BASE_NAME) {
		if(PC.append){
			PC_ERROR("can't set base while appending methods to class '", PC.cclass->type(), "'");
			YYERROR;
		}
		CLASS_ADD;
		if(PC.cclass->base_class()) { // already changed from default?
			PC_ERROR("class already have a base '", PC.cclass->base_class()->type(), "'");
			YYERROR;
		}
		if(strings_code->count()==1*OPERATIONS_PER_OPVALUE) {
			const String& base_name=LA2S(*strings_code)->trim(String::TRIM_END);
			if(VStateless_class *base_class=PC.request.get_class(base_name)) {
				// @CLASS == @BASE sanity check
				if(PC.cclass==base_class) {
					strcpy(PC.error, "@" CLASS_NAME " equals @" BASE_NAME);
					YYERROR;
				}
				PC.cclass->get_class()->set_base(base_class);
			} else {
				PC_ERROR("'", base_name.cstr(), "': undefined class in @" BASE_NAME);
				YYERROR;
			}
		} else {
			strcpy(PC.error, "@" BASE_NAME " must contain sole name");
			YYERROR;
		}
	} else if(command==OPTIONS_CONTROL_METHOD_NAME) {
		for(size_t i=0; i<strings_code->count(); i+=OPERATIONS_PER_OPVALUE) {
			const String& option=LA2S(*strings_code, i)->trim(String::TRIM_END);
			if(option==Symbols::LOCALS_SYMBOL){
				PC.set_all_vars_local();
			} else if(option==Symbols::PARTIAL_SYMBOL){
				if(PC.cclass_new){
					if(VStateless_class* existed=PC.get_existed_class(PC.cclass_new)){
						if(!PC.reuse_existed_class(existed)){
							PC_ERROR("can't append methods to '", PC.cclass_new->type(), "' - the class wasn't marked as partial");
							YYERROR;
						}
					} else {
						// marks the new class as partial. we will be able to add methods here later.
						PC.cclass_new->set_partial();
					}
				} else {
					strcpy(PC.error, "'partial' option should be used straight after @" CLASS_NAME);
					YYERROR;
				}
			} else if(option==Symbols::STATIC_SYMBOL){
				PC.set_methods_call_type(Method::CT_STATIC);
			} else if(option==Symbols::DYNAMIC_SYMBOL){
				PC.set_methods_call_type(Method::CT_DYNAMIC);
			} else {
				PC_ERROR("'", option.cstr(), "' invalid option. valid options are 'partial', 'locals', 'static' and 'dynamic'");
				YYERROR;
			}
		}
	} else {
		PC_ERROR("'", command.cstr(), "' invalid special name. valid names are '" CLASS_NAME "', '" USE_CONTROL_METHOD_NAME "', '" BASE_NAME "' and '" OPTIONS_CONTROL_METHOD_NAME "'.");
		YYERROR;
	}
};
maybe_control_strings: empty | control_strings;
control_strings: control_string | control_strings control_string { $$=$1; P(*$$, *$2); };
control_string: maybe_string '\n';
maybe_string: empty | STRING;

code_method: '@' STRING bracketed_maybe_strings maybe_bracketed_strings maybe_comment '\n' { 
	CLASS_ADD;
	PC.explicit_result=false;

	YYSTYPE params_names_code=$3;
	ArrayString* params_names=0;
	if(int size=params_names_code->count()) {
		params_names=new ArrayString;
		for(int i=0; i<size; i+=OPERATIONS_PER_OPVALUE)
			*params_names+=LA2S(*params_names_code, i);
	}

	YYSTYPE locals_names_code=$4;
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
		GetMethodCallType(PC, *$2),
		0, 0/*min,max numbered_params_count*/, 
		params_names, locals_names, 
		0/*to be filled later in next {} */, 0, all_vars_local);

	*reinterpret_cast<Method**>(&$$)=method;
} maybe_codes {
		Method* method=reinterpret_cast<Method*>($7);
		// fill in the code
		method->parser_code=$8;

		// register in class
		const String& name=*LA2S(*$2);
		PC.cclass->set_method(PC.alias_method(name), method);
};

maybe_bracketed_strings: empty | bracketed_maybe_strings;
bracketed_maybe_strings: '[' maybe_strings ']' {$$=$2;};
maybe_strings: empty | strings;
strings: STRING | strings ';' STRING { $$=$1; P(*$$, *$3); };

maybe_comment: empty | STRING;

/* codes */

maybe_codes: empty | codes;

codes: code | codes code { $$=$1; P(*$$, *$2); };
code: write_string | action;
action: get | put | call;

/* get */

get: get_value {
	$$=N();
	YYSTYPE code=$1;
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

	P(*$$, *code);
};
get_value: '$' get_name_value { $$=$2; };
get_name_value: name_without_curly_rdive EON | name_in_curly_rdive;
name_in_curly_rdive: '{' name_without_curly_rdive '}' { $$=$2; };
name_without_curly_rdive: 
	name_without_curly_rdive_read 
|	name_without_curly_rdive_class;
name_without_curly_rdive_read: name_without_curly_rdive_code {
	$$=N(); 
	YYSTYPE diving_code=$1;
	size_t count=diving_code->count();

	if(maybe_make_self(*$$, *diving_code, count)) {
		// $self.
	} else

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
	if(maybe_make_get_object_element(*$$, *diving_code, count)){
		// optimization for $object.field + ^object.method[
	} else
#endif

#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
	if(maybe_make_get_object_var_element(*$$, *diving_code, count)){
		// optimization for $object.$var
	} else
#endif

#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(count>=4 && (*diving_code)[0].code==OP::OP_VALUE && (*diving_code)[3].code==OP::OP_GET_ELEMENT ){
		 // optimization
		O(*$$,
			(PC.in_call_value && count==4)
			? OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR // ^object[ : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT_OR_OPERATOR+origin+string
			: OP::OP_VALUE__GET_ELEMENT             // $object  : OP_VALUE+origin+string+OP_GET_ELEMENT => OP_VALUE__GET_ELEMENT+origin+string
		);
		P(*$$, *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		if(count>4)
			P(*$$, *diving_code, 4); // copy tail
	} else {
		O(*$$, OP::OP_WITH_READ); /* stack: starting context */
		P(*$$, *diving_code);
	}
#else
	{
		O(*$$, OP::OP_WITH_READ); /* stack: starting context */

		// ^if OP_ELEMENT => ^if OP_ELEMENT_OR_OPERATOR
		// optimized OP_VALUE+origin+string+OP_GET_ELEMENT. => OP_VALUE+origin+string+OP_GET_ELEMENT_OR_OPERATOR.
		if(PC.in_call_value && count==4)
			diving_code->put(count-1, OP::OP_GET_ELEMENT_OR_OPERATOR);
		P(*$$, *diving_code);
	}
#endif
	/* diving code; stack: current context */
};
name_without_curly_rdive_class: class_prefix name_without_curly_rdive_code { $$=$1; P(*$$, *$2); };
name_without_curly_rdive_code: name_advance2 | name_path name_advance2 { $$=$1; P(*$$, *$2); };

/* put */

put: '$' name_expr_wdive construct {
	$$=N();
#ifdef OPTIMIZE_BYTECODE_CONSTRUCT
	if(maybe_optimize_construct(*$$, *$2, *$3)){
		// $a(expr), $.a(expr), $a[value], $.a[value], $self.a[value], $self.a(expr)
	} else 
#endif
	{
		P(*$$, *$2); /* stack: context,name */
		P(*$$, *$3); /* stack: context,name,constructor_value */
	}
};
name_expr_wdive: 
	name_expr_wdive_root
|	name_expr_wdive_write
|	name_expr_wdive_class;
name_expr_wdive_root: name_expr_dive_code {
	$$=N();
	YYSTYPE diving_code=$1;
	size_t count=diving_code->count();

	if(maybe_make_self(*$$, *diving_code, count)) {
		// $self.
	} else
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	if(count>=4 && (*diving_code)[0].code==OP::OP_VALUE && (*diving_code)[3].code==OP::OP_GET_ELEMENT ){
		O(*$$, OP::OP_WITH_ROOT__VALUE__GET_ELEMENT);
		P(*$$, *diving_code, 1/*offset*/, 2/*limit*/); // copy origin+value
		if(count>4)
			P(*$$, *diving_code, 4); // tail
	} else
#endif
	{
		O(*$$, OP::OP_WITH_ROOT); /* stack: starting context */
		P(*$$, *diving_code);
	}
	/* diving code; stack: current context */
};
name_expr_wdive_write: '.' name_expr_dive_code {
	$$=N(); 
	O(*$$, OP::OP_WITH_WRITE); /* stack: starting context */
	P(*$$, *$2); /* diving code; stack: context,name */
};
name_expr_wdive_class: class_prefix name_expr_dive_code { $$=$1; P(*$$, *$2); };

construct:
	construct_square
|	construct_round
|	construct_curly
;
construct_square: '[' {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&$$)=PC.explicit_result; PC.explicit_result=false;
} any_constructor_code_value {
	PC.explicit_result=*reinterpret_cast<bool*>(&$2);
} ']' {
	// stack: context, name
	$$=$3; // stack: context, name, value
	O(*$$, OP::OP_CONSTRUCT_VALUE); /* value=pop; name=pop; context=pop; construct(context,name,value) */
}
;
construct_round: '(' expr_value ')' { 
	$$=N(); 
	// stack: context, name
	P(*$$, *$2); // stack: context, name, value
	O(*$$, OP::OP_CONSTRUCT_EXPR); /* value=pop->as_expr_result; name=pop; context=pop; construct(context,name,value) */
}
;
construct_curly: '{' maybe_codes '}' {
	// stack: context, name
	$$=N(); 
	OA(*$$, OP::OP_CURLY_CODE__CONSTRUCT, $2); /* code=pop; name=pop; context=pop; construct(context,name,junction(code)) */
};

any_constructor_code_value: 
	empty_value /* optimized $var[] case */
|	STRING /* optimized $var[STRING] case */
|	constructor_code_value /* $var[something complex] */
;
constructor_code_value: constructor_code {
	$$=N(); 
	OA(*$$, OP::OP_OBJECT_POOL, $1); /* stack: empty write context */
	/* some code that writes to that context */
	/* context=pop; stack: context.value() */
};
constructor_code: codes__excluding_sole_str_literal;
codes__excluding_sole_str_literal: action | code codes { $$=$1; P(*$$, *$2); };

/* call */

call: call_value {
	size_t count=$1->count();
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
	if(count)
#endif
	{
		$$=$1; /* stack: value */
		if(!change_first(*$$, OP::OP_CONSTRUCT_OBJECT, /*=>*/ OP::OP_CONSTRUCT_OBJECT__WRITE))
			change_or_append(*$$, count-2 /* second last */, OP::OP_CALL, /*=>*/ OP::OP_CALL__WRITE, /*or */ OP::OP_WRITE_VALUE); /* value=pop; wcontext.write(value) */
	}
};
call_value: '^' { 
	PC.in_call_value=true; 
}
call_name {
	PC.in_call_value=false;
} 
store_params EON { /* ^field.$method{vasya} */
#ifdef OPTIMIZE_BYTECODE_CUT_REM_OPERATOR
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT
	const String* operator_name=LA2S(*$3, 0, OP::OP_VALUE__GET_ELEMENT_OR_OPERATOR);
#else
	const String* operator_name=LA2S(*$3, 1);
#endif
	if(operator_name && SYMBOLS_EQ(*operator_name,REM_SYMBOL)){
		$$=N();
	} else 
#endif
		{
			YYSTYPE params_code=$5;
			if(params_code->count()==3) { // probably [] case. [OP::OP_VALUE+origin+Void]
				if(Value* value=LA2V(*params_code)) // it is OP_VALUE+origin+value?
					if(const String * string=value->get_string())
						if(string->is_empty()) // value is empty string?
							params_code=0; // ^zzz[] case. don't append lone empty param.
			}
			/* stack: context, method_junction */

			YYSTYPE var_code=$3;
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
				$$=N();
				O(*$$, OP::OP_CONSTRUCT_OBJECT);
				P(*$$, *var_code, 1/*offset*/, 2/*limit*/); // class name
				P(*$$, *var_code, 5/*offset*/, 2/*limit*/); // constructor name
				OA(*$$, params_code);
			} else 
				{
					$$=var_code; /* with_xxx,diving code; stack: context,method_junction */
					OA(*$$, OP::OP_CALL, params_code); // method_frame=make frame(pop junction); ncontext=pop; call(ncontext,method_frame) stack: value
				}
		}
};

call_name: name_without_curly_rdive {
#ifdef FEATURE_GET_ELEMENT4CALL
	size_t count=$1->count();
	if(count){
		$$=$1;
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_ELEMENT
		!(count==5 && change_first(*$$, OP::OP_GET_OBJECT_ELEMENT, OP::OP_GET_OBJECT_ELEMENT4CALL)) &&
#endif
#ifdef OPTIMIZE_BYTECODE_GET_OBJECT_VAR_ELEMENT
		!(count==5 && change_first(*$$, OP::OP_GET_OBJECT_VAR_ELEMENT, OP::OP_GET_OBJECT_VAR_ELEMENT4CALL)) &&
#endif
		!change(*$$, count-1, OP::OP_GET_ELEMENT, OP::OP_GET_ELEMENT4CALL);
	}
#endif
};

store_params: store_param | store_params store_param { $$=$1; P(*$$, *$2); };
store_param: 
	store_square_param
|	store_round_param
|	store_curly_param
;
store_square_param: '[' {
	// allow ^call[ letters here any time ]
	*reinterpret_cast<bool*>(&$$)=PC.explicit_result; PC.explicit_result=false;
} store_code_param_parts {
	PC.explicit_result=*reinterpret_cast<bool*>(&$2);
} ']' {$$=$3;};
store_round_param: '(' store_expr_param_parts ')' {$$=$2;};
store_curly_param: '{' store_curly_param_parts '}' {$$=$2;};
store_code_param_parts:
	store_code_param_part
|	store_code_param_parts ';' store_code_param_part { $$=$1; P(*$$, *$3); }
;
store_expr_param_parts:
	store_expr_param_part
|	store_expr_param_parts ';' store_expr_param_part { $$=$1; P(*$$, *$3); }
;
store_curly_param_parts:
	store_curly_param_part
|	store_curly_param_parts ';' store_curly_param_part { $$=$1; P(*$$, *$3); }
;
store_code_param_part: code_param_value {
	$$=$1;
};
store_expr_param_part: expr_value {
	YYSTYPE expr_code=$1;
	if(expr_code->count()==3
		&& (*expr_code)[0].code==OP::OP_VALUE) { // optimizing (double/bool/incidently 'string' too) case. [OP::OP_VALUE+origin+Double]. no evaluating
		$$=expr_code; 
	} else {
		YYSTYPE code=N();
		P(*code, *expr_code);
		O(*code, OP::OP_WRITE_EXPR_RESULT);
		$$=N(); 
		OA(*$$, OP::OP_EXPR_CODE__STORE_PARAM, code);
	}
};
store_curly_param_part: maybe_codes {
	$$=N(); 
	OA(*$$, OP::OP_CURLY_CODE__STORE_PARAM, $1);
};
code_param_value:
	empty_value /* optimized [;...] case */
|	STRING /* optimized [STRING] case */
|	constructor_code_value /* [something complex] */
;

/* name */

name_expr_dive_code: name_expr_value | name_path name_expr_value { $$=$1; P(*$$, *$2); };

name_path: name_step | name_path name_step { $$=$1; P(*$$, *$2); };
name_step: name_advance1 '.';
name_advance1: name_expr_value {
	// we know that name_advance1 not called from ^xxx context
	// so we'll not check for operator call possibility as we do in name_advance2

	/* stack: context */
	$$=$1; /* stack: context,name */
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	O(*$$, is_special_element(*$$) ? OP::OP_GET_ELEMENT__SPECIAL : OP::OP_GET_ELEMENT);
#else
	O(*$$, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
#endif
};
name_advance2: name_expr_value {
	/* stack: context */
	$$=$1; /* stack: context,name */
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	O(*$$, is_special_element(*$$) ? OP::OP_GET_ELEMENT__SPECIAL : OP::OP_GET_ELEMENT);
#else
	O(*$$, OP::OP_GET_ELEMENT); /* name=pop; context=pop; stack: context.get_element(name) */
#endif
}
|	STRING BOGUS
;
name_expr_value: 
	STRING /* subname_is_const */
|	name_expr_subvar_value /* $subname_is_var_value */
|	name_expr_with_subvar_value /* xxx$part_of_subname_is_var_value */
|	name_square_code_value /* [codes] */
;
name_expr_subvar_value: '$' subvar_ref_name_rdive {
	$$=$2;
	O(*$$, OP::OP_GET_ELEMENT);
};
name_expr_with_subvar_value: STRING subvar_get_writes {
	YYSTYPE code;
	{
		change_string_literal_to_write_string_literal(*(code=$1));
		P(*code, *$2);
	}
	$$=N(); 
	OA(*$$, OP::OP_STRING_POOL, code);
};
name_square_code_value: '[' {
	// allow $result_or_other_variable[ letters here any time ]
	*reinterpret_cast<bool*>(&$$)=PC.explicit_result; PC.explicit_result=false;
} codes {
	PC.explicit_result=*reinterpret_cast<bool*>(&$2);
} ']' {
	$$=N(); 
#ifdef OPTIMIZE_BYTECODE_GET_ELEMENT__SPECIAL
	if(!maybe_append_simple_diving_code(*$$, *$3))
#endif
	{
		OA(*$$, OP::OP_OBJECT_POOL, $3); /* stack: empty write context */
		/* some code that writes to that context */
		/* context=pop; stack: context.value() */
	}
};
subvar_ref_name_rdive: STRING {
	$$=N(); 
	O(*$$, OP::OP_WITH_READ);
	P(*$$, *$1);
};
subvar_get_writes: subvar__get_write | subvar_get_writes subvar__get_write { $$=$1; P(*$$, *$2); };
subvar__get_write: '$' subvar_ref_name_rdive {
	$$=$2;
	O(*$$, OP::OP_GET_ELEMENT__WRITE);
};

class_prefix:
	class_static_prefix
|	class_constructor_prefix
;
class_static_prefix: STRING ':' {
	$$=$1; // stack: class name string
	OP::OPCODE code = OP::OP_VALUE__GET_CLASS;
	if(*LA2S(*$$) == BASE_NAME) { // pseudo BASE class
		if(VStateless_class* base=PC.cclass->base_class()) {
			change_string_literal_value(*$$, *new String(base->type()));
		} else {
			strcpy(PC.error, "no base class declared");
			YYERROR;
		}
		code = OP::OP_VALUE__GET_BASE_CLASS;
	} else {
		// can't use get_class because it will call @autouse[] if the class wasn't loaded
		VStateless_class* base=PC.request.classes().get(*LA2S(*$$));
		if(base && PC.cclass->derived_from(*base))
			code = OP::OP_VALUE__GET_BASE_CLASS;
	}
	// optimized OP_VALUE+origin+string+OP_GET_CLASS => OP_VALUE__GET_CLASS+origin+string
	change_first(*$$, OP::OP_VALUE, code);
};
class_constructor_prefix: class_static_prefix ':' {
	$$=$1;
	if(!PC.in_call_value) {
		strcpy(PC.error, ":: not allowed here");
		YYERROR;
	}
	O(*$$, OP::OP_PREPARE_TO_CONSTRUCT_OBJECT);
};


/* expr */

expr_value: expr;
expr: 
	double_or_STRING
|   true_value
|   false_value
|	get_value
|	call_value
|	'"' string_inside_quotes_value '"' { $$ = $2; }
|	'\'' string_inside_quotes_value '\'' { $$ = $2; }
|	'(' expr ')' { $$ = $2; }
/* stack: operand // stack: @operand */
|	'-' expr %prec NUNARY { $$=$2;  O(*$$, OP::OP_NEG); }
|	'+' expr %prec NUNARY { $$=$2; }
|	'~' expr { $$=$2;	 O(*$$, OP::OP_INV); }
|	'!' expr { $$=$2;  O(*$$, OP::OP_NOT); }
|	"def" expr { $$=$2;  O(*$$, OP::OP_DEF); }
|	"in" expr { $$=$2;  O(*$$, OP::OP_IN); }
|	"-f" expr { $$=$2;  O(*$$, OP::OP_FEXISTS); }
|	"-d" expr { $$=$2;  O(*$$, OP::OP_DEXISTS); }
/* stack: a,b // stack: a@b */
|	expr '-' expr {	$$=$1;  P(*$$, *$3);  O(*$$, OP::OP_SUB); }
|	expr '+' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_ADD); }
|	expr '*' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_MUL); }
|	expr '/' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_DIV); }
|	expr '%' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_MOD); }
|	expr '\\' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_INTDIV); }
|	expr "<<" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_BIN_SL); }
|	expr ">>" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_BIN_SR); }
|	expr '&' expr { $$=$1; 	P(*$$, *$3);  O(*$$, OP::OP_BIN_AND); }
|	expr '|' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_BIN_OR); }
|	expr "!|" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_BIN_XOR); }
|	expr "&&" expr { $$=$1;  OA(*$$, OP::OP_NESTED_CODE, $3);  O(*$$, OP::OP_LOG_AND); }
|	expr "||" expr { $$=$1;  OA(*$$, OP::OP_NESTED_CODE, $3);  O(*$$, OP::OP_LOG_OR); }
|	expr "!||" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_LOG_XOR); }
|	expr '<' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_NUM_LT); }
|	expr '>' expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_NUM_GT); }
|	expr "<=" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_NUM_LE); }
|	expr ">=" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_NUM_GE); }
|	expr "==" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_NUM_EQ); }
|	expr "!=" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_NUM_NE); }
|	expr "lt" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_STR_LT); }
|	expr "gt" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_STR_GT); }
|	expr "le" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_STR_LE); }
|	expr "ge" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_STR_GE); }
|	expr "eq" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_STR_EQ); }
|	expr "ne" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_STR_NE); }
|	expr "is" expr { $$=$1;  P(*$$, *$3);  O(*$$, OP::OP_IS); }
;

double_or_STRING: STRING {
	// optimized OP_STRING => OP_VALUE for doubles
	maybe_change_string_literal_to_double_literal(*($$=$1));
};

string_inside_quotes_value: maybe_codes {
#ifdef OPTIMIZE_BYTECODE_STRING_POOL
	// it brakes ^if(" 09 "){...}
	YYSTYPE code=$1;
	$$=N();
	if(code->count()==3 && change_first(*code, OP::OP_STRING__WRITE, OP::OP_VALUE)){
		// optimized OP_STRING__WRITE+origin+value => OP_VALUE+origin+value without starting OP_STRING_POOL
		P(*$$, *code);
	} else {
		OA(*$$, OP::OP_STRING_POOL, code); /* stack: empty write context */
	}
#else
	$$=N();
	OA(*$$, OP::OP_STRING_POOL, $1); /* stack: empty write context */
#endif
	/* some code that writes to that context */
	/* context=pop; stack: context.get_string() */
};

/* basics */

write_string: STRING {
	// optimized OP_STRING+OP_WRITE_VALUE => OP_STRING__WRITE
	change_string_literal_to_write_string_literal(*($$=$1));
};

empty_value: /* empty */ { $$=VL(/*we know that we will not change it*/const_cast<VString*>(&vempty), 0, 0, 0); }
true_value: "true" { $$ = VL(/*we know that we will not change it*/const_cast<VBool*>(&vtrue), 0, 0, 0); }
false_value: "false" { $$ = VL(/*we know that we will not change it*/const_cast<VBool*>(&vfalse), 0, 0, 0); }

empty: /* empty */ { $$=N(); };

%%
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
	strncpy(PC.error, s, MAX_STRING);
	return 1;
}

static void yyprint(FILE *file, int type, YYSTYPE value) {
	if(type==STRING)
		fprintf(file, " \"%s\"", LA2S(*value)->cstr());
}

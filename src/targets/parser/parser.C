/*
  $Id: parser.C,v 1.16 2001/02/22 10:43:49 paf Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "pa_hash.h"
#include "pa_array.h"
#include "pa_table.h"
#include "pa_common.h"
#include "pa_pool.h"
#include "pa_vclass.h"

#include "core.h"

char *itoa(int n, char *buf){
    snprintf(buf,MAX_STRING,"%d",n);
    return buf;
}

int main(int argc, char *argv[]) {
	core();
	return 0;

	Exception fatal_exception;
/*	if(EXCEPTION_TRY(fatal_exception)) {*/
		Pool pool(fatal_exception);
		
		char *file="file1";
		String& string=*new(pool) String(pool);
		string.APPEND("Hello, ", 0,file, 1);
		string.APPEND("w", 0,file, 2);
		string.APPEND("o", 0,file, 3);
		string.APPEND("r", 0,file, 4);
		string.APPEND("l", 0,file, 5);
		string.APPEND("d", 0,file, 6);
		string.APPEND("!\n ", 0,file, 7);
		printf(string.cstr());
		
		char *key1_file="key1_file";
		Hash& hash=*new(pool) Hash(pool);
		String key1=string;
		key1.APPEND("1", 0,key1_file, 1);
		String& value1=*new(pool) String(pool);
		value1.APPEND("i'm value1\n", 0,file, 1);
		String& value2=*new(pool) String(pool);
		value2.APPEND("i'm value2\n", 0,file, 1);
		hash.put(key1, &value1);
		char *key2_file="key2_file";
		String key2=string;
		key2.APPEND("2", 0,key2_file, 1);
		hash.put(key2, &value2);
		String *found_value=(String*)hash.get(key2);
		printf(found_value?found_value->cstr():"not found\n");
		
		String& a=*new(pool) String(pool); 	a.APPEND("fi", 0,file, 1); a.APPEND("rst", 0,file, 2);
		String& b=*new(pool) String(pool); 	b.APPEND("fir", 0,file, 1); b.APPEND("st", 0,file, 2);
		printf(a==b?"eq\n":"ne\n");
		
		
		Array& array=*new(pool) Array(pool, 2);
		array+="first";
		array+="second";
		array+="third";
		printf("%s-%s-%s\n", 
			array.get_cstr(0), 
			array.get_cstr(1),
			array.get_cstr(2));
		
		Array& a1=*new(pool) Array(pool);
		a1+="first";
		Array& a2=*new(pool) Array(pool);
		a2+="second";
		Array& asum=*new(pool) Array(pool);
		asum.append_array(a1);
		asum.append_array(a2);
		printf("%s-%s\n", 
			asum.get_cstr(0), 
			asum.get_cstr(1));

		Pool request_pool(fatal_exception);
		Request request(request_pool);

		//Exception operator_exception;
		//Local_request_exception subst(request, operator_exception);
		//if(EXCEPTION_TRY(request.exception())) {
			/*
			Array acolumns(request.pool());
			acolumns+="id";
			acolumns+="name";
			acolumns+="age";
			Table table(request, "_file.cfg", 1, &acolumns);
			*/
			Table table(request, "_file.cfg", 1, 0);
			for(int n=1; n<=5; n++) {
				Array& row=*new(request.pool()) Array(request.pool(), 3/*table.columns()->size()*/);
				char *buf=static_cast<char *>(request.malloc(MAX_STRING));
				row+=itoa(n, buf);
				row+="paf";
				row+="99";
				
				table+=&row;
			}
			/*
			for(int i=0; i<table.columns()->size(); i++) 
				printf("%s\t", table.columns()->get_cstr(i));
			printf("\n");
			*/
			for(table.set_current(0); table.get_current()<table.size(); table.inc_current()) {
				String line(request.pool());
				for(int i=0; i<5/*table.columns()->size()*/; i++) {
					/**/
					String name(request.pool());
					char *buf=static_cast<char *>(request.malloc(MAX_STRING));
					name.APPEND(itoa(i, buf), 0,"names file", 0);
					//name.APPEND("id", "names file", 0);
					table.read_item(line, name);
					/*
					const char *cstr_name=table.columns()->get_cstr(i);
					String name(request.pool());
					name.APPEND(cstr_name, 0, 0);
					table.read_item(line, name);
					/**/
					line.APPEND("\t", 0,0, 0);
				}
				printf("%s\n", line.cstr());
			}

/*
			String it(request.pool());
			it.APPEND("ab.cd[zzz]", 0,0);
			String_iterator si(it);
			/ *si++;
			si++;
			si++;
			si++;
			si++;
			si++;
			* /
			/ *bool found=si.skip_to('.');
			si++;
			* /
			si++;
			Char_types types;
			types.set(' ', 1);
			types.set('[', 2);
			types.set(']', 3);
			int type=si.skip_to(types);
			si++;
*/
			/*
		} else {
			Exception& e=request.exception();
			printf("operator_error occured: %s\n", e.comment());
			const String *type=e.type();
			if(type) {
				printf("  type: %s", type->cstr());
				const String *code=e.code();
				if(code)
					printf(", code: %s", code->cstr());
				printf("\n");
			}
			const String *problem_source=e.problem_source();
			if(problem_source) {
				const Origin& origin=problem_source->origin();
				printf("  '%s'\n", 
					problem_source->cstr());
				if(origin.file)
					printf(" [%s:%d]",
						origin.file, origin.line);
				printf("\n");
			}
		}
	} else {
		printf("fatal exception occured: %s\n", fatal_exception.comment());
	}
*/
	return 0;
}

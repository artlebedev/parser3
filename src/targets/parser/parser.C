/*
  $Id: parser.C,v 1.1 2001/01/30 11:52:22 paf Exp $
*/

#include <stdio.h>
#include <stdlib.h>

#include "pa_pool.h"
#include "pa_hash.h"
#include "pa_array.h"
#include "pa_table.h"
#include "pa_common.h"

char *itoa(int n, char *buf){
    snprintf(buf,MAX_STRING,"%d",n);
    return buf;
}

int main(int argc, char *argv[]) {
	parser_Pool pool;

	char *file="file1";
	String& string=pool.make_string();
	string.APPEND("Hello, ", file, 1);
	string.APPEND("w", file, 2);
	string.APPEND("o", file, 3);
	string.APPEND("r", file, 4);
	string.APPEND("l", file, 5);
	string.APPEND("d", file, 6);
	string.APPEND("!\n ", file, 7);
	printf(string.cstr());

	char *key1_file="key1_file";
	Hash& hash=pool.make_hash();
	String key1=string;
	key1.APPEND("1", key1_file, 1);
	String& value1=pool.make_string();
	value1.APPEND("i'm value1\n", file, 1);
	String& value2=pool.make_string();
	value2.APPEND("i'm value2\n", file, 1);
	hash.put(key1, &value1);
	char *key2_file="key2_file";
	String key2=string;
	key2.APPEND("2", key2_file, 1);
	hash.put(key2, &value2);
	String *found_value=(String*)hash.get(key2);
	printf(found_value?found_value->cstr():"not found\n");

	String& a=pool.make_string(); 	a.APPEND("fi", file, 1); a.APPEND("rst", file, 2);
	String& b=pool.make_string(); 	b.APPEND("fir", file, 1); b.APPEND("st", file, 2);
	printf(a==b?"eq\n":"ne\n");


	Array& array=pool.make_array(2);
	array+="first";
	array+="second";
	array+="third";
	printf("%s-%s-%s\n", 
			array.get_cstr(0), 
			array.get_cstr(1),
			array.get_cstr(2));

	Array& a1=pool.make_array();
	a1+="first";
	Array& a2=pool.make_array();
	a2+="second";
	Array& asum=pool.make_array();
	asum.append_array(a1);
	asum.append_array(a2);
	printf("%s-%s\n", 
			asum.get_cstr(0), 
			asum.get_cstr(1));

	parser_Pool request_pool;
	Request request(&request_pool);
	Exception global_exception;
	request.pool.set_global_exception(&global_exception);
	if(EXCEPTION_TRY(request.pool.global_exception())) {
		Exception local_exception;
		request.pool.set_local_exception(&local_exception);
		if(EXCEPTION_TRY(request.pool.local_exception())) {
			Array acolumns(request.pool);
			acolumns+="id";
			acolumns+="name";
			acolumns+="age";
			Table named_table(request, "_file.cfg", 1, &acolumns);
			for(int n=1; n<=5; n++) {
				Array& row=request.pool.make_array(named_table.columns()->size());
				char *buf=static_cast<char *>(request.pool.malloc(MAX_STRING));
				row+=itoa(n, buf);
				row+="paf";
				row+="99";
				
				named_table+=&row;
			}
			
			for(int i=0; i<named_table.columns()->size(); i++) 
				printf("%s\t", named_table.columns()->get_cstr(i));
			printf("\n");
			for(named_table.set_current(0); named_table.get_current()<named_table.size(); named_table.inc_current()) {
				String line(request.pool);
				for(int i=0; i<named_table.columns()->size(); i++) {
					//String name(request.pool);
					//char *buf=static_cast<char *>(request.pool.malloc(MAX_STRING));
					//name.APPEND(itoa(i, buf), "names file", 0);
					//name.APPEND("id", "names file", 0);
					//named_table.read_item(line, name);
					const char *cstr_name=named_table.columns()->get_cstr(i);
					String name(request.pool);
					name.APPEND(cstr_name, 0, 0);
					named_table.read_item(line, name);
					line.APPEND("\t", 0, 0);
				}
				printf("%s\n", line.cstr());
			}
		} else {
			printf("operator_error occured: \n");
			const String *problem_source=request.operator_error.problem_source();
			if(problem_source) {
				const Origin& origin=problem_source->origin();
				printf("  origin: '%s', file '%s', line %d\n", 
					problem_source->cstr(),
					origin.file, origin.line);
			}
			printf("  comment: %s\n", request.operator_error.comment());
		}
	} else {
		printf("request_error occured: \n");
		const String *problem_source=request.request_error.problem_source();
		if(problem_source) {
			const Origin& origin=problem_source->origin();
			printf("  origin: '%s', file '%s', line %d\n", 
				problem_source->cstr(),
				origin.file, origin.line);
		}
		printf("  comment: %s\n", request.request_error.comment());
	}

	return 0;
}

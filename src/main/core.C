// TODO: $RESULT

enum Prefix {
	NO_PREFIX,
	ROOT_PREFIX,
	SELF_PREFIX
};

enum CHAR_TYPE {
	NO_TYPE=0, // these got skipped by String::skip_to
	VAR_START_TYPE,
	METHOD_START_TYPE,
	STOP_TYPE,///=-1 // same as EOF type(-1), see String::skip_to
	DOT_TYPE,
	CONSTRUCTOR_BODY_START_TYPE,
	CONSTRUCTOR_BODY_FINISH_TYPE,
	BLOCK_START_TYPE,
	BLOCK_FINISH_TYPE,
	
	Z_TYPE
};

Char_types var_or_method_start;
Char_types var_or_method_start_or_constructor_stop;
Char_types var_or_method_start_or_block_stop;
Char_types common_names_breaks, var_names_breaks, method_names_breaks;
String SELF;
String RESULT;

void prepare() {
	var_or_method_start.set('$', VAR_START_TYPE);
	var_or_method_start.set('^', METHOD_START_TYPE);

	var_or_method_start_or_constructor_stop=var_or_method_start;
	var_or_method_start_or_constructor_stop.set(')', STOP_TYPE);

	var_or_method_start_or_block_stop=var_or_method_start;
	var_or_method_start_or_block_stop.set(']', STOP_TYPE);

	common_names_breaks.set(0, ' ', STOP_TYPE);
	common_names_breaks.set('.', DOT_TYPE);
	common_names_breaks.set(')', STOP_TYPE); // var_or_method_start_or_constructor_stop
	common_names_breaks.set(']', STOP_TYPE); // var_or_method_start_or_block_stop

	var_names_breaks=common_names_breaks;
	var_names_breaks.set('(', CONSTRUCTOR_BODY_START_TYPE);

	method_names_breaks=common_names_breaks;
	method_names_breaks.set('[', BLOCK_START_TYPE);

	SELF.APPEND("self", 0, 0);
	RESULT.APPEND("result", 0, 0);
}

CHAR_TYPE process(method_self_n_params_n_locals& root, Value& self,
				  arcontext& arcontext, WContext& awcontext, 
				  String_iterator& iter, Char_types& breaks) {
	while(!iter.eof()) {
		String_iterator start(iter);
		CHAR_TYPE type=iter.skip_to(breaks);
		awcontext.write(start, iter);

		switch(type) {
		case VAR_START_TYPE: 
			process_var(root, self, arcontext, awcontext, iter, breaks);
			break;
		case METHOD_START_TYPE:
			process_method(root, self, arcontext, awcontext, iter, breaks);
			break;
		default:
			return type;
		}
	}
	return -1;
}

void process_var(method_self_n_params_n_locals& root, Value& self,
				 arcontext& arcontext, WContext& awcontext, 
				 String_iterator& iter, Char_types& breaks) {

	// $name.field.subfield -- read
	// $name.field.subfield(constructor code) -- construct
	// $name.field.subfield[usage code & if none existed autoconstructed as VHash] -- use OR auto-VHash construct
	
	Prefix prefix;
	Array/*<String&>*/ names(pool);  // what.they.refer.to left-to-right list
	// the char type after long name
	CHAR_TYPE names_ended_before=get_names( 
		iter, var_names_breaks,
		&prefix, &names); // can return size()==0 when $self alone

	bool read_mode=name_ended_before==' ';
	Value *context=
		prefix?
			prefix==ROOT_PREFIX?root:self:
		read_mode?arcontext:awcontext;
	
	if(read_mode) {
		// 'context' dive into dotted path
		for(int i=0; i<names.size(); i++) {
			context=context->get_element(static_cast<Value *>(names.get[i]));
			if(!context) // no such object field, nothing bad, just ignore that
				return;
		}
		awcontext.write(context);
	} else { // write mode
		iter++; // skip '(' '['

		bool construct_mode=names_ended_before=='(';

		// 'context' dive into dotted path, 
		int steps=names.size();
		// if constructing then "excluding last .name"
		if(construct_mode)
			if(!steps--) // bad: "$self("; now we can safely do ".get[steps]" below
				pool.exception().raise("self re-construction prohibited");
		for(int i=0; i<steps; i++) {
			String& name=static_cast<String&>(names.get[i]);
			Value *next_current=context->get_element(name);
			if(next_current)
				next_current=context->put_element(name, new(pool) VHash(pool));
			context=new_current;
		}

		if(construct_mode) {  
			// .name(construct-code), processing on arcontext in empty temp awcontext
			// pure side effect, no awcontext.write here
			// last .name
			// prepare context
			String& name=static_cast<String&>(names.get[steps]);
			WContext local_wcontext(pool /* empty */);
			// evaluate constructor-code in that context
			process(root, self, arcontext, local_wcontext, iter, ')');
			// store constructed value under 'name'
			context->put_element(name, local_wcontext.value());
		} else { // =='['  .name[with-code]
			// prepare context
			WContext local_context(pool, context);
			// evaluate with-code in that context
			process(root, self, local_context, local_context, iter, ']');
			// emit result
			awcontext.write(local_context);
		}
		
		iter++; // skip ')' ']'
	}
}

void process_method(method_self_n_params_n_locals& root, Value& self,
					arcontext& arcontext, WContext& awcontext, 
					String_iterator& iter, char char_to_stop_before) {
	
	// ^name.field.subfield.method[..] -- plain call
	// ^name.field.subfield.method_ref[..] -- method ref call, when .get_method()!=0
	// ^class:method[..]  -- no dotted path allowed before/after
	//  1: wcontext.object_class == 0?  -- constructor
	//  2: wcontext.object_class.has_parent('class')? -- dynamic call
	//  3: not -------------------------------------? -- static call
	
	Prefix prefix;
	Array/*<String&>*/ names(pool);  // what.they.refer.to left-to-right list
	// the char type after long name
	CHAR_TYPE names_ended_before=get_names(
		iter, method_names_breaks,
		&prefix, &names); // can return size()==0 when ^self alone

	Value *context=
		prefix?
			prefix==ROOT_PREFIX?root:self:
		arcontext;
	iter++; // skip '['

	// 'context' dive into dotted path, excluding last .name
	int steps=names.size()-1;
	if(steps<0) // bad: "^self["; now we can safely do ".get[steps]" below
		pool.exception().raise("call: calling method named 'self'");
	for(int i=0; i<steps; i++) {
		String& name=static_cast<String&>(names.get[i]);
		context=context->get_element(name);
		if(!context) // no such object field, sad story: can't call method of void
			pool.exception().raise(name, "call: to void.method");
	}
	
	// last .name
	String& name=static_cast<String&>(names.get[steps]);
	if(steps==0) { // the sole name on path, maybe ^class:method[ call
		String_iterator ni(name);
		if(ni.skip_to(':')) { // it is
			ni++; // skip ':'
			String method_name(pool); method_name.append(ni, 0);
			name=method_name; // trim "class:" prefix from the name

			String cn(pool);  cn.append(0, ni);
			Class *right_class=classes.get(cn);
			if(!right_class) // bad: no such class
				pool.exception().raise(cn, "call: undefined class");
			Class *left_class=awcontext.get_class();
			if(left_class) {
				if(left_class.has_parent(right_class)) // dynamic call
					context=awcontext.value(); // it's 'self' instance
				else // static call
					context=right_class; // 'self' := class, not instance
			} else { // constructor: $some(^class:method[..]) call
				context=new(pool) VClass(pool, right_class); // 'self' := new instance of 'class:'
				awcontext.write(context);
			}
		}
	}
	// first we're trying to locate method with that 'name'
	Method *method=context.get_method(name);
	if(!method) { // no such method: try to locate method ref field
		Value *value=context.get_element(name);
		if(value) { // good: we have some element of that 'name'
			Method_ref *method_ref=value->get_method_ref();
			if(!method_ref) // bad: that field wasn't method_ref
				pool.exception().raise(name, "call: this field is not a method reference");
			context=method_ref->self;
			method=method_ref->method;
		} else { // no element of that 'name', that must be operator then
			Operator *op=operators.get(name);
			if(!op) // bad: that 'name' is neither method nor field nor operator
				pool.exception().raise(name, "call: neither method nor field nor operator");
			context=op->self;
			method=op;
		}
	}


	// evaluate param values
	Array/*<Value&>*/ param_values(pool);
	get_params(
		iter,
		arcontext,
		&param_values);
	iter++; // skip ']'

	// prepare contexts
	Method_self_n_params_n_locals local_rcontext(pool, 
		context,
		method->param_names, param_values,
		method->local_names);
	WContext local_wcontext(pool, context);
	String_iterator local_iter(method->code);
	// call method/operator in those contexts
	process(
		local_rcontext/* $:vars */, context /* $self.vars */,
		local_rcontext, local_wcontext, 
		local_iter, 0);
	// emit result
	awcontext.write(local_wcontext);
}


CHAR_TYPE get_names(
		String_iterator& iter, Char_types& breaks,
		Prefix& prefix, Array& names) {
	// $name.subname white-space
	// $name.subname( $name.subname[
	// $name.subname) $name.subname]
	// $:name... $self...

	if(iter.eof())
		return -1;

	if(iter()==':') { // $:name ?
		prefix=ROOT_PREFIX;
		iter++; // skip ':'
	} else // $name
		prefix=NO_PREFIX;

	CHAR_TYPE result;
	while(true) {
		// prepare context
		WContext local_wcontext(pool /* empty */);
		// execute code until separator, writing to that context
		result=process(root, self,
			arcontext, local_wcontext, 
			iter, breaks);
		// read resulting name
		String *name=local_wcontext.get_string();
		if(*name==SELF) // is it "self"?
			if(prefix || names.size()) // already $: or $self.  or $name.
				pool.exception().raise("names: 'self' not first on chain");
			else // $self.
				prefix=SELF_PREFIX;
		else // simple $name or .name, emiting
			names+=name;

		if(result!=DOT_TYPE) // not "name." ?
			break;
	}

	// can only return size()==0 when $self alone
	if(names.size()==0 && prefix!=SELF_PREFIX)
		pool.exception().raise("names: empty chain");

	return result;
}

void get_params(
		String_iterator& iter,
		Value *arcontext,
		Array/*<Values&>*/& param_values) {
}

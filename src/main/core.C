void process(method_self_n_params_n_locals& root, Value& self,
			 arcontext& arcontext, WContext& awcontext, 
			 String_iterator& code, char char_to_stop_before) {
	
	// $ on code?
	process_dollar(root, self, arcontext, awcontext, code, char_to_stop_before);

	// ^ on code?
	process_bird(root, self, arcontext, awcontext, code, char_to_stop_before);

	// TODO
	// 2. вызовы статических методов
}

void process_dollar(method_self_n_params_n_locals& root, Value& self,
					arcontext& arcontext, WContext& awcontext, 
					String_iterator& iter, char char_to_stop_before) {

	// $name.field.subfield -- read
	// $name.field.subfield(constructor code) -- construct
	// $name.field.subfield[usage code & if none existed autoconstructed as VHash] -- use OR auto-VHash construct
	
	Array/*<String&>*/ names(pool);  // what.they.refer.to left-to-right list
	char names_ended_before; // the char after long name
	Prefix prefix;
	get_names(
		iter, " ([",
		&prefix, &names, &names_ended_before); // can return count()=0 when $self alone

	bool read_mode=name_ended_before==' ';
	Value *context=
		prefix?
			prefix==ROOT_PREFIX?root:self:
		read_mode?arcontext:awcontext;
	
	if(read_mode) {
		// 'context' dive into dotted path
		for(int i=0; i<names.count(); i++) {
			context=context->get_element(static_cast<Value *>(names.get[i]));
			if(!context) // no such object field, nothing bad, just ignore that
				return;
		}
		awcontext.write(context);
	} else { // write mode
		iter++; // skip '(' '['

		bool construct_mode=names_ended_before=='(';

		// 'context' dive into dotted path, 
		int steps=names.count();
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
			String& name=static_cast<String&>(names.get[steps]);
			WContext local_wcontext(pool /* empty */);
			process(root, self, arcontext, local_wcontext, iter, ')');
			context->put_element(name, local_wcontext.value());
		} else { // =='['
			// .name[with-code], processing on 'context'
			WContext local_context(pool, context);
			process(root, self, local_context, local_context, iter, ']');
			awcontext.write(local_context);
		}
		
		iter++; // skip ')' ']'
	}
}

void process_bird(method_self_n_params_n_locals& root, Value& self,
				  arcontext& arcontext, WContext& awcontext, 
				  String_iterator& iter, char char_to_stop_before) {
	
	// ^name.field.subfield.method[..] -- plain call
	// ^name.field.subfield.method_ref[..] -- method ref call, when .get_method()!=0
	// ^class:method[..]  -- no dotted path allowed before/after
	//  1: wcontext.object_class == 0?  -- constructor
	//  2: wcontext.object_class.has_parent('class')? -- dynamic call
	//  3: not -------------------------------------? -- static call
	
	Array/*<String&>*/ names(pool);  // what.they.refer.to left-to-right list
	char names_ended_before; // the char after long name
	Prefix prefix;
	get_names(
		iter, "[",
		&prefix, &names, &names_ended_before); // can return count()=0 when ^self alone

	Value *context=
		prefix?
			prefix==ROOT_PREFIX?root:self:
		arcontext;
	iter++; // skip '['

	// 'context' dive into dotted path, excluding last .name
	int steps=names.count()-1;
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
			if(!oc) // bad: no such class
				pool.exception().raise(cn, "call: undefined class");
			Class *left_class=awcontext.get_class();
			if(left_class) {
				if(left_class.has_parent(right_class)) // dynamic call
					;
				else // static call
					context=right_class;
			} else { // constructor: $some(^class:method[..]) call
				context=new(pool) VClass(pool, right_class);
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
			method=method_ref->method;
			context=method_ref->self;
		} else { // no element of that 'name', that must be operator then
			method=operators.get(name);
			if(!method) // bad: that 'name' is neither method nor field nor operator
				pool.exception().raise(name, "call: neither method nor field nor operator");
		}
	}


	Array/*<String&>*/ param_values(pool);
	get_params(
		iter,
		awcontext,
		&param_values);
	iter++; // skip ']'

	method_self_n_params_n_locals local_rcontext(pool, 
		context,
		method->param_names, param_values,
		method->local_names);
	WContext local_wcontext(pool, local_self);
	process(
		context, local_rcontext, 
		local_rcontext, local_wcontext, 
		iter, ']');
	awcontext.write(local_wcontext);
}
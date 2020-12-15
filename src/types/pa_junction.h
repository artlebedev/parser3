/** @file
	Parser: Junction class decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_JUNCTION_H
#define PA_JUNCTION_H

#define IDENT_PA_JUNCTION_H "$Id: pa_junction.h,v 1.15 2020/12/15 17:10:38 moko Exp $"


#include "pa_string.h"
#include "pa_array.h"
#include "pa_exception.h"
#include "pa_operation.h"
#include "pa_value.h"

/** \b junction is some code joined with context of it's evaluation.

	there are code-junctions and method-junctions
	- code-junctions are used when some parameter passed in cury brackets
	- method-junctions used in ^method[] calls or $method references

	Junctions register themselves in method_frame [if any] for consequent invalidation.
	This prevents evaluation of junctions in outdated context

	To stop situations like this:
@code
	@main[]
	^method1[]
	^method2[]

	@method1[]
	$junction{
		some code
	}

	@method2[]
	^junction[]
@endcode

	On wcontext[most dynamic context of all] scope exit (WContext::~WContext()) got cleaned - 
	Junction::wcontext becomes WContext.fparent (if any), 
	or Junction::method_frame becomes 0 (if no parent), which later in Request::process triggers exception

	parent changing helps ^switch implementation to remain simple
*/
class Junction {
public:

	/// Code-Junction constructor
	Junction(Value& aself,
		const Method* amethod,
		VMethodFrame* amethod_frame,
		Value* arcontext,
		WContext* awcontext,
		ArrayOperation* acode
	): self(aself),
		method(amethod),
		method_frame(amethod_frame),
		rcontext(arcontext),
		wcontext(awcontext),
		code(acode)
		{}

	/// Method-Junction or Getter-Junction constructor
	Junction(Value& aself,
		const Method* amethod,
		bool ais_getter=false,
		String* aauto_name=0
	): self(aself),
		method(amethod),
		auto_name(aauto_name),
		is_getter(ais_getter)
	 	{}


	void reattach(WContext *new_wcontext);

	/// always present
	Value& self;
	//@{
	/// @name either these // so called 'method-junction'
	const Method* method;
	//@}
	//@{
	/// @name or these are present // so called 'code-junction'
	VMethodFrame* method_frame;
	Value* rcontext;
	WContext* wcontext;
	ArrayOperation* code;
	//@}
	//@{
	String* auto_name;
	//@}
	//@{
	bool is_getter;
	//@{
};

#endif

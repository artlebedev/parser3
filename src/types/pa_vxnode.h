/** @file
	Parser: @b xnode parser class decl.

	Copyright (c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VXNODE_H
#define PA_VXNODE_H

static const char* IDENT_VXNODE_H="$Date: 2002/10/31 15:01:57 $";

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

// defines

#define VXNODE_TYPE "xnode"

// externals

extern Methoded *Xnode_class;

//void VXnode_cleanup(void *);

/// value of type 'xnode'. implemented with GdomeNode
class VXnode : public VStateless_object {
	friend void VXnode_cleanup(void *);
public: // Value

	const char *type() const { return VXNODE_TYPE; }
	VStateless_class *get_class() { return Xnode_class; }

	/// VXnode: true
	bool as_bool() const { return true; }

	/// VXnode: true
	Value *as_expr_result(bool return_string_as_is=false) { return NEW VBool(pool(), as_bool()); }

	/// VXnode: $CLASS,$method, fields
	Value *get_element(const String& aname, Value& aself, bool /*looking_up*/);

public: // usage

	VXnode(Pool& apool, GdomeNode *anode) : 
		VStateless_object(apool),
		fnode(anode/*not adding ref, owning a node*/) {
		GdomeException exc;

		register_cleanup(VXnode_cleanup, this);
	}
protected:
	~VXnode() {
		GdomeException exc;
		if(fnode)			
			gdome_n_unref(fnode, &exc);
	}
public:

	void set_node(GdomeNode *anode, bool aowns_node) { 
		GdomeException exc;
		if(fnode)			
			gdome_n_unref(fnode, &exc);

		gdome_n_ref(fnode=anode, &exc);
	}

public: // VXnode
	virtual GdomeNode *get_node(const String *source) { 
		if(!fnode)
			throw Exception(0,
				source,
				"can not be applied to uninitialized instance");

		return fnode; 
	}

private:

	GdomeNode *fnode;

};

#endif

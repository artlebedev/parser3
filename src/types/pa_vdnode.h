/** @file
	Parser: @b DOM parser class decl.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)

	Author: Alexander Petrosyan <paf@design.ru> (http://design.ru/paf)

	$Id: pa_vdnode.h,v 1.2 2001/09/20 14:25:06 parser Exp $
*/

#ifndef PA_VDNODE_H
#define PA_VDNODE_H

#include "classes.h"
#include "pa_common.h"
#include "pa_vstateless_object.h"

#include <XalanDOM/XalanNode.hpp>
#include <PlatformSupport/XSLException.hpp>

extern Methoded *Dnode_class;

//void VDnode_cleanup(void *);

/// value of type 'dnode'. implemented with XalanNode
class VDnode : public VStateless_object {
	//friend void VDnode_cleanup(void *);
public: // Value

	const char *type() const { return "dnode"; }

	/// VDnode: $CLASS,$method, fields
	Value *get_element(const String& aname);

protected: // VAliased

	/// disable .CLASS element. @see VAliased::get_element
	bool hide_class() { return true; }

public: // usage

	VDnode(Pool& apool, XalanNode *anode=0, VStateless_class& abase=*Dnode_class) : 
		VStateless_object(apool, abase), 
		fnode(anode) {
		//register_cleanup(VDnode_cleanup, this);
	}
private:
	//void cleanup() {}
public:

	void set_node(XalanNode& anode) { fnode=&anode; }

public: // VDnode
	virtual XalanNode &get_node(Pool& pool, const String *source) { 
		if(!fnode)
			PTHROW(0, 0,
				source,
				"can not be applied to uninitialized instance");
		return *fnode; 
	}

private:

	XalanNode *fnode;

};

#endif

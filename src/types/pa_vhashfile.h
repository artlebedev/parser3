/** @file
	Parser: @b hashfile parser type decl.

	Copyright (c) 2001-2003 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASHFILE_H
#define PA_VHASHFILE_H

static const char* IDENT_VHASHFILE_H="$Date: 2003/11/06 11:53:06 $";

#include "classes.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"

#include "apr_sdbm.h"

// defines

#define VHASHFILE_TYPE "hashfile"

// externs

extern Methoded *hashfile_class;

// forwards

class Autosave_marked_to_cancel_cache ;

/// value of type 'hashfile', implemented with SDBM library bundled in ../libs/sdbm
class VHashfile : public VStateless_object, PA_Cleaned {
public: // value

	override const char *type() const { return VHASHFILE_TYPE; }
	override VStateless_class *get_class() { return hashfile_class; }

	/// VHashfile: true
	override bool is_defined() const { return true; }

	/// VHashfile: convert to VHash
	override HashStringValue *get_hash();

	/// VHashfile: (key)=value
	override Value* get_element(const String& aname, Value& aself, bool looking_up) { 
		// $CLASS,$method
		if(Value *result=VStateless_object::get_element(aname, aself, looking_up))
			return result;
		
		// $element
		return get_field(aname);
	}
	
	/// VHashfile: (key)=value, (key)=(value+expires)
	override bool put_element(const String& aname, Value* avalue, bool /*replace*/) { 
		put_field(aname, avalue);
		return false; // nobody is supposed to derive from hashfile, so does not matter
	}

public: // usage

	VHashfile(): db(0) {}
	override ~VHashfile();

	void open(const String& afile_name);
	void make_writable();
	void clear();
	void remove(const String& aname);

	void for_each(void callback(apr_sdbm_datum_t, void*), void* info) const;
	void for_each(void callback(const String::Body, const String&, void*), void* info) const;

private:

	Value *get_field(const String& aname);
	void put_field(const String& aname, Value *avalue);

private:

	const char* file_name;
	apr_sdbm_t *db;

};

#endif

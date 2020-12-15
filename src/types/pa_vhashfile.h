/** @file
	Parser: @b hashfile parser type decl.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#ifndef PA_VHASHFILE_H
#define PA_VHASHFILE_H

#define IDENT_PA_VHASHFILE_H "$Id: pa_vhashfile.h,v 1.48 2020/12/15 17:10:41 moko Exp $"

#include "classes.h"
#include "pa_pool.h"
#include "pa_value.h"
#include "pa_hash.h"
#include "pa_vint.h"

#include "pa_sdbm.h"

// defines

#define VHASHFILE_TYPE "hashfile"

// externs

extern Methoded *hashfile_class;

/// value of type 'hashfile', implemented with SDBM library bundled in ../libs/sdbm
class VHashfile : public VStateless_object, Pooled {
public: // value

	override const char *type() const { return VHASHFILE_TYPE; }
	override VStateless_class *get_class() { return hashfile_class; }

	/// VHashfile: convert to VHash
	override HashStringValue *get_hash();

	override HashStringValue* get_fields() { return get_hash(); }

	/// VHashfile: (key)=value
	override Value* get_element(const String& aname) { 
		// $method
		if(Value *result=VStateless_object::get_element(aname))
			return result;
		
		// $element
		return get_field(aname);
	}
	
	/// VHashfile: (key)=value, (key)=(value+expires)
	override const VJunction* put_element(const String& aname, Value* avalue) {
		put_field(aname, avalue);
		return 0; // nobody is supposed to derive from hashfile, so does not matter
	}

public: // usage

	VHashfile(Pool& apool): Pooled(apool), m_db(0) {}
	override ~VHashfile();

	void open(const String& afile_name);
	void close();
	bool is_open();
	pa_sdbm_t *get_db_for_reading();
	pa_sdbm_t *get_db_for_writing();
	// void clear();
	void delete_files();
	void remove(const String& aname);

	void for_each(bool callback(pa_sdbm_datum_t, void*), void* info);
	void for_each(bool callback(const String::Body, const String&, void*), void* info);

public:

	void remove(const pa_sdbm_datum_t key);

	pa_sdbm_datum_t serialize_value(const String& string, time_t time_to_die) const;
	const String* deserialize_value(const pa_sdbm_datum_t key, const pa_sdbm_datum_t value);

	void check(const char *step, pa_status_t status);

private:

	Value *get_field(const String& aname);
	void put_field(const String& aname, Value *avalue);

private:

	const char* file_name;
	pa_sdbm_t *m_db;

};

#endif

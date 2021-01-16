/** @file
	Parser: @b table class.

	Copyright (c) 2001-2020 Art. Lebedev Studio (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

#include "pa_globals.h"
#include "pa_common.h"
#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_vdate.h"

volatile const char * IDENT_PA_VHASHFILE_C="$Id: pa_vhashfile.C,v 1.73 2021/01/16 15:47:06 moko Exp $" IDENT_PA_VHASHFILE_H;

// consts

const uint HASHFILE_VALUE_SERIALIZED_VERSION=0x0001;

// methods

void check_dir(const char* file_name){
	String& sfile_name = *new String(file_name);
	if(!entry_exists(sfile_name))
		create_dir_for_file(sfile_name);
}

void VHashfile::check(const char *step, pa_status_t status) {
	if(status==PA_SUCCESS)
		return;

	const char* str=strerror(status);
	throw Exception("file.access", 0, "%s error: %s (%d), actual filename '%s'", step, str ? str : "<unknown>", status, file_name);
}

void VHashfile::open(const String& afile_name) {
	file_name=afile_name.taint_cstr(String::L_FILE_SPEC);
}

void VHashfile::close() {
	if(!is_open())
		return;

	check("pa_sdbm_close", pa_sdbm_close(m_db));
	m_db=0;
}

bool VHashfile::is_open() {
	return m_db != 0;
}

pa_sdbm_t *VHashfile::get_db_for_reading() {
	if(is_open()){
		return m_db;
	}

	if(file_name){
		check_dir(file_name);
		check("pa_sdbm_open(shared)", pa_sdbm_open(&m_db, file_name, PA_CREATE|PA_READ|PA_SHARELOCK, 0664, 0));
	}

	if(!is_open())
		throw Exception("file.read", 0, "can't open %s for reading", type());

	return m_db;
}

pa_sdbm_t *VHashfile::get_db_for_writing() {
	if(is_open()){
		if(pa_sdbm_rdonly(m_db)) {
			close(); // close if was opened for reading
		} else {
			return m_db;
		}
	}

	if(file_name) {
		check_dir(file_name);
		// reopen in write mode & exclusive lock
		check("pa_sdbm_open(exclusive)", pa_sdbm_open(&m_db, file_name, PA_CREATE|PA_WRITE, 0664, 0));
	}

	if(!is_open())
		throw Exception("file.access", 0, "can't open %s for writing", type());

	return m_db;
}

VHashfile::~VHashfile() {
	if(is_open())
		close();
}

struct Hashfile_value_serialized_prolog {
	uint version;
	time_t time_to_die;
};

pa_sdbm_datum_t VHashfile::serialize_value(const String& string, time_t time_to_die) const {
	pa_sdbm_datum_t result;

	size_t length=string.length();
	result.dsize=sizeof(Hashfile_value_serialized_prolog)+length;
	result.dptr=new(PointerFreeGC) char[result.dsize];

	Hashfile_value_serialized_prolog& prolog=*reinterpret_cast<Hashfile_value_serialized_prolog*>(result.dptr);
	char *output_cstr=result.dptr+sizeof(Hashfile_value_serialized_prolog);

	prolog.version=HASHFILE_VALUE_SERIALIZED_VERSION;
	prolog.time_to_die=time_to_die;
	if(length) // reported errors on storing empty values to hashfiles, but without details. maybe here [win32, intel:solaris, freebsd were OK...]
		memcpy(output_cstr, string.cstr(), length);

	return result;
}

const String* VHashfile::deserialize_value(pa_sdbm_datum_t key, const pa_sdbm_datum_t value) {
	// key not found || it's surely not in our format
	if(!value.dptr || (size_t)value.dsize<sizeof(Hashfile_value_serialized_prolog))
		return 0; 

	// [WARNING: not cast, addresses must be %4=0 on sparc]
	Hashfile_value_serialized_prolog prolog;
	memcpy(&prolog, value.dptr, sizeof(prolog));
	
	if(prolog.version!=HASHFILE_VALUE_SERIALIZED_VERSION
		|| (prolog.time_to_die/*specified*/ 
			&& (prolog.time_to_die <= time(0)/*expired*/))) {
		// old format || exipred value
		remove(key);
		return 0;
	}
	
	char *input_cstr=value.dptr+sizeof(Hashfile_value_serialized_prolog);
	size_t input_length=value.dsize-sizeof(Hashfile_value_serialized_prolog);

	return new String(input_length ? pa_strdup(input_cstr, input_length) : 0, String::L_TAINTED);
}

#define CHECK(aname) if(aname.is_empty()) throw Exception(PARSER_RUNTIME, 0, "hashfile key must not be empty");

void VHashfile::put_field(const String& aname, Value *avalue) {
	CHECK(aname)

	pa_sdbm_t *db=get_db_for_writing();

	time_t time_to_die=0;
	const String *value_string;

	if(HashStringValue *hash=avalue->get_hash()) {
		if(Value *value_value=hash->get(value_name)) {
			if(value_value->get_junction())
				throw Exception(PARSER_RUNTIME, 0, VALUE_NAME" must not be code");

			value_string=&value_value->as_string();

			if(Value *expires=hash->get(expires_name)) {
				if(Value* vdate=expires->as(VDATE_TYPE))
					time_to_die=(time_t)(static_cast<VDate*>(vdate)->get_time()); // $expires[DATE]
				else if(double days_till_expire=expires->as_double())
					time_to_die=time(NULL)+(time_t)(60*60*24*days_till_expire); // $expires(days)
			}
		} else
			throw Exception(PARSER_RUNTIME, &aname, "put hash value must contain ." VALUE_NAME);
	} else
		value_string=&avalue->as_string();

	pa_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	pa_sdbm_datum_t value=serialize_value(*value_string, time_to_die);

#ifndef PAIRMAX
// !see PAIRMAX definition in sdbm_private.h. values should be the same
#define PAIRMAX 8008
#endif

	if(key.dsize+value.dsize > PAIRMAX)
		throw Exception(PARSER_RUNTIME, 0,"hashfile record length (key+value) exceeds limit (%d bytes)", PAIRMAX);

 	check("pa_sdbm_store", pa_sdbm_store(db, key, value, PA_SDBM_REPLACE));
}

Value *VHashfile::get_field(const String& aname) {
	CHECK(aname)

	pa_sdbm_t *db=get_db_for_reading();

	pa_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	pa_sdbm_datum_t value;

	check("pa_sdbm_fetch", pa_sdbm_fetch(db, &value, key));

	const String *sresult=deserialize_value(key, value);
	return sresult? new VString(*sresult): 0;
}

void VHashfile::remove(const pa_sdbm_datum_t key) {
	pa_sdbm_t *db=get_db_for_writing();

	check("pa_sdbm_delete", pa_sdbm_delete(db, key));
}

void VHashfile::remove(const String& aname) {
	CHECK(aname)

	pa_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	remove(key);
}

void VHashfile::for_each(bool callback(pa_sdbm_datum_t, void*), void* info) {
	pa_sdbm_t *db=get_db_for_reading();

	// collect keys
	Array<pa_sdbm_datum_t>* keys=0;
	check("pa_sdbm_lock", pa_sdbm_lock(db, PA_FLOCK_SHARED));
	try {
		pa_sdbm_datum_t key;
		if(pa_sdbm_firstkey(db, &key)==PA_SUCCESS)
		{
			size_t count=0;
			do {
				// must cound beforehead, becase doing reallocs later would be VERY slow and cause HUGE fragmentation
				count++;
			} while(pa_sdbm_nextkey(db, &key)==PA_SUCCESS);

			keys=new Array<pa_sdbm_datum_t>(count);

			if(pa_sdbm_firstkey(db, &key)==PA_SUCCESS)
				do {
					// must clone because it points to page which may go away 
					// [if they modify hashfile inside foreach]
					key.dptr = pa_strdup(key.dptr, key.dsize);
					*keys+=key;
				} while(pa_sdbm_nextkey(db, &key)==PA_SUCCESS);
		}
	} catch(...) {
		check("pa_sdbm_unlock", pa_sdbm_unlock(db));
		rethrow;
	}
	check("pa_sdbm_unlock", pa_sdbm_unlock(db));

	// iterate them
	if(keys)
		keys->for_each(callback, info);
}

#ifndef DOXYGEN
struct For_each_string_callback_info {
	VHashfile* self;
	void* nested_info;
	bool (*nested_callback)(const String::Body, const String&, void*);
};
#endif
static bool for_each_string_callback(pa_sdbm_datum_t apkey, void* ainfo) {
	For_each_string_callback_info& info=*static_cast<For_each_string_callback_info *>(ainfo);
	pa_sdbm_t *db=info.self->get_db_for_reading();

	pa_sdbm_datum_t apvalue;
	info.self->check("pa_sdbm_fetch", pa_sdbm_fetch(db, &apvalue, apkey));

	if(const String* svalue=info.self->deserialize_value(apkey, apvalue)) {
		const char *clkey=pa_strdup(apkey.dptr, apkey.dsize);

		return info.nested_callback(clkey, *svalue, info.nested_info);
	}
	return false;
}
void VHashfile::for_each(bool callback(const String::Body, const String&, void*), void* ainfo) {
	For_each_string_callback_info info;
	
	info.self=this;
	info.nested_info=ainfo;
	info.nested_callback=callback;

	for_each(for_each_string_callback, &info);
}

static bool get_hash__put(const String::Body key, const String& value, void* aresult) {
	static_cast<HashStringValue*>(aresult)->put(key, new VString(value));
	return false;
}
HashStringValue *VHashfile::get_hash() {
	HashStringValue& result=*new HashStringValue();

	for_each(get_hash__put, &result);
	return &result;
}

static void delete_file(const char* base_name, const char* ext) {
	String sfile_name(base_name);
	sfile_name<<ext;
	file_delete(sfile_name, false);
}

void VHashfile::delete_files() {
	if(is_open())
		close();

	if(file_name){
		delete_file(file_name, PA_SDBM_DIRFEXT);
		delete_file(file_name, PA_SDBM_PAGFEXT);
	}
}

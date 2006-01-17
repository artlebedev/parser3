/** @file
	Parser: @b table class.

	Copyright(c) 2001, 2002 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexandr Petrosian <paf@design.ru> (http://paf.design.ru)
*/

static const char * const IDENT="$Date: 2006/01/17 14:41:17 $";

#include "pa_globals.h"
#include "pa_common.h"
#include "pa_threads.h"
#include "pa_vtable.h"
#include "pa_vstring.h"
#include "pa_vhashfile.h"
#include "pa_vdate.h"

// consts

const uint HASHFILE_VALUE_SERIALIZED_VERSION=0x0001;

// methods

void check(const char *step, apr_status_t status) {
	if(status==APR_SUCCESS)
		return;

	const char* str=strerror(status);
	throw Exception("file.access",
		0,
		"%s error: %s (%d)", 
			step, str?str:"<unknown>", status);
}

void VHashfile::open(const String& afile_name) {
	file_name=afile_name.cstr(String::L_FILE_SPEC);

	if(!entry_exists(file_name))
		create_dir_for_file(afile_name);

	check("apr_sdbm_open(shared)", apr_sdbm_open(&m_db, file_name, 
                                        APR_CREATE|APR_READ|APR_SHARELOCK, 
                                        0664, 0));
}

void VHashfile::close() {
	check_db();

	check("apr_sdbm_close", apr_sdbm_close(m_db));  m_db=0;
}

void VHashfile::check_db() const {
	if(!m_db)
		throw Exception(0,
			0,
			"%s is closed", type());
}

apr_sdbm_t *VHashfile::get_db_for_reading() const {
	check_db();

	return m_db;
}

apr_sdbm_t *VHashfile::get_db_for_writing() {
	check_db();

	if(apr_sdbm_rdonly(m_db)) {
		// reopen in write mode & exclusive lock
		close();
		check("apr_sdbm_open(exclusive)", apr_sdbm_open(&m_db, file_name, 
											APR_WRITE, 
											0664, 0));
	}

	return m_db;
}

VHashfile::~VHashfile() {
	if(m_db)
		close();
}

struct Hashfile_value_serialized_prolog {
	uint version;
	time_t time_to_die;
};

apr_sdbm_datum_t VHashfile::serialize_value(const String& string, time_t time_to_die) const {
	apr_sdbm_datum_t result;

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

const String* VHashfile::deserialize_value(apr_sdbm_datum_t key, const apr_sdbm_datum_t value) {
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

	return new String(input_length? pa_strdup(input_cstr, input_length): 0, input_length, true);
}

void VHashfile::put_field(const String& aname, Value *avalue) {
	apr_sdbm_t *db=get_db_for_writing();

	time_t time_to_die=0;
	const String *value_string;

	if(HashStringValue *hash=avalue->get_hash()) {
		if(Value *value_value=hash->get(value_name)) {
			if(value_value->get_junction())
				throw Exception(0,
					0,
					VALUE_NAME" must not be code");

			value_string=&value_value->as_string();

			if(Value *expires=hash->get(expires_name)) {
				if(Value* vdate=expires->as(VDATE_TYPE, false))
					time_to_die=static_cast<VDate*>(vdate)->get_time(); // $expires[DATE]
				else if(double days_till_expire=expires->as_double())
					time_to_die=time(NULL)+(time_t)(60*60*24*days_till_expire); // $expires(days)
			}
		} else
			throw Exception(0,
				&aname,
				"put hash value must contain ."VALUE_NAME);
	} else
		value_string=&avalue->as_string();

	if(aname.is_empty())
		throw Exception("parser.runtime",
			0,
			"hashfile key must not be empty");

	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	apr_sdbm_datum_t value=serialize_value(*value_string, time_to_die);

 	check("apr_sdbm_store", apr_sdbm_store(db, key, value, APR_SDBM_REPLACE));
}

Value *VHashfile::get_field(const String& aname) {
	apr_sdbm_t *db=get_db_for_reading();

	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	apr_sdbm_datum_t value;

	check("apr_sdbm_fetch", apr_sdbm_fetch(db, &value, key));

	const String *sresult=deserialize_value(key, value);
	return sresult? new VString(*sresult): 0;
}

void VHashfile::remove(const apr_sdbm_datum_t key) {
	apr_sdbm_t *db=get_db_for_writing();

	check("apr_sdbm_delete", apr_sdbm_delete(db, key));
}

void VHashfile::remove(const String& aname) {
	apr_sdbm_datum_t key;
	key.dptr=const_cast<char*>(aname.cstr());
	key.dsize=aname.length();

	remove(key);
}

void VHashfile::for_each(void callback(apr_sdbm_datum_t, void*), void* info) const {
	apr_sdbm_t *db=get_db_for_reading();

	// collect keys
	Array<apr_sdbm_datum_t>* keys=0;
	check("apr_sdbm_lock", apr_sdbm_lock(db, APR_FLOCK_SHARED));
	try {
		apr_sdbm_datum_t key;
		if(apr_sdbm_firstkey(db, &key)==APR_SUCCESS)
		{
			size_t count=0;
			do {
				// must cound beforehead, becase doing reallocs later would be VERY slow and cause HUGE fragmentation
				count++;
			} while(apr_sdbm_nextkey(db, &key)==APR_SUCCESS);

			keys=new Array<apr_sdbm_datum_t>(count);

			if(apr_sdbm_firstkey(db, &key)==APR_SUCCESS)
				do {
					// must clone because it points to page which may go away 
					// [if they modify hashfile inside foreach]
					key.dptr = pa_strdup(key.dptr, key.dsize);
					*keys+=key;
				} while(apr_sdbm_nextkey(db, &key)==APR_SUCCESS);
		}
	} catch(...) {
			check("apr_sdbm_unlock", apr_sdbm_unlock(db));
			rethrow;
	}
	check("apr_sdbm_unlock", apr_sdbm_unlock(db));

	// iterate them
	if(keys)
		keys->for_each(callback, info);
}

#ifndef DOXYGEN
struct For_each_string_callback_info {
	VHashfile* self;
	void* nested_info;
	void (*nested_callback)(const String::Body, const String&, void*);
};
#endif
static void for_each_string_callback(apr_sdbm_datum_t apkey, void* ainfo) {
	For_each_string_callback_info& info=*static_cast<For_each_string_callback_info *>(ainfo);
	apr_sdbm_t *db=info.self->get_db_for_reading();

	apr_sdbm_datum_t apvalue;
	check("apr_sdbm_fetch", apr_sdbm_fetch(db, &apvalue, apkey));

	if(const String* svalue=info.self->deserialize_value(apkey, apvalue)) {
		const char *clkey=pa_strdup(apkey.dptr, apkey.dsize);

		info.nested_callback(clkey, *svalue, info.nested_info);
	}
}
void VHashfile::for_each(void callback(const String::Body, const String&, void*), void* ainfo) {
	For_each_string_callback_info info;
	
	info.self=this;
	info.nested_info=ainfo;
	info.nested_callback=callback;

	for_each(for_each_string_callback, &info);
}

static void clear_delete_key(apr_sdbm_datum_t key, void* adb) {
	check("apr_sdbm_delete", apr_sdbm_delete(static_cast<apr_sdbm_t*>(adb), key));
}
void VHashfile::clear() {
	apr_sdbm_t *db=get_db_for_writing();

	for_each(clear_delete_key, db);
}


static void get_hash__put(const String::Body key, const String& value, void* aresult) {
	static_cast<HashStringValue*>(aresult)->put(key, new VString(value));
}
HashStringValue *VHashfile::get_hash() {
	HashStringValue& result=*new HashStringValue();

	for_each(get_hash__put, &result);
	return &result;
}

static void delete_file(const char* base_name, const char* ext) {
	String sfile_name(base_name, false/*already removed tainting at ::open*/);
	sfile_name<<ext;
	file_delete(sfile_name);
}

void VHashfile::delete_files() {
	close();
	delete_file(file_name, APR_SDBM_DIRFEXT);
	delete_file(file_name, APR_SDBM_PAGFEXT);
}

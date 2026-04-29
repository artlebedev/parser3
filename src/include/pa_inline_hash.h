/** @file
	Parser: inline-storage hash decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_INLINE_HASH_H
#define PA_INLINE_HASH_H

#define IDENT_PA_INLINE_HASH_H "$Id: pa_inline_hash.h,v 1.4 2026/04/29 23:30:44 moko Exp $"

#include "pa_hash.h"

#define PA_INLINE_HASH_N 13

// Linear-probing inline hash with PA_INLINE_HASH_N slots before overflow to HashString<V>.
// Inits 8*N bytes, while HASH allocates (and inits) at least 

template<typename V> class InlineHashString: public PA_Object {
public:

	InlineHashString() : fkeys{}, foverflow(0) {
	}

	~InlineHashString() {
#ifdef USE_DESTRUCTORS
		if(foverflow) delete foverflow;
#endif
	}

	V get(const String& name) const {
		const String::Body& nb = name;
		const uint hash = nb.get_hash_code();
		int i = hash % PA_INLINE_HASH_N;
		for(int probe = 0; probe < PA_INLINE_HASH_N; probe++) {
			if(!fkeys[i]) {
				return foverflow ? foverflow->get(name) : 0;
			}
			if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb)
				return fvalues[i];
			if(++i >= PA_INLINE_HASH_N) i = 0;
		}
		return foverflow ? foverflow->get(name) : 0;
	}

	bool put(const String& name, V value) {
		const String::Body& nb = name;
		const uint hash = nb.get_hash_code();
		int i = hash % PA_INLINE_HASH_N;
		for(int probe = 0; probe < PA_INLINE_HASH_N; probe++) {
			if(!fkeys[i]) {
				fkeys[i] = &nb;
				fvalues[i] = value;
				return false;
			}
			if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) {
				fvalues[i] = value;
				return true;
			}
			if(++i >= PA_INLINE_HASH_N) i = 0;
		}
		if(!foverflow)
			foverflow = new HashString<V>();
		return foverflow->put(name, value);
	}

	bool put_replaced(const String& name, V value) {
		const String::Body& nb = name;
		const uint hash = nb.get_hash_code();
		int i = hash % PA_INLINE_HASH_N;
		for(int probe = 0; probe < PA_INLINE_HASH_N; probe++) {
			if(!fkeys[i])
				return foverflow ? foverflow->put_replaced(name, value) : false;
			if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) {
				fvalues[i] = value;
				return true;
			}
			if(++i >= PA_INLINE_HASH_N) i = 0;
		}
		return foverflow ? foverflow->put_replaced(name, value) : false;
	}

	template<typename I> void for_each(void callback(const String::Body&, V, I), I info) const {
		for(int i = 0; i < PA_INLINE_HASH_N; i++) {
			if(fkeys[i])
				callback(*fkeys[i], fvalues[i], info);
		}
		if(foverflow)
			foverflow->for_each(callback, info);
	}

private:
	HashString<V>* foverflow;
	const String::Body* fkeys[PA_INLINE_HASH_N];  // NULL=empty, initialized in constructor
	V fvalues[PA_INLINE_HASH_N];

};

#endif // PA_INLINE_HASH_H

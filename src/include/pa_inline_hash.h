/** @file
	Parser: inline-storage hash decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (https://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_INLINE_HASH_H
#define PA_INLINE_HASH_H

#define IDENT_PA_INLINE_HASH_H "$Id: pa_inline_hash.h,v 1.6 2026/04/30 15:53:20 moko Exp $"

#include "pa_hash.h"

#define PA_INLINE_HASH_N 13  // Low overflow rate, high first-hit ratio
#define PA_PROBE_LIMIT 8     // Probe chains beyond this are very rare

// Linear-probing inline hash with PA_INLINE_HASH_N slots before overflow to HashString<V>.

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

		// Optimized first, >50% hits
		if(!fkeys[i]) return 0;  // NULL implies foverflow==NULL
		if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) return fvalues[i];
		// Сollision
		for(int n = 0; n < PA_PROBE_LIMIT-1; n++) {
			if(++i >= PA_INLINE_HASH_N) i = 0;
			if(!fkeys[i]) return 0;
			if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) return fvalues[i];
		}
		return foverflow ? foverflow->get(name) : 0;
	}

	bool put(const String& name, V value) {
		const String::Body& nb = name;
		const uint hash = nb.get_hash_code();
		int i = hash % PA_INLINE_HASH_N;

		// Optimized first
  		if(!fkeys[i]) { fkeys[i] = &nb; fvalues[i] = value; return false; }
		if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) { fvalues[i] = value; return true; }
		// Сollision
		for(int n = 0; n < PA_PROBE_LIMIT-1; n++) {
			if(++i >= PA_INLINE_HASH_N) i = 0;
			if(!fkeys[i]) { fkeys[i] = &nb; fvalues[i] = value; return false; }
			if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) { fvalues[i] = value; return true; }
		}
		if(!foverflow)
			foverflow = new HashString<V>();
		return foverflow->put(name, value);
	}

	bool put_replaced(const String& name, V value) {
		const String::Body& nb = name;
		const uint hash = nb.get_hash_code();
		int i = hash % PA_INLINE_HASH_N;

		// Optimized first
		if(!fkeys[i]) return false;
		if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) { fvalues[i] = value; return true; }
		// Сollision
		for(int n = 0; n < PA_PROBE_LIMIT-1; n++) {
			if(++i >= PA_INLINE_HASH_N) i = 0;
			if(!fkeys[i]) return false;
			if(fkeys[i]->get_hash_code() == hash && *fkeys[i] == nb) { fvalues[i] = value; return true; }
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

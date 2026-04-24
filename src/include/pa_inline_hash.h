/** @file
	Parser: inline-storage hash decls.

	Copyright (c) 2001-2026 Art. Lebedev Studio (http://www.artlebedev.com)
	Authors: Konstantin Morshnev <moko@design.ru>
*/

#ifndef PA_INLINE_HASH_H
#define PA_INLINE_HASH_H

#define IDENT_PA_INLINE_HASH_H "$Id: pa_inline_hash.h,v 1.1 2026/04/24 20:00:48 moko Exp $"

#include "pa_hash.h"

#define PA_HASH_INLINE_SIZE 4

/// Inline-storage "hash" stores up to PA_HASH_INLINE_SIZE entries without heap allocation.
/// Fallbacks to HashString for overflow.

template<typename V> class InlineHashString {
public:

	struct Pair {
		String::Body key;
		V value;
	};

	InlineHashString() : fcount(0), foverflow(0) {}

	V get(const String& name) const {
		const String::Body& nb = name;
		const uint code = nb.get_hash_code();
		for(int i = 0; i < fcount; i++) {
			const String::Body& key = fpairs[i].key;
			if(key.get_hash_code() == code && key == nb)
				return fpairs[i].value;
		}
		return foverflow ? foverflow->get(name) : 0;
	}

	bool put(const String& name, V value) {
		const String::Body& nb = name;
		const uint code = nb.get_hash_code();
		for(int i = 0; i < fcount; i++) {
			const String::Body& key = fpairs[i].key;
			if(key.get_hash_code() == code && key == nb) {
				fpairs[i].value = value;
				return true;
			}
		}
		if(fcount < PA_HASH_INLINE_SIZE) {
			fpairs[fcount].key = name;
			fpairs[fcount].value = value;
			fcount++;
			return false;
		}
		if(!foverflow)
			foverflow = new HashString<V>();
		return foverflow->put(name, value);
	}

	bool put_replaced(const String& name, V value) {
		const String::Body& nb = name;
		const uint code = nb.get_hash_code();
		for(int i = 0; i < fcount; i++) {
			const String::Body& key = fpairs[i].key;
			if(key.get_hash_code() == code && key == nb) {
				fpairs[i].value = value;
				return true;
			}
		}
		return foverflow ? foverflow->put_replaced(name, value) : false;
	}

	int count() const {
		return fcount + (foverflow ? foverflow->count() : 0);
	}

	template<typename I> void for_each(void callback(const String::Body&, V, I), I info) const {
		for(int i = 0; i < fcount; i++)
			callback(fpairs[i].key, fpairs[i].value, info);
		if(foverflow)
			foverflow->for_each(callback, info);
	}

private:
	int fcount;
	Pair fpairs[PA_HASH_INLINE_SIZE];
	HashString<V>* foverflow;
};

#endif // PA_INLINE_HASH_H

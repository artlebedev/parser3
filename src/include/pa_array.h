/** @file
	Parser: Array & Array_iter classes decls.

	Copyright (c) 2001 ArtLebedev Group (http://www.artlebedev.com)
	Author: Alexander Petrosyan <paf@design.ru> (http://paf.design.ru)

	$Id: pa_array.h,v 1.47 2001/11/05 11:46:23 paf Exp $
*/

#ifndef PA_ARRAY_H
#define PA_ARRAY_H

#include "pa_config_includes.h"
#include "pa_pool.h"
#include "pa_types.h"
#include "pa_string.h"

class Array_iter;

/**	
	Pooled Array.

	Internal structure:
	@verbatim
		Array               Chunk0
		======              ========
		head--------------->[ptr]
		append_here-------->[ptr]
		link_row            ........
				.			.
				.			[ptr]
				...........>[link to the next chunk]
	@endverbatim
*/

class Array : public Pooled {
	friend class Array_iter;
public:

	/// Array item type
	typedef void Item;

	/*/// for_each iterator function type, const info
	typedef void (*For_each_func_const)(Item *value, const void *info);
	*/

	/// for_each iterator function type
	typedef void (*For_each_func)(Item *value, void *info);

	/// first_that iterator function type, const info
	typedef void *(*Item_that_func_const)(Item *value, const void *info);

	/// first_that iterator function type
	typedef void *(*Item_that_func)(Item *value, void *info);

	enum {
		CR_INITIAL_ROWS_DEFAULT=3, ///< default preallocated row count
		CR_GROW_COUNT=3 ///< each time the Array chunk_is_full() array expanded()
	};

public:

	Array(Pool& apool, int initial_rows=CR_INITIAL_ROWS_DEFAULT);

	/// size Array. how many items are in it
	int size() const { return fused_rows; }
	/// append Item to array
	Array& operator += (Item *src);
	/// append int value to array
	Array& operator += (int value) { return *this+=reinterpret_cast<Item *>(value); }

	/// dirty hack to allow constant items storage. I long for Array<const Item*>
	Array& operator += (const Item *src) { return *this+=const_cast<Item *>(src); }

	/// append other Array portion to this one. starting from offset
	Array& append_array(const Array& src, int offset=0);

	Item *get(int index) const;
	int get_int(int index) const { return reinterpret_cast<int>(get(index)); }

	void put(int index, Item *item);
	/// convinient way to get strings from Array. I long for Array<const String *>
	const String *get_string(int index) const { 
		return const_cast<const String *>(static_cast<String *>(get(index))); 
	}

	/*/// iterate over all elements, const info
	void for_each(For_each_func_const func, const void *info=0) const;
	/*/

	/// iterate over all elements
	void for_each(For_each_func func, void *info=0) const;

	/// iterate over all elements until condition, const info
	void* first_that(Item_that_func_const func, const void *info=0) const;

	/// iterate over all elements until condition
	void* first_that(Item_that_func func, void *info=0) const;

private:

	/// several record elements
	struct Chunk {
		int count;  ///< the number of rows in chunk
		/// item or a link to next chunk union
		union Row {
			Item *item;
			Chunk *link;  ///< link to the next chunk in chain
		} rows[1];
		// next rows are here
	}
		*head;  ///< the head chunk of the chunk chain

	/** last allocated chunk
		helps appending Arrays
	*/
	Chunk *tail;

	/// next append would write to this record
	Chunk::Row *append_here;
	
	/**	the address of place where lies address 
		of the link to the next chunk to allocate
	*/
	Chunk::Row *link_row;

private:

	// array size
	int fused_rows;

private:

	bool chunk_is_full() {
		return append_here == link_row;
	}
	void expand(int chunk_rows);

private: //disabled

	//Array(Array&) { }
	Array& operator = (const Array&) { return *this; }
};


/// handy array iterator
class Array_iter {
public:

	Array_iter(const Array& aarray) : array(aarray),
		chunk(aarray.head),
		row(chunk->rows),
		countdown(chunk->count) {
	}

	/// there are still elements
	bool has_next() {
		return !(chunk==array.tail && row==array.append_here);
	}

	/// quickly extracts next Array::Item
	Array::Item *next() {
		// assuming: never called after has_next()!
		if(countdown==0) { // end of chunk?
			chunk=row->link;
			row=chunk->rows;
			countdown=chunk->count;
		}
		Array::Item *result=row->item;
		row++;  countdown--;
		return result;
	}

	/// quickly extracts next Array::Item as const String
	const String *next_string() { 
		return const_cast<const String *>(static_cast<String *>(next())); 
	}

private:
	const Array& array;
	const Array::Chunk *chunk;
	const Array::Chunk::Row *row;
	int countdown;	

};

#endif

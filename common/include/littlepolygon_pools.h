// Little Polygon SDK
// Copyright (C) 2013 Max Kaufmann
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#pragma once

#include "littlepolygon_base.h"
#include "littlepolygon_bitset.h"

// Several different common/simple object pools that index into a linear
// array of preallocated slots:
//
// * FreelistPool - Store unoccupied slots in a linked-list
// * BitsetPool - Store a bitvector which identifies unoccuped slots (allows iteration)
// * CompactPool - Use slots [0:count), swap-with last on release (fast iteration)
//                 (only useable for "anonymous" objects, like particles)
//


template<typename T, int N=1024>
class FreelistPool {
private:
	union Slot {
		T record;
		struct {
			Slot *next;
		};
	};
	Slot *firstFree;
	Slot slots[N];

public:
	FreelistPool() {
		STATIC_ASSERT(N>1);
		firstFree = slots;
		for(int i=0; i<N-1; ++i) {
			slots[i].next = slots + (i+1);
		}
		slots[N-1].next = 0;
	}

	T* alloc() {
		if (!firstFree) {
			return 0;
		} else {
			auto result = firstFree;
			firstFree = result->next;
			return &(result->record);
		}			
	}

	void release(T* slot) {
		Slot *p = (Slot*)slot;
		ASSERT(p - slots >= 0);
		ASSERT(p - slots < N);		
		p->next = firstFree;
		firstFree = p;
	}
};

template<typename T, int N=1024>
class BitsetPool {
private:
	Bitset<N> mask;
	T slots[N];

public:
	T* alloc() {
		unsigned index;
		if (!(~mask).findFirst(index)) {
			return 0;
		}
		mask.mark(index);
		return slots + index;
	}

	void release(T* slot) {
		ASSERT(slot - slots >= 0);
		ASSERT(slot - slots < N);
		mask.clear(slot-slots);
	}

	class iterator {
		const T *slots;
		Bitset<64>::iterator biterator;

		iterator(const BitsetPool *pool) : slots(pool->slots), biterator(pool->mask) {
		}

		bool next(const T* &result) {
			unsigned idx;
			if (biterator.next(idx)) {
				result = slots + idx;
				return true;
			} else {
				return false;
			}
		}
	};
};

template<typename T, int N=1024>
class CompactPool {
private:
	int count;
	T slots[N];

public:
	CompactPool() : count(0) {}

	T* alloc() {
		if (count >= N) {
			return 0;
		} else {
			++count;
			return slots + (count-1);
		}
	}

	void release(T* slot) {
		ASSERT(slot - slots > 0);
		ASSERT(slot - slots < count);
		--count;
		if (slot != slots+count) {
			*slot = slots[count];
		}
	}

	T* begin() { return slots; }
	T* end() { return slots + count; }
};
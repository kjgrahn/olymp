/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_RANGE_H
#define OLYMP_TIFF_RANGE_H

#include <cstdint>
#include <vector>

#include "error.h"

namespace tiff {

    /**
     * A const range [a..b) of octets, iterating over them, and
     * functions for reading 8-bit, 16-bit and 32-bit off it using the
     * range's endianness. Used to parse IFDs, TIFF field values and
     * so on.
     *
     * The constructors which takes a 'whole' Range parameter throw
     * if the new range isn't a subset.
     */
    class Range {
    public:
	using iterator = const uint8_t*;

	Range() : a{}, b{} {}

	explicit Range(const std::vector<uint8_t>&);

	/**
	 * The range [a..b).
	 */
	Range(iterator a, iterator b) : a{a}, b{b} {}

	/**
	 * A subrange of 'whole' at a certain offset and of a certain
	 * length.
	 */
	Range(const Range& whole, unsigned offset, unsigned len)
	    : a{whole.a + offset},
	      b{a + len}
	{
	    if (a < whole.a) throw Segfault {};
	    if (whole.b < b) throw Segfault {};
	}

	/**
	 * A subrange of 'whole' immediately following 'pred' and of a
	 * certain length.
	 */
	Range(const Range& whole, const Range& pred, unsigned len)
	    : Range(whole, pred.b - whole.a, len)
	{}

	Range(const Range& whole, const Range& pred);

	iterator begin() const { return a; }
	iterator end() const { return b; }
	std::size_t size() const { return b-a; }
	explicit operator bool() const { return a!=b; }

    private:
	const iterator a;
	const iterator b;
    };

    inline bool operator== (const Range&, const std::vector<uint8_t>&);
    inline bool operator!= (const Range&, const std::vector<uint8_t>&);
}
    
#endif

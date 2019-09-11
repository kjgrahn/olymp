/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_RANGE_H
#define OLYMP_TIFF_RANGE_H

#include <cstdint>

namespace tiff {

    /**
     * A const range [a..b) of octets, iterating over them, and
     * functions for reading 8-bit, 16-bit and 32-bit off it using the
     * range's endianness. Used to parse IFDs, TIFF field values and
     * so on.
     */
    class Range {
    public:
    private:
	const uint8_t* a;
	const uint8_t* b;
    };
}
    
#endif

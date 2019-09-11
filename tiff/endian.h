/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_ENDIAN_H
#define OLYMP_TIFF_ENDIAN_H

#include <cstdint>

namespace be {

    template <class It>
    inline unsigned eat8(It& a) { return *(a++); }

    template <class It>
    unsigned eat16(It& a)
    {
	unsigned n = eat8(a) << 8;
	n |= eat8(a);
	return n;
    }

    template <class It>
    unsigned eat32(It& a)
    {
	unsigned n = eat16(a) << 16;
	n |= eat16(a);
	return n;
    }
}

namespace le {

    template <class It>
    inline unsigned eat8(It& a) { return *(a++); }

    template <class It>
    unsigned eat16(It& a)
    {
	unsigned n = eat8(a);
	n |= eat8(a) << 8;
	return n;
    }

    template <class It>
    unsigned eat32(It& a)
    {
	unsigned n = eat16(a);
	n |= eat16(a) << 16;
	return n;
    }
}
#endif

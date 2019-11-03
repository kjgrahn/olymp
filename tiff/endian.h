/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_ENDIAN_H
#define OLYMP_TIFF_ENDIAN_H

#include <cstdint>

namespace tiff {

    /**
     * Consuming unsigned 8-, 16- and 32-bit scalars from an uint8_t
     * array, with some, as of yet undecided, endianness.
     *
     * It's stupid to have detailed processing like this rely on
     * run-time polymorphism, and not to template them with an
     * iterator type as a parameter.  I blame the TIFF standard for
     * the unusual and daft choice to leave endianness undecided.
     * It's only good for people implementing non-portable TIFF
     * writers; everyone else suffers.
     */
    struct Endian {
	using It = const uint8_t*;
	virtual ~Endian() = default;
	inline unsigned eat8(It& a) const { return *(a++); }
	virtual unsigned eat16(It& a) const = 0;
	virtual unsigned eat32(It& a) const = 0;
    };

    struct Motorola final : public Endian {

	inline unsigned eat16(It& a) const
	{
	    unsigned n = eat8(a) << 8;
	    n |= eat8(a);
	    return n;
	}

	inline unsigned eat32(It& a) const
	{
	    unsigned n = eat16(a) << 16;
	    n |= eat16(a);
	    return n;
	}
    };

    struct Intel final : public Endian {

	inline unsigned eat16(It& a) const
	{
	    unsigned n = eat8(a);
	    n |= eat8(a) << 8;
	    return n;
	}

	inline unsigned eat32(It& a) const
	{
	    unsigned n = eat16(a);
	    n |= eat16(a) << 16;
	    return n;
	}
    };
}
#endif

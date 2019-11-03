/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_TYPE_H
#define OLYMP_TIFF_TYPE_H

#include "endian.h"

#include <string>
#include <utility>

namespace tiff {

    namespace type {

	using en = Intel;

	/* The primitive TIFF types.  The interesting things about one
	 * of them are:
	 * - the numeric encoding
	 * - the encoded size of a single element
	 * and if we intend to actually decode:
	 * - the value type (the array element type)
	 * - how to extract a value from a Range
	 */
	template <unsigned T, unsigned Size>
	struct Type {
	    static constexpr unsigned type = T;
	    static constexpr unsigned size = Size;
	};

	struct Byte: public Type<1, 1> {
	    using value_type = uint8_t;
	    using array_type = std::vector<value_type>;
	    const value_type val;

	    template <class It>
	    explicit Byte(It& a) : val(en().eat8(a)) {}
	};

	struct Ascii: public Type<2, 1> {
	    using array_type = std::string;
	};

	struct Short: public Type<3, 2> {
	    using value_type = uint16_t;
	    using array_type = std::vector<value_type>;
	    const value_type val;

	    template <class It>
	    explicit Short(It& a) : val(en().eat16(a)) {}
	};

	struct Long: public Type<4, 4> {
	    using value_type = unsigned;
	    using array_type = std::vector<value_type>;
	    const value_type val;

	    template <class It>
	    explicit Long(It& a) : val(en().eat32(a)) {}
	};

	namespace impl {

	    template <class It>
	    std::pair<unsigned, unsigned> eat_pair(It& a)
	    {
		unsigned m = en().eat32(a);
		unsigned n = en().eat32(a);
		return {m, n};
	    }
	}

	struct Rational: public Type<5, 8> {
	    using value_type = std::pair<unsigned, unsigned>;
	    using array_type = std::vector<value_type>;
	    const value_type val;

	    template <class It>
	    explicit Rational(It& a) : val{impl::eat_pair(a)} {}
	};

	using Sbyte     = Type<6,  1>;

	struct Undefined: public Type<7, 1> {
	    using value_type = uint8_t;
	    using array_type = std::vector<value_type>;
	    const value_type val;

	    template <class It>
	    explicit Undefined(It& a) : val(en().eat8(a)) {}
	};

	using Sshort    = Type<8,  2>;
	using Slong     = Type<9,  4>;
	using Srational = Type<10, 8>;
	using Float     = Type<11, 4>;
	using Double    = Type<12, 8>;

    }
}

#endif

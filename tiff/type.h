/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_TYPE_H
#define OLYMP_TIFF_TYPE_H

#include <string>
#include <utility>

namespace tiff {

    /* The primitive TIFF types.  The interesting things about one
     * of them are:
     * - the numeric encoding
     * - the encoded size of a single element
     * - the value type (the array element type)
     * - how to extract a value from a Range
     */
    template <unsigned T, unsigned Size, class V = void>
    struct Type {
	static constexpr unsigned type = T;
	static constexpr unsigned size = Size;
	using value_type = V;
	using array_type = std::vector<V>;
    };

    using Byte      = Type<1,  1, uint8_t>;
    using Short     = Type<3,  2, unsigned>;
    using Long      = Type<4,  4, unsigned>;
    using Rational  = Type<5,  8, std::pair<unsigned,unsigned>>;
    using Sbyte     = Type<6,  1>;
    using Undefined = Type<7,  1, uint8_t>;
    using Sshort    = Type<8,  2>;
    using Slong     = Type<9,  4>;
    using Srational = Type<10, 8>;
    using Float     = Type<11, 4>;
    using Double    = Type<12, 8>;

    struct Ascii {
	static constexpr unsigned type = 2;
	static constexpr unsigned size = 1;
	using array_type = std::string;
    };
}

#endif

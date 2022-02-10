/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "tiff.h"
#include "error.h"
#include "endian.h"

#include <algorithm>


using namespace tiff;

namespace {

    template <class Ch>
    bool starts(const std::vector<Ch>& haystack,
		const std::vector<Ch>& needle)
    {
	// not an efficient implementation
	auto it = std::search(begin(haystack), end(haystack),
			      begin(needle), end(needle));
	return it==begin(haystack);
    }

    bool equal(const Range& r, const std::vector<uint8_t>& v)
    {
	return r.size()==v.size() && std::equal(r.begin(), r.end(), v.begin());
    }

    /**
     * What should be TIFF of an APP1 segment: the stuff after an Exif
     * marker.  Throws if there's no Exif marker or no TIFF header
     * (the header content is validated later).
     */
    Range tiff_of(const std::vector<uint8_t>& app1)
    {
	const Range app {app1};
	const Range exif {app, 0, 6};
	if (!equal(exif, {'E','x','i','f',0,0})) throw Error {};

	const Range tiff {app, exif};
	Range {tiff, 0, 8};
	return tiff;
    }

    /**
     * The endianness of a TIFF header; throws if it's neither Intel
     * nor Motorola.
     */
    std::unique_ptr<Endian> endianness_of(const Range& tiff)
    {
	const Range endianness {tiff, 0, 4};
	auto it = std::begin(endianness);
	std::unique_ptr<Endian> p;
	switch (*it) {
	case 'M':
	    p.reset(new Motorola); break;
	case 'I':
	    p.reset(new Intel); break;
	default:
	    throw Error {};
	}
	unsigned m0 = p->eat8(it);
	unsigned m1 = p->eat8(it);
	unsigned fortytwo = p->eat16(it);
	if (m0!=m1 || fortytwo != 42) throw Error {};
	return p;
    }

    /**
     * The IFD at a certain offset in the TIFF file 'tiff'. What's
     * returned is the 12-octet IFD entries, excluding the field count
     * and the final next IFD offset.
     */
    Range ifd_of(const Endian& en, const Range& tiff, unsigned offset)
    {
	const Range count {tiff, offset, 2};
	auto it = std::begin(count);
	unsigned n = en.eat16(it);
	const Range entries {tiff, count, n*12};
	const Range next {tiff, entries, 4};
	return entries;
    }

    /**
     * The first IFD in the TIFF file 'tiff', which is large enough to
     * contain the initial IFD offset.
     */
    Range ifd_of(const Endian& en, const Range& tiff)
    {
	auto it = std::begin(tiff);
	it += 4;
	unsigned offset = en.eat32(it);
	return ifd_of(en, tiff, offset);
    }

    /**
     * The IFD at the offset pointed out by a tiff::Long in 'ifd'.
     * This is how you find the Exif and GPS IFDs in IFD 0.
     */
    Range ifd_of(const Range& tiff, const Ifd& ifd, const unsigned tag)
    {
	const auto offset = find<type::Long>(ifd, tag);
	if (!offset) return {};
	return ifd_of(ifd.endian, tiff, *offset);
    }
}

File::File(const std::vector<uint8_t>& app1)
    : tiff {tiff_of(app1)},
      endian {endianness_of(tiff)},
      ifd0 {*endian, tiff, ifd_of(*endian, tiff)},
      exif {*endian, tiff, ifd_of(tiff, ifd0, 0x8769)},
      gps  {*endian, tiff, ifd_of(tiff, ifd0, 0x8825)}
{}

namespace {

    /**
     * The octet size of a field of a certain type and count.
     * Returns 0 for unknown types.
     *
     * Calculations can overflow for malicious data, but I see no
     * harm in that.
     */
    unsigned size(unsigned type, unsigned count)
    {
	using namespace tiff::type;

	switch (type) {
	case Byte::type:      return Byte::size * count;
	case Ascii::type:     return Ascii::size * count;
	case Short::type:     return Short::size * count;
	case Long::type:      return Long::size * count;
	case Rational::type:  return Rational::size * count;
	case Sbyte::type:     return Sbyte::size * count;
	case Undefined::type: return Undefined::size * count;
	case Sshort::type:    return Sshort::size * count;
	case Slong::type:     return Slong::size * count;
	case Srational::type: return Srational::size * count;
	case Float::type:     return Float::size * count;
	case Double::type:    return Double::size * count;
	}
	return 0;
    }
}

/**
 * The value of the first 'tag' of type 'type', or else the empty
 * range.
 */
Range Ifd::find(const unsigned tag, const unsigned type) const
{
    auto a = std::begin(ifd);
    const auto b = std::end(ifd);
    while (a!=b) {
	if (endian.eat16(a)!=tag)  { a += 10; continue; }
	if (endian.eat16(a)!=type) { a += 8; continue; }
	const unsigned count = endian.eat32(a);

	unsigned n = size(type, count);
	if (n>4) {
	    const unsigned offset = endian.eat32(a);
	    return {tiff, offset, n};
	}
	else {
	    return {a, a + n};
	}
    }
    return {};
}

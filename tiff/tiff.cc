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
     * marker.  Throws if there's no Exif marker or no valid TIFF
     * header.
     */
    Range tiff_of(const std::vector<uint8_t>& app1)
    {
	const Range app {app1};
	const Range exif {app, 0, 6};
	if (!equal(exif, {'E','x','i','f',0,0})) throw Error {};

	const Range tiff {app, exif};
	const Range h {tiff, 0, 4};
	if (!equal(h, {'I','I',42,0})) throw Error {};

	Range {tiff, h, 4};
	return tiff;
    }

    /**
     * True if the TIFF header (of a blob known to contain one)
     * says it's big-endian.
     */
    bool is_bigendian(const Range& tiff)
    {
	const Range endianness {tiff, 0, 2};
	return equal(endianness, {'M','M'});
    }

    /**
     * The IFD at a certain offset in the TIFF file 'tiff'. What's
     * returned is the 12-octet IFD entries, excluding the field count
     * and the final next IFD offset.
     */
    Range ifd_of(const Range& tiff, unsigned offset)
    {
	const Range count {tiff, offset, 2};
	auto it = std::begin(count);
	unsigned n = le::eat16(it);
	return {tiff, count, n*12};
    }

    /**
     * The first IFD in the TIFF file 'tiff', which is large enough to
     * contain the initial IFD offset.
     */
    Range ifd_of(const Range& tiff)
    {
	auto it = std::begin(tiff);
	it += 4;
	unsigned offset = le::eat32(it);
	return ifd_of(tiff, offset);
    }
}

File::File(const std::vector<uint8_t>& app1)
    : tiff {tiff_of(app1)},
      bigendian {is_bigendian(tiff)},
      ifd0 {tiff, ifd_of(tiff)}
{
}

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
	switch (type) {
	case  1:
	case  2:
	case  6:
	case  7: return count;
	case  3:
	case  8: return 2*count;
	case  4:
	case  9:
	case 11: return 4*count;
	case  5:
	case 10:
	case 12: return 8*count;
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
	if (le::eat16(a)!=tag)  { a += 10; continue; }
	if (le::eat16(a)!=type) { a += 8; continue; }
	const unsigned count = le::eat32(a);

	unsigned n = size(type, count);
	if (n>4) {
	    const unsigned offset = le::eat32(a);
	    return {tiff, offset, n};
	}
	else {
	    return {a, a + n};
	}
    }
    return {};
}

/*
 * Copyright (c) 2019 J�rgen Grahn
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

    bool equal(const Range&, const std::vector<uint8_t>&);

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
      ifd {ifd_of(tiff)}
{
}

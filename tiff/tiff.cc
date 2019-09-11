/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "tiff.h"
#include "error.h"

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

    /**
     * What should be TIFF of an APP1 segment: the stuff after an Exif
     * marker.  Throws if there's no Exif marker or no valid TIFF
     * header.
     */
    Blob tiff_of(const std::vector<uint8_t>& app1)
    {
	if(!starts(app1, {'E','x','i','f',0,0})) throw Error {};
	Blob b {begin(app1) + 6, end(app1)};
	if (b.size() < 2 + 1 + 4 + (2 + 12 + 4)) throw Segfault {};
	if(b[0] != b[1]) throw Error {};
	auto endian = b[0];
	if (endian != 'I' && endian != 'M') throw Error {};
	if (b[3] != 42) throw Error {};
	return b;
    }

    /**
     * True if the TIFF header (of a blob known to contain one)
     * says it's big-endian.
     */
    bool is_bigendian(const Blob& tiff)
    {
	return tiff[0]=='M';
    }
}

File::File(const std::vector<uint8_t>& app1)
    : content {tiff_of(app1)},
      bigendian {is_bigendian(content)}
{
}

namespace {

    struct Field {
	explicit Field(const uint8_t* p);
	Field(unsigned tag, unsigned type, unsigned val);

	bool operator== (const Field& other) const;
	std::size_t valsize() const;

	const unsigned tag;
	const unsigned type;
	const unsigned count;
    };
}

Blob Ifd::find(unsigned tag, unsigned type, unsigned count) const
{
    const Field f {tag, type, count};

    auto a = begin();
    const auto b = end();
    while (a!=b) {
	const Field g {a};
	if (g==f) {
	    a += 2 + 2 + 4;
	    if (g.valsize() > 4) {
		Offset offset {a};
		return {offset, g.valsize()};
	    }
	    else {
		return {a, a + g.valsize()};
	    }
	}
	a += 12;
    }
    return {};
}

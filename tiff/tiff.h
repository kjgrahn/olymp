/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_H
#define OLYMP_TIFF_H

#include "range.h"
#include "type.h"
#include "endian.h"

#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>

namespace tiff {

    /**
     * A TIFF IFD appearing at a certain offset in a File.  An IFD is
     * a sequence of 1..255 fields and a next IFD offset.
     *
     * One Range contains the N 12-octet fields of the IFD (but not
     * the count and next IFD offset); another contains the whole
     * File.
     */
    class Ifd {
    public:
	Ifd() = default;
	Ifd(const Range& tiff, const Range& ifd)
	    : tiff{tiff},
	      ifd{ifd}
	{}

	bool empty() const { return ifd.size()==0; }

	template <class T>
	typename T::array_type find(unsigned tag) const;

    private:
	Range tiff;
	Range ifd;

	Range find(unsigned tag, unsigned type) const;
    };

    /**
     * Find the first field with a certain tag and of a certain
     * tiff::Type.  Typically returns a vector: e.g. for a tiff::Long
     * field, it returns std::vector<unsigned>.
     */
    template <class T>
    typename T::array_type Ifd::find(unsigned tag) const
    {
	const Range r = find(tag, T::type);
	typename T::array_type v;
	auto a = r.begin();
	const auto b = r.end();
	while (a!=b) {
	    v.push_back(le::eat32(a));
	}
	return v;
    }

    template <> inline
    Ascii::array_type Ifd::find<Ascii>(unsigned tag) const
    {
	const Range r = find(tag, Ascii::type);
	auto e = std::find(r.begin(), r.end(), '\0');
	return {r.begin(), e};
    }


    /**
     * A TIFF file according to TIFF revision 6.0 (Adobe 1992).
     *
     * Here it's defined as the content of a JFIF APP1 segment, after
     * the Exif marker.  And what we're interested in is TIFF fields
     * in the Exif and GPS IFDs, which can be found via IFD 0.
     */
    class File {
    public:
	explicit File(const std::vector<uint8_t>& app1);

    private:
	const Range tiff;
	const bool bigendian;

    public:
	Ifd ifd0;
	Ifd exif;
	Ifd gps;
    };
}

#endif

/* -*- c++ -*-
 *
 * Copyright (c) 2019 J�rgen Grahn
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
     *
     * Returns an empty vector if no matching field is found.  Thus
     * you cannot distinguish between a kind-of valid field with count
     * 0, the absence of that field, or the presence of the tag but
     * with the wrong type.
     *
     * Will throw on malformed TIFF data, such as an offset pointing
     * outside the file.
     *
     * Only some TIFF field types are supported for now: the ones I
     * need and the ones that are easy.
     */
    template <class T>
    typename T::array_type Ifd::find(unsigned tag) const
    {
	const Range r = find(tag, T::type);
	typename T::array_type v;
	auto a = r.begin();
	const auto b = r.end();
	while (a!=b) {
	    v.push_back(T(a).val);
	}
	return v;
    }

    /**
     * Like Ifd::find<T>(tag) in general, but returns a std::string.
     *
     * TIFF has some support for arrays of strings, but this function
     * hasn't.  On the other hand, it supports skipping the \0
     * terminator or having it appear early.  And it doesn't fail on
     * non-ASCII text.
     */
    template <> inline
    type::Ascii::array_type Ifd::find<type::Ascii>(unsigned tag) const
    {
	const Range r = find(tag, type::Ascii::type);
	auto e = std::find(r.begin(), r.end(), '\0');
	return {r.begin(), e};
    }

    /**
     * A TIFF file according to TIFF revision 6.0 (Adobe 1992).
     *
     * Here it's defined as the content of a JFIF APP1 segment, after
     * the Exif marker.  And what we're interested in is TIFF fields
     * in the Exif and GPS IFDs, which can be found via IFD 0, if they
     * exist.
     *
     * We don't look for any other IFDs.
     *
     * The constructor will throw on error, for example if it's not
     * given an Exif APP1 segment, or if the TIFF file inside is
     * malformed in any way. The vector needs to be present throughout
     * the lifetime of the File; it is not copied.
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

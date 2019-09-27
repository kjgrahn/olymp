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

#include <cstdint>
#include <vector>
#include <array>
#include <memory>

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

	Range find(unsigned tag, unsigned type) const;

    private:
	Range tiff;
	Range ifd;
    };

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

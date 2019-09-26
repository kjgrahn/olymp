/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_H
#define OLYMP_TIFF_H

#include "range.h"

#include <cstdint>
#include <vector>
#include <array>
#include <memory>

namespace tiff {

    /**
     * A TIFF IFD appearing at a certain offset in a File.  An IFD is
     * a sequence of 1..255 fields and a next IFD offset.
     */
    class Ifd {
    public:
	Ifd() = default;
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
	Ifd ifd0;
	Ifd exif;
	Ifd gps;

    private:
	const Range tiff;
	const bool bigendian;
    };
}

#endif

/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_EXIF_H
#define OLYMP_EXIF_H

#include "tiff/tiff.h"

namespace exif {

    class DateTimeOriginal {
    public:
	explicit DateTimeOriginal(const tiff::File&);
	bool empty() const;
    };

#if 0
    typedef Ascii<0x9003, 20> DateTimeOriginal;

    typedef Rational<0x829A>  ExposureTime;
    typedef Rational<0x829D>  FNumber;
    typedef Short   <0x8822>  ExposureProgram;
    typedef Long    <0x8833>  ISOSpeed;
#endif
}

#endif

/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_EXIF_H
#define OLYMP_EXIF_H

#include "tiff/tiff.h"


/**
 * Selected Exif attributes from Exif 2.3.
 * See e.g. CIPA DC-008-2012.
 */
namespace exif {

    template <class T, unsigned Tag>
    struct Field {
	static constexpr unsigned tag = Tag;
	using Type = T;
    };

    /* In original, something like "2019:11:20 23:07:39" but we want
     * to access it as "2019-11-20" and "23:07".  We don't sanity-check
     * very carefully.
     */
    class DateTimeOriginal : public Field<tiff::type::Ascii, 0x9003> {
    public:
	explicit DateTimeOriginal(const tiff::File& tiff);

	std::string date() const;
	std::string hhmm() const;
	std::string hhmmss() const;
	bool valid() const;
	bool near(const DateTimeOriginal& other) const;

    private:
	const std::string s;
    };

    typedef Field<tiff::type::Rational, 0x829A> ExposureTime;
    typedef Field<tiff::type::Rational, 0x829D> FNumber;
    typedef Field<tiff::type::Short,    0x8822> ExposureProgram;
    typedef Field<tiff::type::Long,     0x8833> ISOSpeed;
}

#endif

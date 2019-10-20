/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_GPS_H
#define OLYMP_GPS_H

#include "tiff/tiff.h"

/**
 * Selected GPS attributes from Exif 2.3.  Exif defines them, but they
 * are placed in a different IFD compared to the "plain" Exif
 * attributes.  See e.g. CIPA DC-008-2012.
 *
 * Possibly it's daft to model each attribute as a distinct type.
 */
namespace gps {

    template <class T, unsigned Tag, unsigned Count = 1>
    struct Field {
	static constexpr unsigned tag = Tag;
	static constexpr unsigned count = Count;
	using Type = T;

	Field(const tiff::File& file)
	    : val {tiff::find<Type, count>(file.gps, tag)}
	{}

	optional<std::array<typename Type::value_type, count>> val;
    };

    template <class T, unsigned Tag>
    struct Field<T, Tag, 1> {
	static constexpr unsigned tag = Tag;
	static constexpr unsigned count = 1;
	using Type = T;

	Field(const tiff::File& file)
	    : val {tiff::find<Type>(file.gps, tag)}
	{}

	optional<typename Type::value_type> val;
    };

    template <unsigned Tag>
    struct Field<tiff::type::Ascii, Tag, 1u> {

	Field(const tiff::File& file)
	    : val {file.gps.find<tiff::type::Ascii>(Tag)}
	{}
	std::string val;
    };

    typedef Field<tiff::type::Ascii,    0x01>    LatitudeRef;
    typedef Field<tiff::type::Rational, 0x02, 3> Latitude;
    typedef Field<tiff::type::Ascii,    0x03>    LongitudeRef;
    typedef Field<tiff::type::Rational, 0x04, 3> Longitude;

    typedef Field<tiff::type::Rational, 0x07, 3> TimeStamp;
    typedef Field<tiff::type::Ascii,    0x1d>    DateStamp;

    typedef Field<tiff::type::Rational, 0x0b>    DOP;
    typedef Field<tiff::type::Ascii,    0x12>    MapDatum;

    typedef Field<tiff::type::Rational, 0x1f>    HPositioningError;
}

#endif

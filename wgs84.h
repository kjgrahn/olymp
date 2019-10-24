/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_WGS84_H
#define OLYMP_WGS84_H

#include <iosfwd>

#include "tiff/tiff.h"
#include <proj_api.h>

namespace wgs84 {

    /**
     * A coordinate, based on the GPSLatitude, GPSLongitude etc fields
     * which may or may not exist in an image's Exif information.
     */
    class Coordinate {
    public:
	Coordinate() = delete;
	Coordinate(double latitude, double longitude)
	    : longitude(longitude),
	      latitude(latitude)
	{}
	explicit Coordinate(const tiff::File&);

	bool valid() const;
	std::ostream& put(std::ostream& os) const;
	projLP lp() const;

    private:
	double longitude;
	double latitude;
    };

    inline
    std::ostream& operator<< (std::ostream& os, const Coordinate& val)
    {
	return val.put(os);
    }
}

#endif

/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "wgs84.h"

#include "gps.h"
#include <iostream>
#include <unordered_map>

using wgs84::Coordinate;

namespace {

    /**
     * GPSLatitude and GPSLongitude are encoded as (degrees, minutes,
     * seconds), with each of them a TIFF RATIONAL a/b.  Then there
     * are funny formulations in the spec about how you can encode the
     * resolution by specifying e.g. N/100 minutes and no seconds
     * ... I suspect those hints have been ignored by Exif writers,
     * and I will ignore them, too.
     *
     *   A photo from a Sony DSC-HX200V I have access to encodes
     *   degrees, minutes, and seconds with three decimals:
     *   d/1, m/1, s/1000.  xkcd calls that "pointing at Waldo on
     *   a page"; see below.
     *
     * Returns 0 on error.  Error includes division by zero, but not
     * an out of range result.
     *
     * I'm assuming floating-point calculations won't distort the data
     * too much, but I'm too lazy to do an analysis, or to use some
     * bignum class.
     */
    double decode_triplet(const std::array<std::pair<unsigned, unsigned>, 3>& v)
    {
	const auto& d = v[0];
	const auto& m = v[1];
	const auto& s = v[2];

	if (!d.second) return 0;
	if (!m.second) return 0;
	if (!s.second) return 0;

	double n = d.first / double(d.second);
	n += m.first / (60.0 * m.second);
	n += s.first / (3600.0 * s.second);

	return n;
    }

    const std::unordered_map<std::string, int> signs = {
	{"N", 1}, {"S", -1},
	{"E", 1}, {"W", -1}
    };

    /**
     * Form a latitude or longitude as a positive or negative double,
     * from the funny form used for Exif GPS coordinates.
     *
     * Returns double{0} on error.
     */
    template <class Sign, class Digits>
    double coord_of(const gps::MapDatum& datum,
		    const Sign& sign,
		    const Digits& digits)
    {
	if (datum.val != "WGS-84") return 0;
	auto it = signs.find(sign.val);
	if (it == end(signs)) return 0;
	if (!digits.val) return 0;
	return it->second * decode_triplet(*digits.val);
    }
}

Coordinate::Coordinate(const tiff::File& file)
    : longitude {coord_of(gps::MapDatum{file},
			  gps::LongitudeRef{file},
			  gps::Longitude{file})},
      latitude {coord_of(gps::MapDatum{file},
			 gps::LatitudeRef{file},
			 gps::Latitude{file})}
{}

bool Coordinate::valid() const
{
    return latitude != 0 && longitude != 0;
}

std::ostream& Coordinate::put(std::ostream& os) const
{
    char buf[20];
    // https://xkcd.com/2170/
    std::snprintf(buf, sizeof buf,
		  "%.4f %.4f", latitude, longitude);
    return os << buf;
}

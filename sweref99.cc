/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "sweref99.h"

#include <iostream>

using sweref99::Coordinate;

namespace {

    /**
     * True iff b is in [a..c).
     */
    template <class N>
    bool in_range(unsigned a, N b, unsigned c)
    {
	return a <= b && b < c;
    }
}

Coordinate::Coordinate(double north, double east)
    : north{north},
      east{east}
{}

bool Coordinate::valid() const
{
    if(!in_range(6100000, north, 7800000)) return false;
    return in_range(258000, east,  926000);
}

std::ostream& Coordinate::put(std::ostream& os) const
{
    char buf[20];
    std::snprintf(buf, sizeof buf,
		  "%.0f %.0f", north, east);
    return os << buf;
}

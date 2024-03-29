/* -*- c++ -*-
 *
 * Copyright (c) 2019 J�rgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_SWEREF99_H
#define OLYMP_SWEREF99_H

#include <iosfwd>
#include <proj.h>

namespace sweref99 {

    /**
     * A planar coordinate in SWEREF 99 TM.
     */
    class Coordinate {
    public:
	Coordinate(double north, double east);
	explicit Coordinate(const PJ_COORD& xy);

	bool valid() const;
	std::ostream& put(std::ostream& os) const;

    private:
	double north;
	double east;
    };

    inline
    std::ostream& operator<< (std::ostream& os, const Coordinate& val)
    {
	return val.put(os);
    }
}

#endif

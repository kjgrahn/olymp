/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_SWEREF99_H
#define OLYMP_SWEREF99_H

#include <iosfwd>
#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#include <proj_api.h>

namespace sweref99 {

    /**
     * A planar coordinate in SWEREF 99 TM.
     */
    class Coordinate {
    public:
	Coordinate(double north, double east);
	explicit Coordinate(const projXY& xy);

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

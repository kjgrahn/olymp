/*
 * Copyright (c) 2012, 2019, 2021 Jörgen Grahn
 * All rights reserved.
 */
#include "transform.h"

#include <proj.h>
#include <cassert>

Transform::Transform()
    : t{proj_create(nullptr,
		    "+proj=utm "
		    "+zone=33 "
		    "+ellps=GRS80 "
		    "+towgs84=0,0,0,0,0,0,0 "
		    "+units=m "
		    "+no_defs")}
{
    /* In /usr/share/proj/epsg:
     *
     * # SWEREF99 TM
     * <3006> +proj=utm +zone=33 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs  <>
     *
     * Also, with the command-line proj(1) utility this works:
     * % echo 65.6542 14.536 | \
     *   proj -rs +proj=utm +zone=33 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 \
     *   +units=m +no_defs
     */
    assert(t);
}

Transform::~Transform()
{
    proj_destroy(t);
}

sweref99::Coordinate Transform::operator() (const wgs84::Coordinate& c) const
{
    PJ_COORD cc = c.lp();
    PJ_COORD ne = proj_trans(t, PJ_FWD, cc);
    return sweref99::Coordinate{ne};
}

namespace wgs84 {

    PJ_COORD Coordinate::lp() const
    {
	return proj_coord(proj_torad(longitude),
			  proj_torad(latitude),
			  0, 0);
    }
}

namespace sweref99 {

    Coordinate::Coordinate(const PJ_COORD& xy)
	: north{xy.uv.v},
	  east{xy.uv.u}
    {}
}

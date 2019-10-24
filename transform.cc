/*
 * Copyright (c) 2012, 2019 Jörgen Grahn
 * All rights reserved.
 */
#include "transform.h"

#include <proj_api.h>
#include <cassert>

Transform::Transform()
    : t{pj_init_plus("+proj=utm "
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
    pj_free(t);
}

sweref99::Coordinate Transform::operator() (const wgs84::Coordinate& c) const
{
    projLP cc = c.lp();
    projXY ne = pj_fwd(cc, t);
    return sweref99::Coordinate{ne};
}

const char* Transform::pj_release()
{
    return pj_get_release();
}

namespace wgs84 {

    projLP Coordinate::lp() const
    {
	return { longitude * DEG_TO_RAD,
		 latitude * DEG_TO_RAD };
    }
}

namespace sweref99 {

    Coordinate::Coordinate(const projXY& xy)
	: north{xy.v},
	  east{xy.u}
    {}
}

/* -*- c++ -*-
 *
 * Copyright (c) 2020 Jörgen Grahn
 * All rights reserved.
 */
#ifndef OLYMP_METADATA_H
#define OLYMP_METADATA_H

#include "filename.h"
#include "exif.h"
#include "wgs84.h"

#include <iosfwd>

class Transform;

/**
 * Metadata about a photo; used to form the output file name
 * and to form the printed entries.
 */
class Metadata {
public:
    Metadata(const Serial& nnnn,
	     const exif::DateTimeOriginal ts,
	     const wgs84::Coordinate coord);

    bool valid() const { return ts.valid(); }

    std::string filename() const;
    std::string neighbor_of(const std::string& path) const;

    void render(std::ostream& os,
		const Transform* transform) const;

private:
    Serial nnnn;
    exif::DateTimeOriginal ts;
    wgs84::Coordinate coord;
};

#endif

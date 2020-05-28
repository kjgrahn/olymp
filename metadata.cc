/*
 * Copyright (c) 2020 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "metadata.h"
#include "transform.h"

#include <iostream>
#include <sstream>

Metadata::Metadata(const Serial& nnnn,
		   const exif::DateTimeOriginal ts,
		   const wgs84::Coordinate coord)
    : nnnn {nnnn},
      ts {ts},
      coord {coord}
{}

/**
 * The JPG image file name formed by the date and serial
 * number.
 */
std::string Metadata::filename() const
{
    std::ostringstream oss;
    oss << ts.date() << '_' << nnnn << ".jpg";
    return oss.str();
}

/**
 * This file, as a neighbor of 'path'.
 */
std::string Metadata::neighbor_of(const std::string& path) const
{
    return neighbour(path, filename());
}

/**
 * Render the output.
 */
void Metadata::render(std::ostream& os,
		      const Transform* const transform) const
{
    os << '\n'
       << filename() << '\n'
       << ts.date() << ' ' << ts.hhmm() << '\n';

    if (transform) {
	const sweref99::Coordinate sw = (*transform)(coord);
	if (sw.valid()) {
	    os << '{' << sw << "}\n";
	    return;
	}
    }
    if (coord.valid()) {
	os << '{' << coord << "}\n";
    }
}

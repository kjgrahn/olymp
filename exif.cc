/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "exif.h"

#include <algorithm>

using exif::DateTimeOriginal;


DateTimeOriginal::DateTimeOriginal(const tiff::File& tiff)
    : s {tiff.exif.find<Type>(tag)}
{}

/**
 * The date part in ISO format: 2019-10-06.
 */
std::string DateTimeOriginal::date() const
{
    std::string val = s.substr(0, 4+3+3);
    std::replace(begin(val), end(val), ':', '-');
    return val;
}

/**
 * The time of day part, in ISO format without the seconds: 08:54.
 */
std::string DateTimeOriginal::hhmm() const
{
    return s.substr(11, 5);
}

/**
 * True if the attribute exists and is well-formed.
 */
bool DateTimeOriginal::valid() const
{
    return s.size()==4+3+3 + 1 + 3+3+2;
}

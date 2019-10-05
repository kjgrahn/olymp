/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "exif.h"

#include <algorithm>

using exif::DateTimeOriginal;

namespace {
    const struct {
	std::regex datetime {R"r((\d{4}:\d{2}:\d{2}) (\d{2}:\d{2}):\d{2})r",
			     std::regex::optimize};
    } re;
}


DateTimeOriginal::DateTimeOriginal(const tiff::File& tiff)
    : s {tiff.exif.find<Type>(tag)}
{
    std::regex_match(s, match, re.datetime);
}

/**
 * The date part in ISO format: 2019-10-06.
 */
std::string DateTimeOriginal::date() const
{
    std::string val = match.str(1);
    std::replace(begin(val), end(val), ':', '-');
    return val;
}

/**
 * The time of day part, in ISO format without the seconds: 08:54.
 */
std::string DateTimeOriginal::hhmm() const
{
    return match.str(2);
}

/**
 * True if the attribute exists and is well-formed.
 */
bool DateTimeOriginal::valid() const
{
    return !match.empty();
}

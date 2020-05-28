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

std::string DateTimeOriginal::hhmmss() const
{
    return s.substr(11, 8);
}

/**
 * True if the attribute exists and is well-formed.
 */
bool DateTimeOriginal::valid() const
{
    return s.size()==4+3+3 + 1 + 3+3+2;
}

namespace {

    /**
     * Seconds since midnight, basically. Very crude parsing.
     */
    int seconds(const std::string& s)
    {
	auto at = [&s] (size_t offset) {
		      return std::strtoul(s.c_str() + offset,
					  nullptr, 10);
		  };
	// "2019:11:20 23:07:39"
	return at(17) + 60 * (at(14) + 60 * at(11));
    }
}

/**
 * True if both are valid and at most a minute apart.
 */
bool DateTimeOriginal::near(const DateTimeOriginal& other) const
{
    if (!valid()) return false;
    if (!other.valid()) return false;
    if (date() != other.date()) return false;

    auto a = seconds(s);
    auto b = seconds(other.s);
    if (b < a) std::swap(a, b);
    return (b - a) < 60;
}

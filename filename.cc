/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "filename.h"

#include <iostream>
#include <algorithm>
#include <cstdio>

namespace {

    template<class It, class Ch>
    It find_last(It a, It b, const Ch ch)
    {
	It c = b;
	while (a!=b) {
	    if (*a==ch) c = a;
	    a++;
	}
	return c;
    }

    bool digit(char ch)
    {
	return '0' <= ch && ch <= '9';
    }

    /**
     * The 1--4 last digits of the last group of digits, or "".
     */
    std::string last_digits(const char* const a,
			    const char* const b)
    {
	auto c = b;
	while (a!=c) {
	    char ch = *(c-1);
	    if (digit(ch)) break;
	    c--;
	}
	while (a!=c) {
	    char ch = *(c-1);
	    if (!digit(ch)) break;
	    c--;
	}
	auto d = c;
	while (c!=b) {
	    if (!digit(*c)) break;
	    c++;
	}
	d = std::max(d, c-4);
	return {d, c};
    }

    /**
     * Extract a serial number from the path [a, b), or return 0.
     *
     * - the directory part, if any, is ignored
     * - the last sequence of digits is the serial number
     * - if that sequence has more than four digits, only the
     *   last four are considered
     *
     * foo12345bar -> 2345
     * foo12bar    ->   12
     */
    unsigned serial(const char* a, const char* const b)
    {
	auto c = find_last(a, b, '/');
	if (c!=b) a = ++c;

	const auto s = last_digits(a, b);
	if (s.empty()) return 0;

	return std::strtoul(s.c_str(), nullptr, 10);
    }
}

std::ostream& Serial::put(std::ostream& os) const
{
    char buf[5];
    std::snprintf(buf, sizeof buf, "%04u", n);
    return os << buf;
}

/**
 * Pull a serial number from the path to an image file.  There are a
 * number of different camera file naming conventions, and parts of
 * the name is always a serial number.
 *
 * The serial number is never 0, so return zero on failure.
 */
Serial serial(const std::string& path)
{
    auto a = path.c_str();
    const auto b = a + path.size();
    return Serial{ serial(a, b) };
}

/**
 * Given that 'path' is the path of an existing file, form the path to
 * 'filename' in the same directory.
 */
std::string neighbour(const std::string& path, const std::string& filename)
{
    auto a = begin(path);
    auto b = end(path);
    auto c = find_last(a, b, '/');
    if (c==b) return filename;
    c++;
    std::string result {a, c};
    result += filename;
    return result;
}

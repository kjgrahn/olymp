/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "filename.h"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <regex>

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

    struct {
	const std::regex olymp {R"([Pp]...\d{4}\.[Jj][Pp][Gg])"};
	const std::regex dsc   {R"([Dd][Ss][Cc].\d{4}\.[Jj][Pp][Gg])"};
	const std::regex imag  {R"([Ii][Mm][Aa][Gg]\d{4}\.[Jj][Pp][Gg])"};
	const std::regex self  {R"(\d{4}-\d{2}-\d{2}_\d{4}\.jpg)"};
    } re;

    unsigned serial(const char* a, const char* const b)
    {
	auto c = find_last(a, b, '/');
	if (c!=b) a = ++c;

	if (regex_match(a, b, re.olymp)) return std::strtoul(a+4, nullptr, 10);
	if (regex_match(a, b, re.dsc))   return std::strtoul(a+4, nullptr, 10);
	if (regex_match(a, b, re.imag))  return std::strtoul(a+4, nullptr, 10);
	if (regex_match(a, b, re.self))  return std::strtoul(a+11, nullptr, 10);

	return 0;
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

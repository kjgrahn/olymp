/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_FILENAME_H
#define OLYMP_FILENAME_H

#include <string>
#include <iosfwd>

/**
 * The four-digit serial number frequently found in image file
 * names.
 */
class Serial {
public:
    explicit Serial(unsigned n = 0) : n{n} {}

    bool valid() const { return n; }
    bool operator== (const Serial& other) const { return n==other.n; }
    std::ostream& put(std::ostream& os) const;

private:
    unsigned n;
};

inline
std::ostream& operator<< (std::ostream& os, const Serial& val)
{
    return val.put(os);
}

Serial serial(const std::string& path);

std::string neighbour(const std::string& path, const std::string& filename);

#endif

/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_TIFF_OPTIONAL_H
#define OLYMP_TIFF_OPTIONAL_H

/**
 * A subset of C++17 std::optional, for those of us who don't have
 * a bleeding-edge standard library and don't want to depend on
 * Boost.
 */
template <class T>
class optional {
public:
    optional() = default;
    optional(const T& val)
	: valid{true},
	  val{val}
    {}

    bool has_value() const { return valid; }
    explicit operator bool () const { return has_value(); }
    bool operator! () const { return !has_value(); }

    T& operator* () { return val; }
    const T& operator* () const { return val; }

private:
    bool valid = false;
    T val;
};

#endif

/*
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#include "jfif.h"

#include <algorithm>
#include <iterator>

using namespace jfif;

namespace {

    constexpr uint8_t ff = 0xff;
    constexpr uint8_t nil = 0;

    template<class T, class It>
    void append(std::vector<T>& v, It a, It b)
    {
	if (a!=b) v.insert(end(v), a, b);
    }

    /**
     * In [a, b), find the first sequence of 0xff octets,
     * and return an iterator to the last of them:
     *
     * .. .. .. .. .. ff ff ff .. .. ..
     * a                    ^^          b
     */
    template<class It>
    It find_ff(It a, const It b)
    {
	a = std::find(a, b, ff);
	if (a==b) return a;
	a = std::find_if(a, b, [](unsigned ch) { return ch != ff; });
	return std::prev(a);
    }

    bool standalone(unsigned ch)
    {
	return ch==0x01 || (0xd0 <= ch && ch <=0xd9);
    }

    const uint8_t* feed(Accumulator& acc, const uint8_t* a, const uint8_t* b)
    {
	while(a!=b) {
	    if (acc.building()) a = acc.feed(a, b);

	    a = find_ff(a, b);

	    if (std::distance(a, b) < 2) break;
	    const auto ch = a[1];
	    if (ch==nil) {
		a += 2;
	    }
	    else if (standalone(ch)) {
		acc.emit(ch);
		a += 2;
	    }
	    else if (std::distance(a, b) < 4) {
		break;
	    }
	    else {
		a  = acc.feed(ch, a, b);
	    }
	}

	return a;
    }

    unsigned get16(const uint8_t* p)
    {
	unsigned n = *(p++);
	n <<= 8;
	n |= *(p++);
	return n;
    }
}

Decoder::Decoder()
    : acc(v)
{}

void Decoder::feed(const uint8_t *a, const uint8_t *b)
{
    if (remainder.empty()) {
	a = ::feed(acc, a, b);
	append(remainder, a, b);
    }
    else {
	append(remainder, a, b);
	a = remainder.data();
	b = a + remainder.size();
	a = ::feed(acc, a, b);
	remainder = {a, b};
    }
}

std::vector<Segment>& Decoder::end()
{
    if (acc.building()) throw Trailer{};
    if (!remainder.empty()) throw Trailer{};
    if (v.empty()) throw Trailer{};
    return v;
}

Accumulator::Accumulator(std::vector<Segment>& dst)
    : dst(dst)
{}

/**
 * Emit a standalone marker, e.g. FF01.  The accumulator must be
 * empty, and is still empty afterwards.
 */
void Accumulator::emit(unsigned ch)
{
    dst.emplace_back(ch, std::vector<uint8_t>{});
}

/**
 * Begin accumulating from a buffer [a, b) = [ff ch nn nn ...] where the
 * marker specifies a normal segment, e.g. FFE0.
 *
 * Either the buffer contains the full segment, which is emitted and
 * the first octet after it is returned, or it only contains the
 * beginning.
 *
 * A length may be illegal (less than two) in which case this throws.
 *
 * Undefined result if the accumulator isn't empty to begin with.
 */
const uint8_t* Accumulator::feed(const unsigned ch,
				 const uint8_t *a, const uint8_t * const b)
{
    a += 2;
    const size_t n = get16(a);
    a += 2;
    if (n<2) throw Decoder::IllegalLength {};

    missing = n-2;
    marker = ch;
    v.reserve(missing);
    v.resize(0);
    return feed(a, b);
}

/**
 * Accumulate from a buffer [a, b).  Undefined result unless already
 * building().
 */
const uint8_t* Accumulator::feed(const uint8_t *a, const uint8_t * const b)
{
    auto c = std::min(a+missing, b);
    append(v, a, c);
    missing -= c - a;
    if (!missing) dst.emplace_back(marker, v);
    return c;
}

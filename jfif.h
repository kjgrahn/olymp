/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 */
#ifndef OLYMP_JFIF_H
#define OLYMP_JFIF_H

#include <cstdint>
#include <vector>

namespace jfif {

    /**
     * The vital parts of a JFIF segment: the marker (minus the FF
     * part) and the data.  Standalone segments simply have empty
     * data.
     */
    struct Segment {
	Segment() = default;
	Segment(unsigned marker, const std::vector<uint8_t>& v)
	    : marker(marker),
	      v(v)
	{}
	bool operator== (const Segment&) const;
	bool operator< (const Segment&) const;
	uint8_t marker = 0;
	std::vector<uint8_t> v;
    };

    inline bool Segment::operator== (const Segment& other) const
    {
	return marker==other.marker && v==other.v;
    }

    inline bool Segment::operator< (const Segment& other) const
    {
	if (marker==other.marker) {
	    return v < other.v;
	}
	return marker < other.marker;
    }

    /**
     * Helper for growing segments incrementally.
     */
    struct Accumulator {
	explicit Accumulator(std::vector<Segment>& dst);
	Accumulator(const Accumulator&) = delete;
	Accumulator& operator= (const Accumulator&) = delete;

	bool building() const { return missing; }

	void emit(unsigned ch);
	const uint8_t* feed(unsigned ch, const uint8_t *a, const uint8_t *b);
	const uint8_t* feed(const uint8_t *a, const uint8_t *b);

    private:
	uint8_t marker;
	unsigned missing = 0;
	std::vector<uint8_t> v;
	std::vector<Segment>& dst;
    };

    /**
     * JFIF (JPEG) decoder, for extracting the segments (SOI, APP0,
     * APP1 etc). It doesn't interpret the segment contents, and
     * discards the entropy-encoded data.  Also doesn't care what
     * segments are present.
     */
    class Decoder {
    public:
	Decoder();

	class Error {};
	class IllegalLength: public Error {};
	class Trailer: public Error {};
	class Empty: public Error {};

	void feed(const uint8_t *a, const uint8_t *b);
	std::vector<Segment>& end();

	std::vector<Segment> v;

    private:
	Accumulator acc;
	std::vector<uint8_t> remainder;
    };
}

#endif

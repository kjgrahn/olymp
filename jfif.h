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
     * Some selected JFIF segment markers.
     */
    namespace marker {
	constexpr unsigned SOI  = 0xd8;
	constexpr unsigned SOF0 = 0xc0;
	constexpr unsigned SOF2 = 0xc2;
	constexpr unsigned DHT 	= 0xc4;
	constexpr unsigned DQT 	= 0xdb;
	constexpr unsigned DRI 	= 0xdd;
	constexpr unsigned SOS 	= 0xda;
	constexpr unsigned APP0 = 0xe0;
	constexpr unsigned APP1 = 0xe1;
	constexpr unsigned APP2 = 0xe2;
	constexpr unsigned APP3 = 0xe3;
	constexpr unsigned APP4 = 0xe4;
	constexpr unsigned APP5 = 0xe5;
	constexpr unsigned COM = 0xfe;
	constexpr unsigned EOI = 0xd9;
    }

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
     * Helper for growing segments incrementally -- and also for
     * blackholing everything after the EOI marker.
     */
    struct Accumulator {
	explicit Accumulator(std::vector<Segment>& dst);
	Accumulator(const Accumulator&) = delete;
	Accumulator& operator= (const Accumulator&) = delete;

	bool building() const { return blackhole || missing; }

	void emit(unsigned ch);
	const uint8_t* feed(unsigned ch, const uint8_t *a, const uint8_t *b);
	const uint8_t* feed(const uint8_t *a, const uint8_t *b);

	uint8_t marker;
	bool blackhole = false;
	unsigned missing = 0;
	std::vector<uint8_t> v;
	std::vector<Segment>& dst;
    };

    /**
     * JFIF (JPEG) decoder, for extracting the segments (SOI, APP0,
     * APP1 etc). It doesn't interpret the segment contents, and
     * discards the entropy-encoded data.
     *
     * Also doesn't care what segments are present -- except after the
     * first EOI, everything is discarded.  Don't know what the
     * standard says, but I have seen cameras produce files with
     * fragments of segments after EOI.
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

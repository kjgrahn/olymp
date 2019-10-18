/* -*- c++ -*-
 *
 * Copyright (c) 2019 Jörgen Grahn
 * All rights reserved.
 *
 * The JFIF file format, as I understand it:
 *
 * - A file is a sequence of segments, with entropy-encoded data
 *   inbetween.
 * - A segment is a marker, followed by a 16-bit length and length-2
 *   bytes of data.
 * - A marker is [FF nn], where nn != 00.
 * - A standalone marker (at least nn = D0--D9, nn = 01) is followed
 *   by neither length nor data, but kind of forms a mini-segment.
 * - There may be FF padding before the marker.
 * - Entropy-encoded data contains no markers, only FF 00 sequences
 *   (and the FF padding sequence at the end I guess, if you want to
 *   look at it that way.
 * - The image begins with a SOI standalone marker and ends with an
 *   EOI marker.
 *
 * This comes mostly from the JFIF and JPEG Wikipedia articles.
 */
#ifndef OLYMP_JFIF_H
#define OLYMP_JFIF_H

#include <cstdint>
#include <vector>
#include <memory>

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
     * part) and the data.  Standalone segments (e.g. SOI) simply have
     * empty data.
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

    struct Accumulator;

    /**
     * JFIF (JPEG) decoder, for extracting the segments (SOI, APP0,
     * APP1 etc). It doesn't interpret the segment contents, and
     * discards the entropy-encoded data.
     *
     * Also doesn't care what segments are present -- except after the
     * first EOI, everything is discarded.  Don't know what the
     * standard says, but I have seen cameras produce files with
     * fragments of segments (i.e. garbage) after EOI.
     *
     * The decoder gets fed by a sequence of feed() terminated by
     * end(). Throws Decoder::Error subclasses on decoding error.
     */
    class Decoder {
    public:
	Decoder();
	~Decoder();
	Decoder(const Decoder&) = delete;
	Decoder& operator= (const Decoder&) = delete;

	class Error {};
	class IllegalLength: public Error {};
	class Trailer: public Error {};
	class Empty: public Error {};
	class FalseStart: public Error {};

	void feed(const uint8_t *a, const uint8_t *b);
	std::vector<Segment>& end();

	std::vector<Segment> v;

	enum class State {
	    Start,
	    Entropy,
	    Segment,
	    FF, FFmm, FFmmnn,
	    Trailer
	};

    private:
	std::unique_ptr<Accumulator> acc;
	State state;
    };
}

#endif
